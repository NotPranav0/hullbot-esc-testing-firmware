#ifndef LCD_H
#define LCD_H


#include "stm32l4xx_hal.h"

#define NHD_STARTUP_DELAY 	500
#define NHD_I2C_TIMEOUT   	10
#define I2C_ADDR 			0x28

typedef enum {
    LCD_LINE_1 = 0x00,
    LCD_LINE_2 = 0x40
} LCD_Line_t;

void lcd_init(I2C_HandleTypeDef* i2c);
void lcd_print(const char* msg, LCD_Line_t line, ...);

#endif /* LCD_H */
