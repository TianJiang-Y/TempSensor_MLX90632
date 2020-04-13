
/*
 *File: I2C_GPIO.c
 *Description: I2C drive with GPIO 
 */
#include <stdbool.h>
#include <xc.h>
#include <plib/delays.h>

/********************************Define I2C index ID***************************/
#define T_SEN1_I2C_ID            1
#define T_SEN2_I2C_ID            2
#define T_SEN3_I2C_ID            3
#define T_SEN4_I2C_ID            4
/*****************************************************************************/    
/*T_SEN1*/
#define T_SEN1_SDA_INPUT()     TRISBbits.TRISB4 = 1;   // clr 0 == input
#define T_SEN1_SDA_OUTPUT()    TRISBbits.TRISB4 = 0;   // set 1 == output
#define T_SEN1_SCL_OUTPUT()    TRISBbits.TRISB6 = 0;
#define T_SEN1_SDA_HIGH()      PORTBbits.RB4= 1;   // Sets SDA line
#define T_SEN1_SDA_LOW()       PORTBbits.RB4 = 0;  // Clears SDA line
#define T_SEN1_SCL_HIGH()      PORTBbits.RB6 = 1;  // Sets SCL line
#define T_SEN1_SCL_LOW()       PORTBbits.RB6 = 0;  // Clears SCL line
#define T_SEN1_SDA             PORTBbits.RB4
    
/*T_SEN2*/
#define T_SEN2_SDA_INPUT()      TRISCbits.TRISC1 = 1;   // clr 0 == input
#define T_SEN2_SDA_OUTPUT()     TRISCbits.TRISC1 = 0;   // set 1 == output
#define T_SEN2_SCL_OUTPUT()    TRISCbits.TRISC2 = 0;
#define T_SEN2_SDA_HIGH()      PORTCbits.RC1= 1;   // Sets SDA line
#define T_SEN2_SDA_LOW()       PORTCbits.RC1 = 0;  // Clears SDA line
#define T_SEN2_SCL_HIGH()      PORTCbits.RC2 = 1;  // Sets SCL line
#define T_SEN2_SCL_LOW()       PORTCbits.RC2 = 0;  // Clears SCL line
#define T_SEN2_SDA             PORTCbits.RC1
    
/*T_SEN3*/
#define T_SEN3_SDA_INPUT()     TRISCbits.TRISC4 = 1;   // clr 0 == input
#define T_SEN3_SDA_OUTPUT()    TRISCbits.TRISC4= 0;   // set 1 == output
#define T_SEN3_SCL_OUTPUT()    TRISCbits.TRISC5 = 0;
#define T_SEN3_SDA_HIGH()      PORTCbits.RC4= 1;   // Sets SDA line
#define T_SEN3_SDA_LOW()       PORTCbits.RC4 = 0;  // Clears SDA line
#define T_SEN3_SCL_HIGH()      PORTCbits.RC5 = 1;  // Sets SCL line
#define T_SEN3_SCL_LOW()       PORTCbits.RC5 = 0;  // Clears SCL line
#define T_SEN3_SDA             PORTCbits.RC4

/*T_SEN4*/
#define T_SEN4_SDA_INPUT()     TRISAbits.TRISA4 = 1;   // clr 0 == input
#define T_SEN4_SDA_OUTPUT()    TRISAbits.TRISA4 = 0;   // set 1 == output
#define T_SEN4_SCL_OUTPUT()    TRISAbits.TRISA5 = 0;
#define T_SEN4_SDA_HIGH()      PORTAbits.RA4= 1;   // Sets SDA line
#define T_SEN4_SDA_LOW()       PORTAbits.RA4 = 0;  // Clears SDA line
#define T_SEN4_SCL_HIGH()      PORTAbits.RA5 = 1;  // Sets SCL line
#define T_SEN4_SCL_LOW()       PORTAbits.RA5 = 0;  // Clears SCL line
#define T_SEN4_SDA             PORTAbits.RA4

/*************************Define Delay Parameter*******************************/
#define TCYBUF                              3
/******************************************************************************/

/***********************************Define Function************************************/

/**********************************************************************************************/
static void IIC_Delay(void)
{
//    NOP();
    Delay1TCY();
}

