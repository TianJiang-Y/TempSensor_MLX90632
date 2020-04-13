
#include <stdbool.h>
#include <math.h>
#include "uart.h"
#include <plib/delays.h>

#ifndef BIT
#define BIT(n)      (1 << (n))
#endif

#define MLX632_SLAVEADDRESS                 (0x3B << 1)

#define MLX632_GLOBALADDRESS                0x00

#define MLX90632_EE_CTRL                    0x24D4 
#define MLX90632_EE_CONTROL                 MLX90632_EE_CTRL 
#define MLX90632_EE_I2C_ADDRESS             0x24D5 
#define MLX90632_EE_VERSION                 0x240B 

#define MLX90632_EEPROM_VERSION             0x105 
  
#define MLX632_REFRESH_RATE_EE00_ADDR      0x24E0
#define MLX632_REFRESH_RATE_EE01_ADDR      0x24E1
#define MLX632_REFRESH_RATE_EE02_ADDR      0x24E2
  
#define MLX632_GLOBAL_RESET                0x0006
#define MLX632_ADDRESSED_RESET             0x3005
  
#define MLX632_EEPROM_UNLOCK               0x554C
#define MLX632_EEPROM_ERASE                0x0000
  
#define MLX632_REG_CONTROL                 0x3001
#define MLX632_STEPMODE_MEAS_START         0x0008
#define MLX632_SLEEPMODE_SET               0x0002
#define MLX632_STEPMODE_SET                0x000C
#define MLX632_CONTINUOUSMODE_SET          0x0006
#define MLX632_REG_STATUS                  0x3FFF
#define MLX632_REG_STATUS_SET              0x0100

#define IIC_Read                            0x01
#define IIC_Write                           0x00

#define MLX632_REFRESH_RATE_2000MS          0x0000  // 0.5Hz
#define MLX632_REFRESH_RATE_1000MS          0x0100  // 1Hz
#define MLX632_REFRESH_RATE_500MS           0x0200  // 2Hz
#define MLX632_REFRESH_RATE_250MS           0x0300  // 4Hz
#define MLX632_REFRESH_RATE_125MS           0x0400  // 8Hz
#define MLX632_REFRESH_RATE_62P5MS          0x0500  // 16Hz
#define MLX632_REFRESH_RATE_31P25MS         0x0600  // 32Hz
#define MLX632_REFRESH_RATE_15P625MS        0x0700  // 64Hz

//static unsigned short Calc_MLX632TempValue(unsigned char *DataArr, unsigned char n);
extern void Delay_ms(unsigned int ms);

