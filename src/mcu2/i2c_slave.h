#ifndef I2C_SLAVE_H
#define I2C_SLAVE_H

#include <avr/io.h>
#include <stdint.h>

// I2C Slave address
#define I2C_SLAVE_ADDRESS 0x42

// Callback function type
typedef void (*i2c_receive_callback_t)(const uint8_t *data, uint8_t length);

// Function prototypes
void i2c_slave_init(uint8_t address);
void i2c_slave_set_receive_callback(i2c_receive_callback_t callback);
uint8_t i2c_slave_data_available(void);

#endif // I2C_SLAVE_H
