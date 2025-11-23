#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "stm32l4xx_hal.h"
#include "esc.h"
#include <stdbool.h>

#define NUM_VOLTAGE_CHANNELS 7
#define NUM_RESISTANCE_CHANNELS 9
#define NAME_SIZE 10

typedef enum {
    PASS = 0,
    FAIL = 1
} adc_result_t;

typedef enum {
	RESISTANCE = 0,
	VOLTAGE = 1
} measurement_type_t;

#pragma pack(push, 1)
typedef struct {
	char name[NAME_SIZE];
	float measurement;
    uint8_t type;
    uint8_t result;
    bool esc_connected;
} adc_measurement_t;
#pragma pack(pop)

void adc_init(ADC_HandleTypeDef* adc);
void adc_set_1v2_source(esc_power_mode_t mode);
void adc_take_measurements(adc_measurement_t* measurements, measurement_type_t type);



#endif /* INC_ADC_H_ */
