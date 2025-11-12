#include "config.h"

static Config_t config;

const float P1V2_R_THRESHOLD_DEFAULT = 0;
const float P1V8_R_THRESHOLD_DEFAULT = 0;
const float P3V3A_R_THRESHOLD_DEFAULT = 0;
const float P3V3IO_R_THRESHOLD_DEFAULT = 0;
const float P5V_R_THRESHOLD_DEFAULT = 0;
const float P10V_R_THRESHOLD_DEFAULT = 0;
const float PVMAIN_R_THRESHOLD_DEFAULT = 0;

const float SWD_R_THRESHOLD_DEFAULT = 0;
const float CAN_R_THRESHOLD_DEFAULT = 0;


const float P1V2_V_EXPECTED_DEFAULT = 0;
const float P1V8_V_EXPECTED_DEFAULT = 0;
const float P3V3A_V_EXPECTED_DEFAULT = 0;
const float P3V3IO_V_EXPECTED_DEFAULT = 0;
const float P5V_V_EXPECTED_DEFAULT = 0;
const float P10V_V_EXPECTED_DEFAULT = 0;
const float PVMAIN_V_EXPECTED_DEFAULT = 0;

const float P1V2_V_TOLERANCE_DEFAULT = 0;
const float P1V8_V_TOLERANCE_DEFAULT = 0;
const float P3V3A_V_TOLERANCE_DEFAULT = 0;
const float P3V3IO_V_TOLERANCE_DEFAULT = 0;
const float P5V_V_TOLERANCE_DEFAULT = 0;
const float P10V_V_TOLERANCE_DEFAULT = 0;
const float PVMAIN_V_TOLERANCE_DEFAULT = 0;

static uint32_t config_evaluate_resistance_threshold(float measurement, float threshold);
static uint32_t config_evaluate_voltage_tolerance(float measurement, float expected, float tolerance);

// Public

Config_t* config_init() {
	config.P1V2_R_THRESHOLD = P1V2_R_THRESHOLD_DEFAULT;
	config.P1V8_R_THRESHOLD = P1V8_R_THRESHOLD_DEFAULT;
	config.P3V3A_R_THRESHOLD = P3V3A_R_THRESHOLD_DEFAULT;
	config.P3V3IO_R_THRESHOLD = P3V3IO_R_THRESHOLD_DEFAULT;
	config.P5V_R_THRESHOLD = P5V_R_THRESHOLD_DEFAULT;
	config.P10V_R_THRESHOLD = P10V_R_THRESHOLD_DEFAULT;
	config.PVMAIN_R_THRESHOLD = PVMAIN_R_THRESHOLD_DEFAULT;
	config.SWD_R_THRESHOLD = SWD_R_THRESHOLD_DEFAULT;
	config.CAN_R_THRESHOLD = CAN_R_THRESHOLD_DEFAULT;

	config.P1V2_V_EXPECTED = P1V2_V_EXPECTED_DEFAULT;
	config.P1V8_V_EXPECTED = P1V8_V_EXPECTED_DEFAULT;
	config.P3V3A_V_EXPECTED = P3V3A_V_EXPECTED_DEFAULT;
	config.P3V3IO_V_EXPECTED = P3V3IO_V_EXPECTED_DEFAULT;
	config.P5V_V_EXPECTED = P5V_V_EXPECTED_DEFAULT;
	config.P10V_V_EXPECTED = P10V_V_EXPECTED_DEFAULT;
	config.PVMAIN_V_EXPECTED = PVMAIN_V_EXPECTED_DEFAULT;


	config.P1V2_V_TOLERANCE = P1V2_V_TOLERANCE_DEFAULT;
	config.P1V8_V_TOLERANCE = P1V8_V_TOLERANCE_DEFAULT;
	config.P3V3A_V_TOLERANCE = P3V3A_V_TOLERANCE_DEFAULT;
	config.P3V3IO_V_TOLERANCE = P3V3IO_V_TOLERANCE_DEFAULT;
	config.P5V_V_TOLERANCE = P5V_V_TOLERANCE_DEFAULT;
	config.P10V_V_TOLERANCE = P10V_V_TOLERANCE_DEFAULT;
	config.PVMAIN_V_TOLERANCE = PVMAIN_V_TOLERANCE_DEFAULT;
	return &config;
}

uint32_t config_evaluate_resistances(float* measurements) {
	uint32_t results = 0;

	results |= (config_evaluate_resistance_threshold(measurements[0], config.P1V2_R_THRESHOLD) << 0);
	results |= (config_evaluate_resistance_threshold(measurements[1], config.P1V8_R_THRESHOLD) << 1);
	results |= (config_evaluate_resistance_threshold(measurements[2], config.P3V3A_R_THRESHOLD) << 2);
	results |= (config_evaluate_resistance_threshold(measurements[3], config.P3V3IO_R_THRESHOLD) << 3);
	results |= (config_evaluate_resistance_threshold(measurements[4], config.P5V_R_THRESHOLD) << 4);
	results |= (config_evaluate_resistance_threshold(measurements[5], config.P10V_R_THRESHOLD) << 5);
	results |= (config_evaluate_resistance_threshold(measurements[6], config.PVMAIN_R_THRESHOLD) << 6);
	results |= (config_evaluate_resistance_threshold(measurements[7], config.SWD_R_THRESHOLD) << 7);
	results |= (config_evaluate_resistance_threshold(measurements[8], config.CAN_R_THRESHOLD) << 8);

	return results;
}

uint32_t config_evaluate_voltages(float* measurements) {
	uint32_t results = 0;

	results |= (config_evaluate_voltage_tolerance(measurements[0], config.P1V2_V_EXPECTED, config.P1V2_V_TOLERANCE) << 0);
	results |= (config_evaluate_voltage_tolerance(measurements[1], config.P1V8_V_EXPECTED, config.P1V8_V_TOLERANCE) << 1);
	results |= (config_evaluate_voltage_tolerance(measurements[2], config.P3V3A_V_EXPECTED, config.P3V3A_V_TOLERANCE) << 2);
	results |= (config_evaluate_voltage_tolerance(measurements[3], config.P3V3IO_V_EXPECTED, config.P3V3IO_V_TOLERANCE) << 3);
	results |= (config_evaluate_voltage_tolerance(measurements[4], config.P5V_V_EXPECTED, config.P5V_V_TOLERANCE) << 4);
	results |= (config_evaluate_voltage_tolerance(measurements[5], config.P10V_V_EXPECTED, config.P10V_V_TOLERANCE) << 5);
	results |= (config_evaluate_voltage_tolerance(measurements[6], config.PVMAIN_V_EXPECTED, config.PVMAIN_V_TOLERANCE) << 6);

	return results;
}

// Private

static uint32_t config_evaluate_resistance_threshold(float measurement, float threshold) {
	if (measurement >= threshold) {
		return 1; // Pass
	} else {
		return 0; // Fail
	}
}

static uint32_t config_evaluate_voltage_tolerance(float measurement, float expected, float tolerance) {
	if (measurement >= (expected - tolerance) && measurement <= (expected + tolerance)) {
		return 1; // Pass
	} else {
		return 0; // Fail
	}
}

