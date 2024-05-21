/*
 * glove_sensors_reading.c
 *
 * Created: 4/26/2024 12:05:35 AM
 */ 
#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "I2C.h"
#include "MPU6050.h"
#include "uart.h"
#define USART_BAUD_RATE      9600
#define USART_BAUD_PRESCALER (((F_CPU / (USART_BAUD_RATE * 16UL))) - 1)
#define __PRINT_NEW_LINE__  UART0_putstring("\r\n");

#define ESP_TX_PIN 17 // ATmega TX pin connected to ESP RX pin
#define ESP_RX_PIN 16 // ATmega RX pin connected to ESP TX pin

int flex_sensor_reading;

void ADC_initialize() {
	// Setup for ADC (10bit = 0-1023)
	// Clear power reduction bit for ADC
	PRR0 &= ~(1 << PRADC);
	// Select Vref = AVcc
	ADMUX |= (1 << REFS0);
	ADMUX &= ~(1 << REFS1);
	// Set the ADC clock div by 128
	// 16M/128=125kHz
	ADCSRA |= (1 << ADPS0);
	ADCSRA |= (1 << ADPS1);
	ADCSRA |= (1 << ADPS2);
	// Select Channel ADC0 (pin C0)
	ADMUX &= ~(1 << MUX0);
	ADMUX &= ~(1 << MUX1);
	ADMUX &= ~(1 << MUX2);
	ADMUX &= ~(1 << MUX3);
	ADCSRA |= (1 << ADATE);   // Autotriggering of ADC
	// Free running mode ADTS[2:0] = 000
	ADCSRB &= ~(1 << ADTS0);
	ADCSRB &= ~(1 << ADTS1);
	ADCSRB &= ~(1 << ADTS2);
	// Disable digital input buffer on ADC pin
	DIDR0 |= (1 << ADC0D);
	// Enable ADC
	ADCSRA |= (1 << ADEN);
	// Start conversion
	ADCSRA |= (1 << ADSC);
}

void initialize() {
	ADC_initialize(); // ADC initialization to read flex sensor at pin PC0
	// Set up serial UART printing
	UART0_init(USART_BAUD_PRESCALER);
	UART1_init(USART_BAUD_PRESCALER);
	// Set ESP TX pin (ATmega) as output
	DDRB |= (1 << ESP_TX_PIN);
	// Set ESP RX pin (ATmega) as input
	DDRB &= ~(1 << ESP_RX_PIN);
	
	MPU_init(); // initialize MPU6050
	MPU_write(0x1A, 3); // configure the bandwidth and delay for accelerometer and gyroscope
	MPU_write(0x1B, 0); // configure gyroscope range to default range +- 250 degree/second
	MPU_write(0x1C, 0); // configure accelerometer range to default range +- 2g
}
int main(void)
{
	initialize(); // initialization of Timer2 and MPU
	overflowCount = 0;
	previous_time = 0;
	char dataString[100];
	char angleXStr[10], angleYStr[10], gyroZStr[10], flexStr[10];
    while (1) 
    {
		// Readings from FLEX SENSOR and MPU6050
		get_MPU_readings(); // needed readings sent to the arm: angleX, angleY, GyroZ_converted
		flex_sensor_reading = ADC; // store the reading from ADC to flex_sensor_reading variable
		// Sending data from Atmega328PB to ESP32
        // Convert floats to strings
        dtostrf(angleX, 6, 2, angleXStr);
        dtostrf(angleY, 6, 2, angleYStr);
        dtostrf(GyroZ_converted, 6, 2, gyroZStr);
        itoa(flex_sensor_reading, flexStr, 10); // Convert integer to string

        // Manually construct the string to send
        strcpy(dataString, flexStr);
        strcat(dataString, ",");
        strcat(dataString, angleXStr);
        strcat(dataString, ",");
        strcat(dataString, angleYStr);
        strcat(dataString, ",");
        strcat(dataString, gyroZStr);
        strcat(dataString, "\n");
				
        UART1_putstring(dataString); // send to UART1
		UART0_putstring(dataString); // send to UART0 for checking
		_delay_ms(500);
    }
}

