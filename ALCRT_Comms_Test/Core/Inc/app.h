/*
 * app.h
 *
 *  Created on: May 15, 2025
 *      Author: pranav.budhwar
 */

#ifndef INC_APP_H_
#define INC_APP_H_

#include "stm32l4xx_hal.h"


#define TX



void bled_toggle();

void app_init(I2C_HandleTypeDef* i2c_ref, SPI_HandleTypeDef* spi_ref, UART_HandleTypeDef* uart_ref, CAN_HandleTypeDef* can_ref);

void UART_Printf(const char* fmt, ...);

#endif /* INC_APP_H_ */
