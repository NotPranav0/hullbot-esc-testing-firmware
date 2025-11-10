#ifndef LCD_H
#define LCD_H


#include "stm32l4xx_hal.h"

#define NHD_STARTUP_DELAY 	500
#define NHD_I2C_TIMEOUT   	10
#define I2C_ADDR 			0x28
#define LINE1_COL1			0x00
#define LINE2_COL1			0x40


void lcd_init(I2C_HandleTypeDef* i2c);
void lcd_write_string(char* str);
void lcd_set_cursor(uint8_t position);

void lcd_write_byte(uint8_t data);
void lcd_prefix();
void lcd_display_on();
void lcd_display_off();
void lcd_home();
void lcd_clear_screen();
void lcd_set_contrast(uint8_t contrast);
void lcd_set_brightness(uint8_t brightness);
void lcd_firmware();


#endif /* LCD_H */
