// ===================================================================================
// I2C Functions for CH55x
// ===================================================================================

#pragma once
#include <stdint.h>

void I2C_init(void);            // I2C init function
void I2C_start(void);           // I2C start transmission
void I2C_restart(void);         // I2C restart transmission
void I2C_stop(void);            // I2C stop transmission
void I2C_write(uint8_t data);   // I2C transmit one data byte to the slave
uint8_t I2C_read(uint8_t ack);  // I2C receive one data byte from the slave
