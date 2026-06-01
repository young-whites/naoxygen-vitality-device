#include "sys_status.h"



/**
  * @brief  系统全局参数初始化
  * @param  无
  * @retval 无
  */

void SysFlag_Init(void)
{

}






/**
  * @brief  检测系统工作状态标志
  * @param  无
  * @retval 无
  */

void Check_SysWorkStatus(void)
{
    static uint8_t AutoHandCheckCnt = 0;
    uint8_t PowerLEDToggleCnt = 0;
    //-----------------------------------------------------------------------------------------------
    /* 按键计数检测 -- 按键次数表征电源开关 */
    /* KeyPowerPressCnt：0 , it indicates that the device is powered off */
    if((Record.KeyPowerCnt == 0) && (Record.KeyLong4sCnt != 1))
    {
        /*! 运行与暂停标志位 */
        BLUETOOTH_SET_EN(DISABLE);
        macPower_OFF();
        Power_Off_LED();
    }
    /* KeyPowerPressCnt：1 , it indicates that the device is powered on */
    if((Record.KeyPowerCnt == 1) && (Record.KeyLong4sCnt != 1))
    {
        macPower_ON();
        Power_On_LED();
        BLUETOOTH_SET_EN(ENABLE);	
    }
    if((Record.KeyPowerCnt == 2) && (Record.KeyLong4sCnt != 1))
    {
        Record.KeyPowerCnt = 0;
    }

    /* KeyLong4sCnt：1 , it indicates that the device is going to check itself */
    if(Record.KeyLong4sCnt == 1)
    {
        /*! 发送手柄自检测指令 */
        for(;AutoHandCheckCnt < 1; AutoHandCheckCnt = 2){
            /*! 关闭蓝牙的电源，防止在手柄自检过程中响应蓝牙的其他动作指令  */
            BLUETOOTH_SET_EN(DISABLE);
        }

        if(PowerLEDToggleCnt == 0){
            Power_Off_LED();
            PowerLEDToggleCnt++;
        }
        else if(PowerLEDToggleCnt == 1){
            PowerLEDToggleCnt = 0;
            Power_On_LED();
        }
    }


    //-----------------------------------------------------------------------------------------------

}







