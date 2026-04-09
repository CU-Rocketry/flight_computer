/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32h5xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SENSE_2_Pin GPIO_PIN_2
#define SENSE_2_GPIO_Port GPIOE
#define PYRO_2_Pin GPIO_PIN_3
#define PYRO_2_GPIO_Port GPIOE
#define PYRO_1_Pin GPIO_PIN_4
#define PYRO_1_GPIO_Port GPIOE
#define SENSE_1_Pin GPIO_PIN_5
#define SENSE_1_GPIO_Port GPIOE
#define SENSE_3_Pin GPIO_PIN_6
#define SENSE_3_GPIO_Port GPIOE
#define PYRO_3_Pin GPIO_PIN_13
#define PYRO_3_GPIO_Port GPIOC
#define PYRO_4_Pin GPIO_PIN_14
#define PYRO_4_GPIO_Port GPIOC
#define SENSE_4_Pin GPIO_PIN_15
#define SENSE_4_GPIO_Port GPIOC
#define I_PYRO_SENSE_Pin GPIO_PIN_0
#define I_PYRO_SENSE_GPIO_Port GPIOC
#define BATT_SENSE_Pin GPIO_PIN_1
#define BATT_SENSE_GPIO_Port GPIOC
#define I_SENSE_Pin GPIO_PIN_2
#define I_SENSE_GPIO_Port GPIOC
#define PYRO_BATT_SENSE_Pin GPIO_PIN_3
#define PYRO_BATT_SENSE_GPIO_Port GPIOC
#define LED_2_R_Pin GPIO_PIN_0
#define LED_2_R_GPIO_Port GPIOA
#define LED_2_G_Pin GPIO_PIN_1
#define LED_2_G_GPIO_Port GPIOA
#define LED_2_B_Pin GPIO_PIN_2
#define LED_2_B_GPIO_Port GPIOA
#define LM660_ST_Pin GPIO_PIN_3
#define LM660_ST_GPIO_Port GPIOA
#define RF_DIO3_Pin GPIO_PIN_4
#define RF_DIO3_GPIO_Port GPIOC
#define RF_DIO2_Pin GPIO_PIN_5
#define RF_DIO2_GPIO_Port GPIOC
#define RF_DIO1_Pin GPIO_PIN_7
#define RF_DIO1_GPIO_Port GPIOE
#define RF_TXEN_Pin GPIO_PIN_8
#define RF_TXEN_GPIO_Port GPIOE
#define RF_RXEN_Pin GPIO_PIN_9
#define RF_RXEN_GPIO_Port GPIOE
#define RF_RESET_Pin GPIO_PIN_10
#define RF_RESET_GPIO_Port GPIOE
#define RF_BUSY_Pin GPIO_PIN_15
#define RF_BUSY_GPIO_Port GPIOE
#define BARO_CS_Pin GPIO_PIN_12
#define BARO_CS_GPIO_Port GPIOB
#define IMU_CS_Pin GPIO_PIN_8
#define IMU_CS_GPIO_Port GPIOD
#define IMU_INT2_Pin GPIO_PIN_9
#define IMU_INT2_GPIO_Port GPIOD
#define IMU_INT1_Pin GPIO_PIN_10
#define IMU_INT1_GPIO_Port GPIOD
#define BARO_INT_Pin GPIO_PIN_13
#define BARO_INT_GPIO_Port GPIOD
#define GPS_TP_Pin GPIO_PIN_14
#define GPS_TP_GPIO_Port GPIOD
#define GPS_RESET_Pin GPIO_PIN_15
#define GPS_RESET_GPIO_Port GPIOD
#define GPS_EXTI_Pin GPIO_PIN_6
#define GPS_EXTI_GPIO_Port GPIOC
#define BUZZER_Pin GPIO_PIN_7
#define BUZZER_GPIO_Port GPIOC
#define LED_1_B_Pin GPIO_PIN_8
#define LED_1_B_GPIO_Port GPIOA
#define LED_1_G_Pin GPIO_PIN_9
#define LED_1_G_GPIO_Port GPIOA
#define LED_1_R_Pin GPIO_PIN_10
#define LED_1_R_GPIO_Port GPIOA
#define MAG_CS_Pin GPIO_PIN_15
#define MAG_CS_GPIO_Port GPIOA
#define SD_DET_Pin GPIO_PIN_0
#define SD_DET_GPIO_Port GPIOD
#define BTN_3_Pin GPIO_PIN_3
#define BTN_3_GPIO_Port GPIOD
#define BTN_2_Pin GPIO_PIN_4
#define BTN_2_GPIO_Port GPIOD
#define BTN_1_Pin GPIO_PIN_5
#define BTN_1_GPIO_Port GPIOD
#define MAG_INT_Pin GPIO_PIN_0
#define MAG_INT_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
