#ifndef LCD_H
#define LCD_H


#include "stm32l4xx_hal.h"

typedef enum {
    LCD_LINE_1 = 0x00,
    LCD_LINE_2 = 0x40
} LCD_Line_t;

void lcd_init(I2C_HandleTypeDef* i2c);
void lcd_printf(LCD_Line_t line, const char* msg, ...);
void lcd_clear_screen();

#endif /* LCD_H */
