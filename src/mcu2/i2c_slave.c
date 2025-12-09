#include "i2c_slave.h"
#include <avr/interrupt.h>
#include "../common/protocol.h"

// Buffer for received data
static volatile uint8_t rx_buffer[PROTOCOL_PACKET_SIZE];
static volatile uint8_t rx_index = 0;
static volatile uint8_t data_ready = 0;
static i2c_receive_callback_t receive_callback = 0;

void i2c_slave_init(uint8_t address) {
    // Set slave address
    TWAR = (address << 1);
    
    // Enable TWI, enable ACK, enable TWI interrupt
    TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWIE);
}

void i2c_slave_set_receive_callback(i2c_receive_callback_t callback) {
    receive_callback = callback;
}

uint8_t i2c_slave_data_available(void) {
    return data_ready;
}

// TWI/I2C interrupt service routine
ISR(TWI_vect) {
    uint8_t status = TWSR & 0xF8;
    
    switch (status) {
        case 0x60:  // Own SLA+W received, ACK returned
        case 0x70:  // General call received, ACK returned
            rx_index = 0;
            data_ready = 0;
            TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
            break;
            
        case 0x80:  // Data byte received, ACK returned
        case 0x90:  // Data byte received after general call, ACK returned
            if (rx_index < PROTOCOL_PACKET_SIZE) {
                rx_buffer[rx_index++] = TWDR;
            }
            
            // Check if we've received a complete packet
            if (rx_index >= PROTOCOL_PACKET_SIZE) {
                data_ready = 1;
                
                // Call callback if registered
                if (receive_callback) {
                    receive_callback((const uint8_t*)rx_buffer, PROTOCOL_PACKET_SIZE);
                }
            }
            
            TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
            break;
            
        case 0xA0:  // STOP or repeated START received
            TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
            break;
            
        case 0x88:  // Data byte received, NACK returned
        case 0x98:  // Data byte received after general call, NACK returned
        case 0xA8:  // Own SLA+R received, ACK returned
        case 0xB0:  // Arbitration lost in SLA+RW, own SLA+R received, ACK returned
        case 0xB8:  // Data byte transmitted, ACK received
        case 0xC0:  // Data byte transmitted, NACK received
        case 0xC8:  // Last data byte transmitted, ACK received
        default:
            TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
            break;
    }
}
