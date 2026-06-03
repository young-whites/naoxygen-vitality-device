#include <applications/macAPP/Inc/rtt_system_work.h>
/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author      Notes
 * 2024-11-19     teati       the first version
 */



extern void Proportional_Flow_Mode_Config(Mode_StructType mode);
extern void Proportional_Takt_Mode_Config(Mode_StructType mode);



static uint16_t sysTimeTick = 0;
static uint16_t pressCheckTick = 0;
static uint16_t powerOffTick = 0;
static uint16_t sysUnConnectTick = 0;
static void Timing_1ms(void)
{
    if((Record.Atmospheric_Tank_PressDat > (Record.TankPress )) && (Record.KeyPowerCnt != 0) && (Flag.SysConnectStatus == 1) && (Flag.SleepMode == 0)){
        Flag.Solenoid_Valve3_Limit = 1;
        Main_Gas_Circuit_On();
    }
    else if((Flag.APPWorkStatus != 2) && (Record.KeyPowerCnt != 0) && (Flag.SysConnectStatus == 1) && (Flag.SleepMode == 0)){
        Flag.Solenoid_Valve3_Limit = 0;
        Main_Gas_Circuit_Off();
    }
}



int AdjustValue = 0;
static void Timing_10ms(void)
{


    /*系统在运行过程,对休眠计数进行叠加,叠加到一定数时进入休眠*/
    if((Flag.APPWorkStatus == 1) && (Flag.SysConnectStatus == 1)){
        Record.System_Sleep_Time++; /* 这个变量在每次接收到新数据时赋值0 */
        if(Record.System_Sleep_Time > 18000){
            Flag.SleepMode = 1;
        }
    }
    else{
        Record.System_Sleep_Time = 0;
        Flag.SleepMode = 0;
    }

    /* 系统在没有运行的过程中,对待机休眠计数进行叠加,叠加到一定数时进入休眠 */
    if((Flag.APPWorkStatus == 0) && (Flag.SysConnectStatus == 1)){
        Record.System_Sleep_Tick++;
        if(Record.System_Sleep_Tick > 18000){
            Flag.SleepMode = 1;
        }
    }
    else{
        Record.System_Sleep_Tick = 0;
        Flag.SleepMode = 0;
    }

}


extern rt_uint8_t beepStart;

static void Timing_50ms(void)
{
    // 这个状态代表按键关机 -- 保持6秒泄气
    if(Record.KeyPowerCnt == 0 && Flag.SysConnectStatus == 0){
        powerOffTick++;
        if(powerOffTick < 140){
            Main_Gas_Circuit_On();
        }
        else{
            Main_Gas_Circuit_Off();
        }
    }
    else{
        powerOffTick = 0;
    }


    // 系统/蓝牙断开连接 --  关机先保持6秒的泄气
    if(Flag.SysConnectStatus == 0){
        sysUnConnectTick++;
        if(sysUnConnectTick <140){
            Main_Gas_Circuit_On();
        }
        else{
            Main_Gas_Circuit_Off();
        }
        Flag.StartCnt = 0;
        Flag.BlueBeepCnt = 0;
        beepStart = 0;
    }
    else{
        sysUnConnectTick = 0;
    }


    //--------------------------------------------------------------------------------

        if(Record.Flow_mode == 2 && Record.Takt_Mode == 0){
            if(Record.Output_PressDat < 2150){
                AdjustValue --;
                Proportional_Valve_ENBL_PWM_Set(1000, 820 - AdjustValue);
            }

            if(Record.Output_PressDat > 2250 ){
                AdjustValue++;
                Proportional_Valve_ENBL_PWM_Set(1000, 820 - AdjustValue);
            }
        }


        if(Record.Flow_mode == 3 && Record.Takt_Mode == 0){
            if(Record.Output_PressDat < 2450){
                AdjustValue --;
                Proportional_Valve_ENBL_PWM_Set(1000, 835 - AdjustValue);
            }

            if(Record.Output_PressDat > 2550 ){
                AdjustValue++;
                Proportional_Valve_ENBL_PWM_Set(1000, 835 - AdjustValue);
            }
        }


        if(Record.Flow_mode == 4 && Record.Takt_Mode == 0){
            if(Record.Output_PressDat < 2750){
                AdjustValue --;
                Proportional_Valve_ENBL_PWM_Set(1000, 900 - AdjustValue);
            }

            if(Record.Output_PressDat > 2850 ){
                AdjustValue++;
                Proportional_Valve_ENBL_PWM_Set(1000, 900 - AdjustValue);
            }
        }

}



uint8_t nowLocationCnt = 0;
uint8_t FixedTick = 0;

static void Timing_500ms(void)
{
    /*! 定时发送实时气压值到APP */
    if(Record.PressReadEnable == 1){
        USART2_Order_to_Andriod(Order_Android_GET_Real_Press);
    }


    if(Flag.HandDirction == 1){
        FixedTick++;
    }
    else {
        FixedTick = 0;
    }
}



