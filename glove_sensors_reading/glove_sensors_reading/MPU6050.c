/*
 * MPU6050.c
 *
 * Created: 4/26/2024 1:08:02 AM
 */ 
#define F_CPU 16000000
#define BRC ((F_CPU/16/BAUD)-1)
#define MPU6050 0x68
#define PI 3.14159265358979323846

#include "MPU6050.h"
#include "I2C.h"

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <util/twi.h>
#include <stdint.h>


uint8_t MPU_init(){
	I2C_init();
	I2C_start();
	if (I2C_status() != TW_START) return -1;
	I2C_write(((MPU6050 << 1) | TW_WRITE)); //TW_WRITE = 0 defined in twi.h
	if (I2C_status() != TW_MT_SLA_ACK) return -1;
	I2C_write(0x6B);//PWR_MGMT_1 address
	if (I2C_status() != TW_MT_DATA_ACK) return -1;
	I2C_write(0);//set PWR_MGMT_1 to 0
	if (I2C_status() != TW_MT_DATA_ACK) return -1;
	I2C_stop();
	_delay_ms(1);//allow time for stop to be sent
	return 0;
}

uint8_t MPU_write(uint8_t u8addr, uint8_t u8data){
	I2C_init();
	I2C_start();
	if (I2C_status() != TW_START) return -1;
	I2C_write(((MPU6050 << 1) | TW_WRITE)); //TW_WRITE = 0 defined in twi.h
	if (I2C_status() != TW_MT_SLA_ACK) return -1;
	I2C_write(u8addr);//set address
	if (I2C_status() != TW_MT_DATA_ACK) return -1;
	I2C_write(u8data);//set value
	if (I2C_status() != TW_MT_DATA_ACK) return -1;
	I2C_stop();
	_delay_us(10);//allow time for stop to be sent
	return 0;
}

uint8_t MPU_read(uint8_t u8addr, int16_t *int16data){
	I2C_start();
	if (I2C_status() != TW_START) return -1;
	I2C_write(((MPU6050 << 1) | TW_WRITE)); //TW_WRITE = 0 defined in twi.h
	if (I2C_status() != TW_MT_SLA_ACK) return -1;
	I2C_write(u8addr); //send reading address of MPU-6050
	if (I2C_status() != TW_MT_DATA_ACK) return -1;
	I2C_stop();
	_delay_us(10);//wait for data
	
	I2C_start();
	if (I2C_status() != TW_START) return -1;
	I2C_write(((MPU6050 << 1) | TW_READ));//ask to read
	if (I2C_status() != TW_MR_SLA_ACK) return -1;
	
	*int16data = I2C_readACK() << 8; //significant 8 bits with acknowledge
	*int16data |= I2C_readNACK(); //no ACK after the last bytes.
	I2C_stop();
	_delay_us(10);//wait for data
	return 0;
}

void timer2_initialize() {
	//cli();
	TCCR2A = 0;
	TCCR2B = 0;
	TCNT2 = 0; // initialize TImer 2 value
	TCCR2B |= (1 << CS20) | (1 << CS21); // prescaler = 32
	TIMSK2 |= (1 << TOIE2); // enable overflow interrupt
	//sei();
}
ISR(TIMER2_OVF_vect) {
	overflowCount++; // Increment overflow counter
}

void get_MPU_readings() {
	MPU_read(0x3B,&AcX);
	MPU_read(0x3D,&AcY);
	MPU_read(0x3F,&AcZ);
	MPU_read(0x43,&GyroX);
	MPU_read(0x43,&GyroX);
	MPU_read(0x45,&GyroY);
	MPU_read(0x47,&GyroZ);
	// Convert ACCELEROMETER raw data to m/s with g = 9.8 m/s^2
	AcX_converted = (((float) AcX + 300) / 16384.0) * 9.8;
	AcY_converted = (((float) AcY - 890) / 16384.0) * 9.8;
	AcZ_converted = (((float) AcZ + 484)/ 16384.0) * 9.8;
	// Convert GYROSCOPE raw data to degree/s
	GyroX_converted = ((float) GyroX + 140) / 131.0;
	GyroY_converted = ((float) GyroY + 75) / 131.0;
	GyroZ_converted = ((float) GyroZ - 135) / 131.0;
	// Rotational angle around X, Y axis (ROLL ~ X, PITCH ~ Y)
	angleX = (atan(AcY_converted / sqrt(pow(AcX_converted, 2) + pow(AcZ_converted, 2))) * 180 / PI) + 0.60;
	angleY = (atan(-1 * AcX_converted / sqrt(pow(AcY_converted, 2) + pow(AcZ_converted, 2))) * 180 / PI) -4.0;
}

void yaw_calculation() {
	if (GyroZ_converted < -2 || GyroZ_converted > 2) {
		current_time = TCNT2 + (overflowCount * 256);
		elapsed_time = ((current_time - previous_time) * 32.0) / 16000000.0; // elapsed time in seconds
		angleZ = angleZ + GyroZ_converted * elapsed_time;
		previous_time = current_time;
	}
}