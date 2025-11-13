#include "adc.h"
#include "stm32l4xx_hal.h"
#include "main.h"

static ADC_HandleTypeDef* h_adc;

#define ADC_BITS 		12
#define MAX_VOLTS 		3.3
#define MAX_ADC_RAW_VAL ((1 << ADC_BITS) - 1)

#define NUM_SAMPLES 	10 // Number of samples to average per measurement

static float raw_to_volts(uint32_t raw);
static uint32_t adc_read_channel_blocking(uint32_t channel);

typedef struct {
	uint32_t channel;
	char* name;
} adc_channel_t;

/*
ADC Channel to Net Name mapping
*/

adc_channel_t adc_channels_resistance[] = {
	{.channel = ADC_CHANNEL_13, .name = "P1V2"},
	{.channel = ADC_CHANNEL_12, .name = "P1V8"},
	{.channel = ADC_CHANNEL_8, .name = "P3V3_A"},
	{.channel = ADC_CHANNEL_10, .name = "P3V3_IO"},
	{.channel = ADC_CHANNEL_6, .name = "P5V"},
	{.channel = ADC_CHANNEL_2, .name = "P10V"},
	{.channel = ADC_CHANNEL_3, .name = "PVMAIN"},
	{.channel = ADC_CHANNEL_15, .name = "SWD"},
	{.channel = ADC_CHANNEL_16, .name = "CAN"}
};

adc_channel_t adc_channels_voltage[] = {
	{.channel = ADC_CHANNEL_14, .name = "P1V2"},
	{.channel = ADC_CHANNEL_9, .name = "P1V8"},
	{.channel = ADC_CHANNEL_5, .name = "P3V3_A"},
	{.channel = ADC_CHANNEL_11, .name = "P3V3_IO"},
	{.channel = ADC_CHANNEL_7, .name = "P5V"},
	{.channel = ADC_CHANNEL_1, .name = "P10V"},
	{.channel = ADC_CHANNEL_4, .name = "PVMAIN"}
};

// Public
/*
	Initializes ADC peripheral, performs calibration
	@param adc: Pointer to ADC handle
*/
void adc_init(ADC_HandleTypeDef* adc) {
	h_adc = adc;

	HAL_ADCEx_Calibration_Start(h_adc, ADC_SINGLE_ENDED);
}

/*
	Enables or Disconnects 1.2V rail on jig pcb
	@param mode: CONNECTED enables 1.2v
	              FLOATING disables 1.2v
*/
void adc_set_1v2_source(esc_power_mode_t mode) {
    HAL_GPIO_WritePin(EN_1V2_GPIO_Port, EN_1V2_Pin, mode);
}

/*
	Takes measurements on all ADC channels of specified type
	@param measurements: Pointer to array of adc_measurement_t structs to store results, 
	use NUM_RESISTANCE_CHANNELS or NUM_VOLTAGE_CHANNELS for size.
	@param type: Type of measurement to take (RESISTANCE or VOLTAGE)
*/
void adc_take_measurements(adc_measurement_t* measurements, measurement_type_t type) {
	int num_channels = 0;
	adc_channel_t* channels = NULL;
	if (type == RESISTANCE) {
		num_channels = NUM_RESISTANCE_CHANNELS;
		channels = adc_channels_resistance;
	} else if (type == VOLTAGE) {
		num_channels = NUM_VOLTAGE_CHANNELS;
		channels = adc_channels_voltage;
	}

	for (int sample = 0; sample < NUM_SAMPLES; sample++) {
		for (int net = 0; net < num_channels; net++) {
			uint32_t raw_val = adc_read_channel_blocking(channels[net].channel);
			measurements[net].measurement += (raw_to_volts(raw_val) / (float)NUM_SAMPLES);
			measurements[net].name = channels[net].name;
			measurements[net].type = type;
		}
	}


}

// Private 

/*
	Converts raw ADC value to voltage
	@param raw: Raw ADC value
	@return Voltage corresponding to raw ADC value

*/
static float raw_to_volts(uint32_t raw) {
	float scalar = (float)MAX_VOLTS/(float)MAX_ADC_RAW_VAL;
	return (float)raw * scalar;
}

/*
	Reads one ADC channel (blocking)
	@param channel: ADC channel to read
	@return Raw ADC value read from channel
*/
static uint32_t adc_read_channel_blocking(uint32_t channel) {
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = channel;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	HAL_ADC_ConfigChannel(h_adc, &sConfig);

	HAL_ADC_Start(h_adc);
	HAL_ADC_PollForConversion(h_adc, HAL_MAX_DELAY);
	uint32_t raw = HAL_ADC_GetValue(h_adc);
	HAL_ADC_Stop(h_adc);

	return raw;
}