static unsigned char TempBuffer[22] = {0};
/**************************************************************************************/
void IIC_Delay(unsigned char times);
unsigned char GPIO_ReadBit(unsigned char I2C_Index);
void IIC_Start(unsigned char I2C_Index);
void IIC_Stop(unsigned char I2C_Index);
void IIC_Ack(unsigned char I2C_Index);
void IIC_NAck(unsigned char I2C_Index);
void IIC_Send_Byte(unsigned char I2C_Index,unsigned char  data);
unsigned char IIC_Read_Byte(unsigned char I2C_Index,unsigned char ack);
unsigned char IIC_Wait_Ack(unsigned char I2C_Index);
void Total_TSen_I2C_Init(void);
/**************************************************************************************/
static unsigned char MLX632RAM_DirectRead(unsigned char ID, unsigned char *pBuf, unsigned char nBytes);
static unsigned char MLX632_AddressedWrite(unsigned char ID, unsigned int Reg_addr, unsigned int data, unsigned char nBytes);
static unsigned char MLX632_AddressedRead(unsigned char ID,unsigned int Reg_addr, unsigned char *ppBuf, unsigned char nBytes);
static void MLX632_EEPROM_Write(unsigned char ID, unsigned int EE_addr, unsigned int data);
static unsigned char MLX_RefreshRate_Set(unsigned int RefRate);
/**********************************************************************************/
//ok
static unsigned char MLX632_AddressedRead(unsigned char ID,unsigned int Reg_addr, unsigned char *ppBuf, unsigned char nBytes)
{
    unsigned char i;
    unsigned char pBuf[3] = {0};
    
    pBuf[0] = (MLX632_SLAVEADDRESS | IIC_Write);
    pBuf[1] = Reg_addr >> 8;
    pBuf[2] = (unsigned char)(Reg_addr);
    
    IIC_Start(ID);
    
    for(i = 0; i < 3; i++)
    {
        IIC_Send_Byte(ID, pBuf[i]);

        if(IIC_Wait_Ack(ID)) 
        {
            IIC_Stop(ID);
            return 1;
        }
    }
    
    IIC_Start(ID);
    IIC_Send_Byte(ID, MLX632_SLAVEADDRESS | IIC_Read);
    
    if(IIC_Wait_Ack(ID))
      {
          IIC_Stop(ID);
          return 1;
      }

    while(nBytes)
    {
        if(nBytes == 1)
            *ppBuf = IIC_Read_Byte(ID, 0);
        else 
            *ppBuf = IIC_Read_Byte(ID, 1);
        
        nBytes--;
        ppBuf++;
    }

    IIC_Stop(ID); 
    
    return 0;
}
//ok
static unsigned char MLX632RAM_DirectRead(unsigned char ID, unsigned char *pBuf, unsigned char nBytes)
{
    IIC_Start(ID);
    IIC_Send_Byte(ID, (MLX632_SLAVEADDRESS | IIC_Read));

    if(IIC_Wait_Ack(ID))  
    {
        IIC_Stop(ID);
        return 0;
    }

    while(nBytes)
    {
        if(nBytes == 1)
            *pBuf = IIC_Read_Byte(ID, 0);
        else 
            *pBuf = IIC_Read_Byte(ID, 1);
        
        nBytes--;
        pBuf++;
    }
    
    IIC_Stop(ID);
    
    return 1;
}

static unsigned char MLX632_AddressedWrite(unsigned char ID, unsigned int Reg_addr, unsigned int data, unsigned char nBytes)
{
    unsigned char i;
    unsigned char pBuf[4] = {0};
    
    pBuf[0] = Reg_addr >> 8;
    pBuf[1] = (unsigned char)(Reg_addr);
    pBuf[2] = data >> 8;
    pBuf[3] = (unsigned char)(data);

    IIC_Start(ID);
    IIC_Send_Byte(ID, (MLX632_SLAVEADDRESS | IIC_Write));

    if(IIC_Wait_Ack(ID))
    {
        IIC_Stop(ID);
        return 1;
    }

    for(i = 0; i < nBytes; i++)
    {
        IIC_Send_Byte(ID, pBuf[i]);

        if(IIC_Wait_Ack(ID)) 
        {
            IIC_Stop(ID);
            return 1;
        }
    }
    
    IIC_Stop(ID);
    return 0;
}

static void MLX632_EEPROM_Write(unsigned char ID, unsigned int EE_addr, unsigned int data)
{
    // 1. write "CUST-key"
    MLX632_AddressedWrite(ID, MLX632_ADDRESSED_RESET, MLX632_EEPROM_UNLOCK, 4);
    Delay_ms(1);  // Absolute Necessary!!!!!!
    // 2. write 0x0000 to EEPROM addr 0x24E1 or 0x24E2
    MLX632_AddressedWrite(ID, EE_addr, MLX632_EEPROM_ERASE, 4);
    Delay_ms(1);  // Absolute Necessary!!!!!!
    // 3. write "CUST-key" 
    MLX632_AddressedWrite(ID, MLX632_ADDRESSED_RESET, MLX632_EEPROM_UNLOCK, 4);
    Delay_ms(1);  // Absolute Necessary!!!!!!
    // 4. write DATA to EEPROM addr 0x24E1 or 0x24E2
    MLX632_AddressedWrite(ID, EE_addr, data, 4);
    Delay_ms(1);  // Absolute Necessary!!!!!!
}

