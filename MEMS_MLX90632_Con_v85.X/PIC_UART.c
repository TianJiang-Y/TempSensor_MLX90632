
#include <xc.h>
#include "uart.h"
#include <plib/delays.h>

void Delay_ms(unsigned int ms);
////unsigned char m[15] = {'T','R','A','M','0',0,':',' ',0,0,'.',0,0,'`','C'};
//unsigned char w[10] = {0};

void Usart_Init(void)
{        
    SPBRG = 8;            // set baud rate to 57600 baud
    TXSTAbits.TXEN = 1;     // enable transmitter
    TXSTAbits.BRGH = 1;     // high baud rate mode
//    TXSTAbits.SYNC = 1;
    RCSTAbits.CREN = 1;     // enable continous receiving
    
    //configure I/O pins
    ANSELHbits.ANS11 = 0;     // RX input type is digital
    TRISBbits.TRISB5 = 1;     // RX pin is input
    TRISBbits.TRISB7 = 0;     // TX pin is input (automatically configured)
    
//    PIE1bits.RCIE = 1;      //enable USART receive interrupt
    RCSTAbits.SPEN = 1;     //enable USART
}

void Usart_Sendonebyte(unsigned char data)
{
    while (!TXSTAbits.TRMT);  // wait until transmit shift register is empty
    TXREG = data;             // write character to TXREG and start transmission   
//    Delay10TCY();
}

void Usart_Send_Twobyte(unsigned int data)
{
    while (!TXSTAbits.TRMT);
    TXREG = data >> 8;  // MSB
    while (!TXSTAbits.TRMT);
    TXREG = (unsigned char)data; // LSB
}

void Usart_Sendbytes(unsigned char *s)
{
    while (*s)
    {
        Usart_Sendonebyte(*s);     // send character pointed to by s
        s++;                       // increase pointer location to the next character
    }
}
//
//unsigned char Uint16to_Uint8Array( unsigned char sensorID, unsigned char cnt, short val)
//{
//    unsigned char A, C, D;
//    unsigned int B;
//    
//    if (sensorID == MP9250)
//    {
//        switch(cnt)
//        {
//            case 0:w[0]='A';w[1]='x';break;
//            case 1:w[0]='A';w[1]='y';break;
//            case 2:w[0]='A';w[1]='z';break;
//            case 3:w[0]='G';w[1]='x';break;
//            case 4:w[0]='G';w[1]='y';break;
//            case 5:w[0]='G';w[1]='z';break;
//            case 6:w[0]='M';w[1]='x';break;
//            case 7:w[0]='M';w[1]='y';break;
//            case 8:w[0]='M';w[1]='z';break;
//            case 9:w[0]='T';w[1]='m';break;
//            default: break;
//        }
//    }
//    
//    if (sensorID == MLX90632)
//    {
//        switch(cnt)
//        {
//            case 0:w[0]='R';w[1]='G';break;
//            case 1:w[0]='R';w[1]='1';break;
//            case 2:w[0]='R';w[1]='2';break;
//            case 3:w[0]='R';w[1]='3';break;
//            case 4:w[0]='R';w[1]='4';break;
//            case 5:w[0]='R';w[1]='5';break;
//            case 6:w[0]='R';w[1]='6';break;
//            case 7:w[0]='R';w[1]='7';break;
//            case 8:w[0]='R';w[1]='8';break;
//            case 9:w[0]='R';w[1]='9';break;
//            default:break;
//        }
//    }
//
//    if (val < 0)
//      {
//        w[2] = '-';
//        val = -val;
//      }
//    else
//      w[2] = ' ';
//    
//    if(val >= 10000)
//    {
//        A = val / 10000;     // upper the decimal place
//        B = val - A * 10000; // below the decimal place
//        C = B / 100;
//        D = B - C * 100;
//        
//        if(A == 0)
//          w[3] = ' ';
//        else
//          w[3] = A + HPAPP_THRESHOLD;
//
//        if(C >= 10)
//        {
//            w[4] = (C / 10) + HPAPP_THRESHOLD;
//            C = C % 10;
//            w[5] = C  + HPAPP_THRESHOLD;
//        }
//
//        else
//        {
//            w[4] = HPAPP_THRESHOLD;
//            w[5] = C + HPAPP_THRESHOLD;
//        }
//        
//        if(D >= 10)
//        {
//            w[6] = (D / 10) + HPAPP_THRESHOLD;
//            D = D % 10;
//            w[7] = D  + HPAPP_THRESHOLD;
//        }
//
//        else
//        {
//            w[6] = HPAPP_THRESHOLD;
//            w[7] = D + HPAPP_THRESHOLD;
//        }
//        return 8;
//    }
//    
//    else if(val >= 1000)
//    {
//      A = val / 100;
//      C = val - A * 100;
//      
//      w[3] = (A / 10) + HPAPP_THRESHOLD; // UART App threshold = 48
//      A = A % 10;
//      w[4] = A + HPAPP_THRESHOLD;
//
//     if(C >= 10)
//      {
//          w[5] = (C / 10) + HPAPP_THRESHOLD;
//          C = C % 10;
//          w[6] = C  + HPAPP_THRESHOLD;
//      }
//
//     else
//      {
//          w[5] = HPAPP_THRESHOLD;
//          w[6] = C + HPAPP_THRESHOLD;
//      }
//      return 7;
//    }
//    
//    else if (val >= 100)
//    {
//        A = val / 100;
//        C = val - A * 100;
//        
//        w[3] = A + HPAPP_THRESHOLD;
//      
//       if(C >= 10)
//        {
//            w[4] = (C / 10) + HPAPP_THRESHOLD;
//            C = C % 10;
//            w[5] = C  + HPAPP_THRESHOLD;
//        }
//
//       else
//        {
//            w[4] = HPAPP_THRESHOLD;
//            w[5] = C + HPAPP_THRESHOLD;
//        }
//      return 6;
//    }
//    
//    else if (val >= 10)
//    {
//        w[3] = (val / 10) + HPAPP_THRESHOLD;
//        val = val % 10;
//        w[4] = val  + HPAPP_THRESHOLD;
//        return 5;
//    }
//
//    else
//    {
//        w[3] = val + HPAPP_THRESHOLD;
//        return 4;
//    }
//}

//void MeasValue_SendToCom( unsigned char SensorID, short *TempData )
//{
//    unsigned char TempValArrlen = 0; // Array number
//    short SendVal;
//    unsigned char SensorType = 0;
//    
//    SensorType = SensorID;
//
//    for(unsigned char j = 0; j < 10; j++)
//    {
//      SendVal = *(TempData++);
//      TempValArrlen = Uint16to_Uint8Array(SensorType, j, SendVal);
//
//      for(unsigned char k = 0; k < TempValArrlen; k++)
//      {
//          Usart_Sendonebyte(w[k]);
//      }
//    }
//}
//
//void MeasOneValue_SendToCom( unsigned char SensorID, unsigned char meacnt, short TempData )
//{
//    unsigned char TempValArrlen = 0; // Array number
//
//      TempValArrlen = Uint16to_Uint8Array(SensorID, meacnt, TempData);
//
//      for(unsigned char k = 0; k < TempValArrlen; k++)
//      {
//          Usart_Sendonebyte(w[k]);
//          Delay_ms(1);
//      }
//}
