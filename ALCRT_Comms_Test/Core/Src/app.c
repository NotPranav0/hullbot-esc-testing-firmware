/*
 * app.c
 *
 *  Created on: May 15, 2025
 *      Author: pranav.budhwar
 */

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "app.h"
#include "main.h"
#include "lcd.h"
#include "sd.h"
#include "can.h"

void app_main(void);

static I2C_HandleTypeDef *hi2c;
static SPI_HandleTypeDef *hspi;
static UART_HandleTypeDef *huart;
static CAN_HandleTypeDef *hcan;

/****** UTILITY FN ******/

void bled_toggle() {
	HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
}

void bled_set(uint8_t enable) {
	if (enable) {
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
	} else {
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
	}
}

void UART_Printf(const char *fmt, ...) {
	char buff[256];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buff, sizeof(buff), fmt, args);
	HAL_UART_Transmit(huart, (uint8_t*) buff, strlen(buff), HAL_MAX_DELAY);
	va_end(args);
}

void app_init(I2C_HandleTypeDef *i2c_ref, SPI_HandleTypeDef *spi_ref,
		UART_HandleTypeDef *uart_ref, CAN_HandleTypeDef *can_ref) {
	hi2c = i2c_ref;
	hspi = spi_ref;
	huart = uart_ref;
	hcan = can_ref;

	//LCD_Init(hi2c);
	int ret = SD_Init(hspi);
	if (ret < 0) {
		while (1) {
		}
	}
	//can_init(hcan);
	//HAL_CAN_Start(hcan);
	app_main();
}

#define R
void app_main(void) {
//	uint8_t buf[512];
//	for (uint32_t idx1 = 0; idx1 < 128; idx1++) {
//		buf[idx1 * 4] = 0xDE;
//		buf[idx1 * 4 + 1] = 0xAD;
//		buf[idx1 * 4 + 2] = 0xBE;
//		buf[idx1 * 4 + 3] = 0xEF;
//	}
//
//#ifdef W
//	uint32_t addr = 0x1;
//	for (uint32_t idx = 0; idx < 6; idx++) {
//		uint8_t token;
//		sd_write_block(addr, buf, &token);
//		addr = addr << 1;
//	}
//	while(1) {
//		bled_toggle();
//		HAL_Delay(1000);
//	}
//#endif
//#ifdef R
//	uint32_t res = 0;
//	for (uint32_t idx = 0; idx < 6; idx++) {
//		uint8_t arr[512];
//		memset(arr, 0, 512);
//		uint32_t addr = 0x1;
//		uint8_t token;
//		sd_read_block(addr, arr, &token);
//		res += memcmp(buf, arr, 512);
//		addr = addr << 1;
//	}
//
//	if (!res) {
//		while (1) {
//			bled_toggle();
//			HAL_Delay(50);
//		}
//	}
//
//#endif
//	while (1) {
//	}

//	uint8_t data[4] = {0xAB, 0XCD,0XEF,0X00};
//	while (1) {
//#ifdef RX
//
//
//#endif
//#ifdef TX
//		can_set_data(data, sizeof(data));
//		can_send_frame(sizeof(data));
//		HAL_Delay(1000);
//
//#endif
		// cycle LCD LED
		LCD_Set_RGB(255,0,0);
		HAL_Delay(500);
		LCD_Set_RGB(0,255,0);
		HAL_Delay(500);
		LCD_Set_RGB(0,0,255);
		HAL_Delay(500);

		// write to LCD
		LCD_Set_RGB(255,255,255);
		LCD_Send_String(0, 0, "Hullbot");
		LCD_Send_String(0, 1, "Hello,World!");
		HAL_Delay(500);

		//clear LCD
		LCD_Clear();
		HAL_Delay(500);
}

