/*
 * MPU6050.h
 *
 * Created: 4/26/2024 1:06:11 AM
 */ 


#ifndef MPU6050_H_
#define MPU6050_H_

#include <stdint.h>
#include <math.h>
#include <avr/interrupt.h>
#include <util/twi.h>

/////// GLOBAL VARIABLES //////////
int16_t AcX, AcY, AcZ, GyroX, GyroY, GyroZ;
float AcX_converted, AcY_converted, AcZ_converted;
float GyroX_converted, GyroY_converted, GyroZ_converted;
double angleX, angleY, angleZ;
volatile unsigned long overflowCount;
unsigned long previous_time, current_time;
float elapsed_time;
///////////////////////////////////

uint8_t MPU_init(void);
uint8_t MPU_write(uint8_t u8addr, uint8_t u8data);
uint8_t MPU_read(uint8_t u8addr, int16_t *int16data);
void timer2_initialize(void);
ISR(TIMER2_OVF_vect);
void get_MPU_readings(void);
void yaw_calculation(void);

#endif /* MPU6050_H_ */