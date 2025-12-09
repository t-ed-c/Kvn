#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

// I2C Communication Protocol
#define PROTOCOL_START_BYTE 0xAA
#define PROTOCOL_PACKET_SIZE 5

// Mode definitions
#define MODE_SPEED 0x01      // M-Speed mode (value = RPM)
#define MODE_POSITION 0x02   // M-Pos mode (value = Revolutions)
#define MODE_EMERGENCY 0xFF  // Emergency stop

// Packet structure:
// [0] START   = 0xAA
// [1] MODE    = mode byte
// [2] VALUE_H = high byte of value
// [3] VALUE_L = low byte of value
// [4] CHECK   = checksum (sum of bytes 1-3)

typedef struct {
    uint8_t start;
    uint8_t mode;
    uint8_t value_high;
    uint8_t value_low;
    uint8_t checksum;
} I2CPacket;

// Helper function to calculate checksum
static inline uint8_t calculate_checksum(uint8_t mode, uint8_t value_high, uint8_t value_low) {
    return mode + value_high + value_low;
}

// Helper function to pack value into packet
static inline void pack_value(I2CPacket *packet, uint16_t value) {
    packet->value_high = (value >> 8) & 0xFF;
    packet->value_low = value & 0xFF;
}

// Helper function to unpack value from packet
static inline uint16_t unpack_value(const I2CPacket *packet) {
    return ((uint16_t)packet->value_high << 8) | packet->value_low;
}

// Helper function to validate packet
static inline uint8_t validate_packet(const I2CPacket *packet) {
    if (packet->start != PROTOCOL_START_BYTE) {
        return 0;
    }
    uint8_t expected_checksum = calculate_checksum(packet->mode, packet->value_high, packet->value_low);
    return (packet->checksum == expected_checksum);
}

#endif // PROTOCOL_H