static uint16_t progress_is_zero_tick = 0;
static void Timing_1s(void)
{

    Record.tickStart++;


    /* 处于正常连接状态,去主动询问手柄位置 */
    if(BLUETOOTH_GetState() == 1 && Record.BluetoothConnectCnt < 3){
        USART4_Order_to_Mechanical_Handle(Order_HAND_SEND_HAND_LOCATION);
        Record.BluetoothConnectCnt++;
    }

    /* -刚启动时会有一个10秒的倒计时,用于记录这个值 */
    if(Flag.APPWorkStatus == 1 && (Record.StartCnt == 1)){
        Record.SecondDown++;
    }


    /* -气压检测,通过比较实时气压值来限制设备气压工作在预定的气压范围内
     * -如果主气路电磁阀3开启,开启状态是泄气,在泄气的情况下还是超压了,就得关掉
     */
    if(Flag.Solenoid_Valve3_Limit == 1)
    {
        /*! 如果实时监测的大气罐气压大于最大限值叠加计数,持续超过15秒就认为气缸损坏 */
            if(Record.Atmospheric_Tank_PressDat >= PRINCIPAL_AIR_PRESSURE_MAX){
                pressCheckTick++;
                if(pressCheckTick > 15){
                    Record.KeyPowerCnt = 0;
                    Main_Gas_Circuit_Off();
                    Flag.Solenoid_Valve3_Limit = 0;
                }
            }
            else{
                pressCheckTick = 0;
            }
    }


    /* 当手柄的进度条显示为0，即值到0x64时，开启对抵达前端的计数，超过计数时，回退 */
    if((Record.Hand_Location == 0x64 || Record.Hand_Location == 0x63) && (Record.WorkMode == 1)){
        progress_is_zero_tick ++ ;

        if(progress_is_zero_tick > 22 ){
            Flag.Action = 1;
            progress_is_zero_tick = 0;
            Record.HandReset = 1;
        }
    }

    else{
        progress_is_zero_tick = 0;
    }
}



static uint16_t OldWorkCnt = 0;
static void Old_Timing_1s(void)
{

    /* -气压检测,通过比较实时气压值来限制设备气压工作在预定的气压范围内
     * -如果主气路电磁阀3开启,开启状态是泄气,在泄气的情况下还是超压了,就得关掉
     */
    if(Flag.Solenoid_Valve3_Limit == 1)
    {
        /*! 如果实时监测的大气罐气压大于最大限值叠加计数,持续超过15秒就认为气缸损坏 */
            if(Record.Atmospheric_Tank_PressDat >= PRINCIPAL_AIR_PRESSURE_MAX){
                pressCheckTick++;
                if(pressCheckTick > 15){
                    Record.KeyPowerCnt = 0;
                    Main_Gas_Circuit_Off();
                    Flag.Solenoid_Valve3_Limit = 0;
                }
            }
            else{
                pressCheckTick = 0;
            }
    }


    OldWorkCnt++;
    if(OldWorkCnt <= 60){
        Flag.Old_WorkMode = 1;
    }
    else if(OldWorkCnt > 60 && OldWorkCnt <= 120){
        Flag.Old_WorkMode = 2;
    }
    else if(OldWorkCnt > 120 && OldWorkCnt <= 180){
        Flag.Old_WorkMode = 3;
    }
    else if(OldWorkCnt > 180 && OldWorkCnt <= 240){
        Flag.Old_WorkMode = 4;
    }
    else if(OldWorkCnt > 240){
        OldWorkCnt = 0;
    }
}



static void Old_Timing_1ms(void)
{

    /* -气压检测,通过比较实时气压值来限制设备气压工作在预定的气压范围内
     * -如果主气路电磁阀3开启,开启状态是泄气,在泄气的情况下还是超压了,就得关掉
     */
    if(Flag.Solenoid_Valve3_Limit == 1)
    {
        /*! 如果实时监测的大气罐气压大于最大限值叠加计数,持续超过15秒就认为气缸损坏 */
            if(Record.Atmospheric_Tank_PressDat >= PRINCIPAL_AIR_PRESSURE_MAX){
                pressCheckTick++;
                if(pressCheckTick > 15){
                    Record.KeyPowerCnt = 0;
                    Main_Gas_Circuit_Off();
                    Flag.Solenoid_Valve3_Limit = 0;
                }
            }
            else{
                pressCheckTick = 0;
            }
    }
}



/*---------------------------------------------------------------------------------------------------------------*/
/* 以下是系统扫描线程的创建以及回调函数                                                                          */
/*---------------------------------------------------------------------------------------------------------------*/
/**
  * @brief  sysTimer Callback Function -- 10ms entry
  * @retval void
  */

static void sysTimer_callback(void* parameter)
{

    if(Flag.OldMode == 0)
    {
        sysTimeTick++;

        if(sysTimeTick > 60000){
            sysTimeTick = 0;
        }

        if((sysTimeTick % 1)    == 0)   Timing_1ms();
        if((sysTimeTick % 10)   == 0)   Timing_10ms();
        if((sysTimeTick % 50)   == 0)   Timing_50ms();
        if((sysTimeTick % 500)  == 0)   Timing_500ms();
        if((sysTimeTick % 1000) == 0)   Timing_1s();
    }
    else if(Flag.OldMode == 1)
    {
        sysTimeTick++;

        if(sysTimeTick > 60000){
            sysTimeTick = 0;
        }

        if((sysTimeTick % 1)    == 0)   Old_Timing_1ms();
        if((sysTimeTick % 1000) == 0)   Old_Timing_1s();
    }
}



/**
  * @brief  keyTimer initialize
  * @retval int
  */
rt_timer_t sysTimer;
int sysTimer_Init(void)
{
    /* 创建key软件定时器线程 */
    sysTimer = rt_timer_create("sysTimer_callback", sysTimer_callback, RT_NULL, 1, RT_TIMER_FLAG_SOFT_TIMER | RT_TIMER_FLAG_PERIODIC);
    /* 如果keyTimer句柄创建成功,开启软件定时器 */
    if(sysTimer != RT_NULL)
    {
        rt_kprintf("PRINTF:%d. sysTimer initialize succeed!\r\n",Record.kprintf_cnt++);
        rt_timer_start(sysTimer);
    }

    return RT_EOK;
}
INIT_APP_EXPORT(sysTimer_Init);







