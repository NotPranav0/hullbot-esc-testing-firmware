/*
 * can.c
 *
 *  Created on: May 16, 2025
 *      Author: pranav.budhwar
 */

#include "can.h"
#include "app.h"
#include "string.h"

#define CAN_RECEIVER_ID 0x4

CAN_HandleTypeDef* _hcan;
#ifdef RX
CAN_RxHeaderTypeDef   RxHeader;
uint8_t               RxData[8] = {0};
CAN_FilterTypeDef canfil; //CAN Bus Filter
#endif
#ifdef TX
CAN_TxHeaderTypeDef   tx_header;
uint8_t               tx_data[8] = {0};
uint32_t              mailbox;
#endif

void can_init(CAN_HandleTypeDef* hcan){
	_hcan = hcan;

#ifdef RX



	if (HAL_CAN_ActivateNotification(_hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	{

	  while(1){
		  UART_Printf("CAN ERROR");
		  HAL_Delay(10);
		  bled_toggle();
	  }
	}


	  canfil.FilterBank = 0;
	  canfil.FilterMode = CAN_FILTERMODE_IDMASK;
	  canfil.FilterFIFOAssignment = CAN_RX_FIFO0;
	  canfil.FilterIdHigh = 0;
	  canfil.FilterIdLow = 0;
	  canfil.FilterMaskIdHigh = 0;
	  canfil.FilterMaskIdLow = 0;
	  canfil.FilterScale = CAN_FILTERSCALE_32BIT;
	  canfil.FilterActivation = ENABLE;
	  canfil.SlaveStartFilterBank = 14;

	  HAL_CAN_ConfigFilter(_hcan,&canfil); //Initialize CAN Filter



#endif
}

#ifdef RX
// Rx callback
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
	{
		UART_Printf("CAN ERROR 2");
		return;
	}
	bled_toggle();
	UART_Printf("Received %d bytes\r\n",RxHeader.DLC);
	for (uint8_t idx = 0; idx < RxHeader.DLC; idx++) {
		UART_Printf("%x ", RxData[idx]);
	}

}
#endif
#ifdef TX
// Send CAN frame
void can_send_frame(uint32_t num_bytes) {
	  tx_header.IDE = CAN_ID_STD;
	  tx_header.StdId = CAN_RECEIVER_ID;
	  tx_header.ExtId = 0;
	  tx_header.RTR = CAN_RTR_DATA;
	  tx_header.DLC = num_bytes;
	  UART_Printf("Sending %d bytes\r\n",tx_header.DLC);
	  HAL_CAN_AddTxMessage(_hcan, &tx_header, tx_data, &mailbox);
	  bled_toggle();
}

void can_set_data(uint8_t* data, uint8_t size) {
	if (size > 8) {
		return;
	}
	memcpy(tx_data, data, size);
}
#endif
