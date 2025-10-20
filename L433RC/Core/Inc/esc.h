
#ifndef INC_ESC_H_
#define INC_ESC_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum 
{
    ESC_P1V2 = 0U,
    ESC_P1V8,
    ESC_P3V3_A,
    ESC_P3V3_IO,
    ESC_P5V,
    ESC_P10V,
    ESC_PVMAIN
} esc_voltage_net_t;

typedef enum 
{
    VOLTAGE = 0U,
    RESISTANCE
} esc_voltage_net_mode_t;

typedef enum 
{
    FLOATING = 0U,
    CONNECTED
} esc_power_mode_t;

typedef enum 
{
    MEASURING = 0U,
    COMMUNICATING,
} esc_comms_mode_t;

bool esc_is_connected();

void esc_set_1v2_source(esc_power_mode_t mode);

void esc_set_voltage_net_mode(esc_voltage_net_t net, esc_voltage_net_mode_t mode);
void esc_set_all_voltage_nets_mode(esc_voltage_net_mode_t mode);

void esc_set_swd_mode(esc_comms_mode_t mode);
void esc_set_can_mode(esc_comms_mode_t mode);

void esc_set_pwr(esc_power_mode_t mode);

#endif /* INC_ESC_H_ */
