/*
 *File: MPU9250.c
 *Description: MLX9250 drive 
 */

#include <xc.h>
#include "uart.h"
#include <math.h>

/************************************************************************************/
#define MPU9250_ADDR            (0X68 << 1)
#define MPU6500_ID				0X71
#define MPU_DEVICE_ID_REG		0X75

#define AK8963_ADDR				(0X0C << 1)
#define AK8963_ID				0X48
    
#define MPU_GYRO_CFG_REG		0X1B
#define MPU_ACCEL_CFG_REG		0X1C
#define MPU_CFG_REG				0X1A
#define MPU_SAMPLE_RATE_REG		0X19
    
#define MPU6050_GYRO_OUT        0x43
#define MPU6050_ACC_OUT         0x3B
#define MPU_TEMP_OUTH_REG		0X41
#define MAG_XOUT_L				0X03	
#define MAG_CNTL1          	  	0X0A 
#define MAG_CNTL2            	0X0B
    
#define MPU_PWR_MGMT1_REG		0X6B
#define MPU_PWR_MGMT2_REG		0X6C
#define MPU_INT_EN_REG			0X38
#define MPU_USER_CTRL_REG		0X6A
#define MPU_FIFO_EN_REG			0X23
#define MPU_INTBP_CFG_REG		0X37

#define MAG_WIA					0x00
/************************************************************************************/
    /*MEMS_SEN*/
#define MEMS_SDA_IN()            TRISCbits.TRISC3 = 1;   // clr 0 == input
#define MEMS_SDA_OUT()           TRISCbits.TRISC3 = 0;   // set 1 == output
#define MEMS_SCL_OUT()           TRISCbits.TRISC6 = 0;

#define MEMS_SDA_HIGH()          PORTCbits.RC3 = 1;   // Sets SDA line
#define MEMS_SDA_LOW()           PORTCbits.RC3 = 0;  // Clears SDA line
#define MEMS_SDA                 PORTCbits.RC3

#define MEMS_SCL_HIGH()          PORTCbits.RC6 = 1;  // Sets SCL line
#define MEMS_SCL_LOW()           PORTCbits.RC6 = 0;  // Clears SCL line

extern void Delay_ms(unsigned int ms);
/************************************************************************************/
//static void MEMS_i2c_Delay(void);
//static unsigned char MEMS_SDA_ReadBit(void);
//static void MEMS_i2c_Start(void);
//static void MEMS_i2c_Stop(void);
//static void MEMS_i2c_Ack(void);
//static void MEMS_i2c_NAck(void);
//static void MEMS_i2c_SendByte(unsigned char _ucByte);
//static unsigned char MEMS_i2c_ReadByte(unsigned char ack);
//static unsigned char MEMS_i2c_WaitAck(void);
//static void MEMS_I2C_GPIO_Init(void);
//static void MPU6050_WriteReg(unsigned char reg_add,unsigned char reg_dat);
//static void MPU6050_ReadData(unsigned char reg_add, unsigned char*Read, unsigned char num);
//static void AK8963_WriteReg(unsigned char reg_add,unsigned char reg_dat);
//static void AK8963_ReadData(unsigned char reg_add, unsigned char*Read, unsigned char num);
//static void MPU_Set_Gyro_Fsr(unsigned char fsr);
//static void MPU_Set_Accel_Fsr(unsigned char fsr);
//static void MPU_Set_LPF(unsigned short lpf);
//static void MPU_Set_Rate(unsigned short rate);

static void MPU6050ReadAcc(unsigned int *accData);
static void MPU6050ReadGyro(unsigned int *gyroData);
static void MPU6050_ReturnTemp(unsigned int *Temperature);
static void AK8963_ReadMagnetometer(unsigned int *magnetData);

unsigned int AHRVal[10] = {0};
/***************************************************************************************/

static void MEMS_i2c_Delay(void) // 170KHz
{
    NOP();
}

static unsigned char MEMS_SDA_ReadBit()
{
    unsigned char bitVal;
    
    if(MEMS_SDA) // Read bit, save it in bit
        bitVal=1;
    else
        bitVal=0;
    
    return bitVal;
}

static void MEMS_i2c_Start(void)
{
    MEMS_SDA_OUT();
	MEMS_SDA_HIGH();
    MEMS_i2c_Delay();
	MEMS_SCL_HIGH();
	MEMS_i2c_Delay();
	MEMS_SDA_LOW();
	MEMS_i2c_Delay();
	MEMS_SCL_LOW();
	MEMS_i2c_Delay();
}

