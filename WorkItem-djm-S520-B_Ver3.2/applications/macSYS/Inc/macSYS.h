/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-08-30     Administrator       the first version
 */
#ifndef APPLICATIONS_MACSYS_INC_MACSYS_H_
#define APPLICATIONS_MACSYS_INC_MACSYS_H_

#define  USE_PID  1

/* RTT实时操作系统的头文件 */
#include <rtthread.h>
#include <drv_common.h>
#include <board.h>
#include <rtdevice.h>
#include <rthw.h>


/* 该头文件包含了所有CubeMX自动生成的初始化引脚 */
#include "main.h"

/* macBSP中的头文件 */
#include <applications/macBSP/Inc/bsp_beep.h>
#include <applications/macBSP/Inc/bsp_bluetooth.h>
#include <applications/macBSP/Inc/bsp_compressor.h>
#include <applications/macBSP/Inc/bsp_hard.h>
#include <applications/macBSP/Inc/bsp_key.h>
#include <applications/macBSP/Inc/bsp_led.h>
#include <applications/macBSP/Inc/bsp_pwm.h>

/* macAPP中的头文件 */
#include <applications/macAPP/Inc/rtt_system_work.h>
#include <applications/macAPP/Inc/rtt_uart1_tk1300f.h>
#include <applications/macAPP/Inc/rtt_uart2_Decode.h>
#include <applications/macAPP/Inc/rtt_uart3_tk1300f.h>
#include <applications/macAPP/Inc/rtt_uart4_Decode.h>
#include "rtt_iap_update_Timer.h"


/* macSYS中的头文件 */
#include <applications/macSYS/Inc/macTypedef.h>
#include <iap_upgrade.h>


/* macMATH中的头文件 */
#include <applications/macMATH/Inc/moving_average_filtre.h>


/* 标准库头文件 */
#include "string.h"
#include "stdio.h"
#include "float.h"

/* fal-flash头文件 */
#include "fal.h"
#include "falFlash_Test.h"
#include "macFlash.h"








#endif /* APPLICATIONS_MACSYS_INC_MACSYS_H_ */