unsigned char GPIO_ReadBit(unsigned char I2C_Index)
{
    unsigned char bitVal;
    unsigned char tempval;
    
    switch(I2C_Index)
        {
            case T_SEN1_I2C_ID:
                tempval = T_SEN1_SDA; // SEN1_SDA
                break;
            case T_SEN2_I2C_ID:
                tempval = T_SEN2_SDA; // SEN2_SDA
                break;
            case T_SEN3_I2C_ID:
                tempval = T_SEN3_SDA; // SEN3_SDA
                break;
            case T_SEN4_I2C_ID:
                tempval = T_SEN4_SDA; // SEN4_SDA
                break;
            default:
                break;
        }
    if(tempval) // Read bit, save it in bit
        bitVal=1;
    else
        bitVal=0;
    
    return bitVal;
}

void IIC_Start(unsigned char I2C_Index)
{
    switch(I2C_Index)
    {
        case T_SEN1_I2C_ID:
              T_SEN1_SDA_OUTPUT();
              T_SEN1_SDA_HIGH();
              IIC_Delay();
              T_SEN1_SCL_HIGH();
              IIC_Delay();
              T_SEN1_SDA_LOW();
              IIC_Delay();
              T_SEN1_SCL_LOW();
              IIC_Delay();
              break;
        case T_SEN2_I2C_ID:
              T_SEN2_SDA_OUTPUT();
              T_SEN2_SDA_HIGH();
              IIC_Delay();
              T_SEN2_SCL_HIGH();
              IIC_Delay();
              T_SEN2_SDA_LOW();
              IIC_Delay();
              T_SEN2_SCL_LOW();
              IIC_Delay();
              break;
        case T_SEN3_I2C_ID:
              T_SEN3_SDA_OUTPUT();
              T_SEN3_SDA_HIGH();
              IIC_Delay();
              T_SEN3_SCL_HIGH();
              IIC_Delay();
              T_SEN3_SDA_LOW();
              IIC_Delay();
              T_SEN3_SCL_LOW();
              IIC_Delay();
              break;
        case T_SEN4_I2C_ID:
              T_SEN4_SDA_OUTPUT();
              T_SEN4_SDA_HIGH();
              IIC_Delay();
              T_SEN4_SCL_HIGH();
              IIC_Delay();
              T_SEN4_SDA_LOW();
              IIC_Delay();
              T_SEN4_SCL_LOW();
              IIC_Delay();
              break;
        default:
            break;
    }
}

void IIC_Stop(unsigned char I2C_Index)
{
    switch(I2C_Index)
    {
        case T_SEN1_I2C_ID:
                T_SEN1_SDA_OUTPUT();                
                T_SEN1_SCL_LOW();
                IIC_Delay();
                T_SEN1_SDA_LOW();
                IIC_Delay();
                T_SEN1_SCL_HIGH();
                IIC_Delay();
                T_SEN1_SDA_HIGH();
                break;
        case T_SEN2_I2C_ID:
                T_SEN2_SDA_OUTPUT();                
                T_SEN2_SCL_LOW();
                IIC_Delay();
                T_SEN2_SDA_LOW();
                IIC_Delay();
                T_SEN2_SCL_HIGH();
                IIC_Delay();
                T_SEN2_SDA_HIGH();
                break;
        case T_SEN3_I2C_ID:
                T_SEN3_SDA_OUTPUT();                
                T_SEN3_SCL_LOW();
                IIC_Delay();
                T_SEN3_SDA_LOW();
                IIC_Delay();
                T_SEN3_SCL_HIGH();
                IIC_Delay();
                T_SEN3_SDA_HIGH();
                break;
        case T_SEN4_I2C_ID:
                T_SEN4_SDA_OUTPUT();                
                T_SEN4_SCL_LOW();
                IIC_Delay();
                T_SEN4_SDA_LOW();
                IIC_Delay();
                T_SEN4_SCL_HIGH();
                IIC_Delay();
                T_SEN4_SDA_HIGH();
                break;
        default:
            break;
    }    
}

