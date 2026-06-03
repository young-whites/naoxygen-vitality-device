#include <applications/macAPP/Inc/rtt_uart1_tk1300f.h>
/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-06-15     zphu       the first version
 */


/*! 该传感器用于检测输出端口的气压值 */
/*! 该传感器的默认零点是1000，即加压400Kpa，输出的数值是5000 */
/*! 深圳环境下的大气压是100Kpa，对应Tk1300F传感器的默认值1000 */

/* 定义串口设备节点 */
static rt_device_t  serial1;
/* 定义串口设备的名称 */
#define USART1_NAME "uart1"
/* 重定向串口配置结构体 */
struct serial_configure usart1Config = RT_SERIAL_CONFIG_DEFAULT;
/* 创建一个信号量用于接收气压传感器的值 */
rt_sem_t usart1_rec_sem = RT_NULL;




/*---------------------------------------------------------------------------------------------------------------*/
/* 以下是串口1初始化的创建以及回调函数                                                                                                                                                          */
/*---------------------------------------------------------------------------------------------------------------*/
/**
  * @brief  串口1的中断回调函数
  * @retval rt_err_t
  */
rt_err_t Usart1_RX_Callback(rt_device_t dev, rt_size_t size)
{

    rt_sem_release(usart1_rec_sem);
    return RT_EOK;
}


/**
  * @brief  串口1初始化函数
  * @retval int
  */
int USART1_Init(void)
{
    static rt_size_t sendNum = 0;

    // 创建动态信号量
    usart1_rec_sem = rt_sem_create("dynamic_sem1", 0, RT_IPC_FLAG_FIFO);
    if (usart1_rec_sem == RT_NULL){
        rt_kprintf("PRINTF:%d. create dynamic semaphore failed.\n",Record.kprintf_cnt++);
        return -1;
    }
    else{
        rt_kprintf("PRINTF:%d. create done. dynamic semaphore value = 0.\n",Record.kprintf_cnt++);
    }


    serial1 = rt_device_find(USART1_NAME);
    if(serial1 != RT_NULL){
        rt_kprintf("PRINTF:%d. Usart1 Device node created succeed! \r\n",Record.kprintf_cnt++);
        usart1Config.baud_rate = BAUD_RATE_9600;
        usart1Config.bufsz = 1024;
    }
    else {
        rt_kprintf("PRINTF:%d. Usart1 Device node created Failed! \r\n",Record.kprintf_cnt++);
        return RT_ERROR;
    }
    rt_device_control(serial1, RT_DEVICE_CTRL_CONFIG, &usart1Config);
    rt_device_open(serial1, RT_DEVICE_OFLAG_RDONLY | RT_DEVICE_FLAG_INT_RX);
    rt_device_set_rx_indicate(serial1, Usart1_RX_Callback);

    sendNum = rt_device_write(serial1, RT_NULL, "usart1 is opened!\r\n", 19);
    rt_kprintf("PRINTF:%d. The usart1 test send size : %d\r\n\n",Record.kprintf_cnt++,sendNum);

    return RT_EOK;
}
INIT_APP_EXPORT(USART1_Init);






/*---------------------------------------------------------------------------------------------------------------*/
/* 以下是数据解析线程的创建以及回调函数                                                                                                                                                         */
/*---------------------------------------------------------------------------------------------------------------*/
/**
  * @brief  数据解码回调函数入口
  * @retval void
  */
void uart1_thread_entry(void* parameter)
{
    char recDat = 0;
    rt_size_t sizeValue = 0;
    while(1)
    {
        sizeValue = rt_device_read(serial1, RT_NULL, &recDat, 1);

        if(sizeValue == 1){
            rt_sem_take(usart1_rec_sem, RT_WAITING_FOREVER);
            Tk1300F_Calculate1(recDat);
        }
        rt_thread_mdelay(10);
    }
}





/**
  * @brief  初始化数据解码函数
  * @retval int
  */
rt_thread_t uart1_Thread_Handle;
int uart1_Thread_Init(void)
{
    uart1_Thread_Handle = rt_thread_create("uart1_thread_entry", uart1_thread_entry, RT_NULL, 1024, 20, 200);
    if(uart1_Thread_Handle != RT_NULL){
        rt_kprintf("PRINTF:%d. uart1 Thread is created!!\r\n",Record.kprintf_cnt++);
        rt_thread_startup(uart1_Thread_Handle);
    }
    else {
        rt_kprintf("PRINTF:%d. Thread is not created!!\r\n",Record.kprintf_cnt++);
    }

    return RT_EOK;
}
INIT_APP_EXPORT(uart1_Thread_Init);


/*******************************************************************************************************************************/
/*-----------------------------------------------------------------------------------------------------------------------------*/
/*******************************************************************************************************************************/


/**
  * @brief  主输出气压的传感器使能引脚初始化
  * @retval void
  */
void macTk1300f_Uart1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = Tk1300F_EN1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_GPIO_WritePin(Tk1300F_EN1_GPIO_Port, Tk1300F_EN1_Pin, RESET);
}



/**
  * @brief  主输出气压的传感器使能控制
  * @retval void
  */
void macTk1300f_Uart1_EN(TK_STATUS status)
{
    if(tk_enable == status){
        HAL_GPIO_WritePin(Tk1300F_EN1_GPIO_Port, Tk1300F_EN1_Pin, SET);
    }
    else if(tk_disable == status){
        HAL_GPIO_WritePin(Tk1300F_EN1_GPIO_Port, Tk1300F_EN1_Pin, RESET);
    }
}




/* Tk1300F-400Kpa 属于单串口线向 MCU s单片机发送三字节数据（高字节 + 低字节 = 校验和） */
uint8_t PressCnt1 = 0;
uint8_t PressBuf1[5];
uint16_t CheckStartPress = 0;
uint16_t CheckStartPress_Buff[10] = {0};
uint8_t CalCnt = 0;
void Tk1300F_Calculate1(unsigned  char data)
{
    static uint8_t SumCnt = 0;
    uint8_t CheckSum = 0;
    if (PressCnt1 < 3)
    {
        PressBuf1[PressCnt1] = data;
        PressCnt1++;
    }
    else
    {
        PressCnt1 = 0;
        Record.Output_PressDat1_H = PressBuf1[0];
        Record.Output_PressDat1_L = PressBuf1[1];
        CheckSum = Record.Output_PressDat1_H + Record.Output_PressDat1_L;

        if (CheckSum == PressBuf1[2])
        {
            if(SumCnt < 10){
                CheckStartPress_Buff[SumCnt] = (Record.Output_PressDat1_H << 8) | Record.Output_PressDat1_L;
                SumCnt++;
                if(SumCnt == 10){
                    for( ;CalCnt < 10; CalCnt++){
                        CheckStartPress += CheckStartPress_Buff[CalCnt];
                    }
                    CheckStartPress /= 10.0 ;
                    CheckStartPress -= 50;
                    rt_kprintf("标定气压值:%d \n",CheckStartPress);
                }
            }
            Record.Output_PressDat = (Record.Output_PressDat1_H << 8) | Record.Output_PressDat1_L;
            #if TEST1_PRINTF_PRESSURE
            rt_kprintf("<any>:%d \n",Record.Output_PressDat);
            #endif /*TEST_PRINTF_PRESSURE*/
        }
    }
}



