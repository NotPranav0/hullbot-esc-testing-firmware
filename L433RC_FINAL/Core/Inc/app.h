#ifndef INC_APP_H_
#define INC_APP_H_

#include "main.h"
#include "stm32l4xx_hal.h"
#include <stdint.h>

void app_init(ADC_HandleTypeDef* h_adc, CAN_HandleTypeDef* h_can, SPI_HandleTypeDef* h_spi, I2C_HandleTypeDef* h_i2c);


void error(const char* msg, ...);

#endif /* INC_APP_H_ */
