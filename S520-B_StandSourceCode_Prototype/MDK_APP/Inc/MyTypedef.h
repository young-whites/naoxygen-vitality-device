#ifndef __MYTYPEDEF_H
#define __MYTYPEDEF_H
#include "sys.h"


typedef struct
{

  uint16_t  update;	              // 升级指令  
  uint8_t   OldStart;             // 开启老化测试用例    
  uint8_t   WorkStatus;           // 系统工作状态    (0：停止工作    1：启动工作     2：自检状态）
  uint8_t   Hand_ReturnStatus;    // 手柄回退状态    (0：没有回退    1：正在回退中)


} Flag_StructType;
extern   Flag_StructType Flag;




/**
  * @brief  用于记录APP下发的数据的类型结构体
  * @param  Flush_IntervalGears [0:4]
  */
typedef struct
{
  uint8_t  KeyLong4sCnt;  // 电源长按计数
  uint8_t  KeyPowerCnt;   // 电源短按计数
}  Record_StructType;
extern   Record_StructType Record;





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
}Mechanical_Handle_Order_StructType;












#endif /*__MYTYPEDEF_H*/
