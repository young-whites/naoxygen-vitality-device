#include <applications/macBSP/Inc/bsp_bluetooth.h>
/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-11-06     teati       the first version
 */


/**
  * @brief  This is a function that setting the bluetooth's work-station.
  * @retval 0: disconnect
  *         1: connect
  */
void BLUETOOTH_SET_EN(FunctionalState status)
{
    if(status == ENABLE){
        HAL_GPIO_WritePin(macBluetooth_EN_GPIO_Port, macBluetooth_EN_Pin, GPIO_PIN_RESET);
    }
    else if(status == DISABLE){
        HAL_GPIO_WritePin(macBluetooth_EN_GPIO_Port, macBluetooth_EN_Pin, GPIO_PIN_SET);
    }
}


/**
  * @brief  This is a function that retrieves the Bluetooth status.
  * @retval 0: disconnect
  *         1: connect
  */
int8_t BLUETOOTH_GetState(void)
{
    return HAL_GPIO_ReadPin(macBluetooth_STA_GPIO_Port, macBluetooth_STA_Pin) ? 1 : 0;
}




/**
  * @brief  This is a function for scan bluetooth's status
  * @retval int
  */
int8_t  btPowerOnCnt = 0;                        // 蓝牙电源开启计时
int8_t  bBtCnct = 0;                             // 蓝牙连接状态
static void BLUETOOTH_Scan(void)
{
    static  int8_t  send=1;
    static  int8_t  cnt=0;
    int8_t  curr;

    if(btPowerOnCnt < 0xFF){
        btPowerOnCnt ++;
        // 首次上电开机后0.5秒，设置一次名称
        if(send && (btPowerOnCnt == 50)){
            send = 0;
            rt_device_write(serial2, RT_NULL, "AT+NAMES520_B\r\n", 15);
        }
    }

    // 获取蓝牙模块的连接状态
    curr = BLUETOOTH_GetState();
    if(curr != bBtCnct){
        /* 状态改变维持100ms就认为真的改变 */
        if(++cnt >= 100){
            cnt = 0;
            // 保存新的蓝牙连接状态
            bBtCnct = curr;
            if(bBtCnct){
             // 蓝牙刚联通时，蜂鸣器鸣叫2声
               Flag.BlueBeepCnt = 1;
               BEEP_Blink(2);
               Flag.BlueBeepCnt = 2;
            }
        }
    }
    else{
        cnt = 0;
    }
}




/*---------------------------------------------------------------------------------------------------------------*/
/* 以下是蓝牙线程的创建以及回调函数                                                                                                                                                                                                                       */
/*---------------------------------------------------------------------------------------------------------------*/
/**
  * @brief  beepTimer Callback Function
  * @retval void
  */
static void bluetoothTimer_callback(void* parameter)
{
    BLUETOOTH_Scan();
}



/**
  * @brief  beepTimer initialize
  * @retval int
  */
int bluetoothTimer_Init(void)
{
    static rt_timer_t   bluetoothTimer = RT_NULL;
    /* 创建beep的软件定时器线程 */
    bluetoothTimer = rt_timer_create("bluetoothTimer_callback", bluetoothTimer_callback, RT_NULL, 10,
            RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);

    /* 启动蓝牙扫描软件定时器 */
    if(bluetoothTimer != RT_NULL )
    {
        BLUETOOTH_SET_EN(ENABLE);
        rt_kprintf("PRINTF:%d. Bluetooth initialize succeed!\r\n",Record.kprintf_cnt++);
        rt_timer_start(bluetoothTimer);
    }

    return RT_EOK;
}
//INIT_APP_EXPORT(bluetoothTimer_Init);




int Bluetooth_Name_S520(void)
{
    rt_device_write(serial2, RT_NULL, "AT+NAMES520\r\n", 13);
    return RT_EOK;
}
MSH_CMD_EXPORT(Bluetooth_Name_S520,S520_NAME);


int Bluetooth_Name_S520_B(void)
{
    rt_device_write(serial2, RT_NULL, "AT+NAMES520_B\r\n", 15);
    return RT_EOK;
}
MSH_CMD_EXPORT(Bluetooth_Name_S520_B,S520_B_NAME);




