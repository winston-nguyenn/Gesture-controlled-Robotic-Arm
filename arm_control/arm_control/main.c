/*
 * arm_control.c
 *
 * Created: 4/26/2024 12:37:49 PM
 */ 
#define F_CPU                16000000UL   // 16MHz clock
#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> // Include string.h for string manipulation functions


#define USART_BAUD_RATE      9600
#define USART_BAUD_PRESCALER (((F_CPU / (USART_BAUD_RATE * 16UL))) - 1)

#define ESP_TX_PIN 17 // ATmega TX pin connected to ESP RX pin
#define ESP_RX_PIN 16 // ATmega RX pin connected to ESP TX pin

#define BUFFER_SIZE 128
char buffer[BUFFER_SIZE];
volatile uint8_t buffer_pos = 0;
volatile int data_ready = 0;  // Flag to indicate data is ready to be processed

float angleX, angleY, gyroZ_converted, flex_sensor_reading;

// Set up Timer 0 for Fast PWM mode with a frequency of 50Hz
void PWM_timer0_initialize() {
	TCCR0A = 0;
	// Set the waveform generation mode to Fast PWM
	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	TCCR0B &= ~(1 << WGM02);
	// Set the compare output mode to non-inverting mode for both OC0A and OC0B
	TCCR0A |= (1 << COM0A1) | (1 << COM0B1);
	//TCCR0A &= ~((1 << COM0A0) | (1 << COM0B0));
	// Set the prescaler to 1024 to get a 61Hz frequency
	TCCR0B |= (1 << CS02) | (1 << CS00);
	TCCR0B &= ~(1 << CS01);
	DDRD |= (1 << DDD6); // PD6 (OC0A) as output
	DDRD |= (1 << DDD5); // PD5 (OC0B) as output
}
void PWM_timer1_initialize() {
	DDRB |= (1 << DDB1);// PB1 for OC1A
	DDRB |= 1 << DDB2; //  PB2 for OC1B
	TCCR1A = 0;
	// Set up Timer1 for Fast PWM mode
	//TCCR1A |= (1 << COM1A1) | (1 << COM1B1);
	TCCR1A |= (1 << WGM11);
	TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11); // prescaler = 8
	// Set the TOP value for 20ms period (50Hz frequency)
	ICR1 = 39999;
}
// up_down_servo connects to PB1
void up_down_servo() {
	if (angleX > 35) {
		TCCR1A |= (1 << COM1A1);
		OCR1A = 1999;
	}
	else if (angleX < -35) {
		TCCR1A |= (1 << COM1A1);
		OCR1A = 4499;
	}
}
void fordward_backward_servo() {
	if (angleY > 35) {
		TCCR1A |= (1 << COM1B1);
		OCR1B = 1999;
	}
	else if (angleY < -35) {
		TCCR1A |= (1 << COM1B1);
		OCR1B = 3499;
	}
}
void gripper_servo() {
	if (flex_sensor_reading < 40 ) {
		TCCR0A |= (1 << COM0A1) ;
		OCR0A = 12; // close the gripper
	}
	else if (flex_sensor_reading > 40) {
		TCCR0A |= (1 << COM0A1) ;
		OCR0A = 30; // open the gripper
	}
}
void base_servo() {
	if (gyroZ_converted > 100) {
		TCCR0A |= (1 << COM0B1);
		OCR0B = 35; // rotate to the right
	}
	else if (gyroZ_converted < -100) {
		TCCR0A |= (1 << COM0B1);
		OCR0B = 12; // rotate to the left
	}
}

void parseBuffer(char *buffer) {
	char *token = strtok(buffer, ",");  // Get the first token
	if (token != NULL) {
		flex_sensor_reading = atof(token);  // Convert the first token to float
		token = strtok(NULL, ",");
	}
	if (token != NULL) {
		angleX = atof(token);  // Convert to float
		token = strtok(NULL, ",");
	}
	if (token != NULL) {
		angleY = atof(token);  // Convert to float
		token = strtok(NULL, ",");
	}
	if (token != NULL) {
		gyroZ_converted = atof(token);  // Convert to float
	}
	char debugStr[20];
	dtostrf(flex_sensor_reading, 6, 2, debugStr);
	UART0_putstring("Flex: ");
	UART0_putstring(debugStr);  // Output parsed values for verification
	
	dtostrf(angleX, 6, 2, debugStr);
	UART0_putstring(", X: ");
	UART0_putstring(debugStr);  // Output parsed values for verification
	
	dtostrf(angleY, 6, 2, debugStr);
	UART0_putstring(" Y: ");
	UART0_putstring(debugStr);  // Output parsed values for verification
	
	dtostrf(gyroZ_converted, 6, 2, debugStr);
	UART0_putstring(" Z: ");
	UART0_putstring(debugStr);  // Output parsed values for verification
	
}
int main(void) {
	// Initialize UART for communication
	UART0_init(USART_BAUD_PRESCALER);
	UART1_init(USART_BAUD_PRESCALER);
	cli();
	PWM_timer0_initialize();
	PWM_timer1_initialize();
	sei();
	
	//static char buffer[20]; // Buffer to hold incoming characters
	static char float_buffer[20];
	static uint8_t bufferIndex = 0;

	// Set ESP RX pin (ATmega) as input
	DDRB &= ~(1 << ESP_RX_PIN);

	while (1) {
		// Check if there is data available on UART1
		
		if (UART1_available()) {
			char receivedChar = UART1_read();  // Read one character from UART1
			if (receivedChar == '\n' || receivedChar == '\r') {
				// If the character is a newline, terminate the string
				buffer[buffer_pos] = '\0';
				// Output the complete string to UART0 (serial monitor)
				parseBuffer(buffer);  // Parse the buffer into variables
				//UART0_putstring(buffer);
				UART0_putstring("\r\n");
				// Reset buffer for the next line
				buffer_pos = 0;
				} 
			else if (buffer_pos < BUFFER_SIZE - 1) {
				// If the buffer is not full, add the character to the buffer
				buffer[buffer_pos++] = receivedChar;
			}
		}
		up_down_servo();
		fordward_backward_servo();
		gripper_servo();
		base_servo();
    }
		//_delay_ms(50);
}
