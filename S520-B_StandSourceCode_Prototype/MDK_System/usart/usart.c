#include "usart.h"



/*******************************************/
/*              串口中断优先级             */
/*-----------------------------------------*/
/*	USART1  -> 主优先级：1  子优先级：0
 *  USART2  -> 主优先级：7  子优先级：0
 *  USART3  -> 主优先级：4  子优先级：0
 * 
 */

















#if USART1_EN
USART_ReceiveDataTypedef    		 USART1_QueueBuf;
USART_SendDataTypedef        	 	 USART1_SendBuf;
uint8_t    							 Uart1_Dma_Dat[Uart1_Dma_Max];  /*DMA数据传输缓冲区*/
#endif /*USART1_EN*/



#if USART2_EN
USART_ReceiveDataTypedef    		 USART2_QueueBuf;
USART_SendDataTypedef        	 	 USART2_SendBuf;
uint8_t    							 Uart2_Dma_Dat[Uart2_Dma_Max];  /*DMA数据传输缓冲区*/
#endif /*USART2_EN*/


#if USART3_EN
USART_ReceiveDataTypedef    		 USART3_QueueBuf;
USART_SendDataTypedef        	 	 USART3_SendBuf;
uint8_t    							 Uart3_Dma_Dat[Uart3_Dma_Max];  /*DMA数据传输缓冲区*/
#endif /*USART3_EN*/


#if UART4_EN
USART_ReceiveDataTypedef    		 UART4_QueueBuf;
USART_SendDataTypedef        	 	 UART4_SendBuf;
uint8_t    							 Uart4_Dma_Dat[Uart4_Dma_Max];  /*DMA数据传输缓冲区*/
#endif /*USART4_EN*/




#if UART5_EN
USART_ReceiveDataTypedef    		 UART5_QueueBuf;
USART_SendDataTypedef        	 	 UART5_SendBuf;
uint8_t    							 Uart5_Dma_Dat[Uart5_Dma_Max];  /*DMA数据传输缓冲区*/
#endif /*USART4_EN*/



/******************************************************************************************/
/* 加入以下代码, 支持printf函数, 而不需要选择use MicroLIB */

#if 1

#if (__ARMCC_VERSION >= 6010050)            /* 使用AC6编译器时 */
__asm(".global __use_no_semihosting\n\t");  /* 声明不使用半主机模式 */
__asm(".global __ARM_use_no_argv \n\t");    /* AC6下需要声明main函数为无参数格式，否则部分例程可能出现半主机模式 */

#else
/* 使用AC5编译器时, 要在这里定义__FILE 和 不使用半主机模式 */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};

#endif

/* 不使用半主机模式，至少需要重定义_ttywrch\_sys_exit\_sys_command_string函数,以同时兼容AC6和AC5模式 */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* 定义_sys_exit()以避免使用半主机模式 */
void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}


/* FILE 在 stdio.h里面定义. */
FILE __stdout;

/* MDK下需要重定义fputc函数, printf函数最终会通过调用fputc输出字符串到串口 */
int fputc(int ch, FILE *f)
{
	Uart_Send_Char(&USART2_SendBuf,(uint8_t)ch);
    return ch;
}
#endif
/******************************************************************************************/







/* 串口1 DMA初始化*****************************************************************************************/
#if USART1_EN

 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void USART1_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 嵌套向量中断控制器组选择 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  
  /* 配置USART为中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART1_IRQ;
  /* 抢断优先级*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* 子优先级 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  /* 使能中断 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* 初始化配置NVIC */
  NVIC_Init(&NVIC_InitStructure);
}

 /**
  * @brief  USART GPIO 配置,工作参数配置
  * @param  无
  * @retval 无
  */
void USART1_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// 打开串口GPIO的时钟
	DEBUG_USART1_GPIO_APBxClkCmd(DEBUG_USART1_GPIO_CLK, ENABLE);
	
	// 打开串口外设的时钟
	DEBUG_USART1_APBxClkCmd(DEBUG_USART1_CLK, ENABLE);

	// 将USART Tx的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART1_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART1_TX_GPIO_PORT, &GPIO_InitStructure);

  // 将USART Rx的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART1_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART1_RX_GPIO_PORT, &GPIO_InitStructure);
	
	// 配置串口的工作参数
	// 配置波特率
	USART_InitStructure.USART_BaudRate = DEBUG_USART1_BAUDRATE;
	// 配置 针数据字长
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// 配置停止位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// 配置校验位
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	// 配置硬件流控制
	USART_InitStructure.USART_HardwareFlowControl = 
	USART_HardwareFlowControl_None;
	// 配置工作模式，收发一起
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// 完成串口的初始化配置
	USART_Init(DEBUG_USART_1, &USART_InitStructure);
	
	// 串口中断优先级配置
	USART1_NVIC_Configuration();
	
	// 使能串口接收中断
	USART_ITConfig ( DEBUG_USART_1, USART_IT_IDLE, ENABLE ); //使能串口总线空闲中断 	
	
	// 使能串口
	USART_Cmd(DEBUG_USART_1, ENABLE);	    
}



