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


#define  MOVE_CMD              0x2d  // Motor control: stop/forward/backward
#define  LITTLE_CMD            0x15  // Speed mode selection
#define  ACK_CMD               0x1c  // Handle acknowledge
#define  HANDLE_CHECK          0xbb  // Handle online response
#define  REVISION_CMD          0x19  // Firmware version query
#define  REVISION              75    // Firmware version (1-99)

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
