/* 
 * File:   uart.h
 * Author: CHONGJU
 *
 * Created on December 4, 2018, 3:28 PM
 */

#ifndef UART_H
#define	UART_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h>

#define MP9250         5
#define MLX90632       10
    
#define UART_TX_DIR     TRISBbits.TRISB7
#define UART_RX_DIR     TRISBbits.TRISB5
    
#define RATE_DEBUG_DIR  TRISAbits.TRISA0
#define RATE_DEBUG      PORTAbits.RA0
    
#define HPAPP_THRESHOLD                     48  // HandPhone Threshold Value
    

void Usart_Sendbytes(unsigned char *s);
void Usart_Sendonebyte(unsigned char data);
void Usart_Send_Twobyte(unsigned int data);

void Usart_Init(void);

void MLX632MeasValue_SendToCom( short* Tempaddr );
void Uint16to_Uint8BBSArray( unsigned int val);
void MeasValue_SendToCom( unsigned char SensorID, short *TempData );

void MeasOneValue_SendToCom( unsigned char SensorID, unsigned char meacnt, short TempData );
//////unsigned char Uint16to_Uint8Array( unsigned char cnt, int val);
unsigned char Uint16to_Uint8Array( unsigned char sensorID, unsigned char cnt, short val);


#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