static void MEMS_i2c_Stop(void)
{
    MEMS_SDA_OUT();
    MEMS_SCL_LOW();
    MEMS_i2c_Delay();
	MEMS_SDA_LOW();
    MEMS_i2c_Delay();
	MEMS_SCL_HIGH();
	MEMS_i2c_Delay();
	MEMS_SDA_HIGH();
}

static void MEMS_i2c_Ack(void)
{
    MEMS_SCL_LOW();
    MEMS_SDA_OUT();
    MEMS_i2c_Delay();
	MEMS_SDA_LOW();
	MEMS_i2c_Delay();
	MEMS_SCL_HIGH();
	MEMS_i2c_Delay();
	MEMS_SCL_LOW();
}

static void MEMS_i2c_NAck(void)
{
    MEMS_SCL_LOW();
    MEMS_SDA_OUT();
    MEMS_i2c_Delay();
	MEMS_SDA_HIGH();
	MEMS_i2c_Delay();
	MEMS_SCL_HIGH();
	MEMS_i2c_Delay();
	MEMS_SCL_LOW();
}

static void MEMS_i2c_SendByte(unsigned char _ucByte)
{
	unsigned char i;
    MEMS_SDA_OUT();
    MEMS_SCL_LOW();
    MEMS_i2c_Delay();

	for (i = 0; i < 8; i++)
	{		
		if (((_ucByte & 0x80) >> 7) & 0x01)
		{
			MEMS_SDA_HIGH();
		}
		else
		{
			MEMS_SDA_LOW();
		}
        
        _ucByte <<= 1;       
		
		MEMS_SCL_HIGH();
		MEMS_i2c_Delay();	
		MEMS_SCL_LOW();
        MEMS_i2c_Delay();
	}
}

static unsigned char MEMS_i2c_ReadByte(unsigned char ack)
{
	unsigned char i;
	unsigned char value = 0;
    unsigned char nbit;
    
    MEMS_SDA_IN();
    MEMS_i2c_Delay();

	for (i = 0; i < 8; i++)
	{
        MEMS_SCL_LOW();
		MEMS_i2c_Delay();
        MEMS_SCL_HIGH();
        
		value <<= 1;
        
        nbit = MEMS_SDA_ReadBit();
        
        if (nbit)
		{
			value++;
		}
    }

	if(ack == 0)
		MEMS_i2c_NAck();
	else
		MEMS_i2c_Ack();
    
	return value;
}

static unsigned char MEMS_i2c_WaitAck(void)
{
    unsigned char re;
    unsigned char ucErrTime = 0;

    MEMS_SDA_IN(); // SDA-input
    MEMS_SCL_HIGH();
    MEMS_i2c_Delay();
    
    re = MEMS_SDA_ReadBit();

    while (re)
    {
        ucErrTime++;

        if(ucErrTime > 250)
        {
            MEMS_i2c_Stop();
            return 1;
        } 

        MEMS_i2c_Delay();
        re = MEMS_SDA_ReadBit();
    }

    MEMS_SCL_LOW();

    return 0;    
}

static void MEMS_I2C_GPIO_Init(void)
{   
    MEMS_SCL_OUT();
    MEMS_SCL_HIGH();
    MEMS_SDA_OUT();
    MEMS_SDA_HIGH();
}

static void MPU6050_WriteReg(unsigned char reg_add,unsigned char reg_dat)
{
    MEMS_i2c_Start();
	MEMS_i2c_SendByte(MPU9250_ADDR);
	MEMS_i2c_WaitAck();
	MEMS_i2c_SendByte(reg_add);
	MEMS_i2c_WaitAck();
	MEMS_i2c_SendByte(reg_dat);
	MEMS_i2c_WaitAck();
	MEMS_i2c_Stop();
}

static void MPU6050_ReadData(unsigned char reg_add, unsigned char*Read, unsigned char num)
{
	unsigned char i;
	MEMS_i2c_Start();
	MEMS_i2c_SendByte(MPU9250_ADDR);
	MEMS_i2c_WaitAck();
	MEMS_i2c_SendByte(reg_add);
	MEMS_i2c_WaitAck();
	
	MEMS_i2c_Start();
	MEMS_i2c_SendByte(MPU9250_ADDR + 1);
	MEMS_i2c_WaitAck();
	
	for(i=0;i<(num-1);i++)
    {
		*Read = MEMS_i2c_ReadByte(1);
		Read++;
	}
	*Read = MEMS_i2c_ReadByte(0);
	MEMS_i2c_Stop();
}

