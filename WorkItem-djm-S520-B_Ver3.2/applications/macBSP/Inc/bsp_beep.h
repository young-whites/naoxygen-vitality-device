/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-04-16     teati       the first version
 */
#ifndef APPLICATIONS_MACBSP_INC_BSP_BEEP_H_
#define APPLICATIONS_MACBSP_INC_BSP_BEEP_H_

#include "macSYS.h"




/******************** BEEP 函数宏定义 **************************/
#define             macBEEP_OFF()                            HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);
#define             macBEEP_ON()                             HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET);


void BEEP_Blink(uint8_t num);



#endif /* APPLICATIONS_MACBSP_INC_BSP_BEEP_H_ */
