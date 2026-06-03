#include "usart.h"
#include "stm8s_uart1.h"
#include "stm8s_uart3.h"
#include "stm8s_wwdg.h"
#include "motor.h"


USART_ReceiveDataTypedef    USART1_QueueBuf;


void USARTAPP_Config(void)
{
  //初始化USART的TXD RXD对应的2个管脚  

    UART1_DeInit();
    CLK_PeripheralClockConfig( CLK_PERIPHERAL_UART1, ENABLE ); //开启USART时钟
    UART1_Init( 9600, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE ); //设置USART参数115200，8N1，接收/发送
    UART1_ITConfig( UART1_IT_RXNE_OR, ENABLE ); //使能接收中断,中断向量号为28
    UART1_Cmd( ENABLE ); //使能USART

    UART_ReceiveValueInit(&USART1_QueueBuf,UART_DATALENGTH);
    
    GPIO_Init(UARTAPP_RX_GPIO_PORT,UARTAPP_RX_GPIO_PIN,GPIO_MODE_IN_PU_NO_IT);//初始化，带上拉输入，不带中断
    GPIO_Init(UARTAPP_TX_GPIO_PORT,UARTAPP_TX_GPIO_PIN,GPIO_MODE_OUT_PP_HIGH_FAST);//初始化，推挽输出，快速  
    
}


/********************************************************
//函数名称:void    UART_SendData(USART_TypeDef* USARTx,u8 * buffer, u16 size)
//函数功能: 串口数据发送
//入口参数：USARTx==串口编号;buffer==要发送的数据缓冲区;size==要发送的数据缓冲区大小
//返回类型:void
//作者：dinghx
//日期：20180709
*********************************************************/
void UART_SendData(u8 * buffer , u16 size )
{
    u16 i = 0;

    for ( i = 0; i < size; i++ )
    {
        UART1_SendData8( *buffer );

        while ( UART1_GetFlagStatus( UART1_FLAG_TC ) == RESET );

        buffer++;
    }
}



/********************************************************
//函数名称:void    UART_ReceiveValueInit(USART_ReceiveDataTypedef *Uart_Device_Rx,u8 Length)
//函数功能: 串口设备赋值初始化
//入口参数：Uart_Device_Rx==数据接收队列；Length==数据长度
//返回类型:void
//作者：daniel
//日期：20181206
*********************************************************/
void    UART_ReceiveValueInit(USART_ReceiveDataTypedef *Uart_Device_Rx,u8 Length)
{
    Uart_Device_Rx->receive_last = Uart_Device_Rx->Receive_Buffer;
    Uart_Device_Rx->get_last = Uart_Device_Rx->Receive_Buffer;//赋值读取指针,把指针指向缓冲区首地址
    Uart_Device_Rx->receive_length = Length;
    Uart_Device_Rx->receive_signal_flag=0;
    Uart_Device_Rx->receive_signal_flag=0;
}

/***********************************************************************
//函数名称:void    UART_Receive (USART_ReceiveDataTypedef *Uart_Device_Rx,u8 Data)
//函数功能: 数据入队操作
//入口参数：Uart_Device_Rx==数据接收队列；Data==入队数据
//返回类型:void
//作者：daniel
//日期：20181206
************************************************************************/
void    UART_Receive (USART_ReceiveDataTypedef *Uart_Device_Rx,u8 Data)
{
    if(!Uart_Device_Rx->receive_full_flag)
    {
        *(Uart_Device_Rx->receive_last) = Data;  //把数据放入所指向的地址
        Uart_Device_Rx->receive_last++;
        Uart_Device_Rx->receive_signal_flag = 1;

        if (Uart_Device_Rx->receive_last >= Uart_Device_Rx->Receive_Buffer + Uart_Device_Rx->receive_length)
        {
            Uart_Device_Rx->receive_last = Uart_Device_Rx->Receive_Buffer;
        }
        if (Uart_Device_Rx->receive_last == Uart_Device_Rx->get_last)
        {
            Uart_Device_Rx->receive_full_flag = 1;
        }
    }
}

/**********************************************************************
//函数名称:u8 UART_GetByte (USART_ReceiveDataTypedef *Uart_Device_Rx)
//函数功能：数据出队操作
//入口参数：数据队列
//返回类型:void
//作者：daniel
//日期：20181206
**********************************************************************/
u8 UART_GetByte (USART_ReceiveDataTypedef *Uart_Device_Rx)
{
    u8 data = 0;

    if (!Uart_Device_Rx->receive_signal_flag)
    {
        return 0;
    }

    data = *(Uart_Device_Rx->get_last);
    Uart_Device_Rx->get_last++;   
    Uart_Device_Rx->receive_full_flag = 0;

    if (Uart_Device_Rx->get_last >= Uart_Device_Rx->Receive_Buffer + Uart_Device_Rx->receive_length)
    {
        Uart_Device_Rx->get_last = Uart_Device_Rx->Receive_Buffer;
    }

    if (Uart_Device_Rx->get_last == Uart_Device_Rx->receive_last) //获取完时清除信号
    {
        Uart_Device_Rx->receive_signal_flag = 0;
    }
    return  data;
}