static void AK8963_WriteReg(unsigned char reg_add,unsigned char reg_dat)
{
	MEMS_i2c_Start();
	MEMS_i2c_SendByte(AK8963_ADDR);
	MEMS_i2c_WaitAck();
	MEMS_i2c_SendByte(reg_add);
	MEMS_i2c_WaitAck();
	MEMS_i2c_SendByte(reg_dat);
	MEMS_i2c_WaitAck();
	MEMS_i2c_Stop();
}

static void AK8963_ReadData(unsigned char reg_add, unsigned char*Read, unsigned char num)
{
	unsigned char i;
	
	MEMS_i2c_Start();
	MEMS_i2c_SendByte(AK8963_ADDR);
	MEMS_i2c_WaitAck();
	MEMS_i2c_SendByte(reg_add);
	MEMS_i2c_WaitAck();
	
	MEMS_i2c_Start();
	MEMS_i2c_SendByte(AK8963_ADDR+1);
	MEMS_i2c_WaitAck();
	
	for(i=0;i<(num-1);i++)
    {
		*Read = MEMS_i2c_ReadByte(1);
		Read++;
	}
	*Read = MEMS_i2c_ReadByte(0);
	MEMS_i2c_Stop();
}

//Set MPU9250 Gyro sensor Scale
//fsr:0,?1?70dps;1,?1?70dps;2,?1?700dps;3,?1?700dps
//return: 0, success
//        other, error
static void MPU_Set_Gyro_Fsr(unsigned char fsr)
{
    MPU6050_WriteReg(MPU_GYRO_CFG_REG,(fsr<<3)|3);

}
//Set MPU9250 Accelerometer Scale
//fsr:0,?1?7;1,?1?7;2,?1?7;3,?1?7g
//return: 0, success
//        other, error
static void MPU_Set_Accel_Fsr(unsigned char fsr)
{
    MPU6050_WriteReg(MPU_ACCEL_CFG_REG,fsr<<3);
}

//Set MPU9250 LPF
//lpf:LPF Frequency(Hz)
//return: 0, success
//        other, error
static void MPU_Set_LPF(unsigned short lpf)
{
	unsigned char data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
    MPU6050_WriteReg(MPU_CFG_REG,data);
}

//Set MPU9250 Sampling Rate(supposition Fs = 1KHz)
//rate:4~1000(Hz)
//return: 0, success
//        other, error
static void MPU_Set_Rate(unsigned short rate)
{
	unsigned char data;
	if(rate>1000) rate=1000;
	if(rate<4) rate=4;
	data=(unsigned char)((unsigned short)(1000)/(rate-1));
    MPU6050_WriteReg(MPU_SAMPLE_RATE_REG,data);
 	MPU_Set_LPF(rate/2);	//Auto set LPF as half of sampling rate
}

unsigned char MPU9250_Init(void)
{
    unsigned char Re, Ree;
    MEMS_I2C_GPIO_Init();     //I2C Initialize
    
    //Reset MPU9250
	MPU6050_WriteReg(MPU_PWR_MGMT1_REG,0X80); // 0x6B
    Delay_ms(100);
	
	//Wake up MPU9250
	MPU6050_WriteReg(MPU_PWR_MGMT1_REG,0X00); // 0x6B
	
    MPU_Set_Gyro_Fsr(3);					        	//Gyroscope,?1?700dps
	MPU_Set_Accel_Fsr(3);					       	 	//Accelerometer,±À2g
    MPU_Set_Rate(50);						       	 	//Sampling Rate 50Hz
	
	//Disable Interrupt
	MPU6050_WriteReg(MPU_INT_EN_REG,0X00);  // 0x38
	
	//Disable I2C Master Mode 
	MPU6050_WriteReg(MPU_USER_CTRL_REG,0X00); // 0x6A
	
	//Disable FIFO
	MPU6050_WriteReg(MPU_FIFO_EN_REG,0X00); // 0x23
	
	//Enable INT Pin low level,bypass mode,enable directly access magnetic sensor 
	MPU6050_WriteReg(MPU_INTBP_CFG_REG,0X82); // 0x37
    
    //Get MPU6050 ID
    MPU6050_ReadData(MPU_DEVICE_ID_REG, &Re, 1);

    if(Re == MPU6500_ID)
        {
            //Set CLKSEL,PLL X-axis parameter
            MPU6050_WriteReg(MPU_PWR_MGMT1_REG,0X01); // 0x6B
            //Work both of Gyro and Accel
            MPU6050_WriteReg(MPU_PWR_MGMT2_REG,0X00); // 0x6C	
            //Set Sampling Rate 50Hz   
            MPU_Set_Rate(50); 

//            Usart_Sendbytes("MPU6050_OK!\n");
        }
    else
        return 1;
	 
    //Get AK8963 ID
     AK8963_ReadData(MAG_WIA, &Ree, 1);
     
     if(Ree == AK8963_ID)
     {
        //Reset AK8963
        AK8963_WriteReg(MAG_CNTL2,0X01);	
        
        Delay_ms(100);		
        //Set AK8963 one capture
        AK8963_WriteReg(MAG_CNTL1,0X11); 
        
//        Usart_Sendbytes("AK8963_OK!\n");
     }
     else 
         return 1;
     
     return 0;
}

