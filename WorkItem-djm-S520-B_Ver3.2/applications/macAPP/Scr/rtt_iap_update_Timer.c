/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-12-30     teati       the first version
 */
#include "rtt_iap_update_Timer.h"




/*---------------------------------------------------------------------------------------------------------------*/
/* 以下是iap扫描线程的创建以及回调函数                                                                          */
/*---------------------------------------------------------------------------------------------------------------*/
/**
  * @brief  keyTimer Callback Function
  * @retval void
  */
extern rt_thread_t uart1_Thread_Handle;
extern rt_thread_t uart2_decodeThread_Handle;
extern rt_thread_t uart3_Thread_Handle;
extern rt_thread_t uart4_decodeThread_Handle;
extern rt_timer_t sysTimer;
static void iapTimer_callback(void* parameter)
{
    static rt_err_t usart1_delete;
    static rt_err_t usart2_delete;
    static rt_err_t usart3_delete;
    static rt_err_t usart4_delete;
    static rt_err_t sysTimer_stop;

    /* 远程升级 */
    if(Flag.update == 1)
    {

        /* 关闭其他所有的线程 */
        usart1_delete = rt_thread_delete(uart1_Thread_Handle);
        if(usart1_delete == RT_EOK){
            rt_kprintf("PRINTF:%d. usart1_thread was deleted!!\r\n",Record.kprintf_cnt++);
        }
        usart2_delete = rt_thread_delete(uart2_decodeThread_Handle);
        if(usart2_delete == RT_EOK){
            rt_kprintf("PRINTF:%d. usart2_thread was deleted!!\r\n",Record.kprintf_cnt++);
        }
        usart3_delete = rt_thread_delete(uart3_Thread_Handle);
        if(usart3_delete == RT_EOK){
            rt_kprintf("PRINTF:%d. usart3_thread was deleted!!\r\n",Record.kprintf_cnt++);
        }
        usart4_delete = rt_thread_delete(uart4_decodeThread_Handle);
        if(usart4_delete == RT_EOK){
            rt_kprintf("PRINTF:%d. usart4_thread was deleted!!\r\n",Record.kprintf_cnt++);
        }
        sysTimer_stop = rt_timer_stop(sysTimer);
        if(sysTimer_stop == RT_EOK){
            rt_kprintf("PRINTF:%d. sysTimer was stop!!\r\n",Record.kprintf_cnt++);
        }

        rt_enter_critical();
        /* 写入之前先对0x0801 FFF0进行页擦除 */
#if 0
        falFlash_Onchip_Optional(FAL_ERASE_INDEX,"data",0,1,NULL);
        uint8_t wr_updata[1] = {0};
        wr_updata[0] = 1;
        falFlash_Onchip_Optional(FAL_WRITE_INDEX,"data",0,1,wr_updata);
#endif
        macNorFlash_Erase_Page(0x0801FFF0,1);
        rt_tick_t erase_tick = rt_tick_get();
        rt_thread_delay_until(&erase_tick, 100);
        macNorFlash_Write_Word(0x0801FFF0, 1);
        rt_kprintf("PRINTF:%d. iap_upgrade_test \r\n",Record.kprintf_cnt++);
        rt_tick_t jump_tick = rt_tick_get();
        rt_thread_delay_until(&jump_tick, 1000);
        jump_sys_bootloader();
        rt_exit_critical();
    }

}



/**
  * @brief  keyTimer initialize
  * @retval int
  */
int iapTimer_Init(void)
{
    static rt_timer_t iapTimer;
    /* 创建key软件定时器线程 */
    iapTimer = rt_timer_create("iapTimer_callback", iapTimer_callback, RT_NULL, 50, RT_TIMER_FLAG_SOFT_TIMER | RT_TIMER_FLAG_PERIODIC);
    /* 如果keyTimer句柄创建成功,开启软件定时器 */
    if(iapTimer != RT_NULL)
    {
        rt_kprintf("PRINTF:%d. IapTimer initialize succeed!\r\n",Record.kprintf_cnt++);
        rt_timer_start(iapTimer);
    }

    return RT_EOK;
}
INIT_APP_EXPORT(iapTimer_Init);








