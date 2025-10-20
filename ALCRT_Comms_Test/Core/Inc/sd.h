/*
 * sd.h
 *
 *  Created on: May 15, 2025
 *      Author: pranav.budhwar
 */

#ifndef INC_SD_H_
#define INC_SD_H_



int SD_Init(SPI_HandleTypeDef* hspi1);

uint8_t sd_write_block(uint32_t addr, uint8_t *buf, uint8_t *token);
uint8_t sd_read_block(uint32_t addr, uint8_t *buf, uint8_t *token);



#endif /* INC_SD_H_ */