/**
  * @brief  USARTx TX DMA 配置，内存到外设(USART1->DR)
  * @param  无
  * @retval 无
  */
void USART1_DMA_Config(void)
{
		DMA_InitTypeDef DMA_InitStructure;
	
		// 开启DMA时钟
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		// 设置DMA源地址：串口数据寄存器地址*/
    	DMA_InitStructure.DMA_PeripheralBaseAddr = USART_DR_ADDRESS;
		// 内存地址(要传输的变量的指针)
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Uart1_Dma_Dat;
		// 方向：从外设到内存	
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		// 传输大小	
		DMA_InitStructure.DMA_BufferSize = SENDBUFF_Max_SIZE;
		// 外设地址不增	    
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		// 内存地址自增
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		// 外设数据单位	
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		// 内存数据单位
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 
		// DMA模式，一次或者循环模式
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
		//DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
		// 优先级：中	
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 
		// 禁止内存到内存的传输
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		// 配置DMA通道		   
		DMA_Init(USART_RX_DMA_CHANNEL, &DMA_InitStructure);		
		// 使能DMA
		DMA_Cmd (USART_RX_DMA_CHANNEL,ENABLE);
		// 使能串口1,DMA接收
		USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);   
}





/**
  * @brief  串口1的DMA传输
  * @param  无
  * @retval 无
  */
void  USART1_DMA_Send(void)
{
	u16 	Dat_Lenght;

	/*DMA1_FLAG_TC4 代表DMA传输通道4的传输完成标志位*/
	if (DMA_GetFlagStatus(DMA1_FLAG_TC4) != RESET)
	{
		DMA_ClearFlag(DMA1_FLAG_TC4);
		USART1_SendBuf.DMA_Flag = IDLE;
	}

	if (USART1_SendBuf.DMA_Flag == IDLE)
	{
		if (USART1_SendBuf.Send_Wait_DatNum)
		{
			if ((USART1_SendBuf.Send_StarArr + USART1_SendBuf.Send_Wait_DatNum) > (Uart_Send_Lenght - 1))
			{
				USART1_SendBuf.Send_Wait_DatNum -= (Uart_Send_Lenght - USART1_SendBuf.Send_StarArr);
				Dat_Lenght = (Uart_Send_Lenght - USART1_SendBuf.Send_StarArr);
				MyDMA_Config(DMA1_Channel4, (u32)&USART1->DR, (u32)(USART1_SendBuf.Send_Buff + USART1_SendBuf.Send_StarArr), Dat_Lenght);
				USART1_SendBuf.Send_StarArr = 0;
			}
			else
			{
				Dat_Lenght = USART1_SendBuf.Send_Wait_DatNum;
				MyDMA_Config(DMA1_Channel4, (u32)&USART1->DR, (u32)(USART1_SendBuf.Send_Buff + USART1_SendBuf.Send_StarArr), Dat_Lenght);
				USART1_SendBuf.Send_StarArr += USART1_SendBuf.Send_Wait_DatNum; //下一轮发送的起始位置
				USART1_SendBuf.Send_Wait_DatNum = 0;
			}
			USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
			MyDMA_SetTransmission(DMA1_Channel4, Dat_Lenght);
			USART1_SendBuf.DMA_Flag = BUSY;
		}
	}
}




/**
  * @brief  DMA 串口传输之前需要初始化的参数内容
  * @param  无
  * @retval 串口1发送和接收队列初始化
  */
void USART1_SEND_RECEIVE_Queue_Init()
{
	UART_ReceiveValueInit(&USART1_QueueBuf, UART_DATALENGTH);
	DMAData_Init(&USART1_SendBuf);
}




#endif /* USART1_EN */







/* 串口2 DMA初始化*****************************************************************************************/
#if USART2_EN

/**
 * @brief  配置嵌套向量中断控制器NVIC
 * @param  无
 * @retval 无
 */
static void USART2_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 嵌套向量中断控制器组选择 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	/* 配置USART为中断源 */
	NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART2_IRQ;
	/* 抢断优先级*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	/* 子优先级 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	/* 使能中断 */
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	/* 初始化配置NVIC */
	NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief  USART GPIO 配置,工作参数配置
 * @param  无
 * @retval 无
 */
void USART2_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// 打开串口GPIO的时钟
	DEBUG_USART2_GPIO_APBxClkCmd(DEBUG_USART2_GPIO_CLK, ENABLE);

	// 打开串口外设的时钟
	DEBUG_USART2_APBxClkCmd(DEBUG_USART2_CLK, ENABLE);

	// 将USART Tx的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART2_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART2_TX_GPIO_PORT, &GPIO_InitStructure);

	// 将USART Rx的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART2_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART2_RX_GPIO_PORT, &GPIO_InitStructure);

	// 配置串口的工作参数
	// 配置波特率
	USART_InitStructure.USART_BaudRate = DEBUG_USART2_BAUDRATE;
	// 配置 针数据字长
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// 配置停止位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// 配置校验位
	USART_InitStructure.USART_Parity = USART_Parity_No;
	// 配置硬件流控制
	USART_InitStructure.USART_HardwareFlowControl =
		USART_HardwareFlowControl_None;
	// 配置工作模式，收发一起
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// 完成串口的初始化配置
	USART_Init(DEBUG_USART_2, &USART_InitStructure);

	// 串口中断优先级配置
	USART2_NVIC_Configuration();

	// 使能串口接收中断
	USART_ITConfig(DEBUG_USART_2, USART_IT_IDLE, ENABLE); //使能串口总线空闲中断 	

	// 使能串口
	USART_Cmd(DEBUG_USART_2, ENABLE);
}



