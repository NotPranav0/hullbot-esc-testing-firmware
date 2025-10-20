/*
 * app.c
 *
 *  Created on: Oct 3, 2025
 *      Author: pranav.budhwar
 */

#include "app.h"
#include "main.h"
#include "stm32_spi_link.h"
#include <stdio.h>

SPI_HandleTypeDef *h_spi;

TIM_HandleTypeDef *h_tim;

UART_HandleTypeDef *h_uart;

void app_init(SPI_HandleTypeDef *spi, TIM_HandleTypeDef *tim,
		UART_HandleTypeDef *uart)
{
	h_spi = spi;
	h_tim = tim;
	h_uart = uart;

	// disable printf buffering
	setvbuf(stdout, NULL, _IONBF, 0);

	link_init();
	HAL_TIM_Base_Start_IT(h_tim);

	app_main();
}
void app_main()
{
	link_send_string("HELLO_FROM_STM32");
	opcode_t opcode;

	while (1)
	{
		if (HAL_SPI_Receive(h_spi, &opcode, 1, 2) == HAL_OK)
		{
			link_task(opcode);
		}
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == h_tim)
	{
		link_on_timer_tick();
	}
}



int _write(int file, char *ptr, int len)
{
	if (h_uart->gState != HAL_UART_STATE_RESET)
	{
		HAL_UART_Transmit(h_uart, (uint8_t*) ptr, (uint16_t) len, 50);
	}
	return len;
}

