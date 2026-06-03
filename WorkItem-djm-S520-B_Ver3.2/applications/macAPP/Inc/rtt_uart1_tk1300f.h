/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-06-15     zphu       the first version
 */
#ifndef APPLICATIONS_MACAPP_INC_RTT_UART1_TK1300F_H_

#define APPLICATIONS_MACAPP_INC_RTT_UART1_TK1300F_H_
#include <applications/macSYS/Inc/macSYS.h>

#define TEST1_PRINTF_PRESSURE    0   // 输出口
#define TEST3_PRINTF_PRESSURE    0   // 主气缸
#define DEBUG_PID                0   // PID输出打印

/**
  * @brief  枚举类型,气压传感器使能
  * @param  None
  */
typedef enum TK_STATUS{
    tk_disable = 0,
    tk_enable,
}TK_STATUS;

void macTk1300f_Uart1_Init(void);
void macTk1300f_Uart1_EN(TK_STATUS status);
void Tk1300F_Calculate1(unsigned  char data);

void macTk1300f_Uart3_Init(void);
void macTk1300f_Uart3_EN(TK_STATUS status);
uint16_t Tk1300F_Calculate3(unsigned  char data);

#endif /* APPLICATIONS_MACAPP_INC_RTT_UART1_TK1300F_H_ */
