#include <applications/macAPP/Inc/rtt_uart3_tk1300f.h>
/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-11-14     teati       the first version
 */


/*! 注：该传感器用于检测大气罐的气压 */

/* 定义串口设备节点 */
static rt_device_t  serial3;
/* 定义串口设备的名称 */
#define USART3_NAME "uart3"
/* 重定向串口配置结构体 */
struct serial_configure usart3Config = RT_SERIAL_CONFIG_DEFAULT;
/* 创建一个信号量用于接收气压传感器的值 */
rt_sem_t usart3_rec_sem = RT_NULL;




/*---------------------------------------------------------------------------------------------------------------*/
/* 以下是串口1初始化的创建以及回调函数                                                                                                                                                          */
/*---------------------------------------------------------------------------------------------------------------*/
/**
  * @brief  串口3的中断回调函数
  * @retval rt_err_t
  */
rt_err_t Usart3_RX_Callback(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(usart3_rec_sem);
    return RT_EOK;
}


/**
  * @brief  串口3初始化函数
  * @retval int
  */
int USART3_Init(void)
{
    static rt_size_t sendNum = 0;

    // 创建动态信号量
    usart3_rec_sem = rt_sem_create("dynamic_sem3", 0, RT_IPC_FLAG_FIFO);
    if (usart3_rec_sem == RT_NULL){
        rt_kprintf("PRINTF:%d. create dynamic semaphore failed.\n",Record.kprintf_cnt++);
        return -1;
    }
    else{
        rt_kprintf("PRINTF:%d. create done. dynamic semaphore value = 0.\n",Record.kprintf_cnt++);
    }


    serial3 = rt_device_find(USART3_NAME);
    if(serial3 != RT_NULL){
        rt_kprintf("PRINTF:%d. Usart3 Device node created succeed! \r\n",Record.kprintf_cnt++);
        usart3Config.baud_rate = BAUD_RATE_9600;
        usart3Config.bufsz = 512;
    }
    else {
        rt_kprintf("PRINTF:%d. Usart3 Device node created Failed! \r\n",Record.kprintf_cnt++);
        return RT_ERROR;
    }
    rt_device_control(serial3, RT_DEVICE_CTRL_CONFIG, &usart3Config);
    rt_device_open(serial3, RT_DEVICE_OFLAG_RDONLY | RT_DEVICE_FLAG_INT_RX);
    rt_device_set_rx_indicate(serial3, Usart3_RX_Callback);

    sendNum = rt_device_write(serial3, RT_NULL, "usart3 is opened!\r\n", 19);
    rt_kprintf("PRINTF:%d. The usart3 test send size : %d\r\n\n",Record.kprintf_cnt++,sendNum);

    return RT_EOK;
}
INIT_APP_EXPORT(USART3_Init);






/*---------------------------------------------------------------------------------------------------------------*/
/* 以下是数据解析线程的创建以及回调函数                                                                                                                                                         */
/*---------------------------------------------------------------------------------------------------------------*/
/**
  * @brief  数据解码回调函数入口
  * @retval void
  */
void uart3_thread_entry(void* parameter)
{
    char recDat = 0;
    rt_size_t sizeValue = 0;
    while(1)
    {
        sizeValue = rt_device_read(serial3, RT_NULL, &recDat, 1);
        if(sizeValue == 1){
            rt_sem_take(usart3_rec_sem, RT_WAITING_FOREVER);
            Tk1300F_Calculate3(recDat);
        }
        rt_thread_mdelay(10);
    }
}





/**
  * @brief  初始化数据解码函数
  * @retval int
  */
rt_thread_t uart3_Thread_Handle;
int uart3_Thread_Init(void)
{
    uart3_Thread_Handle = rt_thread_create("uart3_thread_entry", uart3_thread_entry, RT_NULL, 1024, 20, 200);
    if(uart3_Thread_Handle != RT_NULL){
        rt_kprintf("PRINTF:%d. uart3 Thread is created!!\r\n",Record.kprintf_cnt++);
        rt_thread_startup(uart3_Thread_Handle);
    }
    else {
        rt_kprintf("PRINTF:%d. Thread is not created!!\r\n",Record.kprintf_cnt++);
    }

    return RT_EOK;
}
INIT_APP_EXPORT(uart3_Thread_Init);


/*******************************************************************************************************************************/
/*-----------------------------------------------------------------------------------------------------------------------------*/
/*******************************************************************************************************************************/


/**
  * @brief  主输出气压的传感器使能引脚初始化
  * @retval void
  */
void macTk1300f_Uart3_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = Tk1300F_EN3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_GPIO_WritePin(Tk1300F_EN3_GPIO_Port, Tk1300F_EN3_Pin, RESET);
}



/**
  * @brief  主输出气压的传感器使能控制
  * @retval void
  */
void macTk1300f_Uart3_EN(TK_STATUS status)
{
    if(tk_enable == status){
        HAL_GPIO_WritePin(Tk1300F_EN3_GPIO_Port, Tk1300F_EN3_Pin, SET);
    }
    else if(tk_disable == status){
        HAL_GPIO_WritePin(Tk1300F_EN3_GPIO_Port, Tk1300F_EN3_Pin, RESET);
    }
}




/* Tk1300F-400Kpa 属于单串口线向 MCU s单片机发送三字节数据（高字节 + 低字节 = 校验和） */
uint8_t PressCnt3 = 0;
uint8_t PressBuf3[5];
uint16_t Tk1300F_Calculate3(unsigned  char data)
{
    uint8_t CheckSum = 0;
    if (PressCnt3 < 3)
    {
        PressBuf3[PressCnt3] = data;
        PressCnt3++;
    }
    else
    {
        PressCnt3 = 0;
        Record.Atmospheric_Tank_PressDat3_H = PressBuf3[0];
        Record.Atmospheric_Tank_PressDat3_L = PressBuf3[1];
        CheckSum = Record.Atmospheric_Tank_PressDat3_H + Record.Atmospheric_Tank_PressDat3_L;
        if (CheckSum == PressBuf3[2])
        {
            Record.Atmospheric_Tank_PressDat = (Record.Atmospheric_Tank_PressDat3_H << 8) | Record.Atmospheric_Tank_PressDat3_L;
            /* 压力限幅  -- 由于传感器偶尔会回传 溢出值(0xFFFF) */
            if(Record.Atmospheric_Tank_PressDat > COMPRESSOR_AIR_PRESSURE_MAX){
                Record.Atmospheric_Tank_PressDat = COMPRESSOR_AIR_PRESSURE_MAX;
            }

            #if TEST3_PRINTF_PRESSURE
                rt_kprintf("Tank <any>:%d \n",Record.Atmospheric_Tank_PressDat);
            #endif /*TEST_PRINTF_PRESSURE*/

#if USE_MOVING_AVERAGE_FILTER
            /* 由于传感器输出气压存在溢出波动,为了减少扰动使用滑动滤波器 */
            Record.Atmospheric_Tank_Filter_PressDat = moving_average_filtre(&Mov_Filtre,(float)Record.Atmospheric_Tank_PressDat);
            #if TEST3_PRINTF_PRESSURE
                rt_kprintf("<any>:%d \n",Record.Atmospheric_Tank_Filter_PressDat);
            #endif /*TEST_PRINTF_PRESSURE*/
#endif/* USE_MOVING_AVERAGE_FILTER */
        }
    }
}








