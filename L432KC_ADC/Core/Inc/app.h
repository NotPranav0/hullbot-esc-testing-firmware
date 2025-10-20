#ifndef INC_APP_H_
#define INC_APP_H_

#include "stm32l4xx_hal.h"


void app_init(ADC_HandleTypeDef* adc, UART_HandleTypeDef *uart);
void app_main();

#endif /* INC_APP_H_ */
