#ifndef ENCODER_H
#define ENCODER_H

#include <avr/io.h>
#include <stdint.h>

// Encoder configuration
#define ENCODER_PPR 100  // Pulses Per Revolution (configurable)

// Encoder pin configuration (using INT0 on PD2)
#define ENCODER_PORT PORTD
#define ENCODER_DDR  DDRD
#define ENCODER_PIN  PIND
#define ENCODER_INT_PIN PD2  // INT0

// Function prototypes
void encoder_init(void);
void encoder_reset(void);
int32_t encoder_get_pulses(void);
int32_t encoder_get_revolutions(void);
uint16_t encoder_calculate_rpm(uint32_t time_ms);

#endif // ENCODER_H
