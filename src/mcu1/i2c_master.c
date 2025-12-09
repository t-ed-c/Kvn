#include "i2c_master.h"
#include <util/delay.h>

// Initialize I2C as master at 100kHz
// F_SCL = F_CPU / (16 + 2 * TWBR * Prescaler)
// For F_CPU = 16MHz, F_SCL = 100kHz:
// 100000 = 16000000 / (16 + 2 * TWBR * 1)
// TWBR = 72
void i2c_master_init(void) {
    // Set bit rate register for 100kHz SCL
    TWBR = 72;
    
    // Set prescaler to 1
    TWSR = 0x00;
    
    // Enable TWI
    TWCR = (1 << TWEN);
}

uint8_t i2c_get_status(void) {
    return TWSR & 0xF8;
}

uint8_t i2c_start(void) {
    // Send START condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    
    // Wait for TWINT flag to be set
    while (!(TWCR & (1 << TWINT)));
    
    // Check status
    uint8_t status = i2c_get_status();
    return (status == I2C_START || status == I2C_REP_START) ? 1 : 0;
}

uint8_t i2c_write(uint8_t data) {
    // Load data into data register
    TWDR = data;
    
    // Start transmission
    TWCR = (1 << TWINT) | (1 << TWEN);
    
    // Wait for TWINT flag to be set
    while (!(TWCR & (1 << TWINT)));
    
    // Check status
    uint8_t status = i2c_get_status();
    return (status == I2C_MT_SLA_ACK || status == I2C_MT_DATA_ACK) ? 1 : 0;
}

void i2c_stop(void) {
    // Send STOP condition
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    
    // Wait for STOP to complete
    _delay_us(100);
}

uint8_t i2c_send_packet(uint8_t slave_addr, const uint8_t *data, uint8_t length) {
    // Send START condition
    if (!i2c_start()) {
        i2c_stop();
        return 0;
    }
    
    // Send slave address with write bit
    if (!i2c_write(slave_addr << 1)) {
        i2c_stop();
        return 0;
    }
    
    // Send data bytes
    for (uint8_t i = 0; i < length; i++) {
        if (!i2c_write(data[i])) {
            i2c_stop();
            return 0;
        }
    }
    
    // Send STOP condition
    i2c_stop();
    
    return 1;
}