void IIC_Ack(unsigned char I2C_Index)
{
    switch(I2C_Index)
    {
        case T_SEN1_I2C_ID:
                T_SEN1_SCL_LOW();
                T_SEN1_SDA_OUTPUT();                
                IIC_Delay();
                T_SEN1_SDA_LOW();
                IIC_Delay();
                T_SEN1_SCL_HIGH();
                IIC_Delay();
                T_SEN1_SCL_LOW();
                break;
        case T_SEN2_I2C_ID:
                T_SEN2_SCL_LOW();
                T_SEN2_SDA_OUTPUT();                
                IIC_Delay();
                T_SEN2_SDA_LOW();
                IIC_Delay();
                T_SEN2_SCL_HIGH();
                IIC_Delay();
                T_SEN2_SCL_LOW();
                break;
        case T_SEN3_I2C_ID:
                T_SEN3_SCL_LOW();
                T_SEN3_SDA_OUTPUT();                
                IIC_Delay();
                T_SEN3_SDA_LOW();
                IIC_Delay();
                T_SEN3_SCL_HIGH();
                IIC_Delay();
                T_SEN3_SCL_LOW();
                break;
        case T_SEN4_I2C_ID:
                T_SEN4_SCL_LOW();
                T_SEN4_SDA_OUTPUT();                
                IIC_Delay();
                T_SEN4_SDA_LOW();
                IIC_Delay();
                T_SEN4_SCL_HIGH();
                IIC_Delay();
                T_SEN4_SCL_LOW();
                break;
        default:
            break;
    }    
}

void IIC_NAck(unsigned char I2C_Index)
{
    switch(I2C_Index)
    {
        case T_SEN1_I2C_ID:
                T_SEN1_SCL_LOW();
                T_SEN1_SDA_OUTPUT();
                IIC_Delay();
                T_SEN1_SDA_HIGH();
                IIC_Delay();
                T_SEN1_SCL_HIGH();
                IIC_Delay();
                T_SEN1_SCL_LOW();
                break;
        case T_SEN2_I2C_ID:
                T_SEN2_SCL_LOW();
                T_SEN2_SDA_OUTPUT();
                IIC_Delay();
                T_SEN2_SDA_HIGH();
                IIC_Delay();
                T_SEN2_SCL_HIGH();
                IIC_Delay();
                T_SEN2_SCL_LOW();
                break;
        case T_SEN3_I2C_ID:
                T_SEN3_SCL_LOW();
                T_SEN3_SDA_OUTPUT();
                IIC_Delay();
                T_SEN3_SDA_HIGH();
                IIC_Delay();
                T_SEN3_SCL_HIGH();
                IIC_Delay();
                T_SEN3_SCL_LOW();
                break;
        case T_SEN4_I2C_ID:
                T_SEN4_SCL_LOW();
                T_SEN4_SDA_OUTPUT();
                IIC_Delay();
                T_SEN4_SDA_HIGH();
                IIC_Delay();
                T_SEN4_SCL_HIGH();
                IIC_Delay();
                T_SEN4_SCL_LOW();
                break;
        default:
            break;
    }    
}

