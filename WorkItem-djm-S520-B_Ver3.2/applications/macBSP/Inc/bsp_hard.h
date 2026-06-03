/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-06-04     zphu       the first version
 */
#ifndef APPLICATIONS_MACBSP_INC_BSP_HARD_H_

#define APPLICATIONS_MACBSP_INC_BSP_HARD_H_

#include <applications/macSYS/Inc/macSYS.h>

void Fan_Off(void);
void Fan_On(void);
void Main_Gas_Circuit_Off(void);
void Main_Gas_Circuit_On(void);
void Power_Off_LED(void);
void Power_On_LED(void);
void Power_EN(void);

#endif /* APPLICATIONS_MACBSP_INC_BSP_HARD_H_ */
