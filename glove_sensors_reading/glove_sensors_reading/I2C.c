/*
 * I2C.c
 *
 * Created: 4/26/2024 1:04:37 AM
 */ 

#include "i2c.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <util/twi.h>

// Initialization of I2C with 100 kHz SCL clock
void I2C_init(void){
	TWSR0 = 0x00; // TWPS[1:0] = 00 -> prescaler = 1
	TWBR0 = 0x48; // TWBR = 72 => TWI clock = 100kHz, SCL frequency = F_CPU/(16 + 2(TWBR)*(TWSR prescale))
	//TWCR0 = (1 << TWEN); // TWEN bit enables TWI0 operation and activates the TWI0interface
}

void I2C_start(void){
	// TWI enable/clear TWI Interrupt flag/START condition for TWI0 to become a Master
	TWCR0 = (1 << TWEN) | (1 << TWINT) |(1 << TWSTA);
	// Wait for TWINT flag set, which indicates that the START condition has been transmitted
	while (!(TWCR0 & (1 << TWINT)));
	// Check value of TWI status register and mask the prescaler bits
	//if ((TWSR0 & 0xF8) != TW_START) return -1;
}

void I2C_stop(void){
	// transmit STOP condition
	TWCR0 = (1 << TWEN) | (1 << TWINT) |(1 << TWSTO);
}

void I2C_write(uint8_t data){
	// load data into TWDR register
	TWDR0 = data;
	// Clear TWINT bit in TWCR to start transmission of data
	TWCR0 = (1 << TWINT) | (1 << TWEN);
	// Wait for TWINT flag set, which indicates that data has been transmitted and ACK/NACK has been received
	while (!(TWCR0 & (1 << TWINT)));
	// Check value of TWI status register and mask prescaler bits
	//if ((TWSR0 & 0xF8) != TW_MT_SLA_ACK) return -1;
}

uint8_t I2C_readACK(){
	// The ACK pulse is generated on the TWI0 bus
	TWCR0 = (1 << TWINT)|(1 << TWEN)|(1 << TWEA);
	// Wait for TWINT flag set, which indicates that data has been transmitted and ACK/NACK has been received
	while (!(TWCR0 & (1 << TWINT)));
	
	return TWDR0;
}

uint8_t I2C_readNACK(){
	// the device can be virtually disconnected from the 2-wire Serial Bus temporarily
	TWCR0 = (1 << TWINT)|(1 << TWEN);
	// Wait for TWINT flag set, which indicates that data has been transmitted and ACK/NACK has been received
	while (!(TWCR0 & (1 << TWINT)));

	return TWDR0;
}

uint8_t I2C_status(void)
{
	uint8_t status;
	//mask status
	status = TWSR0 & 0xF8;
	return status;
}