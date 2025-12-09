#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include "../common/lcd.h"
#include "../common/protocol.h"
#include "i2c_slave.h"
#include "motor.h"
#include "encoder.h"
#include "pid.h"

#define F_CPU 16000000UL

// Control modes
typedef enum {
    CTRL_IDLE,
    CTRL_SPEED,
    CTRL_POSITION,
    CTRL_EMERGENCY
} ControlMode;

// Global variables
static volatile ControlMode control_mode = CTRL_IDLE;
static volatile uint16_t target_value = 0;
static volatile uint8_t emergency_stop = 0;
static PIDController speed_pid;
static uint16_t current_rpm = 0;
static int32_t current_position = 0;

// Function prototypes
static void i2c_receive_handler(const uint8_t *data, uint8_t length);
static void update_display(void);
static void control_loop(void);
static void speed_control(void);
static void position_control(void);

int main(void) {
    // Initialize peripherals
    lcd_init();
    motor_init();
    encoder_init();
    
    // Initialize PID controller
    pid_init(&speed_pid, PID_KP, PID_KI, PID_KD);
    
    // Initialize I2C slave
    i2c_slave_init(I2C_SLAVE_ADDRESS);
    i2c_slave_set_receive_callback(i2c_receive_handler);
    
    // Enable global interrupts
    sei();
    
    // Display initial state
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("MCU2 Ready");
    lcd_set_cursor(1, 0);
    lcd_print("Waiting...");
    
    // Timer variable for control loop
    uint16_t loop_count = 0;
    
    // Main loop
    while (1) {
        // Run control loop
        control_loop();
        
        // Update display every ~200ms
        loop_count++;
        if (loop_count >= 20) {
            loop_count = 0;
            update_display();
        }
        
        _delay_ms(10);
    }
    
    return 0;
}

static void i2c_receive_handler(const uint8_t *data, uint8_t length) {
    if (length != PROTOCOL_PACKET_SIZE) {
        return;
    }
    
    I2CPacket packet;
    memcpy(&packet, data, PROTOCOL_PACKET_SIZE);
    
    // Validate packet
    if (!validate_packet(&packet)) {
        return;
    }
    
    // Process command
    switch (packet.mode) {
        case MODE_SPEED:
            if (!emergency_stop) {
                control_mode = CTRL_SPEED;
                target_value = unpack_value(&packet);
                pid_reset(&speed_pid);
                encoder_reset();
            }
            break;
            
        case MODE_POSITION:
            if (!emergency_stop) {
                control_mode = CTRL_POSITION;
                target_value = unpack_value(&packet);
                encoder_reset();
            }
            break;
            
        case MODE_EMERGENCY:
            control_mode = CTRL_EMERGENCY;
            emergency_stop = 1;
            motor_stop();
            break;
            
        default:
            break;
    }
}

static void control_loop(void) {
    if (emergency_stop) {
        motor_stop();
        return;
    }
    
    switch (control_mode) {
        case CTRL_SPEED:
            speed_control();
            break;
            
        case CTRL_POSITION:
            position_control();
            break;
            
        case CTRL_IDLE:
        case CTRL_EMERGENCY:
        default:
            motor_stop();
            break;
    }
}

static void speed_control(void) {
    static uint32_t last_rpm_calc = 0;
    static uint32_t time_counter = 0;
    
    time_counter++;
    
    // Calculate RPM every 100ms
    if (time_counter - last_rpm_calc >= 10) {
        uint32_t time_diff = (time_counter - last_rpm_calc) * 10;  // in ms
        current_rpm = encoder_calculate_rpm(time_diff);
        last_rpm_calc = time_counter;
        
        // Run PID controller
        int16_t pid_output = pid_compute(&speed_pid, target_value, current_rpm);
        
        // Set motor speed
        motor_set_direction(MOTOR_FORWARD);
        motor_set_speed((uint8_t)pid_output);
    }
}

static void position_control(void) {
    // Get current position in revolutions
    current_position = encoder_get_revolutions();
    
    // Check if target reached
    if (current_position >= (int32_t)target_value) {
        motor_stop();
        control_mode = CTRL_IDLE;
    } else {
        // Simple bang-bang control for position
        // Could be enhanced with PID for smoother motion
        motor_set_direction(MOTOR_FORWARD);
        motor_set_speed(150);  // Medium speed for position control
    }
}

static void update_display(void) {
    lcd_clear();
    
    if (emergency_stop) {
        lcd_set_cursor(0, 0);
        lcd_print("Emergency!");
        lcd_set_cursor(1, 0);
        lcd_print("Stop!");
        return;
    }
    
    switch (control_mode) {
        case CTRL_SPEED:
            lcd_set_cursor(0, 0);
            lcd_print("M-Speed->");
            lcd_print_number(target_value);
            lcd_print(" RPM");
            
            lcd_set_cursor(1, 0);
            lcd_print("Cur:");
            lcd_print_number(current_rpm);
            lcd_print("    RPM");
            break;
            
        case CTRL_POSITION:
            current_position = encoder_get_revolutions();
            
            lcd_set_cursor(0, 0);
            lcd_print("M-Pos->");
            lcd_print_number(target_value);
            lcd_print("   REV");
            
            lcd_set_cursor(1, 0);
            lcd_print("Cur:");
            lcd_print_number(current_position);
            lcd_print("      REV");
            break;
            
        case CTRL_IDLE:
        default:
            lcd_set_cursor(0, 0);
            lcd_print("MCU2 Ready");
            lcd_set_cursor(1, 0);
            lcd_print("Waiting...");
            break;
    }
}
