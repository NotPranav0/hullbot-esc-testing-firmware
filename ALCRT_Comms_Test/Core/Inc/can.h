/*
 * can.h
 *
 *  Created on: May 16, 2025
 *      Author: pranav.budhwar
 */

#ifndef INC_CAN_H_
#define INC_CAN_H_

#include <stdint.h>

#include "stm32l4xx_hal.h"

void can_init(CAN_HandleTypeDef* hcan);

#ifndef RX
void can_send_frame(uint32_t num_bytes);
void can_set_data(uint8_t data[], uint8_t size);
#endif

#endif /* INC_CAN_H_ */