/**
  * @brief  USARTx TX DMA 配置，内存到外设(USART2->DR)
  * @param  无
  * @retval 无
  */
void USART2_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	// 开启DMA时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	// 设置DMA源地址：串口数据寄存器地址*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = USART2_DR_ADDRESS;
	// 内存地址(要传输的变量的指针)
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Uart2_Dma_Dat;
	// 方向：从外设到内存	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	// 传输大小	
	DMA_InitStructure.DMA_BufferSize = SENDBUFF2_Max_SIZE;
	// 外设地址不增	    
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	// 内存地址自增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	// 外设数据单位	
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	// 内存数据单位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	// DMA模式，一次或者循环模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	//DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
	// 优先级：中	
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	// 禁止内存到内存的传输
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	// 配置DMA通道		   
	DMA_Init(USART2_RX_DMA_CHANNEL, &DMA_InitStructure);
	// 使能DMA
	DMA_Cmd(USART2_RX_DMA_CHANNEL, ENABLE);
	// 使能串口2,DMA接收
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
}





/**
  * @brief  串口1的DMA传输
  * @param  无
  * @retval 无
  */
void  USART2_DMA_Send(void)
{
	u16 	Dat_Lenght;

	if (DMA_GetFlagStatus(DMA1_FLAG_TC7) != RESET)
	{
		DMA_ClearFlag(DMA1_FLAG_TC7);
		USART2_SendBuf.DMA_Flag = IDLE;
	}

	if (USART2_SendBuf.DMA_Flag == IDLE)
	{
		if (USART2_SendBuf.Send_Wait_DatNum)
		{
			if ((USART2_SendBuf.Send_StarArr + USART2_SendBuf.Send_Wait_DatNum) > (Uart_Send_Lenght - 1))
			{
				USART2_SendBuf.Send_Wait_DatNum -= (Uart_Send_Lenght - USART2_SendBuf.Send_StarArr);
				Dat_Lenght = (Uart_Send_Lenght - USART2_SendBuf.Send_StarArr);
				MyDMA_Config(DMA1_Channel7, (u32)&USART2->DR, (u32)(USART2_SendBuf.Send_Buff + USART2_SendBuf.Send_StarArr), Dat_Lenght);
				USART2_SendBuf.Send_StarArr = 0;
			}
			else
			{
				Dat_Lenght = USART2_SendBuf.Send_Wait_DatNum;
				MyDMA_Config(DMA1_Channel7, (u32)&USART2->DR, (u32)(USART2_SendBuf.Send_Buff + USART2_SendBuf.Send_StarArr), Dat_Lenght);
				USART2_SendBuf.Send_StarArr += USART2_SendBuf.Send_Wait_DatNum; //下一轮发送的起始位置
				USART2_SendBuf.Send_Wait_DatNum = 0;
			}
			USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
			MyDMA_SetTransmission(DMA1_Channel7, Dat_Lenght);
			USART2_SendBuf.DMA_Flag = BUSY;
		}
	}
}





/**
  * @brief  DMA 串口传输之前需要初始化的参数内容
  * @param  无
  * @retval 串口2发送和接收队列初始化
  */
void USART2_SEND_RECEIVE_Queue_Init()
{
	UART_ReceiveValueInit(&USART2_QueueBuf, UART_DATALENGTH);
	DMAData_Init(&USART2_SendBuf);
}




#endif /* USART2_EN */
















/* 串口3 DMA初始化*****************************************************************************************/
#if USART3_EN

/**
 * @brief  配置嵌套向量中断控制器NVIC
 * @param  无
 * @retval 无
 */
static void USART3_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 嵌套向量中断控制器组选择 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	/* 配置USART为中断源 */
	NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART3_IRQ;
	/* 抢断优先级*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	/* 子优先级 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	/* 使能中断 */
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	/* 初始化配置NVIC */
	NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief  USART GPIO 配置,工作参数配置
 * @param  无
 * @retval 无
 */
void USART3_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// 打开串口GPIO的时钟
	DEBUG_USART3_GPIO_APBxClkCmd(DEBUG_USART3_GPIO_CLK, ENABLE);

	// 打开串口外设的时钟
	DEBUG_USART3_APBxClkCmd(DEBUG_USART3_CLK, ENABLE);

	// 将USART Tx的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART3_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART3_TX_GPIO_PORT, &GPIO_InitStructure);

	// 将USART Rx的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART3_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART3_RX_GPIO_PORT, &GPIO_InitStructure);

	// 配置串口的工作参数
	// 配置波特率
	USART_InitStructure.USART_BaudRate = DEBUG_USART3_BAUDRATE;
	// 配置 针数据字长
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// 配置停止位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// 配置校验位
	USART_InitStructure.USART_Parity = USART_Parity_No;
	// 配置硬件流控制
	USART_InitStructure.USART_HardwareFlowControl =
		USART_HardwareFlowControl_None;
	// 配置工作模式，收发一起
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// 完成串口的初始化配置
	USART_Init(DEBUG_USART_3, &USART_InitStructure);

	// 串口中断优先级配置
	USART3_NVIC_Configuration();

	// 使能串口接收中断
	USART_ITConfig(DEBUG_USART_3, USART_IT_IDLE, ENABLE); //使能串口总线空闲中断 	

	// 使能串口
	USART_Cmd(DEBUG_USART_3, ENABLE);
}



