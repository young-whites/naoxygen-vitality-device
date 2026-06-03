/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-09-08     Administrator       the first version
 */
#ifndef APPLICATIONS_MACSYS_INC_MACTYPEDEF_H_

#define APPLICATIONS_MACSYS_INC_MACTYPEDEF_H_

#include <applications/macSYS/Inc/macSYS.h>

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

#define USE_MOVING_AVERAGE_FILTER       0

/*! 经测试，气泵的物理泄压值为3850左右，即绝对值为285Kpa左右 */
#define PRINCIPAL_AIR_PRESSURE_MAX      3700    // 主气路的最大限压值 -- 280Kpa(这是绝对气压值)
#define COMPRESSOR_AIR_PRESSURE_MAX     3800    // 压缩机物理泄压阀工作的最大气压

#define KEY_OLD_ENABLE  0                   //使能按键启用老化模式

// 几个用于配置的压力参数
#define PRESS_CONFIG_VALUE_1    1250
#define PRESS_CONFIG_VALUE_2    1350
#define PRESS_CONFIG_VALUE_3    1500
#define PRESS_CONFIG_VALUE_4    2000


// 以下为移植时必须需要的结构体等的初始化---------------------------------------------------------------------------------------------------------
extern uint8_t usartID;
typedef struct {
    rt_uint8_t  Empty;                           // 空值
    rt_uint8_t  kprintf_cnt;                     // 用于打印序列
    rt_uint8_t  Log_cnt;                         // 用于日志打印序列
    rt_uint32_t Log_Uart_cnt;
    rt_uint16_t DBG_cnt;

    //------------------------------------------------------------
    rt_uint8_t  KeyPowerCnt;                     // 开关电源按下次数
    rt_uint8_t  KeyLong4sCnt;                    // 开关键长按4s次数
    rt_uint8_t  Bluetooth_Status;                // 蓝牙连接状态
    rt_uint8_t  StartCnt;                        // 记录启动的次数
    rt_uint16_t  SecondDown;                     // 记录倒计时


    //------------------------------------------------------------
    /*! 气压的相关参数  */
    rt_uint16_t Output_PressDat;                 // 串口1检测的气压值 -- 输出口的气压
    rt_uint8_t  Output_PressDat1_H;              // 串口1检测的气压值的高字节
    rt_uint8_t  Output_PressDat1_L;              // 串口1检测的气压值的低字节
    rt_uint16_t Atmospheric_Tank_PressDat;       // 串口3检测的气压值 -- 大气罐的气压
    rt_uint8_t  Atmospheric_Tank_PressDat3_H;    // 串口3检测的气压值的高字节
    rt_uint8_t  Atmospheric_Tank_PressDat3_L;    // 串口3检测的气压值的低字节
    rt_uint16_t Atmospheric_Tank_Filter_PressDat;// 大气罐经过滤波器后的压力数值
    rt_uint32_t PressModeValue;                  // 设置的手柄不同喷射力度下的压力参数
    rt_uint16_t PressReadValue;                  // 用于给APP读取当前喷射力度模式下的压力值
    rt_uint8_t  PressReadEnable;                 // 气压实时读取标志位
    rt_uint16_t TankPress;                       // 主缸稳定气压
    float       PressOut;                        // PID计算后的气压值
    //------------------------------------------------------------
    /*! 手柄的相关参数 */
    rt_uint8_t  Hand_Version;                    // 手柄的版本号信息
    int16_t  Hand_Location;                      // 手柄的位置信息


    rt_uint16_t Real_FlowValue;                  // 当前的流量值
    rt_uint16_t Set_FlowValue;                   // 设定的流量值
    rt_uint8_t  Flow_mode;                       // 流量模式值
    rt_uint8_t  Takt_Mode;                       // 当前节拍
    rt_uint8_t  Hand_Press_Mode;                 // 手柄喷射压力模式
    rt_uint32_t System_Sleep_Time;               // 系统进入休眠计数
    rt_uint32_t System_Sleep_Tick;               // 系统进入休眠计数
    rt_uint8_t  Main_Tank_Excess_quantity;       // 大气罐的气压数据超越限制的数量
    rt_uint8_t  HandReset;                       // 手柄复位回退指令
    rt_uint16_t BluetoothConnectCnt;             // 蓝牙连接后计数
    rt_uint16_t HandSpeed;                       // 手柄速度
    rt_uint8_t  HandSpeedAdjust_Mode;            // 调整手柄电机速度的模式（1：减   0：加）
    rt_uint8_t  HandsSpeedAdjust;                // 调整手柄电机速度的差值
    rt_uint8_t  WorkMode;                        // 工作模式，0：手动模式，1：进程模式
    rt_uint32_t tickStart;                       // 自上电后就开始进行计数

}RecordStruct;
extern RecordStruct Record;