void IIC_Send_Byte(unsigned char I2C_Index, unsigned char  data)
{
    unsigned char i;
    
    switch(I2C_Index)
    {
        case T_SEN1_I2C_ID:
                T_SEN1_SDA_OUTPUT();
                T_SEN1_SCL_LOW();
                IIC_Delay();
                
                for (i = 0; i < 8; i++)
                {		
                    if (((data & 0x80) >> 7) & 0x01)
                    {
                        T_SEN1_SDA_HIGH();
                    }
                    else
                    {
                        T_SEN1_SDA_LOW();
                    }
                    
                    data <<= 1;                     
                    
                    T_SEN1_SCL_HIGH();
                    IIC_Delay();
                    T_SEN1_SCL_LOW();
                    IIC_Delay();
                }
                break;
        case T_SEN2_I2C_ID:
                T_SEN2_SDA_OUTPUT();
                T_SEN2_SCL_LOW();
                IIC_Delay();
                
                for (i = 0; i < 8; i++)
                {		
                    if (((data & 0x80) >> 7) & 0x01)
                    {
                        T_SEN2_SDA_HIGH();
                    }
                    else
                    {
                        T_SEN2_SDA_LOW();
                    }
                    
                    data <<= 1;
                    
                    T_SEN2_SCL_HIGH();
                    IIC_Delay();
                    T_SEN2_SCL_LOW();
                    IIC_Delay();
                }
                break;
        case T_SEN3_I2C_ID:
                T_SEN3_SDA_OUTPUT();
                T_SEN3_SCL_LOW();
                IIC_Delay();
                
                for (i = 0; i < 8; i++)
                {		
                    if (((data & 0x80) >> 7) & 0x01)
                    {
                        T_SEN3_SDA_HIGH();
                    }
                    else
                    {
                        T_SEN3_SDA_LOW();
                    }
                    
                    data <<= 1;                     
                    
                    T_SEN3_SCL_HIGH();
                    IIC_Delay();
                    T_SEN3_SCL_LOW();
                    IIC_Delay();
                }
                break;
        case T_SEN4_I2C_ID:
                T_SEN4_SDA_OUTPUT();
                T_SEN4_SCL_LOW();
                IIC_Delay();
                
                for (i = 0; i < 8; i++)
                {		
                    if (((data & 0x80) >> 7) & 0x01)
                    {
                        T_SEN4_SDA_HIGH();
                    }
                    else
                    {
                        T_SEN4_SDA_LOW();
                    }
                    
                    data <<= 1;                     
                    
                    T_SEN4_SCL_HIGH();
                    IIC_Delay();
                    T_SEN4_SCL_LOW();
                    IIC_Delay();
                }
                break;
        default:
            break;
    }
}

unsigned char IIC_Read_Byte(unsigned char I2C_Index,unsigned char ack)
{
    unsigned char i, receive = 0;
    unsigned char nbit;
    unsigned char bytes = 0;

    switch(I2C_Index)
    {
        case T_SEN1_I2C_ID:
                T_SEN1_SDA_INPUT(); // SDA-input
                IIC_Delay();
                
                for (i = 0; i < 8; i++)
                { 
                    T_SEN1_SCL_LOW();
                    IIC_Delay();
                    T_SEN1_SCL_HIGH();

                    receive <<= 1;
                    nbit = GPIO_ReadBit(T_SEN1_I2C_ID);
                    if (nbit) 
                        receive++;
                }

                if (!ack) 
                    IIC_NAck(T_SEN1_I2C_ID);
                else 
                    IIC_Ack(T_SEN1_I2C_ID); 
                bytes = receive;
                break;
        case T_SEN2_I2C_ID:
                T_SEN2_SDA_INPUT(); // SDA-input
                IIC_Delay();
                
                for (i = 0; i < 8; i++)
                { 
                    T_SEN2_SCL_LOW();
                    IIC_Delay();
                    T_SEN2_SCL_HIGH();

                    receive <<= 1;
                    nbit = GPIO_ReadBit(T_SEN2_I2C_ID);
                    if (nbit) 
                        receive++;            
                }

                if (!ack) 
                    IIC_NAck(T_SEN2_I2C_ID);
                else 
                    IIC_Ack(T_SEN2_I2C_ID); 
                bytes = receive;
                break;
        case T_SEN3_I2C_ID:
                T_SEN3_SDA_INPUT(); // SDA-input
                IIC_Delay();
                
                for (i = 0; i < 8; i++)
                { 
                    T_SEN3_SCL_LOW();
                    IIC_Delay();
                    T_SEN3_SCL_HIGH();

                    receive <<= 1;
                    nbit = GPIO_ReadBit(T_SEN3_I2C_ID);
                    if (nbit) 
                        receive++;            
                }

                if (!ack) 
                    IIC_NAck(T_SEN3_I2C_ID);
                else 
                    IIC_Ack(T_SEN3_I2C_ID); 
                bytes = receive;
                break;
        case T_SEN4_I2C_ID:
                T_SEN4_SDA_INPUT(); // SDA-input
                IIC_Delay();
                
                for (i = 0; i < 8; i++)
                { 
                    T_SEN4_SCL_LOW();
                    IIC_Delay();
                    T_SEN4_SCL_HIGH();

                    receive <<= 1;
                    nbit = GPIO_ReadBit(T_SEN4_I2C_ID);
                    if (nbit) 
                        receive++;            
                }

                if (!ack) 
                    IIC_NAck(T_SEN4_I2C_ID);
                else 
                    IIC_Ack(T_SEN4_I2C_ID); 
                bytes = receive;
                break;
        default:
            break;
    }
    
    return bytes;
}