static unsigned char MLX_RefreshRate_Set(unsigned int RefRate)
{
    unsigned char i;
    unsigned char ReadData[2] = {0};
    unsigned int refreshReg = 0;
    
    i = 1; // For Debug........
    
    for(i = 1; i < 5; i++)
        {
            MLX632_EEPROM_Write(i, MLX632_REFRESH_RATE_EE01_ADDR, 0x800D | RefRate);
            Delay_ms(1);
            MLX632_EEPROM_Write(i, MLX632_REFRESH_RATE_EE02_ADDR, 0x801D | RefRate);
            Delay_ms(1);
            MLX632_AddressedRead(i, MLX632_REFRESH_RATE_EE02_ADDR, ReadData, 2);
            
            refreshReg = (ReadData[0] << 8) | ReadData[1];
            
            if(refreshReg != (0x801D | RefRate))
            {
                return 1;
            }
        }
    return 0;
}

 unsigned char MLX_Sensors_CheckInit(void)
 {
    unsigned char i;
    unsigned char ReadData[2] = {0};
    unsigned int refreshReg = 0;
    
    Total_TSen_I2C_Init();

    i = 1; // For Debug........
    
    for(i = 1; i < 5; i++)
    {
        MLX632_AddressedWrite(i, MLX632_ADDRESSED_RESET, MLX632_GLOBAL_RESET, 4);
        Delay_ms(70);
        MLX632_AddressedWrite(i, MLX632_REG_CONTROL, MLX632_CONTINUOUSMODE_SET, 4);
        Delay_ms(1);
        MLX632_AddressedRead(i, MLX632_REFRESH_RATE_EE00_ADDR, ReadData, 2);
        
        refreshReg = (ReadData[0] << 8) | ReadData[1];
        
        if(refreshReg != 0x070B)
        {
            Usart_Sendbytes("MLX_Sensor0");
            Usart_Sendonebyte(i + 48);
            Usart_Sendbytes("Error!\n");
            return 1;
        }
    }
    
    if(MLX_RefreshRate_Set(MLX632_REFRESH_RATE_62P5MS))
    {
//        Usart_Sendbytes("632_RateSet_Error!\n");
        return 1;
    }
//    else Usart_Sendbytes("632_RateSet_OK!\n");

    return 0;
 }

unsigned char MLX90632_Start_Measurement(unsigned char ID)
{
     unsigned char ret, tries = 500;
     unsigned char ReadData[2] = {0};

     ret = MLX632_AddressedWrite(ID, MLX632_REG_STATUS, MLX632_REG_STATUS_SET, 4);
     if (ret)
         return 0;
////     while(((reg_status >> 15) & 0x0001)); // For Debugging......
     while (tries-- > 0)
     {
         ret = MLX632_AddressedRead(ID, MLX632_REG_STATUS, ReadData, 2);
         if (ret)
             return 0;
         
//         if (!((ReadData[1] >> 7) & 0x00))
//             RATE_DEBUG = 1;
         
         if ((ReadData[1] >> 7) & 0x01)
         {
//            MLX632_AddressedWrite(ID, MLX632_REG_STATUS, MLX632_REG_STATUS_SET, 4);
            break;
         }

//         Delay_ms(delaytime);
     }
//     RATE_DEBUG = 0;
     return 1;
 }

/*******************************************************************************/

//static unsigned short Calc_MLX632TempValue(unsigned char *DataArr, unsigned char n)
//{
//   short A[10] = {0};
//   char S, Sto;
//   float VRto, VRta, AMB, TAdut, Tak, To1, To2, To3;
//   unsigned char TA0 = 25;
//   unsigned char TO0 = 25;
//   unsigned short ResultData;
//   
//   for(unsigned char i = 0; i < n; i++)
//      A[i] = *DataArr++;
//   
//   if(((A[0] & 0x0001) == 0x0001) & ((A[0] & 0x0004) == 0x0004))
////      S = ((A[4]) + (A[5])) / 2;
//       S = ((A[4]) + (A[5])) >> 1;
//   if(((A[0] & 0x0001) == 0x0001) & ((A[0] & 0x0008) == 0x0008))
////      S = ((A[7]) + (A[8])) / 2;
//       S = ((A[7]) + (A[8])) >> 1;
//   
//   VRta = A[9] + 9.5 * (A[6] / 12);
//   AMB = (A[6] / 12) / VRta * 524288;
//   VRto = A[9] + ((10.5 * A[6]) / 12);
//   Sto = ((S / 12) / VRto * 524288);
//   TAdut = ((AMB - 23809.01) / 81.81125) + 25;
//   Tak = TAdut + 273.15;
//   
//   To1 = sqrt(sqrt(Sto / (0.00000079 * (1 + (-0.00049) * (25 - TO0) + (-0.00045)* (TAdut - TA0))) + pow(Tak, 4))) - 273.15;
//   To2 = sqrt(sqrt(Sto / (0.00000079 * (1 + (-0.00049) * (To1 - TO0) + (-0.00045)* (TAdut - TA0))) + pow(Tak, 4))) - 273.15;
//   To3 = sqrt(sqrt(Sto / (0.00000079 * (1 + (-0.00049) * (To2 - TO0) + (-0.00045)* (TAdut - TA0))) + pow(Tak, 4))) - 273.15;
//   
//   ResultData = (unsigned short)To3 * 100;
//
//   return ResultData;
//}

