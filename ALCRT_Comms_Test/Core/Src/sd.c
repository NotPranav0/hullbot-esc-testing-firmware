/*
 * sd.c
 *
 *  Created on: May 15, 2025
 *      Author: pranav.budhwar
 */

#include "stm32l4xx_hal.h"

#include "sd.h"
#include "main.h"
#include "app.h"

static SPI_HandleTypeDef *_hspi;

static void sd_select(void) {
	HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_RESET);
}

static void sd_unselect(void) {
	HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_SET);
}

static uint8_t sd_tx_rx_byte(uint8_t data) {
	uint8_t rx;
	HAL_SPI_TransmitReceive(_hspi, &data, &rx, 1, 10000);
	return rx;
}

void sd_powerup_seq(void) {
	sd_unselect();

	HAL_Delay(1);

	for (int idx = 0; idx < 10; idx++) {
		sd_tx_rx_byte(0xFF);
	}

	sd_unselect();

	sd_tx_rx_byte(0xFF);
}

void sd_command(uint8_t cmd, uint32_t arg, uint8_t crc) {
	// transmit command to sd card
	sd_tx_rx_byte(cmd | 0x40);

	// transmit argument
	sd_tx_rx_byte((uint8_t) (arg >> 24));
	sd_tx_rx_byte((uint8_t) (arg >> 16));
	sd_tx_rx_byte((uint8_t) (arg >> 8));
	sd_tx_rx_byte((uint8_t) (arg));

	// transmit crc
	sd_tx_rx_byte(crc | 0x01);
}

uint8_t sd_read_res1() {
	uint8_t idx = 0, res1;
	while ((res1 = sd_tx_rx_byte(0xFF)) == 0xFF && idx < 8) {
		idx++;
	}

	return res1;
}

uint8_t sd_go_idle_state() {
	// assert chip select
	sd_tx_rx_byte(0XFF);
	sd_select();
	sd_tx_rx_byte(0XFF);

	// send CMD0
	uint8_t cmd0 = 0;
	uint32_t cmd0_arg = 0;
	uint8_t cmd0_crc = 0x94;
	sd_command(cmd0, cmd0_arg, cmd0_crc);

	// read response
	uint8_t res1 = sd_read_res1();

	// deassert chip select
	sd_tx_rx_byte(0XFF);
	sd_select();
	sd_tx_rx_byte(0XFF);

	return res1;
}

void sd_read_res7(uint8_t *res) {
	res[0] = sd_read_res1();

	if (res[0] > 1) {
		return;
	}

	res[1] = sd_tx_rx_byte(0xFF);
	res[2] = sd_tx_rx_byte(0xFF);
	res[3] = sd_tx_rx_byte(0xFF);
	res[4] = sd_tx_rx_byte(0xFF);

}

void sd_send_if_cond(uint8_t *res) {
	// assert chip select
	sd_tx_rx_byte(0XFF);
	sd_select();
	sd_tx_rx_byte(0XFF);

	// send CMD8
	uint8_t cmd8 = 8;
	uint32_t cmd8_arg = 0x1AA;
	uint8_t cmd8_crc = 0x86;
	sd_command(cmd8, cmd8_arg, cmd8_crc);

	// read response
	sd_read_res7(res);

	// deassert chip select
	sd_tx_rx_byte(0XFF);
	sd_select();
	sd_tx_rx_byte(0XFF);

}

void sd_read_ocr(uint8_t *res) {
	// assert chip select
	sd_tx_rx_byte(0XFF);
	sd_select();
	sd_tx_rx_byte(0XFF);

	// send CMD58
	uint8_t cmd58 = 58;
	uint32_t cmd58_arg = 0;
	uint8_t cmd58_crc = 0;
	sd_command(cmd58, cmd58_arg, cmd58_crc);

	// read response
	sd_read_res7(res);

	// deassert chip select
	sd_tx_rx_byte(0XFF);
	sd_select();
	sd_tx_rx_byte(0XFF);
}

uint8_t sd_send_app() {
	// assert chip select
	sd_tx_rx_byte(0XFF);
	sd_select();
	sd_tx_rx_byte(0XFF);

	// send CMD55
	uint8_t cmd55 = 55;
	uint32_t cmd55_arg = 0;
	uint8_t cmd55_crc = 0;
	sd_command(cmd55, cmd55_arg, cmd55_crc);

	// read response
	uint8_t res1 = sd_read_res1();

	// deassert chip select
	sd_tx_rx_byte(0XFF);
	sd_select();
	sd_tx_rx_byte(0XFF);

	return res1;
}

