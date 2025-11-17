#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "stm32l4xx_hal.h"
#include "esc.h"

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

typedef struct {
	char name[NAME_SIZE];
	float measurement;
    measurement_type_t type;
    adc_result_t result;
} adc_measurement_t;

// ---- Wire representation for SPI (fixed-size, no padding surprises) ----
#pragma pack(push, 1)
typedef struct {
    char    name[NAME_SIZE];   
    float   measurement;       
    uint8_t type;              // RESISTANCE / VOLTAGE
    uint8_t result;            // PASS / FAIL
} adc_measurement_wire_t;
#pragma pack(pop)

void adc_init(ADC_HandleTypeDef* adc);
void adc_set_1v2_source(esc_power_mode_t mode);
void adc_take_measurements(adc_measurement_t* measurements, measurement_type_t type);
void adc_measurements_to_wire(const adc_measurement_t* measurements, adc_measurement_wire_t* wire_measurements, int num_measurements);



#endif /* INC_ADC_H_ */
