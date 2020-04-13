/*
 * File:   main.c
 * Author: yun
 * This is project that MEMS's refresh rate is 68Hz and MLX90632's refresh rate is 8 ~ 9Hz
 * Created on December 4, 2018, 3:24 PM
 */

#include <xc.h>
#include "uart.h"
#include <pic16f690.h>
#include <plib/delays.h>

// CONFIG
#pragma config FOSC = INTRCIO  // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF       // MCLR Pin Function Select bit (MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Selection bits (BOR enabled)
#pragma config IESO = OFF        // Internal External Switchover bit (Internal External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)

/*************************************************************************************************/
unsigned char MLX_Sensors_CheckInit(void);
unsigned char MLX90632_Temp_Measuring(unsigned char ID, unsigned int *MLX632_Data);
unsigned char MLX90632_Start_Measurement(unsigned char ID);
/***********************************************************************************************/
unsigned char MPU9250_Init(void);
void MPU9250_Measuring(void);
/***********************************************************************************************/
void Delay_ms(unsigned int ms);
unsigned int checksum16(void *data, unsigned int bytes);
/***********************************************************************************************/
unsigned int MLX632_ReadData[10] = {0};
/***********************************************************************************************/
void Delay_ms(unsigned int ms)
{                         
    unsigned int a;
    while(ms)
    {
        a = 195;
        while(a--);
        ms--;
    }
}

void interrupt ISR(void)
{
    
}

//unsigned int checksum16(void *data, unsigned int bytes)
//{
//    unsigned int *data_pointer = (unsigned int *) data;
//    unsigned int total_sum;
//
//    while(bytes > 1)
//	    {
//	        total_sum += *data_pointer++;
//	        //If it overflows to the MSBs add it straight away
//	        if(total_sum >> 16)
//		        {
//		            total_sum = (total_sum >> 16) + (total_sum & 0x0000FFFF);
//		        }
//	        bytes -= 2; //Consumed 2 bytes
//	    }
//
//    if(1 == bytes)
//	    {
//	        //Add the last byte
//	        total_sum += *(((unsigned char *) data_pointer) + 1);
//	        //If it overflows to the MSBs add it straight away
//	        if(total_sum >> 16)
//		        {
//		            total_sum = (total_sum >> 16) + (total_sum & 0x0000FFFF);
//		        }
//	        bytes -= 1;
//	    }
//
//    return (~((unsigned int) total_sum));
//}

void main(void) 
{
    unsigned char h = 1, hh = 1;
    
    OSCCONbits.IRCF = 0b111;   //internal clock = 8MHz
    OSCCONbits.SCS = 1;        //system clock = internal oscillator
    OSCTUNE = 0b00000;
    RATE_DEBUG_DIR = 0;
    WPUA = 0;                  //PORTA pull-up disable
    WPUB = 0;                  //PORTB pull-up disable
    ANSEL = 0;                 //Analog input disable
    ANSELH = 0;                //Analog input disable
    CM1CON0= 0;
    CM2CON0= 0;
    ADCON0 = 0;
    RATE_DEBUG = 0;
    Usart_Init();
    while(MLX_Sensors_CheckInit());
    Usart_Sendbytes("Whole MLX90632 OK!\n");
    while(MPU9250_Init());    
    Usart_Sendbytes("MPU9250_OK!\n");
    Delay_ms(20);
    
    while(1)
    {
////        RATE_DEBUG = 1;
        for(h = 1; h < 8; h++) // One cycle is 1000mS = 1S / 72Hz
            {
                Usart_Sendonebyte(0xAA); // Start Byte
                Usart_Sendonebyte(0xAA); // Start Byte
                Usart_Sendonebyte(0x99);// data type
                //Usart_Sendonebyte(h + 48); // Start Byte// counter
            /*************< MPU9250 Measurement Start! >****************************/
                MPU9250_Measuring();
            /***************************************************************************/
                Usart_Sendonebyte(0xDD); // MEMS End Byte
                Usart_Sendonebyte(0xDD); // MEMS End Byte
            }  // < 7 cycles time: 60mS, 1-cycle = 60/7 = approximately 8.6mS
////        RATE_DEBUG = 0;Delay_ms(50);
////        RATE_DEBUG = 1;
        for(hh = 1; hh < 5; hh++)  // 90632 Sensor Measuring Rate is 8.2Hz
            {
                if(MLX90632_Start_Measurement(hh))
                    {
                        Usart_Sendonebyte(0xAA);           // 90632 Sensor's Start Byte             
                        Usart_Sendonebyte(0xAA);           // 90632 Sensor's Start Byte
                        Usart_Sendonebyte(hh);             // 90632 Sensor Number
                        
                        while(MLX90632_Temp_Measuring(hh, MLX632_ReadData));
                        
                        Usart_Sendonebyte(0xDD);  // 90632 Sensor Stop Byte
                        Usart_Sendonebyte(0xDD);  // 90632 Sensor Stop Byte
                    }
           }  // <4-Temp sensor measure complete time: 59.2mS, 59.2/4 = 14.8mS>
////        RATE_DEBUG = 0;Delay_ms(100);
    } 
}