uint8_t sd_send_opcond() {
	// assert chip select
	sd_tx_rx_byte(0XFF);
	sd_select();
	sd_tx_rx_byte(0XFF);

	// send ACMD41
	uint8_t acmd41 = 41;
	uint32_t acmd41_arg = 0x40000000;
	uint8_t acmd41_crc = 0;
	sd_command(acmd41, acmd41_arg, acmd41_crc);

	// read response
	uint8_t res1 = sd_read_res1();

	// deassert chip select
	sd_tx_rx_byte(0XFF);
	sd_select();
	sd_tx_rx_byte(0XFF);

	return res1;

}

int SD_Init(SPI_HandleTypeDef *hspi) {
	_hspi = hspi;
	uint8_t res[5];
	uint8_t cmd_attempts = 0;

	sd_powerup_seq();

	while ((res[0] = sd_go_idle_state()) != 0x01) {
		cmd_attempts++;
		if (cmd_attempts > 10) {
			return -1;
		}
	}

	sd_send_if_cond(res);
	if (res[0] != 0x01) {
		return -1;
	}

	if (res[4] != 0xAA) {
		return -1;
	}

	cmd_attempts = 0;
	do {
		if (cmd_attempts > 100) {
			return -1;
		}

		res[0] = sd_send_app();

		if (res[0] < 2) {
			res[0] = sd_send_opcond();
		}

		HAL_Delay(10);

		cmd_attempts++;
	} while (res[0] != 0);

	sd_read_ocr(res);

	if (!(res[1] & 0x80)) {
		return -1;
	}

	return 0;
}

/*******************************************************************************
 Read single 512 byte block
 token = 0xFE - Successful read
 token = 0x0X - Data error
 token = 0xFF - timeout
 *******************************************************************************/
uint8_t sd_read_block(uint32_t addr, uint8_t *buf, uint8_t *token) {
	uint8_t res1;
	uint8_t read;
	uint16_t read_attempts;
	*token = 0xFF;

	// assert chip select
	sd_tx_rx_byte(0XFF);
	sd_select();
	sd_tx_rx_byte(0XFF);

	// send CMD17
	uint8_t cmd17 = 17;
	uint32_t cmd17_arg = addr;
	uint8_t cmd17_crc = 0;
	sd_command(cmd17, cmd17_arg, cmd17_crc);

	// read R1
	res1 = sd_read_res1();

	// if response received from card
	if (res1 != 0xFF) {
		// wait for a response token (timeout = 100ms)
		read_attempts = 0;
		while (++read_attempts != 3125)
			if ((read = sd_tx_rx_byte(0xFF)) != 0xFF)
				break;

		// if response token is 0xFE
		if (read == 0xFE) {
			// read 512 byte block
			for (uint16_t i = 0; i < 512; i++)
				*buf++ = sd_tx_rx_byte(0xFF);

			// read 16-bit CRC
			sd_tx_rx_byte(0xFF);
			sd_tx_rx_byte(0xFF);
		}

		// set token to card response
		*token = read;
	}

	// deassert chip select
	sd_tx_rx_byte(0XFF);
	sd_unselect();
	sd_tx_rx_byte(0XFF);

	return res1;
}

/*******************************************************************************
 Write single 512 byte block
 token = 0x00 - busy timeout
 token = 0x05 - data accepted
 token = 0xFF - response timeout
 *******************************************************************************/
uint8_t sd_write_block(uint32_t addr, uint8_t *buf, uint8_t *token) {
	uint16_t read_attempts;
	uint8_t res1;
	uint8_t read;

	// set token to none
	*token = 0xFF;

	// assert chip select
	sd_tx_rx_byte(0xFF);
	sd_select();
	sd_tx_rx_byte(0xFF);

	// send CMD24
	uint8_t cmd24 = 24;
	uint32_t cmd24_arg = addr;
	uint8_t cmd24_crc = 0;
	sd_command(cmd24, cmd24_arg, cmd24_crc);

	// read response
	res1 = sd_read_res1();

	// if no error
	if (res1 == 0) {
		// send start token
		sd_tx_rx_byte(0xFE);

		// write buffer to card
		for (uint16_t i = 0; i < 512; i++)
			sd_tx_rx_byte(buf[i]);

		// wait for a response (timeout = 250ms)
		read_attempts = 0;
		while (++read_attempts != 7813)
			if ((read = sd_tx_rx_byte(0xFF)) != 0xFF) {
				*token = 0xFF;
				break;
			}

		// if data accepted
		if ((read & 0x1F) == 0x05) {
			// set token to data accepted
			*token = 0x05;

			// wait for write to finish (timeout = 250ms)
			read_attempts = 0;
			while (sd_tx_rx_byte(0xFF) == 0x00)
				if (++read_attempts == 7813) {
					*token = 0x00;
					break;
				}
		}
	}

	// deassert chip select
	sd_tx_rx_byte(0xFF);
	sd_unselect();
	sd_tx_rx_byte(0xFF);

	return res1;
}

