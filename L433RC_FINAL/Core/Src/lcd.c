#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "lcd.h"

#define NUM_CHARS 			16

#define STARTUP_DELAY 		500
#define I2C_TIMEOUT   		10

#define I2C_ADDR 			0x28

#define PREFIX 				0xFE
#define SET_CURSOR_CMD 		0x45
#define CLEAR_SCREEN_CMD 	0x51
#define CONTRAST_CMD 		0x52
#define BRIGHTNESS_CMD		0x53

#define CONTRAST_DEFAULT 	40 // 1-50
#define BRIGHTNESS_DEFAULT 	7 // 1-8 (1 = OFF, 8 = MAX)
	
#define PRINTF_MAX_BUFFER 	80   
#define SCROLL_DELAY_MS 	500
#define SCROLL_PAUSE_MS 	1000   

static I2C_HandleTypeDef* h_i2c;

static void lcd_set_cursor(uint8_t position);
static void lcd_write_byte(uint8_t data);
static void lcd_write_string(char* str);
static void lcd_prefix();
static void lcd_set_contrast(uint8_t contrast);
static void lcd_set_brightness(uint8_t brightness);



// Public 

/*
	Initialize LCD
	@param i2c: Pointer to I2C handle
*/

void lcd_init(I2C_HandleTypeDef* i2c) {
	h_i2c = i2c;

    // Wait for display to power ON
    HAL_Delay(STARTUP_DELAY);

    lcd_clear_screen();
    lcd_set_contrast(CONTRAST_DEFAULT);
    lcd_set_brightness(BRIGHTNESS_DEFAULT);
}

/*
	Printf-style function to write formatted string to LCD
	@param line: LCD line to write to (LCD_LINE_1 or LCD_LINE_2)
	@param msg: Format string (like printf)
	@note If string exceeds display width, it will scroll
*/

void lcd_printf(LCD_Line_t line, const char* msg, ...) {
    char formatted_string[PRINTF_MAX_BUFFER];
    char frame_buffer[NUM_CHARS + 1];
    
    
	va_list args;
	va_start(args, msg);
	vsnprintf(formatted_string, sizeof(formatted_string), msg, args);
	va_end(args);

    size_t len = strlen(formatted_string);
    
    lcd_set_cursor(line);

	if (len <= NUM_CHARS) {
		snprintf(frame_buffer, sizeof(frame_buffer), "%-*.*s", NUM_CHARS, NUM_CHARS, formatted_string);
        
		lcd_write_string(frame_buffer);
	} 
    else {
		for (size_t i = 0; i <= (len - NUM_CHARS); i++) {
			strncpy(frame_buffer, &formatted_string[i], NUM_CHARS);
			frame_buffer[NUM_CHARS] = '\0'; 

			lcd_set_cursor(line);
			lcd_write_string(frame_buffer);

			if (i == 0) {
				HAL_Delay(SCROLL_PAUSE_MS); 
			} else {
				HAL_Delay(SCROLL_DELAY_MS);
			}
		}

		HAL_Delay(SCROLL_PAUSE_MS);
	}
}


/*
	Clear LCD screen
*/
void lcd_clear_screen() {
	lcd_prefix();
	lcd_write_byte(CLEAR_SCREEN_CMD);
	HAL_Delay(2);
}

// Private functions

/*
	Write a byte to the LCD
	@param data: Byte to write
*/
static void lcd_write_byte(uint8_t data) {
    uint8_t pData[1];
    pData[0] = data;

    uint16_t dev_addr = I2C_ADDR << 1;

    HAL_I2C_Master_Transmit(h_i2c, dev_addr, pData, 1, I2C_TIMEOUT);

    HAL_Delay(2);
}

/*
	Write a string to the LCD
	@param str: String to write
*/
static void lcd_write_string(char* str) {
    while (*str != '\0') {
    	lcd_write_byte((uint8_t)*str);
        str++;
    }
}

/*
	Write the prefix byte to the LCD
*/
static void lcd_prefix() {
    lcd_write_byte(PREFIX);
}

/*
	Set cursor position on LCD
	@param position: Position to set cursor to (0-15 for line 1, 64-79 for line 2)
*/
static void lcd_set_cursor(uint8_t position) {
	lcd_prefix();
	lcd_write_byte(SET_CURSOR_CMD);
	lcd_write_byte(position);
}

/*
	Set contrast on LCD
	@param contrast: Contrast level (1-50)
*/
static void lcd_set_contrast(uint8_t contrast) {
	lcd_prefix();
	lcd_write_byte(CONTRAST_CMD);
	lcd_write_byte(contrast);
}

/*
	Set brightness on LCD
	@param brightness: Brightness level (1-8)
*/
static void lcd_set_brightness(uint8_t brightness) {
	lcd_prefix();
	lcd_write_byte(BRIGHTNESS_CMD);
	lcd_write_byte(brightness);
}
