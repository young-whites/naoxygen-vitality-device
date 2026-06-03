/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <applications/macSYS/Inc/macSYS.h>
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

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PowerKey_Pin GPIO_PIN_0
#define PowerKey_GPIO_Port GPIOA
#define LED_Orange_Pin GPIO_PIN_1
#define LED_Orange_GPIO_Port GPIOA
#define LED_White_Pin GPIO_PIN_4
#define LED_White_GPIO_Port GPIOA
#define LED_4G_Pin GPIO_PIN_6
#define LED_4G_GPIO_Port GPIOA
#define LED_Status_Pin GPIO_PIN_7
#define LED_Status_GPIO_Port GPIOA
#define LED_Message_Pin GPIO_PIN_4
#define LED_Message_GPIO_Port GPIOC
#define macBluetooth_STA_Pin GPIO_PIN_5
#define macBluetooth_STA_GPIO_Port GPIOC
#define macBluetooth_EN_Pin GPIO_PIN_0
#define macBluetooth_EN_GPIO_Port GPIOB
#define BEEP_Pin GPIO_PIN_1
#define BEEP_GPIO_Port GPIOB
#define Tk1300F_EN3_Pin GPIO_PIN_10
#define Tk1300F_EN3_GPIO_Port GPIOB
#define Compressor_EN_Pin GPIO_PIN_14
#define Compressor_EN_GPIO_Port GPIOB
#define FAN_Pin GPIO_PIN_6
#define FAN_GPIO_Port GPIOC
#define Tk1300F_EN1_Pin GPIO_PIN_9
#define Tk1300F_EN1_GPIO_Port GPIOA
#define Vavle_3_EN_Pin GPIO_PIN_15
#define Vavle_3_EN_GPIO_Port GPIOA
#define SS6840_PEN_Pin GPIO_PIN_6
#define SS6840_PEN_GPIO_Port GPIOB
#define SS6840_ENBL_Pin GPIO_PIN_7
#define SS6840_ENBL_GPIO_Port GPIOB
#define SS6840_PHASE_Pin GPIO_PIN_8
#define SS6840_PHASE_GPIO_Port GPIOB
#define SS6840_DECAY_Pin GPIO_PIN_9
#define SS6840_DECAY_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
