/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-06-12     zphu       the first version
 */
#ifndef APPLICATIONS_MACBSP_INC_BSP_PWM_H_

#define APPLICATIONS_MACBSP_INC_BSP_PWM_H_
#include <applications/macSYS/Inc/macSYS.h>

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* nDECAY 引脚配置 */
typedef enum{
  H_decay = 0,
  L_decay,
  X_decay
}SS6840_DECAY;



/**
  * @brief  定义一个控制电机正反旋转的数据类型
  * @param  dir    : 0 -> clockwise        1 -> anticlockwise
  * @retval None
  */
typedef enum{
    anticlockwise = 0,
    clockwise,
}SS6840_DIR;




int PWM_Init(void);
void SS6840H_Mode_Config(SS6840_DECAY decayMode,SS6840_DIR Dir);
void Proportional_Valve_ENBL_PWM_Set(rt_uint32_t Period, rt_uint32_t Dutyfactor);
void Proportional_Valve_ENBL_PWM_Disable(void);
void Proportional_Valve_ENBL_PWM_Enable(void);
void Proportional_Valve_PEN_PWM_Set(rt_uint32_t Period, rt_uint32_t Dutyfactor);
void Proportional_Valve_PEN_PWM_Enable(void);
void Proportional_Valve_PEN_PWM_Disable(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* APPLICATIONS_MACBSP_INC_BSP_PWM_H_ */