/**
  * @brief  USARTx TX DMA 配置，内存到外设(USART3->DR)
  * @param  无
  * @retval 无
  */
void USART3_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	// 开启DMA时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	// 设置DMA源地址：串口数据寄存器地址*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = USART3_DR_ADDRESS;
	// 内存地址(要传输的变量的指针)
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Uart3_Dma_Dat;
	// 方向：从外设到内存	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	// 传输大小	
	DMA_InitStructure.DMA_BufferSize = SENDBUFF3_Max_SIZE;
	// 外设地址不增	    
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	// 内存地址自增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	// 外设数据单位	
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	// 内存数据单位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	// DMA模式，一次或者循环模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	//DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
	// 优先级：中	
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	// 禁止内存到内存的传输
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	// 配置DMA通道		   
	DMA_Init(USART3_RX_DMA_CHANNEL, &DMA_InitStructure);
	// 使能DMA
	DMA_Cmd(USART3_RX_DMA_CHANNEL, ENABLE);
	// 使能串口3,DMA接收
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
}





/**
  * @brief  串口1的DMA传输
  * @param  无
  * @retval 无
  */
void  USART3_DMA_Send(void)
{
	u16 	Dat_Lenght;

	if (DMA_GetFlagStatus(DMA1_FLAG_TC2) != RESET)
	{
		DMA_ClearFlag(DMA1_FLAG_TC2);
		USART3_SendBuf.DMA_Flag = IDLE;
	}

	if (USART3_SendBuf.DMA_Flag == IDLE)
	{
		if (USART3_SendBuf.Send_Wait_DatNum)
		{
			if ((USART3_SendBuf.Send_StarArr + USART3_SendBuf.Send_Wait_DatNum) > (Uart_Send_Lenght - 1))
			{
				USART3_SendBuf.Send_Wait_DatNum -= (Uart_Send_Lenght - USART3_SendBuf.Send_StarArr);
				Dat_Lenght = (Uart_Send_Lenght - USART3_SendBuf.Send_StarArr);
				MyDMA_Config(DMA1_Channel2, (u32)&USART3->DR, (u32)(USART3_SendBuf.Send_Buff + USART3_SendBuf.Send_StarArr), Dat_Lenght);
				USART3_SendBuf.Send_StarArr = 0;
			}
			else
			{
				Dat_Lenght = USART3_SendBuf.Send_Wait_DatNum;
				MyDMA_Config(DMA1_Channel2, (u32)&USART3->DR, (u32)(USART3_SendBuf.Send_Buff + USART3_SendBuf.Send_StarArr), Dat_Lenght);
				USART3_SendBuf.Send_StarArr += USART3_SendBuf.Send_Wait_DatNum; //下一轮发送的起始位置
				USART3_SendBuf.Send_Wait_DatNum = 0;
			}
			USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
			MyDMA_SetTransmission(DMA1_Channel2, Dat_Lenght);
			USART3_SendBuf.DMA_Flag = BUSY;
		}
	}
}



/**
  * @brief  DMA 串口传输之前需要初始化的参数内容
  * @param  无
  * @retval 串口3发送和接收队列初始化
  */
void USART3_SEND_RECEIVE_Queue_Init()
{
	UART_ReceiveValueInit(&USART3_QueueBuf, UART_DATALENGTH);
	DMAData_Init(&USART3_SendBuf);
}

#endif /* USART3_EN */










/* 串口4 DMA初始化*****************************************************************************************/
#if UART4_EN

/**
 * @brief  配置嵌套向量中断控制器NVIC
 * @param  无
 * @retval 无
 */
static void UART4_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 嵌套向量中断控制器组选择 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	/* 配置USART为中断源 */
	NVIC_InitStructure.NVIC_IRQChannel = DEBUG_UART4_IRQ;
	/* 抢断优先级*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	/* 子优先级 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	/* 使能中断 */
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	/* 初始化配置NVIC */
	NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief  USART GPIO 配置,工作参数配置
 * @param  无
 * @retval 无
 */
