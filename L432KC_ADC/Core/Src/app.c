
#include "app.h"
#include "main.h"
#include <stdio.h>

ADC_HandleTypeDef* h_adc;
UART_HandleTypeDef *h_uart;

const uint8_t ADC_BITS = 12;
const float MAX_VOLTS = 3.3;
const uint16_t MAX_ADC_VAL = (1 << ADC_BITS) - 1;

void app_init(ADC_HandleTypeDef* adc, UART_HandleTypeDef *uart) {
	h_adc = adc;
	h_uart = uart;

	setvbuf(stdout, NULL, _IONBF, 0);
	HAL_Delay(1000);
	if (HAL_ADCEx_Calibration_Start(h_adc, ADC_SINGLE_ENDED) != HAL_OK)
	{
		printf("calibration failed\r\n");
	}
	printf("calibration success\r\n");
	app_main();
}


float raw_to_volts(uint32_t raw_value){
	printf("raw = %ld\r\n", raw_value);
	float scale_factor = (float)raw_value/(float)MAX_ADC_VAL;
	float volts = scale_factor * (float)MAX_VOLTS;
	printf("volts = %f\r\n", volts);
	return volts;
}

uint32_t adc_read_channel_blocking(uint32_t channel)
{
	ADC_ChannelConfTypeDef ADC_CH_Cfg = {0};
	ADC_CH_Cfg.Channel = channel;
	ADC_CH_Cfg.Rank = ADC_REGULAR_RANK_1;
	ADC_CH_Cfg.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
	ADC_CH_Cfg.SingleDiff = ADC_SINGLE_ENDED;
	ADC_CH_Cfg.OffsetNumber = ADC_OFFSET_NONE;
	ADC_CH_Cfg.Offset = 0;
	HAL_ADC_ConfigChannel(h_adc, &ADC_CH_Cfg);
	HAL_ADC_Start(h_adc);
	if (HAL_ADC_PollForConversion(h_adc, HAL_MAX_DELAY) != HAL_OK)
	{
		printf("ADC poll failed\r\n");
	}
	uint32_t raw = HAL_ADC_GetValue(h_adc);
	HAL_ADC_Stop(h_adc);
	return raw;
}

void app_main() {
	printf("RESET\r\n");
	while(1) {
		uint32_t val = adc_read_channel_blocking(ADC_CHANNEL_6);
		raw_to_volts(val);
		HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
		HAL_Delay(1000);
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

