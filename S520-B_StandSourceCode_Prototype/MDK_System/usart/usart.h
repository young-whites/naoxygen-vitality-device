#ifndef __USART_H
#define __USART_H
#include "sys.h"

/*******
 *  以下串口使用说明是针对S520-B产品 
 *  串口1： 
 *  串口2： 右安卓屏与主板连接
 *  串口3： 左安卓屏与主板连接
 *  串口4： 大手柄与主板连接
 */






/* 串口使能宏定义 */
#define     USART1_EN               0   
#define     USART2_EN               1
#define     USART3_EN               0
#define     UART4_EN                1
#define     UART5_EN                0

/* DMA使能宏定义 */
#define     DMA1_EN                 0
#define     DMA2_EN                 1
#define     DMA3_EN                 0
#define     DMA4_EN                 1
#define     DMA5_EN                 0

/* 串口和DMA发送最大字节长度宏定义 */
#define     UART_DATALENGTH         1024
#define	    Uart1_Dma_Max	        1024
#define     Uart2_Dma_Max           1024
#define     Uart3_Dma_Max           1024
#define     Uart4_Dma_Max           1024
#define     Uart5_Dma_Max           1024


extern uint8_t   Uart1_Dma_Dat[Uart1_Dma_Max];
extern uint8_t   Uart2_Dma_Dat[Uart2_Dma_Max];
extern uint8_t   Uart3_Dma_Dat[Uart3_Dma_Max];
extern uint8_t   Uart4_Dma_Dat[Uart4_Dma_Max];
extern uint8_t   Uart5_Dma_Dat[Uart5_Dma_Max];


typedef struct
{
    u8 Receive_Buffer[UART_DATALENGTH]; // 接收数据存放数组
    u8 *receive_last;                   // 新数据存放地址                 
    u8 *get_last;                       // 指向最先需要处理的数据,先进先出
    u8 receive_signal_flag;             // 是否有数据需要处理标志
    u8 receive_full_flag;               // 缓冲区满标志位
    u16 receive_length;                 // 缓冲区内存大小
} USART_ReceiveDataTypedef; 


#define   Uart_Send_Lenght     1024
typedef struct
{
    u8 Send_Buff[Uart_Send_Lenght]; 
    u16 Send_StarArr;
    u16 Send_EndtArr;
    u16 Send_Wait_DatNum; 
    enum {BUSY,IDLE}DMA_Flag; 
} USART_SendDataTypedef; 








#if  USART1_EN
#define  DEBUG_USART_1                   USART1
#define  DEBUG_USART1_CLK                RCC_APB2Periph_USART1
#define  DEBUG_USART1_APBxClkCmd         RCC_APB2PeriphClockCmd
#define  DEBUG_USART1_BAUDRATE           9600

#define  DEBUG_USART1_GPIO_CLK           (RCC_APB2Periph_GPIOA)
#define  DEBUG_USART1_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
    
#define  DEBUG_USART1_TX_GPIO_PORT       GPIOA   
#define  DEBUG_USART1_TX_GPIO_PIN        GPIO_Pin_9
#define  DEBUG_USART1_RX_GPIO_PORT       GPIOA
#define  DEBUG_USART1_RX_GPIO_PIN        GPIO_Pin_10

#define  DEBUG_USART1_IRQ                USART1_IRQn
#define  DEBUG_USART1_IRQHandler         USART1_IRQHandler

extern   USART_ReceiveDataTypedef        USART1_QueueBuf;
extern   USART_SendDataTypedef        	 USART1_SendBuf;

#endif /*USART1_EN*/



#if USART2_EN
#define  DEBUG_USART_2                   USART2
#define  DEBUG_USART2_CLK                RCC_APB1Periph_USART2
#define  DEBUG_USART2_APBxClkCmd         RCC_APB1PeriphClockCmd
#define  DEBUG_USART2_BAUDRATE           9600

#define  DEBUG_USART2_GPIO_CLK           (RCC_APB2Periph_GPIOA)
#define  DEBUG_USART2_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
   
#define  DEBUG_USART2_TX_GPIO_PORT       GPIOA   
#define  DEBUG_USART2_TX_GPIO_PIN        GPIO_Pin_2
#define  DEBUG_USART2_RX_GPIO_PORT       GPIOA
#define  DEBUG_USART2_RX_GPIO_PIN        GPIO_Pin_3

