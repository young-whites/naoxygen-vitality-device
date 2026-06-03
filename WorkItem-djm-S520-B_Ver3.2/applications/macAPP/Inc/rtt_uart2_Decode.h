 /*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-06-05     zphu       the first version
 */
#ifndef RTT_UART2_DECODE_H
#define RTT_UART2_DECODE_H
#include <applications/macSYS/Inc/macSYS.h>

/* 消息队列方法1,该方法弃用 */
/*!当解析数据高并发的状态下,DMA接收数据会把连续的两条55AA的解析指令缓存进入到一条消息队列中，此时解析会忽略掉后续的指令，造成指令遗失 */
#define VERSION_2point0_Method  1
/* 消息队列方法2 */
#define VERSION_2point1_Method  0


/* 信息打印控制宏 */
#define WORK_VERSION1                   0       // 工作模式版本1

#ifdef BSP_USING_UART1
#define USART1_SEND_CMD_INFO_PRINTF     0       // 串口1发送指令信息打印
#define USART1_REC_CMD_PRINTF           0       // 串口1接收指令信息打印
#endif

#ifdef BSP_USING_UART2
#define USART2_BAUD_RATE_9600           9600    // 波特率
#define USART2_SEND_CMD_INFO_PRINTF     1       // 串口2发送指令信息打印
#define USART2_REC_CMD_PRINTF           1       // 串口2接收指令信息打印
#endif

#ifdef BSP_USING_UART3
#define USART3_SEND_CMD_INFO_PRINTF     0       // 串口3发送指令信息打印
#define USART3_REC_CMD_PRINTF           0       // 串口3接收指令信息打印
#endif


#ifdef BSP_USING_UART4
#define USART4_BAUD_RATE_9600           9600    // 波特率
#define USART4_SEND_CMD_INFO_PRINTF     0       // 串口4发送指令信息打印
#define USART4_REC_CMD_PRINTF           1       // 串口4接收指令信息打印
#define USE_USART4_AGAIN_DECODE         1       // 使用串口4的二次解析
#endif



#if VERSION_2point0_Method

extern rt_device_t  serial2;

/* 创建消息队列结构体参数 */
typedef struct{
    rt_device_t device_t;
    rt_size_t   size;
}MessageQueue;


/* 创建串口接收与发送缓冲区结构体参数 */
#define MAX_DATA_LENGTH 512
typedef struct{
    uint8_t rxBuffer[MAX_DATA_LENGTH];  // 循环队列缓冲区
    volatile rt_uint16_t rx_index;      // 数据索引
    volatile rt_uint16_t head;          // 队列头指针（读位置）
    volatile rt_uint16_t tail;          // 队列尾指针（写位置）
    rt_mutex_t lock;                    // 互斥锁
}xUsart_Structure;

int uart2_decodeThread_Init(void);
uint8_t USART2_Portocol_Get_Command(rt_device_t dev, uint8_t USART_ID);
void USART2_Send_Command_to_Principal(uint8_t DataLen, uint8_t CmdType, uint8_t CmdStatus, uint8_t* DataBuf);
void Protocol_Operation_USART2(rt_device_t dev,uint8_t* CmdBuf);
void USART2_Order_to_Andriod(uint8_t order);

#endif /* VERSION_2point0_Method */




//---------------------------------------------------------------------------------------------------------------------------

// 指令的最小长度为4
#define CMD_MINI_LENGTH 4

/* 主板设备码以及版本号 */
#define DEVICE_ID_H     0x00
#define DEVICE_ID_L     0x01
#define DEVICE_VERSION  0x20  // Version 3.2

/* 手柄设备码以及版本号 */
#define HAND_DEVICE_ID_H    0x00
#define HAND_DEVICE_ID_L    0x01


/* 解析指令数据的指令类型以及状态宏 */
//------------------------------------------------------------------------
#define       FRAME_HEAD1                                        (0x55)      // 帧头1
#define       FRAME_HEAD2                                        (0xAA)      // 帧头2
#define       FRAME_TYPE_ACT                                     (0x31)      // 帧类型:动作命令
#define       FRAME_TYPE_SET                                     (0x32)      // 帧类型:参数设置
#define       FRAME_TYPE_GET                                     (0x33)      // 帧类型:参数获取
#define       FRAME_TYPE_POST                                    (0x66)      // 帧类型:主动上报
#define       FRAME_STATE_ASK                                    (0x02)      // 帧状态:上位请求
#define       FRAME_STATE_ACK                                    (0x01)      // 帧状态:下位应答
#define       FRAME_STATE_ERR                                    (0x00)      // 帧状态:校验出错


// APP（主控）串口2链路相关的指令宏------------------------------------------------------------
#define      FRAME_APP_SET_WORK_SWITCH_CMD                       (0x01)      // 设置：关机指令
#define      FRAME_APP_SET_SYS_UPDATE_CMD                        (0xAA)      // 设置：升级指令
#define      FRAME_APP_SET_HAND_WORK_STATUS_CMD                  (0x02)      // 设置：手柄睡眠状态指令
#define      FRAME_APP_SET_TAKT_MODE_CMD                         (0x0F)      // 设置：配置节拍模式指令
#define      FRAME_APP_SET_FLOW_MODE_CMD                         (0x0D)      // 设置：配置流量模式指令
#define      FRAME_APP_SET_PRESSURE_MODE_CMD                     (0x0E)      // 设置：配置压力模式指令
#define      FRAME_APP_SET_MASTER_CYLINDER_CMD                   (0x2C)      // 设置：配置主气缸的限压模式指令
#define      FRAME_APP_SET_MACHINE_HAND_CMD                      (0x2D)      // 设置：控制机械推杆运动指令
#define      FRAME_APP_SET_MOTOR_LOCATION_CMD                    (0x2E)      // 设置：设置手柄的电机的位置指令
#define      FRAME_APP_SET_HAND_RETURN_STATUS_CMD                (0x2F)      // 设置：手柄的退回状态
#define      FRAME_APP_SET_HAND_ADJUST_SPEED_CMD                 (0x29)      // 设置：接收到此命令给手柄发送0x15调速指令
#define      FRAME_APP_SET_HAND_OLD_MODE_CMD                     (0xEC)      // 设置：进入老化模式
#define      FRAME_APP_SET_HAND_SPEED_SET                        (0xED)      // 设置：手柄电机的速度


#define      FRAME_APP_GET_FLOW_MODE_CMD                         (0x0D)      // 获取：获取流量模式
#define      FRAME_APP_GET_PRESS_MODE_CMD                        (0x0E)      // 获取：获取压力模式
#define      FRAME_APP_GET_TAKT_MODE_CMD                         (0x0F)      // 获取：获取节拍模式
#define      FRAME_APP_GET_REAL_PRESSURE_CMD                     (0x14)      // 获取：实时气压值
#define      FRAME_APP_GET_BOARD_VERSION_CMD                     (0x18)      // 获取：主板版本号
#define      FRAME_APP_GET_HAND_VERSION_CMD                      (0x19)      // 获取：手柄版本号


// 广播指令---------------------------------------------------------------------------------------
#define      FRAME_BOARDCAST_CMD                                 (0x00)      // 广播指令（用于测试）


/* 函数进行解析指令后的返回宏 */
#define CMD_ERROR   0
#define CMD_TRUE    1

uint16_t CrcCalc_Crc16Modbus(uint8_t *dat,uint8_t len);




#endif /* RTT_UART2_DECODE_H */
