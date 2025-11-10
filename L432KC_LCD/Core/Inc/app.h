#ifndef INC_APP_H_
#define INC_APP_H_

#include "main.h"
#include "stm32l4xx_hal.h"
#include <stdint.h>

void app_init(I2C_HandleTypeDef* h_i2c);
void app_main(void);


#endif /* INC_APP_H_ */
