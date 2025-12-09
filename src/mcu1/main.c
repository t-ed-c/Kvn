#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include "../common/lcd.h"
#include "../common/protocol.h"
#include "i2c_master.h"
#include "keypad.h"

// Application states
typedef enum {
    APP_MODE_SPEED,
    APP_MODE_POSITION
} OperationMode;

// Global variables
static OperationMode current_mode = APP_MODE_SPEED;
static char input_buffer[6] = {0};
static uint8_t input_index = 0;

// Function prototypes
static void update_display(void);
static void clear_input_buffer(void);
static void send_command(uint8_t mode, uint16_t value);
static void send_emergency_stop(void);
static void handle_digit(char digit);
static void handle_confirm(void);
static void toggle_mode(void);

int main(void) {
    // Initialize peripherals
    lcd_init();
    keypad_init();
    i2c_master_init();
    
    // Enable global interrupts
    sei();
    
    // Display initial mode
    clear_input_buffer();
    update_display();
    
    // Main loop
    while (1) {
        uint8_t key = keypad_read();
        
        if (key == KEY_NONE) {
            continue;
        }
        
        char key_char = keypad_to_char(key);
        
        // Handle different key types
        if (key_char == 'X') {
            // Emergency stop
            send_emergency_stop();
            lcd_clear();
            lcd_set_cursor(0, 0);
            lcd_print("Emergency Sent!");
            _delay_ms(1000);
            clear_input_buffer();
            update_display();
        }
        else if (key_char == '+') {
            // Toggle mode
            toggle_mode();
        }
        else if (key_char == 'C') {
            // Confirm and send
            handle_confirm();
        }
        else if (key_char >= '0' && key_char <= '9') {
            // Digit input
            handle_digit(key_char);
        }
        
        _delay_ms(10);
    }
    
    return 0;
}

static void clear_input_buffer(void) {
    memset(input_buffer, 0, sizeof(input_buffer));
    input_index = 0;
}

static void update_display(void) {
    lcd_clear();
    
    // First line: Mode
    lcd_set_cursor(0, 0);
    lcd_print("Mode: ");
    if (current_mode == APP_MODE_SPEED) {
        lcd_print("M-Speed");
    } else {
        lcd_print("M-Pos");
    }
    
    // Second line: Setpoint
    lcd_set_cursor(1, 0);
    if (current_mode == APP_MODE_SPEED) {
        lcd_print("Speed:");
    } else {
        lcd_print("Pos:");
    }
    
    // Display current input or spaces
    lcd_set_cursor(1, 6);
    if (input_index > 0) {
        for (uint8_t i = 0; i < input_index && i < 5; i++) {
            lcd_print_char(input_buffer[i]);
        }
    } else {
        lcd_print("___");
    }
    
    // Display unit
    lcd_set_cursor(1, 12);
    if (current_mode == APP_MODE_SPEED) {
        lcd_print("RPM");
    } else {
        lcd_print("REV");
    }
}

static void handle_digit(char digit) {
    if (input_index < 5) {  // Maximum 5 digits (up to 65535)
        input_buffer[input_index++] = digit;
        update_display();
    }
}

static void toggle_mode(void) {
    if (current_mode == APP_MODE_SPEED) {
        current_mode = APP_MODE_POSITION;
    } else {
        current_mode = APP_MODE_SPEED;
    }
    clear_input_buffer();
    update_display();
}

static void handle_confirm(void) {
    if (input_index == 0) {
        // No input, do nothing
        return;
    }
    
    // Convert input to number
    uint16_t value = atoi(input_buffer);
    
    // Send command via I2C
    uint8_t mode = (current_mode == APP_MODE_SPEED) ? MODE_SPEED : MODE_POSITION;
    send_command(mode, value);
    
    // Display confirmation
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Sent to MCU2!");
    lcd_set_cursor(1, 0);
    if (current_mode == APP_MODE_SPEED) {
        lcd_print("Speed:");
    } else {
        lcd_print("Pos:");
    }
    lcd_set_cursor(1, 6);
    lcd_print(input_buffer);
    
    _delay_ms(1500);
    
    // Clear for next input
    clear_input_buffer();
    update_display();
}

static void send_command(uint8_t mode, uint16_t value) {
    I2CPacket packet;
    packet.start = PROTOCOL_START_BYTE;
    packet.mode = mode;
    pack_value(&packet, value);
    packet.checksum = calculate_checksum(packet.mode, packet.value_high, packet.value_low);
    
    // Send packet via I2C
    i2c_send_packet(MCU2_I2C_ADDRESS, (uint8_t*)&packet, PROTOCOL_PACKET_SIZE);
}

static void send_emergency_stop(void) {
    I2CPacket packet;
    packet.start = PROTOCOL_START_BYTE;
    packet.mode = MODE_EMERGENCY;
    pack_value(&packet, 0);
    packet.checksum = calculate_checksum(packet.mode, packet.value_high, packet.value_low);
    
    // Send emergency stop packet
    i2c_send_packet(MCU2_I2C_ADDRESS, (uint8_t*)&packet, PROTOCOL_PACKET_SIZE);
}
