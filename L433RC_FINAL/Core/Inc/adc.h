#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "stm32l4xx_hal.h"

#define NUM_VOLTAGE_CHANNELS 7
#define NUM_RESISTANCE_CHANNELS 9

void adc_init(ADC_HandleTypeDef* adc);
void adc_take_resistance_measurements(float* measurements);
void adc_take_voltage_measurements(float* measurements);

uint32_t adc_read_channel_blocking(uint32_t channel);

#endif /* INC_ADC_H_ */
