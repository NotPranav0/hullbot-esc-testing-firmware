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

static SPI_HandleTypeDef *h_spi;
static UART_HandleTypeDef *h_uart;
static ADC_HandleTypeDef* h_adc;
static DAC_HandleTypeDef* h_dac;

void app_init(SPI_HandleTypeDef* spi, UART_HandleTypeDef* uart,
		ADC_HandleTypeDef* adc, DAC_HandleTypeDef* dac)
{
	h_spi = spi;
	h_uart = uart;
	h_adc = adc;
	h_dac = dac;

	// disable printf buffering
	setvbuf(stdout, NULL, _IONBF, 0);

	link_init(h_spi);
	app_main();
}
void app_main()
{
//	char* message = "Hello!!!!";
//	send_packet_to_pi(LOG_MESSAGE, (uint8_t*)message, strlen(message));
	while (1)
	{

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