#define  DEBUG_USART2_IRQ                USART2_IRQn
#define  DEBUG_USART2_IRQHandler         USART2_IRQHandler

extern   USART_ReceiveDataTypedef        USART2_QueueBuf;
extern   USART_SendDataTypedef        	 USART2_SendBuf;

#endif /*USART2_EN*/


#if USART3_EN
#define  DEBUG_USART_3                   USART3
#define  DEBUG_USART3_CLK                RCC_APB1Periph_USART3
#define  DEBUG_USART3_APBxClkCmd         RCC_APB1PeriphClockCmd
#define  DEBUG_USART3_BAUDRATE           9600


#define  DEBUG_USART3_GPIO_CLK           (RCC_APB2Periph_GPIOB)
#define  DEBUG_USART3_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
   
#define  DEBUG_USART3_TX_GPIO_PORT       GPIOB   
#define  DEBUG_USART3_TX_GPIO_PIN        GPIO_Pin_10
#define  DEBUG_USART3_RX_GPIO_PORT       GPIOB
#define  DEBUG_USART3_RX_GPIO_PIN        GPIO_Pin_11

#define  DEBUG_USART3_IRQ                USART3_IRQn
#define  DEBUG_USART3_IRQHandler         USART3_IRQHandler

extern   USART_ReceiveDataTypedef        USART3_QueueBuf;
extern   USART_SendDataTypedef        	 USART3_SendBuf;

#endif /*USART3_EN*/


#if UART4_EN
#define  DEBUG_UART_4                   UART4
#define  DEBUG_UART4_CLK                RCC_APB1Periph_UART4
#define  DEBUG_UART4_APBxClkCmd         RCC_APB1PeriphClockCmd
#define  DEBUG_UART4_BAUDRATE           9600


#define  DEBUG_UART4_GPIO_CLK           (RCC_APB2Periph_GPIOC)
#define  DEBUG_UART4_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
    
#define  DEBUG_UART4_TX_GPIO_PORT       GPIOC   
#define  DEBUG_UART4_TX_GPIO_PIN        GPIO_Pin_10
#define  DEBUG_UART4_RX_GPIO_PORT       GPIOC
#define  DEBUG_UART4_RX_GPIO_PIN        GPIO_Pin_11

#define  DEBUG_UART4_IRQ                UART4_IRQn
#define  DEBUG_UART4_IRQHandler         UART4_IRQHandler

extern   USART_ReceiveDataTypedef        UART4_QueueBuf;
extern   USART_SendDataTypedef        	 UART4_SendBuf;

#endif /*UART4_EN*/


#if UART5_EN
#define  DEBUG_UART_5                   UART5
#define  DEBUG_UART5_CLK                RCC_APB1Periph_UART5
#define  DEBUG_UART5_APBxClkCmd         RCC_APB1PeriphClockCmd
#define  DEBUG_UART5_BAUDRATE           9600


#define  DEBUG_UART5_GPIO_CLK           (RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD)
#define  DEBUG_UART5_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
   
#define  DEBUG_UART5_TX_GPIO_PORT       GPIOC   
#define  DEBUG_UART5_TX_GPIO_PIN        GPIO_Pin_12
#define  DEBUG_UART5_RX_GPIO_PORT       GPIOD
#define  DEBUG_UART5_RX_GPIO_PIN        GPIO_Pin_2

#define  DEBUG_UART5_IRQ                UART5_IRQn
#define  DEBUG_UART5_IRQHandler         UART5_IRQHandler

extern   USART_ReceiveDataTypedef        UART5_QueueBuf;
extern   USART_SendDataTypedef        	 UART5_SendBuf;

#endif /*UART5_EN*/


#if DMA1_EN
#define  USART_TX_DMA_CHANNEL     DMA1_Channel4					// 串口发送时对应的DMA请求通道
#define  USART_RX_DMA_CHANNEL	  DMA1_Channel5					// 串口接收时对应的DMA请求通道
#define  USART_DR_ADDRESS        (USART1_BASE+0x04)			    // 外设寄存器地址
#define  SENDBUFF_Max_SIZE        1024							// 一次发送的数据量
#endif /*DMA1_EN*/



