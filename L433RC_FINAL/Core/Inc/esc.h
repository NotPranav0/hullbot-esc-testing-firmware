
#ifndef INC_ESC_H_
#define INC_ESC_H_


#include <stdbool.h>

typedef enum
{
    ORIGINAL = 0U,
    MEASUREMENT
} esc_net_mode_t;

typedef enum
{
    FLOATING = 0U,
    CONNECTED
} esc_power_mode_t;

bool esc_is_connected(void);
void esc_set_pwr(esc_power_mode_t mode);
void esc_set_all_nets_mode(esc_net_mode_t mode);

#endif /* INC_ESC_H_ */

/*
=====Voltage Nets=====

ESC_P1V2_SEL_Pin
ESC_P1V2_SEL_GPIO_Port
LOW: ADC14 (V)
HIGH: ADC13 (R)

ESC_P1V8_SEL_Pin
ESC_P1V8_SEL_GPIO_Port
LOW: ADC12 (R)
HIGH: ADC9 (V)

ESC_P3V3_A_SEL_Pin
ESC_P3V3_A_SEL_GPIO_Port
LOW: ADC8 (R)
HIGH: ADC5 (V)

ESC_P3V3_IO_SEL_Pin
ESC_P3V3_IO_SEL_GPIO_Port
LOW: ADC11 (V)
HIGH: ADC10 (R)

ESC_P5V_SEL_Pin
ESC_P5V_SEL_GPIO_Port
LOW: ADC7 (V)
HIGH: ADC6 (R)

ESC_P10V_SEL_Pin
ESC_P10V_SEL_GPIO_Port
LOW: ADC1 (V)
HIGH: ADC2 (R)

ESC_PVMAIN_SEL_Pin
ESC_PVMAIN_SEL_GPIO_Port
LOW: ADC3 (R)
HIGH: ADC4 (V)

=====Power=====

ESC_PWR_SEL_Pin
ESC_PWR_SEL_GPIO_Port
LOW: Floating
HIGH: Connected

=====Serial Wire Debug=====

ESC_SWDIO_SEL_Pin
ESC_SWDIO_SEL_GPIO_Port
LOW: STLink
HIGH: ADC16

ESC_SWCLK_SEL_Pin
ESC_SWCLK_SEL_GPIO_Port
LOW: GND
HIGH: STLink

=====CAN=====

ESC_CAN_P_SEL_Pin
ESC_CAN_P_SEL_GPIO_Port
LOW: XCVR
HIGH: ADC15

ESC_CAN_N_SEL_Pin
ESC_CAN_N_SEL_GPIO_Port
LOW: GND
HIGH: XCVR
*/
