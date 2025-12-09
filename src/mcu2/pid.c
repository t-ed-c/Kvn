#include "pid.h"

void pid_init(PIDController *pid, float kp, float ki, float kd) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    
    pid->integral = 0.0f;
    pid->last_error = 0.0f;
    
    pid->output_min = PID_OUTPUT_MIN;
    pid->output_max = PID_OUTPUT_MAX;
}

void pid_reset(PIDController *pid) {
    pid->integral = 0.0f;
    pid->last_error = 0.0f;
}

int16_t pid_compute(PIDController *pid, int16_t setpoint, int16_t measured) {
    // Calculate error
    float error = (float)(setpoint - measured);
    
    // Proportional term
    float p_term = pid->kp * error;
    
    // Integral term (with anti-windup)
    pid->integral += error;
    
    // Anti-windup: limit integral
    float max_integral = (float)pid->output_max / pid->ki;
    if (pid->integral > max_integral) {
        pid->integral = max_integral;
    } else if (pid->integral < -max_integral) {
        pid->integral = -max_integral;
    }
    
    float i_term = pid->ki * pid->integral;
    
    // Derivative term
    float derivative = error - pid->last_error;
    float d_term = pid->kd * derivative;
    
    // Save error for next iteration
    pid->last_error = error;
    
    // Calculate total output
    float output = p_term + i_term + d_term;
    
    // Clamp output to limits
    if (output > pid->output_max) {
        output = pid->output_max;
    } else if (output < pid->output_min) {
        output = pid->output_min;
    }
    
    return (int16_t)output;
}
