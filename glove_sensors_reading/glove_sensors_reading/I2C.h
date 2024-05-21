/*
 * I2C.h
 *
 * Created: 4/26/2024 1:03:50 AM
 */ 


#ifndef I2C_H_
#define I2C_H_
#include <stdint.h>

void I2C_init(void);
void I2C_start(void);
void I2C_stop(void);
void I2C_write(uint8_t data);
uint8_t I2C_readACK(void);
uint8_t I2C_readNACK(void);
uint8_t I2C_status(void);

#endif /* I2C_H_ */