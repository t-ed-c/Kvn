#include "keypad.h"
#include <util/delay.h>

void keypad_init(void) {
    // Configure keypad pins as input
    KEYPAD_DDR &= ~((1 << KEYPAD_DA_PIN) | (1 << KEYPAD_D0_PIN) | 
                    (1 << KEYPAD_D1_PIN) | (1 << KEYPAD_D2_PIN) | 
                    (1 << KEYPAD_D3_PIN));
    
    // Enable pull-ups (optional, MM74C922 has internal latches)
    PORTA |= (1 << KEYPAD_DA_PIN) | (1 << KEYPAD_D0_PIN) | 
             (1 << KEYPAD_D1_PIN) | (1 << KEYPAD_D2_PIN) | 
             (1 << KEYPAD_D3_PIN);
}

uint8_t keypad_available(void) {
    // Check if Data Available pin is high
    return (KEYPAD_PORT & (1 << KEYPAD_DA_PIN)) ? 1 : 0;
}

uint8_t keypad_read(void) {
    // Check if key is available
    if (!keypad_available()) {
        return KEY_NONE;
    }
    
    // Read the 4-bit encoded key value
    uint8_t key = 0;
    if (KEYPAD_PORT & (1 << KEYPAD_D0_PIN)) key |= 0x01;
    if (KEYPAD_PORT & (1 << KEYPAD_D1_PIN)) key |= 0x02;
    if (KEYPAD_PORT & (1 << KEYPAD_D2_PIN)) key |= 0x04;
    if (KEYPAD_PORT & (1 << KEYPAD_D3_PIN)) key |= 0x08;
    
    // Debounce delay
    _delay_ms(50);
    
    // Wait for key release (DA goes low)
    while (keypad_available()) {
        _delay_ms(10);
    }
    
    // Additional debounce after release
    _delay_ms(50);
    
    return key;
}

char keypad_to_char(uint8_t key) {
    switch (key) {
        case KEY_0: return '0';
        case KEY_1: return '1';
        case KEY_2: return '2';
        case KEY_3: return '3';
        case KEY_4: return '4';
        case KEY_5: return '5';
        case KEY_6: return '6';
        case KEY_7: return '7';
        case KEY_8: return '8';
        case KEY_9: return '9';
        case KEY_A: return 'A';
        case KEY_B: return 'B';
        case KEY_C: return 'C';  // ON/C - Confirm
        case KEY_D: return 'X';  // Emergency
        case KEY_STAR: return '*';
        case KEY_HASH: return '+';  // Toggle mode
        default: return '\0';
    }
}
