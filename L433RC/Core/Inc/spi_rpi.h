#ifndef INC_RPI_H_
#define INC_RPI_H_


#include "stm32l4xx_hal.h"
#include <stdbool.h>
#include <stdint.h>

void rpi_init(SPI_HandleTypeDef* h_spi);

bool rpi_is_awake();
void rpi_wake();
void rpi_sleep();

void rpi_transmit(uint8_t* data, uint16_t size);
void rpi_receive(uint8_t* data, uint16_t size);
void rpi_printf(const char* fmt, ...);



#endif /* INC_RPI_H_ */
