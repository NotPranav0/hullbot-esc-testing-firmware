#include <string.h>
#include <stdio.h>

#include "tinymovr.h"
#include "app.h"
#include "main.h"


tinymovr_can_tx_t g_can_tx;
tinymovr_can_rx_t g_can_rx;

void tm_init(CAN_HandleTypeDef* h_can)
{
    if (HAL_CAN_ActivateNotification(h_can, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
    {
	  Error_Handler();
	}

    g_can_tx.h_can = h_can;
    g_can_rx.h_can = h_can;
    memset(g_can_tx.tx_data,0,8);
    memset(g_can_rx.rx_data,0,8);

    g_can_rx.canfil.FilterBank = 0;
    g_can_rx.canfil.FilterMode = CAN_FILTERMODE_IDMASK;
    g_can_rx.canfil.FilterFIFOAssignment = CAN_RX_FIFO0;
    g_can_rx.canfil.FilterIdHigh = 0;
    g_can_rx.canfil.FilterIdLow = 0;
    g_can_rx.canfil.FilterMaskIdHigh = 0;
    g_can_rx.canfil.FilterMaskIdLow = 0;
    g_can_rx.canfil.FilterScale = CAN_FILTERSCALE_32BIT;
    g_can_rx.canfil.FilterActivation = ENABLE;
    g_can_rx.canfil.SlaveStartFilterBank = 14;

    if (HAL_CAN_ConfigFilter(h_can,&g_can_rx.canfil) != HAL_OK)
    {
    	printf("can rx filter config failed");
        Error_Handler();
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *h_can)
{
	if (HAL_CAN_GetRxMessage(h_can, CAN_RX_FIFO0, &g_can_rx.rx_header, g_can_rx.rx_data) != HAL_OK)
    {
		printf("can rx get message failed");
		Error_Handler();
		return;
	}
}

void can_send_frame(uint32_t num_bytes, uint8_t endpoint)
{
	  g_can_tx.tx_header.IDE = CAN_ID_STD;
	  g_can_tx.tx_header.StdId = CAN_ID << 6 | endpoint;
	  g_can_tx.tx_header.ExtId = 0;
	  g_can_tx.tx_header.RTR = CAN_RTR_DATA;
	  g_can_tx.tx_header.DLC = num_bytes;

	  HAL_CAN_AddTxMessage(g_can_tx.h_can, &g_can_tx.tx_header, g_can_tx.tx_data, &g_can_tx.mailbox);
}

void tm_set_gains(float position, float velocity)
{
    float32_to_4_byte_array(position, &g_can_tx.tx_data[0]);
    float32_to_4_byte_array(velocity, &g_can_tx.tx_data[4]);
    can_send_frame(8, TM_SET_GAINS);
}

void tm_set_vel_inc(float increment)
{
    float32_to_4_byte_array(increment, &g_can_tx.tx_data[0]);
    can_send_frame(4, TM_SET_VEL_INC);
}

void tm_set_limits(float velocity, float current)
{
	float32_to_4_byte_array(velocity, &g_can_tx.tx_data[0]);
	float32_to_4_byte_array(current, &g_can_tx.tx_data[4]);
    can_send_frame(8, TM_SET_LIMITS);
}

tinymovr_encoder_config_t tm_get_encoder_config(void) {
	tinymovr_encoder_config_t config;
    memset(&config, 0, sizeof(tinymovr_encoder_config_t));
    //TODO: 0 byte can tranfer?
    can_send_frame(0, TM_GET_ENCODER_CONFIG);
    // endpoint is lower 6 bits of can id
    while ((g_can_rx.rx_header.StdId & 0x3F) != TM_GET_ENCODER_CONFIG) {}
    memcpy(&config, &g_can_rx.rx_data, sizeof(tinymovr_encoder_config_t));
    return config;
}

void tm_set_encoder_config(tinymovr_encoder_config_t config)
{
    g_can_tx.tx_data[0] = config.type;
    float32_to_4_byte_array(config.bandwidth, &g_can_tx.tx_data[1]);
    can_send_frame(5, TM_SET_ENCODER_CONFIG);
}

void tm_set_motor_config(tinymovr_motor_config_t config)
{
    g_can_tx.tx_data[0] = config.flags;
    g_can_tx.tx_data[1] = config.pole_pairs;
    float32_to_4_byte_array(config.calibration_constant, &g_can_tx.tx_data[2]);
    can_send_frame(6, TM_SET_MOTOR_CONFIG);
}

void tm_set_state(tinymovr_state_config_t config)
{
    g_can_tx.tx_data[0] = config.state;
    g_can_tx.tx_data[1] = config.mode;
    can_send_frame(2, TM_SET_STATE);
}

tinymovr_state_config_t tm_get_state(void) {
	tinymovr_state_config_t config;
    memset(&config, 0, sizeof(tinymovr_state_config_t));
    can_send_frame(2, TM_GET_STATE);
    // endpoint is lower 6 bits of can id
    while ((g_can_rx.rx_header.StdId & 0x3F) != TM_GET_STATE) {}
    memcpy(&config, &g_can_rx.rx_data, sizeof(tinymovr_state_config_t));
    return config;
}

void tm_save_config(void) {
     //TODO: 0 byte can tranfer?
    can_send_frame(1, TM_SAVE_CONFIG);
}

void tm_set_vel_setpoint(float velocity) {
    float32_to_4_byte_array(velocity, &g_can_tx.tx_data[0]);
    //TODO: double check this endpoint, need to set current to ...?
    float32_to_4_byte_array(0, &g_can_tx.tx_data[4]);
    can_send_frame(8, TM_SET_VEL_SETPOINT);
}

float tm_get_encoder_estimates(void) {
    float estimates;
    can_send_frame(0, TM_GET_ENCODER_ESTIMATES);
    // endpoint is lower 6 bits of can id
    while ((g_can_rx.rx_header.StdId & 0x3F) != TM_GET_ENCODER_ESTIMATES) {}
    memcpy(&estimates, &g_can_rx.rx_data, sizeof(float));
    return estimates;
}

void float32_to_4_byte_array(float value, uint8_t* array) {
    memcpy(array, &value, sizeof(float));
}