void UART4_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// 打开串口GPIO的时钟
	DEBUG_UART4_GPIO_APBxClkCmd(DEBUG_UART4_GPIO_CLK, ENABLE);

	// 打开串口外设的时钟
	DEBUG_UART4_APBxClkCmd(DEBUG_UART4_CLK, ENABLE);

	// 将UART Tx的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_UART4_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_UART4_TX_GPIO_PORT, &GPIO_InitStructure);

	// 将UART Rx的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_UART4_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_UART4_RX_GPIO_PORT, &GPIO_InitStructure);

	// 配置串口的工作参数
	// 配置波特率
	USART_InitStructure.USART_BaudRate = DEBUG_UART4_BAUDRATE;
	// 配置 针数据字长
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// 配置停止位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// 配置校验位
	USART_InitStructure.USART_Parity = USART_Parity_No;
	// 配置硬件流控制
	USART_InitStructure.USART_HardwareFlowControl =
		USART_HardwareFlowControl_None;
	// 配置工作模式，收发一起
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// 完成串口的初始化配置
	USART_Init(DEBUG_UART_4, &USART_InitStructure);

	// 串口中断优先级配置
	UART4_NVIC_Configuration();

	// 使能串口接收中断
	USART_ITConfig(DEBUG_UART_4, USART_IT_IDLE, ENABLE); //使能串口总线空闲中断 	

	// 使能串口
	USART_Cmd(DEBUG_UART_4, ENABLE);
}



/**
  * @brief  USARTx TX DMA 配置，内存到外设(USART3->DR)
  * @param  无
  * @retval 无
  */
void UART4_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	// 开启DMA时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	// 设置DMA源地址：串口数据寄存器地址*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = UART4_DR_ADDRESS;
	// 内存地址(要传输的变量的指针)
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Uart4_Dma_Dat;
	// 方向：从外设到内存	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	// 传输大小	
	DMA_InitStructure.DMA_BufferSize = SENDBUFF4_Max_SIZE;
	// 外设地址不增	    
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	// 内存地址自增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	// 外设数据单位	
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	// 内存数据单位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	// DMA模式，一次或者循环模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	//DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
	// 优先级：中	
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	// 禁止内存到内存的传输
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	// 配置DMA通道		   
	DMA_Init(UART4_RX_DMA_CHANNEL, &DMA_InitStructure);
	// 使能DMA
	DMA_Cmd(UART4_RX_DMA_CHANNEL, ENABLE);
	// 使能串口3,DMA接收
	USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE);
}





/**
  * @brief  串口4的DMA传输
  * @param  无
  * @retval 无
  */
void  UART4_DMA_Send(void)
{
	u16 	Dat_Lenght;

	if (DMA_GetFlagStatus(DMA2_FLAG_TC5) != RESET)
	{
		DMA_ClearFlag(DMA2_FLAG_TC5);
		UART4_SendBuf.DMA_Flag = IDLE;
	}

	if (UART4_SendBuf.DMA_Flag == IDLE)
	{
		if (UART4_SendBuf.Send_Wait_DatNum)
		{
			if ((UART4_SendBuf.Send_StarArr + UART4_SendBuf.Send_Wait_DatNum) > (Uart_Send_Lenght - 1))
			{
				UART4_SendBuf.Send_Wait_DatNum -= (Uart_Send_Lenght - UART4_SendBuf.Send_StarArr);
				Dat_Lenght = (Uart_Send_Lenght - UART4_SendBuf.Send_StarArr);
				MyDMA_Config(DMA2_Channel5, (u32)&UART4->DR, (u32)(UART4_SendBuf.Send_Buff + UART4_SendBuf.Send_StarArr), Dat_Lenght);
				UART4_SendBuf.Send_StarArr = 0;
			}
			else
			{
				Dat_Lenght = UART4_SendBuf.Send_Wait_DatNum;
				MyDMA_Config(DMA2_Channel5, (u32)&UART4->DR, (u32)(UART4_SendBuf.Send_Buff + UART4_SendBuf.Send_StarArr), Dat_Lenght);
				UART4_SendBuf.Send_StarArr += UART4_SendBuf.Send_Wait_DatNum; //下一轮发送的起始位置
				UART4_SendBuf.Send_Wait_DatNum = 0;
			}
			USART_DMACmd(UART4, USART_DMAReq_Tx, ENABLE);
			MyDMA_SetTransmission(DMA2_Channel5, Dat_Lenght);
			UART4_SendBuf.DMA_Flag = BUSY;
		}
	}
}


/**
  * @brief  DMA 串口传输之前需要初始化的参数内容
  * @param  无
  * @retval 串口4发送和接收队列初始化
  */
void UART4_SEND_RECEIVE_Queue_Init()
{
	UART_ReceiveValueInit(&UART4_QueueBuf, UART_DATALENGTH);
	DMAData_Init(&UART4_SendBuf);
}


#endif /* UART4_EN */








/* 串口5 DMA初始化*****************************************************************************************/
#if UART5_EN

/**
 * @brief  配置嵌套向量中断控制器NVIC
 * @param  无
 * @retval 无
 */
static void UART5_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 嵌套向量中断控制器组选择 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	/* 配置USART为中断源 */
	NVIC_InitStructure.NVIC_IRQChannel = DEBUG_UART4_IRQ;
	/* 抢断优先级*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
	/* 子优先级 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	/* 使能中断 */
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	/* 初始化配置NVIC */
	NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief  USART GPIO 配置,工作参数配置
 * @param  无
 * @retval 无
 */
