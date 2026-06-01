#ifndef __USART_H
#define __USART_H

#include "stm8s.h"
#include "stm8s_gpio.h"
#include "stm8s_uart1.h"
#include "stm8s_uart3.h"
#include "stm8s_clk.h"






#define UARTAPP_TX_GPIO_PORT GPIOD
#define UARTAPP_TX_GPIO_PIN  GPIO_PIN_5
#define UARTAPP_RX_GPIO_PORT GPIOD
#define UARTAPP_RX_GPIO_PIN  GPIO_PIN_6
      
#define  UART_DATALENGTH 100

typedef struct
{
    u8 Receive_Buffer[UART_DATALENGTH];//数据缓冲区
    u8 *receive_last;//接收末尾
    u8 *get_last;//获取末尾
    u8 receive_signal_flag;//有收到数据标志位
    u8 receive_full_flag;//接收满标志位
    u8 receive_length;//接收长度
} USART_ReceiveDataTypedef; //定义队列数据类型

extern USART_ReceiveDataTypedef    USART1_QueueBuf;


void    USARTAPP_Config(void);
void    UART_SendData(u8 * buffer, u16 size);
void    UART_ReceiveValueInit(USART_ReceiveDataTypedef *Uart_Device_Rx,u8 Length);
void    UART_Receive (USART_ReceiveDataTypedef *Uart_Device_Rx,u8 Data);
u8      UART_GetByte (USART_ReceiveDataTypedef *Uart_Device_Rx);

#endif