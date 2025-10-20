/*
 * app.h
 *
 *  Created on: Oct 3, 2025
 *      Author: pranav.budhwar
 */

#ifndef INC_APP_H_
#define INC_APP_H_

#include "stm32l4xx_hal.h"

void app_init(SPI_HandleTypeDef* spi, TIM_HandleTypeDef* tim, UART_HandleTypeDef* uart);
void app_main();



#endif /* INC_APP_H_ */
