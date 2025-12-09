#ifndef LCD_H
#define LCD_H

#include <avr/io.h>
#include <stdint.h>

// LCD Pin Configuration
// Adjust these according to your hardware connections
#define LCD_PORT PORTB
#define LCD_DDR  DDRB
#define LCD_RS   PB0
#define LCD_EN   PB1
#define LCD_D4   PB4
#define LCD_D5   PB5
#define LCD_D6   PB6
#define LCD_D7   PB7

// LCD Commands
#define LCD_CLEAR_DISPLAY        0x01
#define LCD_RETURN_HOME          0x02
#define LCD_ENTRY_MODE_SET       0x04
#define LCD_DISPLAY_CONTROL      0x08
#define LCD_CURSOR_SHIFT         0x10
#define LCD_FUNCTION_SET         0x20
#define LCD_SET_CGRAM_ADDR       0x40
#define LCD_SET_DDRAM_ADDR       0x80

// Entry Mode
#define LCD_ENTRY_RIGHT          0x00
#define LCD_ENTRY_LEFT           0x02
#define LCD_ENTRY_SHIFT_INCREMENT 0x01
#define LCD_ENTRY_SHIFT_DECREMENT 0x00

// Display Control
#define LCD_DISPLAY_ON           0x04
#define LCD_DISPLAY_OFF          0x00
#define LCD_CURSOR_ON            0x02
#define LCD_CURSOR_OFF           0x00
#define LCD_BLINK_ON             0x01
#define LCD_BLINK_OFF            0x00

// Function Set
#define LCD_8BIT_MODE            0x10
#define LCD_4BIT_MODE            0x00
#define LCD_2LINE                0x08
#define LCD_1LINE                0x00
#define LCD_5x10_DOTS            0x04
#define LCD_5x8_DOTS             0x00

// LCD dimensions
#define LCD_ROWS 2
#define LCD_COLS 16

// Function prototypes
void lcd_init(void);
void lcd_clear(void);
void lcd_home(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_print(const char *str);
void lcd_print_char(char c);
void lcd_print_number(int16_t num);
void lcd_command(uint8_t cmd);
void lcd_data(uint8_t data);

#endif // LCD_H
