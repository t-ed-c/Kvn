#ifndef MOTOR_H
#define MOTOR_H

#include <avr/io.h>
#include <stdint.h>

// Motor driver pin configuration (L293D/L298)
// Using Timer1 for PWM generation
#define MOTOR_PWM_DDR  DDRD
#define MOTOR_PWM_PORT PORTD
#define MOTOR_PWM_PIN  PD5     // OC1A - Timer1 PWM output

#define MOTOR_DIR_DDR  DDRD
#define MOTOR_DIR_PORT PORTD
#define MOTOR_DIR1_PIN PD6     // Direction control 1
#define MOTOR_DIR2_PIN PD7     // Direction control 2

// Motor direction
typedef enum {
    MOTOR_STOP = 0,
    MOTOR_FORWARD,
    MOTOR_BACKWARD
} MotorDirection;

// Function prototypes
void motor_init(void);
void motor_set_speed(uint8_t speed);
void motor_set_direction(MotorDirection dir);
void motor_stop(void);

#endif // MOTOR_H