unsigned char IIC_Wait_Ack(unsigned char I2C_Index)
{
    unsigned char nbit = 0;
    unsigned char ucErrTime = 0;
    
    if(I2C_Index == T_SEN1_I2C_ID)
    {
        T_SEN1_SDA_INPUT(); // SDA-input
        T_SEN1_SCL_HIGH();
        IIC_Delay();
        
        nbit = GPIO_ReadBit(T_SEN1_I2C_ID);

        while (nbit)
        {
            ucErrTime++;

            if(ucErrTime > 250)
            {
                IIC_Stop(T_SEN1_I2C_ID);
                return 1;
            } 

            IIC_Delay();
            nbit = GPIO_ReadBit(T_SEN1_I2C_ID);
        }

        T_SEN1_SCL_LOW();

        return 0;  
     }
    
    else if(I2C_Index == T_SEN2_I2C_ID)
    {
        T_SEN2_SDA_INPUT(); // SDA-input
        T_SEN2_SCL_HIGH();
        IIC_Delay();
        nbit = GPIO_ReadBit(T_SEN2_I2C_ID);

        while (nbit)
        {
            ucErrTime++;

            if(ucErrTime > 250)
            {
                IIC_Stop(T_SEN2_I2C_ID);
                return 1;
            } 

            IIC_Delay();
            nbit = GPIO_ReadBit(T_SEN2_I2C_ID);
        }

        T_SEN2_SCL_LOW();

        return 0;  
     }
    
    else if(I2C_Index == T_SEN3_I2C_ID)
    {
        T_SEN3_SDA_INPUT(); // SDA-input
        T_SEN3_SCL_HIGH();
        IIC_Delay();
        
        nbit = GPIO_ReadBit(T_SEN3_I2C_ID);

        while (nbit)
        {
            ucErrTime++;

            if(ucErrTime > 250)
            {
                IIC_Stop(T_SEN3_I2C_ID);
                return 1;
            } 

            IIC_Delay();
            nbit = GPIO_ReadBit(T_SEN3_I2C_ID);
        }

        T_SEN3_SCL_LOW();

        return 0;  
     
    }
    
    else if(I2C_Index == T_SEN4_I2C_ID)
    {
        T_SEN4_SDA_INPUT(); // SDA-input
        T_SEN4_SCL_HIGH();
        IIC_Delay();
        nbit = GPIO_ReadBit(T_SEN4_I2C_ID);

        while (nbit)
        {
            ucErrTime++;

            if(ucErrTime > 250)
            {
                IIC_Stop(T_SEN4_I2C_ID);
                return 1;
            } 

            IIC_Delay();
            nbit = GPIO_ReadBit(T_SEN4_I2C_ID);
        }

        T_SEN4_SCL_LOW();

        return 0;  
        
    }
    else
        return 0;
}

void Total_TSen_I2C_Init(void)
{
    T_SEN1_SCL_OUTPUT();
    T_SEN1_SCL_HIGH();
    T_SEN1_SDA_OUTPUT();
    T_SEN1_SDA_HIGH();
    
    T_SEN2_SCL_OUTPUT();
    T_SEN2_SCL_HIGH();
    T_SEN2_SDA_OUTPUT();
    T_SEN2_SDA_HIGH();
    
    T_SEN2_SCL_OUTPUT();
    T_SEN2_SCL_HIGH();
    T_SEN2_SDA_OUTPUT();
    T_SEN2_SDA_HIGH();
    
    T_SEN3_SCL_OUTPUT();
    T_SEN3_SCL_HIGH();
    T_SEN3_SDA_OUTPUT();
    T_SEN3_SDA_HIGH();
    
    T_SEN4_SCL_OUTPUT();
    T_SEN4_SCL_HIGH();
    T_SEN4_SDA_OUTPUT();
    T_SEN4_SDA_HIGH();
}