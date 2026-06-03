/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-04-16     teati       the first version
 */
#include "bsp_beep.h"




void BEEP_Blink(uint8_t num)
{
    rt_tick_t tickk = 0;
    for(uint8_t i = 0; i < num; i++){
        macBEEP_ON();
        tickk = rt_tick_get();
        rt_thread_delay_until(&tickk, 150);
        macBEEP_OFF();
        tickk = rt_tick_get();
        rt_thread_delay_until(&tickk, 150);
    }
}







