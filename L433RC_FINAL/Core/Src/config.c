#include <string.h>
#include "config.h"

static config_t config;

#define SOURCE_V 1.215f


static const config_t default_config = { // NAME, THRESHOLD
    .resistance_thresholds = {
        { "P1V2",    10000.0f },
        { "P1V8",    10000.0f },
        { "P3V3_A",  10000.0f },
        { "P3V3_IO", 10000.0f },
        { "P5V",     10000.0f },
        { "P10V",    10000.0f },
        { "PVMAIN",  10000.0f },
        { "SWD",     100000.0f },
        { "CAN",     100000.0f },
    },
    .voltage_tolerances = { // NAME, EXPECTED, TOLERANCE
        { "P1V2",   1.2f,  0.1f },
        { "P1V8",   1.8f,  0.1f },
        { "P3V3_A", 3.3f,  0.1f },
        { "P3V3_IO",3.3f,  0.1f },
        { "P5V",    5.0f,  0.1f },
        { "P10V",   10.0f, 0.1f },
        { "PVMAIN", 36.0f, 0.1f },
    }
};

static adc_result_t config_evaluate_resistance_threshold(float measurement, float threshold);
static adc_result_t config_evaluate_voltage_tolerance(float measurement, float expected, float tolerance);

// Public

/*
	Initializes config with default thresholds and tolerances
	@return Pointer to initialized config_t struct
*/
config_t* config_init() {
	memcpy(&config, &default_config, sizeof(config_t));
	return &config;
}

void config_apply(config_t* new_config) {
	memcpy(&config, new_config, sizeof(config_t));
}


/*
	Evaluates resistance measurements against configured thresholds
	@param measurements: Pointer to array of adc_measurement_t structs with resistance measurements
	@return boolean did_any_fail: true if any measurement failed, false if all passed
	@note: Modifies measurements array to convert raw voltage measurements to resistance values based on voltage divider configuration
*/
bool config_evaluate_resistances(adc_measurement_t* measurements) {
	for (int i = 0; i < NUM_RESISTANCE_CHANNELS; i++) {

		//avoid division by zero
		if (measurements[i].measurement >= SOURCE_V) {
			measurements[i].measurement = SOURCE_V - 0.0001f;
		}
		float gain = measurements[i].measurement / SOURCE_V;
		if (strcmp(measurements[i].name, "SWD") == 0 || strcmp(measurements[i].name, "CAN") == 0) {
			measurements[i].measurement = 100000.0f * (gain / (1.0f - gain));
		} else {
			measurements[i].measurement = 10000.0f * (gain / (1.0f - gain));
		}
	}

	uint32_t results = 0;
	for (int i = 0; i < NUM_RESISTANCE_CHANNELS; i++) {
		measurements[i].result = config_evaluate_resistance_threshold(measurements[i].measurement, config.resistance_thresholds[i].threshold);
		results |= (measurements[i].result << i);
	}

	return results;
}
	

/*
	Evaluates voltage measurements against configured expected values and tolerances
	@param measurements: Pointer to array of adc_measurement_t structs with voltage measurements
	@return boolean did_any_fail: true if any measurement failed, false if all passed
*/
bool config_evaluate_voltages(adc_measurement_t* measurements) {
	uint32_t results = 0;
	for (int i = 0; i < NUM_VOLTAGE_CHANNELS; i++) {
		measurements[i].result = config_evaluate_voltage_tolerance(measurements[i].measurement, config.voltage_tolerances[i].expected, config.voltage_tolerances[i].tolerance);
		results |= (measurements[i].result << i);
	}

	return results;
}

// Private

/*
	Measured Resistance must be > threshold to PASS
	@param measurement: Measured resistance value
	@param threshold: Configured resistance threshold
	@return adc_result_t: PASS or FAIL
*/
static adc_result_t config_evaluate_resistance_threshold(float measurement, float threshold) {
	if (measurement >= threshold) {
		return PASS;
	} else {
		return FAIL;
	}
}

/*
	Measured Voltage must be within expected +/- tolerance to PASS
	@param measurement: Measured voltage value
	@param expected: Configured expected voltage value
	@param tolerance: Configured voltage tolerance
	@return adc_result_t: PASS or FAIL
*/
static adc_result_t config_evaluate_voltage_tolerance(float measurement, float expected, float tolerance) {
	if (measurement >= (expected - tolerance) && measurement <= (expected + tolerance)) {
		return PASS;
	} else {
		return FAIL;
	}
}

