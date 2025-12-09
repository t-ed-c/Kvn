#ifndef KEYPAD_H
#define KEYPAD_H

#include <avr/io.h>
#include <stdint.h>

// MM74C922 Keypad Encoder Pin Configuration
#define KEYPAD_PORT      PINA
#define KEYPAD_DDR       DDRA
#define KEYPAD_DA_PIN    PA0  // Data Available (active high when key pressed)
#define KEYPAD_D0_PIN    PA1  // Data bit 0
#define KEYPAD_D1_PIN    PA2  // Data bit 1
#define KEYPAD_D2_PIN    PA3  // Data bit 2
#define KEYPAD_D3_PIN    PA4  // Data bit 3

// Key definitions (4x4 keypad layout)
#define KEY_1      0x01
#define KEY_2      0x02
#define KEY_3      0x03
#define KEY_A      0x04  // 'A' key
#define KEY_4      0x05
#define KEY_5      0x06
#define KEY_6      0x07
#define KEY_B      0x08  // 'B' key
#define KEY_7      0x09
#define KEY_8      0x0A
#define KEY_9      0x0B
#define KEY_C      0x0C  // 'C' key (ON/C - Confirm)
#define KEY_STAR   0x0D  // '*' key
#define KEY_0      0x0E
#define KEY_HASH   0x0F  // '#' key (+ - Toggle mode)
#define KEY_D      0x00  // 'D' key (X - Emergency)

#define KEY_NONE   0xFF

// Function prototypes
void keypad_init(void);
uint8_t keypad_read(void);
uint8_t keypad_available(void);
char keypad_to_char(uint8_t key);

#endif // KEYPAD_H
