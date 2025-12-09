#ifndef I2C_MASTER_H
#define I2C_MASTER_H

#include <avr/io.h>
#include <stdint.h>

// I2C Status codes
#define I2C_START             0x08
#define I2C_REP_START         0x10
#define I2C_MT_SLA_ACK        0x18
#define I2C_MT_SLA_NACK       0x20
#define I2C_MT_DATA_ACK       0x28
#define I2C_MT_DATA_NACK      0x30
#define I2C_ARB_LOST          0x38

// I2C Slave address for MCU2
#define MCU2_I2C_ADDRESS      0x42

// Function prototypes
void i2c_master_init(void);
uint8_t i2c_start(void);
uint8_t i2c_write(uint8_t data);
void i2c_stop(void);
uint8_t i2c_get_status(void);

// High-level functions
uint8_t i2c_send_packet(uint8_t slave_addr, const uint8_t *data, uint8_t length);

#endif // I2C_MASTER_H
