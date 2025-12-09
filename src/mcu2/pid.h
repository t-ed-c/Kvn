#ifndef PID_H
#define PID_H

#include <stdint.h>

// PID constants (configurable)
#define PID_KP 1.0f
#define PID_KI 0.1f
#define PID_KD 0.05f

// PID limits
#define PID_OUTPUT_MIN 0
#define PID_OUTPUT_MAX 255  // PWM duty cycle max

typedef struct {
    float kp;
    float ki;
    float kd;
    
    float integral;
    float last_error;
    
    int16_t output_min;
    int16_t output_max;
} PIDController;

// Function prototypes
void pid_init(PIDController *pid, float kp, float ki, float kd);
void pid_reset(PIDController *pid);
int16_t pid_compute(PIDController *pid, int16_t setpoint, int16_t measured);

#endif // PID_H
