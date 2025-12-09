#include "motor.h"

void motor_init(void) {
    // Configure motor control pins as output
    MOTOR_PWM_DDR |= (1 << MOTOR_PWM_PIN);
    MOTOR_DIR_DDR |= (1 << MOTOR_DIR1_PIN) | (1 << MOTOR_DIR2_PIN);
    
    // Initialize Timer1 for Fast PWM mode, 8-bit
    // PWM frequency = F_CPU / (prescaler * 256)
    // With prescaler = 64: f = 16MHz / (64 * 256) = ~976 Hz
    
    // Set Fast PWM mode, 8-bit (WGM12:0 = 5)
    TCCR1A = (1 << WGM10) | (1 << COM1A1);  // Non-inverting mode on OC1A
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);  // Prescaler = 64
    
    // Initialize PWM to 0
    OCR1A = 0;
    
    // Initialize direction to stop
    motor_stop();
}

void motor_set_speed(uint8_t speed) {
    // Set PWM duty cycle (0-255)
    OCR1A = speed;
}

void motor_set_direction(MotorDirection dir) {
    switch (dir) {
        case MOTOR_FORWARD:
            MOTOR_DIR_PORT |= (1 << MOTOR_DIR1_PIN);
            MOTOR_DIR_PORT &= ~(1 << MOTOR_DIR2_PIN);
            break;
            
        case MOTOR_BACKWARD:
            MOTOR_DIR_PORT &= ~(1 << MOTOR_DIR1_PIN);
            MOTOR_DIR_PORT |= (1 << MOTOR_DIR2_PIN);
            break;
            
        case MOTOR_STOP:
        default:
            MOTOR_DIR_PORT &= ~((1 << MOTOR_DIR1_PIN) | (1 << MOTOR_DIR2_PIN));
            break;
    }
}

void motor_stop(void) {
    motor_set_speed(0);
    motor_set_direction(MOTOR_STOP);
}