typedef struct {
    rt_uint32_t log_cnt;
    rt_uint8_t  kprintf_cnt;
    rt_uint8_t  update;                 // 远程升级标志位
    rt_uint8_t  OldMode;                // 老化模式标志

    rt_uint8_t  HandConnect;            // 机械手柄的连接状态        （0：未连接                      1：已连接）
    rt_uint8_t  Abnormal;               // 检查系统是否异常状态    （0：系统异常需要关机    1：系统正常可以开机）
    rt_uint8_t  APPWorkStatus;          // 指APP指示暂停或者运行工作状态
    rt_uint8_t  SysConnectStatus;       // 指蓝牙连接成功后的状态（0：停止工作                   1：启动工作                       2：自检状态）
    rt_uint8_t  Hand_ReturnStatus;      // 手柄回退状态                   （0：没有回退                   1：正在回退中）
    rt_uint8_t  Solenoid_Valve3_Limit;  // 主气路电磁阀限制            （0：关闭                           1：开启）
    rt_uint8_t  Output_Press_EN;        // 压力输出允许标志位        （0：不允许                       1：允许）
    rt_uint8_t  SleepMode;              // 休眠模式                           （0：唤醒                           1：休眠）
    rt_uint8_t  Old_WorkMode;           // 老化手柄的节拍工作模式
    rt_uint8_t  HandDirction;           // 手柄电机的运行方向
    rt_uint8_t  Action;                 // 回退动作
    rt_uint16_t StartCnt;               // 用于记录自上电以来的启动次数
    rt_uint8_t  BlueBeepCnt;           // 用于记录蓝牙上电以来的连接启动次数
}FlagStruct;
extern FlagStruct Flag;







/**
  * @brief  枚举类型,指令解码步骤
  * @param  None
  */
typedef enum
{
    Decode_Step_0 = 0,
    Decode_Step_1,
    Decode_Step_2,
    Decode_Step_3,
    Decode_Step_4,
    Decode_Step_5,
    Decode_Step_6,
    Decode_Step_7
}DecodeStep_StructType;


/**
  * @brief  枚举类型,指令解码步骤
  * @param  None
  */
typedef enum
{
    MODE_ZERO = 0,
    MODE_ONE,
    MODE_TWO,
    MODE_THREE,
    MODE_FOUR,
    MODE_FIVE,
    MODE_SIX,
    MODE_SEVEN,
    MODE_EIGHT,
    MODE_NINE,
    MODE_TEN,
    MODE_ELEVEN
}Mode_StructType;


/**
  * @brief  枚举类型,指令解码步骤
  * @param  None
  */
typedef enum
{
    Dirction_stop = 0,
    Dirction_back,
    Dirction_forward,
    Dirction_reset,
    Dirction_back_stop,
}Dirction_StructType;







//---------------------------------------------------------------------------------------------------------

/**
  * @brief  枚举类型,指令码
  * @param  None
  */
typedef enum
{
    Order_Android_ACT_HAND_LOCATION_CMD = 0,
    Order_Android_ACT_HAND_LOCATION_FULL_CMD,
    Order_Android_ACT_HAND_RETURN_STATUS,
    Order_Android_GET_HAND_VERSION,
    Order_Android_ACT_HAND_IS_RETURNING_STATUS,
    Order_Android_GET_Real_Press,
}Android_Order_StructType;



/**
  * @brief  枚举类型,指令码
  * @param  None
  */
typedef enum
{
    Order_HAND_ACT_WORK_MODE_ZERO_CMD = 50,
    Order_HAND_ACT_WHETHER_HAVE_HAND,
    Order_HAND_ACT_SAVE_STEP_PROGRESS,
    Order_HAND_ACT_AUTO_CHECK,
    Order_HAND_ACT_WORK_MODE_CMD,
    Order_HAND_ACT_ADJUST_SPEED,
    Order_HAND_ACT_CTRL_BACK,
    Order_HAND_ACT_CTRL_FORWARD,
    Order_HAND_ACT_CTRL_STOP,
    Order_HAND_ACT_CTRL_RESET,
    Order_HAND_ACT_CTRL_BACK_STOP,
    Order_HAND_SEND_HAND_LOCATION,
    Order_HAND_SEND_HAND_VERSION,
    Order_HAND_SEND_HAND_LOCATION_EXSIT,
    Order_HAND_SEND_HAND_Speed,
    Order_HAND_ACT_CTRL_BACK_STOP_FORCE,
}Mechanical_Handle_Order_StructType;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* APPLICATIONS_MACSYS_INC_MACTYPEDEF_H_ */
