#include "encoder.h"
#include <avr/interrupt.h>

// Global variables
static volatile int32_t pulse_count = 0;
static volatile int32_t last_pulse_count = 0;

void encoder_init(void) {
    // Configure encoder pin as input
    ENCODER_DDR &= ~(1 << ENCODER_INT_PIN);
    
    // Enable pull-up on encoder pin
    ENCODER_PORT |= (1 << ENCODER_INT_PIN);
    
    // Configure INT0 to trigger on rising edge
    MCUCR |= (1 << ISC01) | (1 << ISC00);
    
    // Enable INT0
    GICR |= (1 << INT0);
    
    // Reset pulse count
    pulse_count = 0;
    last_pulse_count = 0;
}

void encoder_reset(void) {
    pulse_count = 0;
    last_pulse_count = 0;
}

int32_t encoder_get_pulses(void) {
    return pulse_count;
}

int32_t encoder_get_revolutions(void) {
    return pulse_count / ENCODER_PPR;
}

uint16_t encoder_calculate_rpm(uint32_t time_ms) {
    // Calculate RPM based on pulse difference and time
    // RPM = (pulses / PPR) * (60000 / time_ms)
    
    if (time_ms == 0) {
        return 0;
    }
    
    int32_t pulse_diff = pulse_count - last_pulse_count;
    last_pulse_count = pulse_count;
    
    // Avoid overflow: RPM = (pulse_diff * 60000) / (PPR * time_ms)
    // Simplify: RPM = (pulse_diff * 600) / (PPR * (time_ms / 100))
    int32_t rpm = (pulse_diff * 60000L) / (ENCODER_PPR * time_ms);
    
    if (rpm < 0) rpm = 0;
    if (rpm > 9999) rpm = 9999;
    
    return (uint16_t)rpm;
}

// INT0 interrupt service routine
ISR(INT0_vect) {
    pulse_count++;
}
