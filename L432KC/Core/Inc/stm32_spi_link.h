#ifndef STM32_SPI_LINK_H
#define STM32_SPI_LINK_H

#include "stm32l4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define TX_MAX 1024
#define RX_MAX 1024

extern SPI_HandleTypeDef *h_spi;
extern TIM_HandleTypeDef *h_tim;

typedef enum
{
	SEND_BYTE = 0x5A,
	POLL_BYTE = 0xA5,
	READ_BYTE = 0xA7
} opcode_t;

typedef enum
{
	STATUS_EMPTY = 0xFF,
	STATUS_READY = 0x01
} tx_state_t;

typedef enum
{
    ST_IDLE = 0,
    ST_RUN,
    ST_ERROR,
    ST_COUNT
} app_state_t;

void link_init(void);
void link_task(opcode_t opcode);
void link_send_string(const char* s);

void app_state_init(void);
const char* app_state_name(app_state_t s);
app_state_t link_get_state(void);
const char* link_get_state_str(void);

void link_on_timer_tick(void);

#endif

