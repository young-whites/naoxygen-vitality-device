/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-11-14     teati       the first version
 */
#ifndef APPLICATIONS_MACAPP_INC_RTT_UART4_DECODE_H_

#define APPLICATIONS_MACAPP_INC_RTT_UART4_DECODE_H_
#include <applications/macSYS/Inc/macSYS.h>

/* 解析指令数据的实际操作指令宏 */
// 手柄与串口4链路相关的指令宏------------------------------------------------------------
#define      FRAME_HAND_SET_CHECK_HAND_CMD                       (0xBB)      // 设置：手柄检测
#define      FRAME_HAND_SET_CHECK_MOTOR_PROGRESS_CMD             (0x2E)      // 设置：设置手柄的进度
#define      FRAME_HAND_SET_COME_BACK_CMD                        (0x2F)      // 设置：设置手柄退回
#define      FRAME_HAND_SET_WORK_MODE_CMD                        (0x17)      // 设置：设置手柄的工作模式
#define      FRAME_HAND_SET_WHETHER_HAVE_HAND_CMD                (0x1C)      // 设置：检测是否是机械手柄
#define      FRAME_HAND_SET_SAVE_STEP_PROGRESS                   (0x3E)      // 设置：告诉手柄此时需要保存电机进度
#define      FRAME_HAND_SET_ATUO_CHECK_CMD                       (0x3D)      // 设置：手柄自检测指令
#define      FRAME_HAND_SET_ADJUST_SPEED_CMD                     (0x15)      // 设置：手柄速度调整指令
#define      FRAME_HAND_ACT_WORK_CMD                             (0xC0)      // 设置：运行过程中控制节拍和流量
#define      FRAME_HAND_SET_HAND_BACK_FORCE_CMD                  (0xF5)      // 设置：告知手柄强制回退

#define      FRAME_HAND_GET_VERSION_CMD                          (0x19)      // 获取：手柄的版本号
#define      FRAME_HAND_SET_DIRCTION_MOVE_CMD                    (0x2D)      // 设置：手柄行进指令
#define      FRAME_HAND_GET_MOTOR_RUN_CMD                        (0xF2)      // 获取：手柄电机处于工作状态
#define      FRAME_HAND_GET_HANDLE_EXSIT_CMD                     (0xF3)      // 获取：手柄重新插上的指令

uint8_t USART4_Portocol_Get_Command(rt_device_t dev, uint8_t USART_ID);
void Protocol_Operation_USART4(rt_device_t dev,uint8_t* CmdBuf);
void USART4_Send_Command_to_Principal(uint8_t DataLen, uint8_t CmdType, uint8_t CmdStatus, uint8_t* DataBuf);
void USART4_Order_to_Mechanical_Handle(uint8_t order);
int uart4_decodeThread_Init(void);


#endif /* APPLICATIONS_MACAPP_INC_RTT_UART4_DECODE_H_ */
