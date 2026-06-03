/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-11-25     RT-Thread    first version
 */

#include <rtthread.h>

#define DBG_TAG "Test"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>


int Test(void)
{
    LOG_D("Hello RT-Thread!");
    return RT_EOK;
}
MSH_CMD_EXPORT(Test,Test Demo);