static void MPU6050ReadAcc(unsigned int *accData)
{
    unsigned char buf[6];
    MPU6050_ReadData(MPU6050_ACC_OUT, buf, 6);
    accData[0] = (buf[0] << 8) | buf[1];
    accData[1] = (buf[2] << 8) | buf[3];
    accData[2] = (buf[4] << 8) | buf[5];
}

static void MPU6050ReadGyro(unsigned int *gyroData)
{
    unsigned char buf[6];
    MPU6050_ReadData(MPU6050_GYRO_OUT,buf,6);
    gyroData[0] = (buf[0] << 8) | buf[1];
    gyroData[1] = (buf[2] << 8) | buf[3];
    gyroData[2] = (buf[4] << 8) | buf[5];
}

//void MPU6050ReadTemp(short tempData)
//{
//	unsigned char buf[2];
//    MPU6050_ReadData(MPU_TEMP_OUTH_REG,buf,2);
//    tempData = (buf[0] << 8) | buf[1];
//}

static void MPU6050_ReturnTemp(unsigned int *Temperature)
{
	short temp3;
	unsigned char buf[2];
	
	MPU6050_ReadData(MPU_TEMP_OUTH_REG,buf,2);
    temp3= (buf[0] << 8) | buf[1];	
	*Temperature = ((unsigned int) (temp3/333.87))+21;
}

static void AK8963_ReadMagnetometer(unsigned int *magnetData)
{
   unsigned char buf[6];  
	
 	AK8963_ReadData(MAG_XOUT_L,buf, 6);

    magnetData[0]=((unsigned int)buf[1]<<8)|buf[0];  
    magnetData[1]=((unsigned int)buf[3]<<8)|buf[2];  
    magnetData[2]=((unsigned int)buf[5]<<8)|buf[4];
	 
	AK8963_WriteReg(MAG_CNTL1,0X11);
}

void MPU9250_Measuring(void)
{
    unsigned int accVal[3];
    unsigned int gyroVal[3];
//    unsigned int magVal[3];
//    unsigned int tempVal;

    MPU6050ReadAcc(accVal);
    AHRVal[0] = accVal[0];
    Usart_Send_Twobyte(AHRVal[0]);

    AHRVal[1] = accVal[1];
    Usart_Send_Twobyte(AHRVal[1]);

    AHRVal[2] = accVal[2];
    Usart_Send_Twobyte(AHRVal[2]);

    
    MPU6050ReadGyro(gyroVal);
    AHRVal[3] = gyroVal[0];
    Usart_Send_Twobyte(AHRVal[3]);

    AHRVal[4] = gyroVal[1];
    Usart_Send_Twobyte(AHRVal[4]);

    AHRVal[5] = gyroVal[2];
    Usart_Send_Twobyte(AHRVal[5]);

    
//    AK8963_ReadMagnetometer(magVal);
//    AHRVal[6] = magVal[0];
//    Usart_Send_Twobyte(AHRVal[6]);
    Usart_Send_Twobyte(0x0000);

//    AHRVal[7] = magVal[1];
//    Usart_Send_Twobyte(AHRVal[7]);
    Usart_Send_Twobyte(0x0000);

//    AHRVal[8] = magVal[2];
//    Usart_Send_Twobyte(AHRVal[8]);
    Usart_Send_Twobyte(0x0000);
    
//    MPU6050_ReturnTemp(&tempVal);
//    AHRVal[9] = tempVal;
//    Usart_Send_Twobyte(AHRVal[9]);
    Usart_Send_Twobyte(0x0000);
}