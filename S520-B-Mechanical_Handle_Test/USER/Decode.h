#ifndef  _DECODE_H
#define  _DECODE_H

#define  DeviceID_H 0x00
#define  DeviceID_L 0X01

#include "stm8s.h"
#include "usart.h"

#define  SENDCMD                0X31//发送命令
#define  SENDDATA_STATUS        0X32//发送数据或状态
#define  READDATA_STATUS        0X33//读取数据或状态

#define  ILLEGAL_DATA           0x00
#define  RECEIVE_DATA_SUCCESS   0x01
#define  NORMAL_DATA_SEND       0x02


#define  MOTOR_MODE_CMD        0x17//工作模式功能码
#define  Speed_Adjust_Cmd      0xED//工作模式功能码
#define  MOVE_CMD              0x2d//控制电机前进后退和停止
#define  CHECK_MOTOR_CMD       0x3d//控制电机校验
#define  LITTLE_CMD            0x15//电机速度微调//改为调节电机模式速度。
#define  ACK_CMD               0x1c//询问是否有机械手柄功能码
#define  MOTOR_STEP_SAVE        0x3e     //表示电机进度保存
#define  MOTOR_STEP_CMD        0x2e     //表示电机进度
#define  RUTURN_FLAG_CMD       0x2F     //表示电机正在自动退回
#define  HANDLE_CHECK          0xbb//表示手柄检测
#define  DATA_CMD              0xbf//发送数据到主板，主板仿真可以看到参数值。
#define  REVISION_CMD          0x19  //版本号指令
#define  REVISION              75//版本号，从1到99整数  此版本号是在程序变动后烧录到flash中的
#define  LIMIT_TEST_CMD        0x40 //limit switch test: back to rear limit, then forward N pulses

#define  Channel   1

//#define  RUN                 0X01
typedef struct
{
    u16 FrameHeader;//帧头
    u8  DataLength;//数据长度
    u8  DeviceIDH;//设备ID高位
    u8  DeviceIDL;//设备ID低位
    u8  CmdType;//命令类型
    u8  CmdStatu;//命令状态
    u8  DataField[20];//数据域
    u16 Crc16;//CRC校验
} DecodeReceiveDataTypedef;

#define DecodeReceiveDataTypedef_Defaults {0,\
                                          0,\
                                          0,\
                                          0,\
                                          0,\
                                          0,\
                                          0,\
                                          0}
typedef enum
{
    FRAMEHEADER,
    DATALENGTH,
    DEVICEID,
    CMDTYPE,
    CMDSTATU,
    DATAFIELD,
    CRC16
} USARTCMD_StatusTypedef;


void   SendDataPacketToPeripheral(u8 Datalength,u8 Cmdtype,u8 Cmdstatus,u8 *Datafile);
void   DecodeMessageHandle(u8 Usart_Num,DecodeReceiveDataTypedef *Decode_Buffer);
void   CMD_MessageHandle(u8 Usart_Num,u8 data_length,u8 *data_buffer);
void   SENDDATA_STATUS_MessageHandle(u8 Usart_Num,u8 data_length,u8 *data_buffer);
void   SendMessageToDevice(void);
void   CommunicationWithMainBoard(void);
void   senddata(u8 type, u8 data );



#endif
