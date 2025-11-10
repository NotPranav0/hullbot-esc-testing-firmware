/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ESC_P3V3_IO_SEL_Pin GPIO_PIN_13
#define ESC_P3V3_IO_SEL_GPIO_Port GPIOC
#define ESC_P5V_SEL_Pin GPIO_PIN_14
#define ESC_P5V_SEL_GPIO_Port GPIOC
#define ESC_P3V3_A_SEL_Pin GPIO_PIN_15
#define ESC_P3V3_A_SEL_GPIO_Port GPIOC
#define ESC_PVMAIN_SEL_Pin GPIO_PIN_0
#define ESC_PVMAIN_SEL_GPIO_Port GPIOH
#define ESC_P10V_SEL_Pin GPIO_PIN_1
#define ESC_P10V_SEL_GPIO_Port GPIOH
#define ESC_SWDIO_SEL_Pin GPIO_PIN_2
#define ESC_SWDIO_SEL_GPIO_Port GPIOB
#define ESC_P1V2_SEL_Pin GPIO_PIN_10
#define ESC_P1V2_SEL_GPIO_Port GPIOB
#define ESC_SWCLK_SEL_Pin GPIO_PIN_12
#define ESC_SWCLK_SEL_GPIO_Port GPIOB
#define ESC_CAN_N_SEL_Pin GPIO_PIN_13
#define ESC_CAN_N_SEL_GPIO_Port GPIOB
#define ESC_CAN_P_SEL_Pin GPIO_PIN_14
#define ESC_CAN_P_SEL_GPIO_Port GPIOB
#define EN_1V2_Pin GPIO_PIN_15
#define EN_1V2_GPIO_Port GPIOB
#define USER_BTN_Pin GPIO_PIN_9
#define USER_BTN_GPIO_Port GPIOC
#define ESC_PWR_SEL_Pin GPIO_PIN_8
#define ESC_PWR_SEL_GPIO_Port GPIOA
#define USER_LED_Pin GPIO_PIN_10
#define USER_LED_GPIO_Port GPIOC
#define START_BUTTON_Pin GPIO_PIN_11
#define START_BUTTON_GPIO_Port GPIOC
#define SHUTDOWN_Pin GPIO_PIN_12
#define SHUTDOWN_GPIO_Port GPIOC
#define SPI_INT_Pin GPIO_PIN_2
#define SPI_INT_GPIO_Port GPIOD
#define ESC_DET_Pin GPIO_PIN_4
#define ESC_DET_GPIO_Port GPIOB
#define OVER_TEMP_Pin GPIO_PIN_5
#define OVER_TEMP_GPIO_Port GPIOB
#define OVER_TEMP_EXTI_IRQn EXTI9_5_IRQn
#define OVER_CURRENT_Pin GPIO_PIN_6
#define OVER_CURRENT_GPIO_Port GPIOB
#define OVER_CURRENT_EXTI_IRQn EXTI9_5_IRQn
#define ESC_P1V8_SEL_Pin GPIO_PIN_7
#define ESC_P1V8_SEL_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