void UART5_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// 打开串口GPIO的时钟
	DEBUG_UART5_GPIO_APBxClkCmd(DEBUG_UART5_GPIO_CLK, ENABLE);

	// 打开串口外设的时钟
	DEBUG_UART5_APBxClkCmd(DEBUG_UART5_CLK, ENABLE);

	// 将UART Tx的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_UART5_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_UART5_TX_GPIO_PORT, &GPIO_InitStructure);

	// 将UART Rx的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_UART5_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_UART5_RX_GPIO_PORT, &GPIO_InitStructure);

	// 配置串口的工作参数
	// 配置波特率
	USART_InitStructure.USART_BaudRate = DEBUG_UART5_BAUDRATE;
	// 配置 针数据字长
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// 配置停止位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// 配置校验位
	USART_InitStructure.USART_Parity = USART_Parity_No;
	// 配置硬件流控制
	USART_InitStructure.USART_HardwareFlowControl =
		USART_HardwareFlowControl_None;
	// 配置工作模式，收发一起
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// 完成串口的初始化配置
	USART_Init(DEBUG_UART_5, &USART_InitStructure);

	// 串口中断优先级配置
	UART5_NVIC_Configuration();

	// 使能串口接收中断
	USART_ITConfig(DEBUG_UART_5, USART_IT_IDLE, ENABLE); //使能串口总线空闲中断 	

	// 使能串口
	USART_Cmd(DEBUG_UART_5, ENABLE);
}



/**
  * @brief  USARTx TX DMA 配置，内存到外设(USART3->DR)
  * @param  无
  * @retval 无
  */
void UART5_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	// 开启DMA时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	// 设置DMA源地址：串口数据寄存器地址*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = UART5_DR_ADDRESS;
	// 内存地址(要传输的变量的指针)
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Uart5_Dma_Dat;
	// 方向：从外设到内存	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	// 传输大小	
	DMA_InitStructure.DMA_BufferSize = SEND5UFF5_Max_SIZE;
	// 外设地址不增	    
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	// 内存地址自增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	// 外设数据单位	
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	// 内存数据单位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	// DMA模式，一次或者循环模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	//DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
	// 优先级：中	
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	// 禁止内存到内存的传输
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	// 配置DMA通道		   
	DMA_Init(UART5_RX_DMA_CHANNEL, &DMA_InitStructure);
	// 使能DMA
	DMA_Cmd(UART5_RX_DMA_CHANNEL, ENABLE);
	// 使能串口5,DMA接收
	USART_DMACmd(UART5, USART_DMAReq_Rx, ENABLE);
}





/**
  * @brief  串口4的DMA传输
  * @param  无
  * @retval 无
  */
void  UART5_DMA_Send(void)
{
	u16 	Dat_Lenght;

	if (DMA_GetFlagStatus(DMA2_FLAG_TC4) != RESET)
	{
		DMA_ClearFlag(DMA2_FLAG_TC4);
		UART5_SendBuf.DMA_Flag = IDLE;
	}

	if (UART5_SendBuf.DMA_Flag == IDLE)
	{
		if (UART5_SendBuf.Send_Wait_DatNum)
		{
			if ((UART5_SendBuf.Send_StarArr + UART5_SendBuf.Send_Wait_DatNum) > (Uart_Send_Lenght - 1))
			{
				UART5_SendBuf.Send_Wait_DatNum -= (Uart_Send_Lenght - UART5_SendBuf.Send_StarArr);
				Dat_Lenght = (Uart_Send_Lenght - UART5_SendBuf.Send_StarArr);
				MyDMA_Config(DMA2_Channel4, (u32)&UART5->DR, (u32)(UART5_SendBuf.Send_Buff + UART5_SendBuf.Send_StarArr), Dat_Lenght);
				UART5_SendBuf.Send_StarArr = 0;
			}
			else
			{
				Dat_Lenght = UART5_SendBuf.Send_Wait_DatNum;
				MyDMA_Config(DMA2_Channel4, (u32)&UART5->DR, (u32)(UART5_SendBuf.Send_Buff + UART5_SendBuf.Send_StarArr), Dat_Lenght);
				UART5_SendBuf.Send_StarArr += UART5_SendBuf.Send_Wait_DatNum; //下一轮发送的起始位置
				UART5_SendBuf.Send_Wait_DatNum = 0;
			}
			USART_DMACmd(UART5, USART_DMAReq_Tx, ENABLE);
			MyDMA_SetTransmission(DMA2_Channel4, Dat_Lenght);
			UART5_SendBuf.DMA_Flag = BUSY;
		}
	}
}


/**
  * @brief  DMA 串口传输之前需要初始化的参数内容
  * @param  无
  * @retval 串口4发送和接收队列初始化
  */
void UART5_SEND_RECEIVE_Queue_Init()
{
	UART_ReceiveValueInit(&UART5_QueueBuf, UART_DATALENGTH);
	DMAData_Init(&UART5_SendBuf);
}


#endif /* UART4_EN */








/**
  * @brief  自定义DMA的初始化配置参数，以方便修改DMA传输通道，数据传输方向
  * @param  DMA_CHx --> DMA 传输通道
  * @param  cpar    --> 外设基地址
  * @param	cmar    --> 内存基地址
  * @param  cndtr   --> DMA数据传输大小
  * @retval 无
  */
void MyDMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_DeInit(DMA_CHx);   
	DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;  
	DMA_InitStructure.DMA_MemoryBaseAddr = cmar;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  
	DMA_InitStructure.DMA_BufferSize = cndtr; 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; 
	DMA_Init(DMA_CHx, &DMA_InitStructure);  
    //配置指定的DMAy通道x的中断，DMA_IT_TC：传输完成 DMA_IT_HT：传输一半 DMA_IT_TE：传输错误*
    // DMA_ITConfig(DMA_CHx , DMA_IT_TC , ENABLE);
} 



/**
  * @brief  自定义刷新DMA使能和数据缓冲区，以便触发完成一次DMA传输
  * @param  DMA_CHx --> DMA 传输通道
  * @param  Lenght  --> DMA 传输数据的大小
  * @retval 无
  */
void MyDMA_SetTransmission(DMA_Channel_TypeDef *DMA_CHx, u16 Lenght)
{
	DMA_Cmd(DMA_CHx, DISABLE );  //关闭USART1 TX DMA1 所指示的通道      
 	DMA_SetCurrDataCounter(DMA_CHx,Lenght);//DMA通道的DMA缓存的大小
 	DMA_Cmd(DMA_CHx, ENABLE);  //使能USART1 TX DMA1 所指示的通道 	
}














/* 环形队列算法*************************************************************************************************************/

/**
  * @brief  DMA要发送的数据的结构体成员初始化
  * @param  无
  * @retval 无
  */
void  DMAData_Init(USART_SendDataTypedef *USART_SendBuf)
{
  USART_SendBuf->DMA_Flag=IDLE;
  USART_SendBuf->Send_StarArr=0;
  USART_SendBuf->Send_EndtArr=0;
  USART_SendBuf->Send_Wait_DatNum=0;
}




/**
  * @brief  串口接收结构体成员初始化
  * @param  Uart_Device_Rx --> 数据接收队列
					Length --> 数据长度
  * @retval 无
  */
void UART_ReceiveValueInit(USART_ReceiveDataTypedef *Uart_Device_Rx,u16 Length)
{
	/*赋值读指针,把指针指向缓冲区首地址*/
    Uart_Device_Rx->receive_last = Uart_Device_Rx->Receive_Buffer;
    Uart_Device_Rx->get_last = Uart_Device_Rx->Receive_Buffer;
    Uart_Device_Rx->receive_length = Length;
    Uart_Device_Rx->receive_signal_flag=0;
    Uart_Device_Rx->receive_signal_flag=0;
}



/**
  * @brief  数据接收，队列入队
  * @param  Uart_Device_Rx --> 数据接收队列
  *	@param		     *Data --> 入队数据（接收得到的数据数组）
  *	@param			Lenght --> 入队数据长度
  * @retval 无
  */
void UART_Receive (USART_ReceiveDataTypedef *Uart_Device_Rx,u8 *Data,u16 Lenght)
{
  u16  i;
  for(i=0;i<Lenght;i++)
  {
    *(Uart_Device_Rx->receive_last) = Data[i];  //把数据放入所指向的地址
    Uart_Device_Rx->receive_last++;				//数据存放地址递增
    Uart_Device_Rx->receive_signal_flag = 1;    //有新数据入队，需要处理标志
    if (Uart_Device_Rx->receive_last >= Uart_Device_Rx->Receive_Buffer + Uart_Device_Rx->receive_length)
    {
        Uart_Device_Rx->receive_last = Uart_Device_Rx->Receive_Buffer;//数据满了，循环进行数据记录
    }    
  }
}



/**
  * @brief  数据发送，数据出队
  * @param  Uart_Device_Rx --> 数据接收队列
  * @retval 无
  */
u8 UART_GetByte (USART_ReceiveDataTypedef *Uart_Device_Rx)
{
    u8 data = 0;
    if (!Uart_Device_Rx->receive_signal_flag)   //队列内无新的数据需要处理
    {
        return 0;
    }
    data = *(Uart_Device_Rx->get_last);   //获得当前需要处理的数据  
    Uart_Device_Rx->get_last++;           //数据处理地址自增
    Uart_Device_Rx->receive_full_flag = 0;

    if (Uart_Device_Rx->get_last >= Uart_Device_Rx->Receive_Buffer + Uart_Device_Rx->receive_length)//数据处理到达队列尾部  
    {
        Uart_Device_Rx->get_last = Uart_Device_Rx->Receive_Buffer;  //队列内数据循环处理，遵循先进先处理的原则
    }

    if (Uart_Device_Rx->get_last == Uart_Device_Rx->receive_last) //处理完了队列内所有的数据
    {
        Uart_Device_Rx->receive_signal_flag = 0;
    }
    return  data;
}



/**
  * @brief  串口发送缓冲数据
  * @param  *USART_SendBuf --> 数据结构结构体指针
  * @param  		  *dat --> 要发送的数据的数组指针首地址
  * @param			lenght --> 要发送的数据长度
  * @retval 无
  */
