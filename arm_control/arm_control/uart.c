#include "uart.h"
#include <avr/io.h>

void UART0_init(int BAUD_PRESCALER)
{
  
  /*Set baud rate */
  UBRR0H = (unsigned char)(BAUD_PRESCALER>>8);
  UBRR0L = (unsigned char)BAUD_PRESCALER;
  //Enable receiver and transmitter 
  UCSR0B = (1<<RXEN0)|(1<<TXEN0);
  /* Set frame format: 2 stop bits, 8 data bits */
  UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); // 8 data bits
  UCSR0C |= (1<<USBS0); // 2 stop bits
}

void UART1_init(int BAUD_PRESCALER)
{
	
	/*Set baud rate */
	UBRR1H = (unsigned char)(BAUD_PRESCALER>>8);
	UBRR1L = (unsigned char)BAUD_PRESCALER;
	//Enable receiver and transmitter
	UCSR1B = (1<<RXEN1)|(1<<TXEN1);
	/* Set frame format: 2 stop bits, 8 data bits */
	UCSR1C = (1<<UCSZ11) | (1<<UCSZ10); // 8 data bits
	UCSR1C |= (1<<USBS1); // 2 stop bits
}

void UART0_send(unsigned char data)
{
  // Wait for empty transmit buffer
  while(!(UCSR0A & (1<<UDRE0)));
  // Put data into buffer and send data
  UDR0 = data;
  
}

void UART1_send(unsigned char data)
{
	// Wait for empty transmit buffer
	while(!(UCSR1A & (1<<UDRE0)));
	// Put data into buffer and send data
	UDR1 = data;
	
}

void UART0_putstring(char* StringPtr)
{
  while(*StringPtr != 0x00)
  {
    UART0_send(*StringPtr);
	StringPtr++;
  }
}

void UART1_putstring(char* StringPtr)
{
	while(*StringPtr != 0x00)
	{
		UART1_send(*StringPtr);
		StringPtr++;
	}
}

// Function to check if data is available to read from UART0
int UART0_available() {
	return (UCSR0A & (1 << RXC0)); // Check Receive Complete flag
}

// Function to check if data is available to read from UART1
int UART1_available() {
	return (UCSR1A & (1 << RXC1)); // Check Receive Complete flag
}

// Function to read a byte from UART0
unsigned char UART0_read() {
	UART0_putstring("start reading");
	while (!(UCSR0A & (1 << RXC0))); // Wait until data is received
	UART0_putstring("data available");
	return UDR0; // Return received data
}

// Function to read a byte from UART1
unsigned char UART1_read() {
	while (!(UCSR1A & (1 << RXC1))); // Wait until data is received
	return UDR1; // Return received data
}

