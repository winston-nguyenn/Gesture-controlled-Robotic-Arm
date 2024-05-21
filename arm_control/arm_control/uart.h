#ifndef UART_H
#define UART_H

void UART0_init(int prescale);

void UART1_init(int prescale);

void UART0_send( unsigned char data);

void UART1_send( unsigned char data);

void UART0_putstring(char* StringPtr);

void UART1_putstring(char* StringPtr);

int UART0_available();

int UART1_available();

unsigned char UART0_read();

unsigned char UART1_read();

#endif 