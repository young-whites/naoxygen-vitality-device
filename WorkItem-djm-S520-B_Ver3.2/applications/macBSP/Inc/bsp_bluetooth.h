/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-11-06     teati       the first version
 */
#ifndef APPLICATIONS_MACBSP_INC_BSP_BLUETOOTH_H_

#define APPLICATIONS_MACBSP_INC_BSP_BLUETOOTH_H_
#include <applications/macSYS/Inc/macSYS.h>






void BLUETOOTH_SET_EN(FunctionalState status);
int bluetoothTimer_Init(void);
int8_t BLUETOOTH_GetState(void);






#endif /* APPLICATIONS_MACBSP_INC_BSP_BLUETOOTH_H_ */
