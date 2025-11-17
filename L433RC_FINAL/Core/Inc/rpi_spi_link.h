#ifndef RPI_SPI_LINK_H
#define RPI_SPI_LINK_H

#include <config.h>
#include <stdbool.h>

#include "stm32l4xx_hal.h"

// MAX SPI BUFFER SIZE
#define BUFFER_SIZE 1024

// MAX SPI PAYLOAD SIZE
#define PAYLOAD_SIZE BUFFER_SIZE - 4

void link_init(SPI_HandleTypeDef *spi, bool* packet_recieved, config_t* config);
uint32_t link_process_packet(uint8_t* ret_buf);

void rpi_printf(const char* msg, ...);
void rpi_send_debug_info(const uint8_t* data, uint16_t length);
void rpi_send_firmware_flash_request();
void rpi_press_power_button(void);
bool rpi_is_awake(void);



#endif

