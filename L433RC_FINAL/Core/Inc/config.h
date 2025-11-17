#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

#include <stdint.h>
#include "adc.h"

/*
see adc.h for 
NAME_SIZE
NUM_VOLTAGE_CHANNELS
NUM_RESISTANCE_CHANNELS
*/

// no padding bytes.
#pragma pack(push, 1)

typedef struct {
    char name[NAME_SIZE];
    float threshold;
} config_resistance_thresholds_t;

typedef struct {
    char name[NAME_SIZE];
    float expected;
    float tolerance;
} config_voltage_tolerances_t;

typedef struct {
    config_resistance_thresholds_t resistance_thresholds[NUM_RESISTANCE_CHANNELS];
    config_voltage_tolerances_t voltage_tolerances[NUM_VOLTAGE_CHANNELS];
} config_t;

#pragma pack(pop)

config_t* config_init();
void config_apply(config_t* new_config);
bool config_evaluate_resistances(adc_measurement_t* measurements);
bool config_evaluate_voltages(adc_measurement_t* measurements);



#endif /* INC_CONFIG_H_ */
