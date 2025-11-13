#include "esc.h"
#include "main.h"
#include "stm32l4xx_hal.h"

#include <stdbool.h>

#define ESC_NET_COUNT 11

// ESC net to index mapping
typedef enum {
    ESC_P1V2 = 0U,
    ESC_P1V8,
    ESC_P3V3_A,
    ESC_P3V3_IO,
    ESC_P5V,
    ESC_P10V,
    ESC_PVMAIN,
    ESC_SWDIO,
    ESC_SWCLK,
    ESC_CAN_P,
    ESC_CAN_N
} esc_net_t;

static void esc_set_net_mode(esc_net_t net, esc_net_mode_t mode);
static GPIO_PinState invert(GPIO_PinState state);

/*
    Control pins for ESC nets
*/
uint16_t esc_net_pins[] = {
    ESC_P1V2_SEL_Pin,
    ESC_P1V8_SEL_Pin,
    ESC_P3V3_A_SEL_Pin,
    ESC_P3V3_IO_SEL_Pin,
    ESC_P5V_SEL_Pin,
    ESC_P10V_SEL_Pin,
    ESC_PVMAIN_SEL_Pin,
    ESC_SWDIO_SEL_Pin,
    ESC_SWCLK_SEL_Pin,
    ESC_CAN_P_SEL_Pin,
    ESC_CAN_N_SEL_Pin
};

GPIO_TypeDef *esc_net_ports[] = {
    ESC_P1V2_SEL_GPIO_Port,
    ESC_P1V8_SEL_GPIO_Port,
    ESC_P3V3_A_SEL_GPIO_Port,
    ESC_P3V3_IO_SEL_GPIO_Port,
    ESC_P5V_SEL_GPIO_Port,
    ESC_P10V_SEL_GPIO_Port,
    ESC_PVMAIN_SEL_GPIO_Port,
    ESC_SWDIO_SEL_GPIO_Port,
    ESC_SWCLK_SEL_GPIO_Port,
    ESC_CAN_P_SEL_GPIO_Port,
    ESC_CAN_N_SEL_GPIO_Port
};


/*
    Required state of control pins for original/intented ESC net modes
    Invert for measurement mode
*/
GPIO_PinState esc_nets_original_modes[] = {
	GPIO_PIN_RESET,
	GPIO_PIN_SET,
	GPIO_PIN_SET,
	GPIO_PIN_RESET,
	GPIO_PIN_RESET,
	GPIO_PIN_RESET,
	GPIO_PIN_SET,
    GPIO_PIN_RESET,
    GPIO_PIN_SET,
    GPIO_PIN_RESET,
    GPIO_PIN_SET
};

// Public

/*
    Check if ESC is connected
*/
bool esc_is_connected() {
	//return (HAL_GPIO_ReadPin(ESC_DET_GPIO_Port, ESC_DET_Pin) == GPIO_PIN_RESET);
	return true; //TODO
}

/*
    Set ESC power mode
    @param mode: ESC power mode. CONNECTED or FLOATING

*/
void esc_set_pwr(esc_power_mode_t mode) {
	HAL_GPIO_WritePin(ESC_PWR_SEL_GPIO_Port, ESC_PWR_SEL_Pin, mode);
}

/*
    Set mode for all ESC nets
    @param mode: ESC net mode. ORIGINAL or MEASUREMENT
*/
void esc_set_all_nets_mode(esc_net_mode_t mode) {
	for (esc_net_t net = 0; net < ESC_NET_COUNT; net++) {
		esc_set_net_mode(net, mode);
	}
}

// Private

/*
    Invert GPIO pin state
    @param state: Current GPIO pin state
    @return Inverted GPIO pin state
*/
static GPIO_PinState invert(GPIO_PinState state) {
    if (state == GPIO_PIN_RESET) {
        return GPIO_PIN_SET;
    } else {
        return GPIO_PIN_RESET;
    }
}

/*
    Set mode for a specific ESC net
    @param net: ESC net to set mode for
    @param mode: ESC net mode. ORIGINAL or MEASUREMENT
*/
static void esc_set_net_mode(esc_net_t net, esc_net_mode_t mode) {
    if (mode == ORIGINAL) {
		HAL_GPIO_WritePin(esc_net_ports[net], esc_net_pins[net], esc_nets_original_modes[net]);
	} else if (mode == MEASUREMENT) {
		HAL_GPIO_WritePin(esc_net_ports[net], esc_net_pins[net], invert(esc_nets_original_modes[net]));
	}
}