/*******************************************************************************/
unsigned char MLX90632_Temp_Measuring(unsigned char ID, unsigned int *MLX632_Data)
{
  if(MLX632RAM_DirectRead(ID,TempBuffer, 20))
  {
      MLX632_Data[0] = ((TempBuffer[0] & 0xFF) << 8) | (TempBuffer[1] & 0xFF); // REG_STATUS = 0x3FFF(DEF.ADDR)
      Usart_Send_Twobyte(MLX632_Data[0]);
      
//      MLX632_Data[1] = ((TempBuffer[2] & 0xFF) << 8) | (TempBuffer[3] & 0xFF); // RAM1 = 0x4000(DEF.ADDR + 1)
//      Usart_Send_Twobyte(MLX632_Data[1]);
      Usart_Send_Twobyte(0x5555);
      
//      MLX632_Data[2] = ((TempBuffer[4] & 0xFF) << 8) | (TempBuffer[5] & 0xFF); // RAM2 = 0x4001(DEF.ADDR + 2)
//      Usart_Send_Twobyte(MLX632_Data[2]);
      Usart_Send_Twobyte(0x5555);
      
//      MLX632_Data[3] = ((TempBuffer[6] & 0xFF) << 8) | (TempBuffer[7] & 0xFF); // RAM3 = 0x4002(DEF.ADDR + 3)
//      Usart_Send_Twobyte(MLX632_Data[3]);
      Usart_Send_Twobyte(0x5555);
      
      MLX632_Data[4] = (0xFF << 8) | (TempBuffer[9] & 0xFF); // RAM4 = 0x4003(DEF.ADDR + 4)
      Usart_Send_Twobyte(MLX632_Data[4]);
      
      MLX632_Data[5] = (0xFF << 8) | (TempBuffer[11] & 0xFF); // RAM5 = 0x4004(DEF.ADDR + 5)
      Usart_Send_Twobyte(MLX632_Data[5]);
      
      MLX632_Data[6] = ((TempBuffer[12] & 0xFF) << 8) | (TempBuffer[13] & 0xFF); // RAM6 = 0x4005(DEF.ADDR + 6)
      Usart_Send_Twobyte(MLX632_Data[6]);
      
      MLX632_Data[7] = (0xFF << 8) | (TempBuffer[15] & 0xFF); // RAM7 = 0x4006(DEF.ADDR + 7)
      Usart_Send_Twobyte(MLX632_Data[7]);
      
      MLX632_Data[8] = (0xFF << 8) | (TempBuffer[17] & 0xFF); // RAM8 = 0x4007(DEF.ADDR + 8)
      Usart_Send_Twobyte(MLX632_Data[8]);
      
      MLX632_Data[9] = ((TempBuffer[18] & 0xFF) << 8) | (TempBuffer[19] & 0xFF); // RAM9 = 0x4008(DEF.ADDR + 9)   
      Usart_Send_Twobyte(MLX632_Data[9]);
     
//      MLX632_Data[10] = ((TempBuffer[20] & 0xFF) << 8) | (TempBuffer[21] & 0xFF); // leaves the address space
      
      for(unsigned char j = 0; j < 22; j++) TempBuffer[j] = 0;
      
      return 0;
  }
  else
      return 1;
}
