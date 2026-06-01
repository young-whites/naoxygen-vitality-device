#include "fire_usart.h"



#if FireUSART1_ENABLE
/**
 * @brief  配置嵌套向量中断控制器NVIC
 * @param  无
 * @retval 无
 */
static void FireUSART1_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 嵌套向量中断控制器组选择 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	/* 配置USART为中断源 */
	NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART1_IRQ;
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
void FireUSART_1_Config(void)
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
	USART_InitStructure.USART_Parity = USART_Parity_No;
	// 配置硬件流控制
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	// 配置工作模式，收发一起
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// 完成串口的初始化配置
	USART_Init(DEBUG_USART_1, &USART_InitStructure);

	// 串口中断优先级配置
	FireUSART1_NVIC_Configuration();

	// 使能串口接收中断
	USART_ITConfig(DEBUG_USART_1, USART_IT_RXNE, ENABLE);

	// 使能串口
	USART_Cmd(DEBUG_USART_1, ENABLE);

}



// 串口中断服务函数
void DEBUG_USART1_IRQHandler(void)
{
	uint8_t ucTemp;
	if (USART_GetITStatus(DEBUG_USART_1, USART_IT_RXNE) != RESET)
	{
		ucTemp = USART_ReceiveData(DEBUG_USART_1);
//		Tk1300F_Calculate(ucTemp);
	}
}


#endif /*FireUSART1_ENABLE*/



















//--------------------------------------------------------------------------------------------------------------------------------
#if FireUSART3_ENABLE
/**
 * @brief  配置嵌套向量中断控制器NVIC
 * @param  无
 * @retval 无
 */
static void FireUSART3_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 嵌套向量中断控制器组选择 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	/* 配置USART为中断源 */
	NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART3_IRQ;
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
void FireUSART_3_Config(void)
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
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	// 配置工作模式，收发一起
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// 完成串口的初始化配置
	USART_Init(DEBUG_USART_3, &USART_InitStructure);

	// 串口中断优先级配置
	FireUSART3_NVIC_Configuration();

	// 使能串口接收中断
	USART_ITConfig(DEBUG_USART_3, USART_IT_RXNE, ENABLE);

	// 使能串口
	USART_Cmd(DEBUG_USART_3, ENABLE);

}



// 串口中断服务函数
void DEBUG_USART3_IRQHandler(void)
{
	uint8_t ucTemp;
	if (USART_GetITStatus(DEBUG_USART_3, USART_IT_RXNE) != RESET)
	{
		ucTemp = USART_ReceiveData(DEBUG_USART_3);
//		Tk1300F_Calculate(ucTemp);

	}
}


#endif /*FireUSART3_ENABLE*/





























































/****************************************************************************************************************************************************/
/*                                                              共用输出函数                                                                        */
/****************************************************************************************************************************************************/



/*****************  发送一个字符 **********************/
void Usart_SendByte(USART_TypeDef* pUSARTx, uint8_t ch)
{
	/* 发送一个字节数据到USART */
	USART_SendData(pUSARTx, ch);

	/* 等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

/*****************  发送字符串 **********************/
void Usart_SendString(USART_TypeDef* pUSARTx, char* str)
{
	unsigned int k = 0;
	do
	{
		Usart_SendByte(pUSARTx, *(str + k));
		k++;
	} while (*(str + k) != '\0');

	/* 等待发送完成 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET)
	{
	}
}

/*****************  发送一个16位数 **********************/
void Usart_SendHalfWord(USART_TypeDef* pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;

	/* 取出高八位 */
	temp_h = (ch & 0XFF00) >> 8;
	/* 取出低八位 */
	temp_l = ch & 0XFF;

	/* 发送高八位 */
	USART_SendData(pUSARTx, temp_h);
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);

	/* 发送低八位 */
	USART_SendData(pUSARTx, temp_l);
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}