#if DMA2_EN
#define  USART2_TX_DMA_CHANNEL     DMA1_Channel7			    // 串口发送时对应的DMA请求通道
#define  USART2_RX_DMA_CHANNEL	   DMA1_Channel6				// 串口接收时对应的DMA请求通道
#define  USART2_DR_ADDRESS        (USART2_BASE+0x04)			// 外设寄存器地址
#define  SENDBUFF2_Max_SIZE        1024							// 一次发送的数据量
#endif  /*DMA2_EN*/


#if DMA3_EN
#define  USART3_TX_DMA_CHANNEL     DMA1_Channel2			    // 串口发送时对应的DMA请求通道
#define  USART3_RX_DMA_CHANNEL	   DMA1_Channel3				// 串口接收时对应的DMA请求通道
#define  USART3_DR_ADDRESS        (USART3_BASE+0x04)			// 外设寄存器地址
#define  SENDBUFF3_Max_SIZE        1024							// 一次发送的数据量
#endif  /*DMA3_EN*/


#if DMA4_EN
#define  UART4_TX_DMA_CHANNEL     DMA2_Channel5			        // 串口发送时对应的DMA请求通道
#define  UART4_RX_DMA_CHANNEL	  DMA2_Channel3				    // 串口接收时对应的DMA请求通道
#define  UART4_DR_ADDRESS        (UART4_BASE+0x04)			    // 外设寄存器地址
#define  SENDBUFF4_Max_SIZE       1024							// 一次发送的数据量
#endif  /*DMA4_EN*/



#if DMA5_EN
#define  UART5_TX_DMA_CHANNEL     DMA2_Channel4			        // 串口发送时对应的DMA请求通道
#define  UART5_RX_DMA_CHANNEL	  DMA2_Channel1				    // 串口接收时对应的DMA请求通道
#define  UART5_DR_ADDRESS        (UART5_BASE+0x04)			    // 外设寄存器地址
#define  SEND5UFF5_Max_SIZE       1024							// 一次发送的数据量
#endif  /*DMA5_EN*/





/* USART 相关函数声明*/
#if USART1_EN
void USART1_Config(void);
void USART1_DMA_Config(void);
void USART1_DMA_Send(void);
void USART1_SEND_RECEIVE_Queue_Init(void);
#endif /*USART1_EN*/


#if USART2_EN
void USART2_Config(void);
void USART2_DMA_Config(void);
void USART2_DMA_Send(void);
void USART2_SEND_RECEIVE_Queue_Init(void);
#endif /*USART2_EN*/



#if USART3_EN
void USART3_Config(void);
void USART3_DMA_Config(void);
void USART3_DMA_Send(void);
void USART3_SEND_RECEIVE_Queue_Init(void);
#endif /*USART3_EN*/


#if UART4_EN
void UART4_Config(void);
void UART4_DMA_Config(void);
void UART4_DMA_Send(void);
void UART4_SEND_RECEIVE_Queue_Init(void);
#endif /*UART4_EN*/


#if UART5_EN
void UART5_Config(void);
void UART5_DMA_Config(void);
void UART5_DMA_Send(void);
void UART5_SEND_RECEIVE_Queue_Init(void);
#endif /*UART5_EN*/




/* 环形队列 相关函数声明*/
void DMAData_Init(USART_SendDataTypedef *USART_SendBuf);
void UART_ReceiveValueInit(USART_ReceiveDataTypedef* Uart_Device_Rx, u16 Length);
void UART_Receive(USART_ReceiveDataTypedef* Uart_Device_Rx, u8* Data, u16 Lenght);
u8   UART_GetByte(USART_ReceiveDataTypedef* Uart_Device_Rx);
void Uart_Send_Upat(USART_SendDataTypedef* USART_SendBuf, u8* dat, u16 lenght);
void Uart_Send_Char(USART_SendDataTypedef* USART_SendBuf, u8 dat);
void USART1_Printf_Char(USART_TypeDef* USARTx, uint8_t dat);
void USART1_Printf_String(USART_TypeDef* USARTx, uint8_t* str);
void USART2_Printf_Char(USART_TypeDef* USARTx, uint8_t dat);
void USART2_Printf_String(USART_TypeDef* USARTx, uint8_t* str);


/* DMA 配置相关的函数声明*/
void MyDMA_Config(DMA_Channel_TypeDef* DMA_CHx, u32 cpar, u32 cmar, u16 cndtr);
void MyDMA_SetTransmission(DMA_Channel_TypeDef* DMA_CHx, u16 Lenght);



#endif /*__USART_H*/
