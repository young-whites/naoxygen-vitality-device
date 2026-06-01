#ifndef __USART2_DECODE_H
#define __USART2_DECODE_H
#include "sys.h"



//------------------------------------------------------------------------
#define		          DEVICE_ID_H					                   0x00		// 设备ID高字节
#define		          DEVICE_ID_L					                   0x01		// 设备ID低字节
//------------------------------------------------------------------------
#define		          FRAME_HEAD1					                   0x55	    // 帧头1
#define		          FRAME_HEAD2					                   0xAA		// 帧头2
#define		          FRAME_TYPE_ACT				                   0x31		// 帧类型:动作命令
#define		          FRAME_TYPE_SET				                   0x32		// 帧类型:参数设置
#define		          FRAME_TYPE_GET				                   0x33		// 帧类型:参数获取
#define		          FRAME_TYPE_POST				                   0x66		// 帧类型:主动上报
#define		          FRAME_STATE_ASK				                   0x02		// 帧状态:上位请求
#define		          FRAME_STATE_ACK				                   0x01		// 帧状态:下位应答
#define		          FRAME_STATE_ERR				                   0x00		// 帧状态:校验出错
//功能码协议参数------------------------------------------------------------------------
#define               FRAME_APP_SET_WORK_SWITCH_CMD                    0x01     // 设置：关机指令
#define               FRAME_APP_SET_SYS_UPDATE_CMD                     0xAA     // 设置：升级指令
#define               FRAME_APP_SET_HAND_WORK_STATUS_CMD               0x02     // 设置：手柄睡眠状态指令
#define               FRAME_APP_SET_TAKT_MODE_CMD                      0x0F     // 设置：配置节拍模式指令
#define               FRAME_APP_SET_FLOW_MODE_CMD                      0x0D     // 设置：配置流量模式指令
#define               FRAME_APP_SET_PRESSURE_MODE_CMD                  0x0E     // 设置：配置压力模式指令
#define               FRAME_APP_SET_MASTER_CYLINDER_CMD                0x2C     // 设置：配置主气缸的限压模式指令
#define               FRAME_APP_SET_MACHINE_HAND_CMD                   0x2D     // 设置：控制机械推杆运动指令
#define               FRAME_APP_SET_MOTOR_LOCATION_CMD                 0x2E     // 设置：设置手柄的电机的位置指令
#define               FRAME_APP_SET_HAND_RETURN_STATUS_CMD             0x2F     // 设置：手柄的退回状态
#define               FRAME_APP_SET_HAND_ADJUST_SPEED_CMD              0x29     // 设置：接收到此命令给手柄发送0x15调速指令




#define               SYS_STATION                                      0x01
#define               C                                                0X43


uint16_t CrcCalc_Crc16Modbus(uint8_t *dat,uint8_t len);
void USART2_Decode_Receive_Command(unsigned  char data);
void USART2_Send_Command_to_Principal(u8 Datalength,u8 Cmdtype,u8 Cmdstatus,u8 *Datafile);
void USART2_Order_to_Andriod(uint8_t Order);
void USART2_Deal_correct_data(unsigned  char  *buf, unsigned char len);








#endif