void  Uart_Send_Upat(USART_SendDataTypedef *USART_SendBuf,u8 *dat,u16 lenght)
{
  	u16 i;
    for(i=0;i<lenght;i++)
    {
      *(USART_SendBuf->Send_Buff+USART_SendBuf->Send_EndtArr)=dat[i];
      if(USART_SendBuf->Send_EndtArr<Uart_Send_Lenght-1)USART_SendBuf->Send_EndtArr++;   //结束发送的数组编号
      else USART_SendBuf->Send_EndtArr=0;
    }
    USART_SendBuf->Send_Wait_DatNum+=lenght;  //等待发送数据的数量
}



/**
  * @brief  串口发送一个字符
  * @param  *USART_SendBuf --> 数据结构结构体指针
  * @param  		   dat --> 要发送的数据
  * @retval 无
  */
void  Uart_Send_Char(USART_SendDataTypedef *USART_SendBuf,u8 dat)
{

    *(USART_SendBuf->Send_Buff+USART_SendBuf->Send_EndtArr)=dat;
    if(USART_SendBuf->Send_EndtArr<Uart_Send_Lenght-1)
    {
      USART_SendBuf->Send_EndtArr++;   //结束发送的数组编号
    }
    else 
    {
      USART_SendBuf->Send_EndtArr=0;
    }
    USART_SendBuf->Send_Wait_DatNum+=1;  //等待发送数据的数量

}
/* END 环形队列算法*****************************************************************************************************************************/


//------------------------------------------------------------------------------------------------------------------------------
/*! 重定向串口打印 */
void USART1_Printf_Char(USART_TypeDef* USARTx, uint8_t dat)
{
	USART_SendData(USARTx, dat);
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
}



void USART1_Printf_String(USART_TypeDef* USARTx,uint8_t *str)
{
	while (*str != '\0')
	{
		USART1_Printf_Char(USARTx, *str);
		str++;
	}
}


/*! 重定向串口打印 */
void USART2_Printf_Char(USART_TypeDef* USARTx, uint8_t dat)
{
	USART_SendData(USARTx, dat);
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
}



void USART2_Printf_String(USART_TypeDef* USARTx,uint8_t *str)
{
	while (*str != '\0')
	{
		USART2_Printf_Char(USARTx, *str);
		str++;
	}
}

//------------------------------------------------------------------------------------------------------------------------------









#if DMA1_EN
void USART1_IRQHandler(void)
{
	u16 Usart1_Rec_Cnt;
	if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
	{
		USART_ReceiveData(USART1);
		Usart1_Rec_Cnt = Uart1_Dma_Max - DMA_GetCurrDataCounter(DMA1_Channel5);

		//***********帧数据处理函数************//
		UART_Receive(&USART1_QueueBuf, Uart1_Dma_Dat, Usart1_Rec_Cnt);
		//*************************************//
		MyDMA_SetTransmission(USART_RX_DMA_CHANNEL, Usart1_Rec_Cnt);
	}
}
#endif /*DMA_TRANSMIT_ENABLE*/




#if DMA2_EN
void USART2_IRQHandler(void)
{
	u16 Usart2_Rec_Cnt;
	if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
	{
		USART_ReceiveData(USART2);
		Usart2_Rec_Cnt = Uart2_Dma_Max - DMA_GetCurrDataCounter(USART2_RX_DMA_CHANNEL);

		//***********帧数据处理函数************//
		UART_Receive(&USART2_QueueBuf, Uart2_Dma_Dat, Usart2_Rec_Cnt);
		//*************************************//
		MyDMA_SetTransmission(USART2_RX_DMA_CHANNEL, Usart2_Rec_Cnt);
	}
}
#endif /*DMA_TRANSMIT_ENABLE*/




#if DMA3_EN
void USART3_IRQHandler(void)
{
	u16 Usart3_Rec_Cnt;
	if (USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
	{
		USART_ReceiveData(USART3);
		Usart3_Rec_Cnt = Uart3_Dma_Max - DMA_GetCurrDataCounter(USART3_RX_DMA_CHANNEL);

		//***********帧数据处理函数************//
		UART_Receive(&USART3_QueueBuf, Uart3_Dma_Dat, Usart3_Rec_Cnt);
		//*************************************//
		MyDMA_SetTransmission(USART3_RX_DMA_CHANNEL, Usart3_Rec_Cnt);
	}
}
#endif /*DMA_TRANSMIT_ENABLE*/




#if DMA4_EN
void USART4_IRQHandler(void)
{
	u16 Uart4_Rec_Cnt;
	if (USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)
	{
		USART_ReceiveData(UART4);
		Uart4_Rec_Cnt = Uart4_Dma_Max - DMA_GetCurrDataCounter(UART4_RX_DMA_CHANNEL);

		//***********帧数据处理函数************//
		UART_Receive(&UART4_QueueBuf, Uart4_Dma_Dat, Uart4_Rec_Cnt);
		//*************************************//
		MyDMA_SetTransmission(UART4_RX_DMA_CHANNEL, Uart4_Rec_Cnt);
	}
}
#endif /*DMA_TRANSMIT_ENABLE*/

/* END 中断服务函数*****************************************************************************************************************************/
