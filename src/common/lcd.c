#include "lcd.h"
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>

// Private helper functions
static void lcd_enable_pulse(void) {
    LCD_PORT |= (1 << LCD_EN);
    _delay_us(1);
    LCD_PORT &= ~(1 << LCD_EN);
    _delay_us(50);
}

static void lcd_send_nibble(uint8_t nibble) {
    // Clear data pins
    LCD_PORT &= ~((1 << LCD_D4) | (1 << LCD_D5) | (1 << LCD_D6) | (1 << LCD_D7));
    
    // Set data pins
    if (nibble & 0x01) LCD_PORT |= (1 << LCD_D4);
    if (nibble & 0x02) LCD_PORT |= (1 << LCD_D5);
    if (nibble & 0x04) LCD_PORT |= (1 << LCD_D6);
    if (nibble & 0x08) LCD_PORT |= (1 << LCD_D7);
    
    lcd_enable_pulse();
}

void lcd_command(uint8_t cmd) {
    LCD_PORT &= ~(1 << LCD_RS);  // RS = 0 for command
    
    // Send high nibble
    lcd_send_nibble(cmd >> 4);
    
    // Send low nibble
    lcd_send_nibble(cmd & 0x0F);
    
    _delay_ms(2);
}

void lcd_data(uint8_t data) {
    LCD_PORT |= (1 << LCD_RS);  // RS = 1 for data
    
    // Send high nibble
    lcd_send_nibble(data >> 4);
    
    // Send low nibble
    lcd_send_nibble(data & 0x0F);
    
    _delay_us(50);
}

void lcd_init(void) {
    // Configure pins as output
    LCD_DDR |= (1 << LCD_RS) | (1 << LCD_EN) | 
               (1 << LCD_D4) | (1 << LCD_D5) | 
               (1 << LCD_D6) | (1 << LCD_D7);
    
    // Wait for LCD to power up
    _delay_ms(50);
    
    LCD_PORT &= ~(1 << LCD_RS);
    LCD_PORT &= ~(1 << LCD_EN);
    
    // Initialization sequence for 4-bit mode
    _delay_ms(15);
    lcd_send_nibble(0x03);
    _delay_ms(5);
    lcd_send_nibble(0x03);
    _delay_us(150);
    lcd_send_nibble(0x03);
    _delay_us(150);
    lcd_send_nibble(0x02);  // Switch to 4-bit mode
    _delay_us(150);
    
    // Function set: 4-bit mode, 2 lines, 5x8 dots
    lcd_command(LCD_FUNCTION_SET | LCD_4BIT_MODE | LCD_2LINE | LCD_5x8_DOTS);
    
    // Display control: display on, cursor off, blink off
    lcd_command(LCD_DISPLAY_CONTROL | LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF);
    
    // Clear display
    lcd_clear();
    
    // Entry mode: increment cursor, no shift
    lcd_command(LCD_ENTRY_MODE_SET | LCD_ENTRY_LEFT | LCD_ENTRY_SHIFT_DECREMENT);
    
    _delay_ms(2);
}

void lcd_clear(void) {
    lcd_command(LCD_CLEAR_DISPLAY);
    _delay_ms(2);
}

void lcd_home(void) {
    lcd_command(LCD_RETURN_HOME);
    _delay_ms(2);
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t row_offsets[] = {0x00, 0x40};
    if (row >= LCD_ROWS) row = LCD_ROWS - 1;
    if (col >= LCD_COLS) col = LCD_COLS - 1;
    
    lcd_command(LCD_SET_DDRAM_ADDR | (col + row_offsets[row]));
}

void lcd_print(const char *str) {
    while (*str) {
        lcd_data(*str++);
    }
}

void lcd_print_char(char c) {
    lcd_data(c);
}

void lcd_print_number(int16_t num) {
    char buffer[7];  // Enough for -32768 + null terminator
    itoa(num, buffer, 10);
    lcd_print(buffer);
}
