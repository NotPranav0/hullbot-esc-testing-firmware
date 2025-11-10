#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

#include <stdint.h>

// no padding bytes.
#pragma pack(push, 1)

typedef struct {
	// readings greater than the threshold specified will pass the test
	float P1V2_R_THRESHOLD;
    float P1V8_R_THRESHOLD;
	float P3V3A_R_THRESHOLD;
    float P3V3IO_R_THRESHOLD;
	float P5V_R_THRESHOLD;
    float P10V_R_THRESHOLD;
	float PVMAIN_R_THRESHOLD;
    float SWD_R_THRESHOLD;
    float CAN_R_THRESHOLD;

    // expected voltage values for each net
    float P1V2_V_EXPECTED;
    float P1V8_V_EXPECTED;
    float P3V3A_V_EXPECTED;
    float P3V3IO_V_EXPECTED;
    float P5V_V_EXPECTED;
    float P10V_V_EXPECTED;
    float PVMAIN_V_EXPECTED;

    // readings within +/- the tolerance will pass the test
    float P1V2_V_TOLERANCE;
    float P1V8_V_TOLERANCE;
	float P3V3A_V_TOLERANCE;
    float P3V3IO_V_TOLERANCE;
	float P5V_V_TOLERANCE;
    float P10V_V_TOLERANCE;
	float PVMAIN_V_TOLERANCE;
} Config_t;

#pragma pack(pop)

Config_t* config_init();
uint32_t config_evaluate_resistances(float* measurements);
uint32_t config_evaluate_voltages(float* measurements);



#endif /* INC_CONFIG_H_ */
