#include"init.h"

/********************************************************************
函数功能：配置系统时钟
入口参数：void
返    回：void
备    注：无
********************************************************************/
#define RCC_PLLSource_PREDIV1            ((uint32_t)0x00010000)
void Sys_Clock_Init(void)
{
    ErrorStatus HSEStartUpStatus;

    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x08000000);
    /* Enable HSE */
    RCC_HSEConfig(RCC_HSE_ON);

    /* Wait till HSE is ready */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();

    if(HSEStartUpStatus == SUCCESS)
    {
        /* Enable Prefetch Buffer */
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

        /* Flash 2 wait state */
        FLASH_SetLatency(FLASH_Latency_2);

        /* HCLK = SYSCLK AHB */
        RCC_HCLKConfig(RCC_SYSCLK_Div1);

        /* PCLK2 = HCLK ,APB2 CLOCK*/
        RCC_PCLK2Config(RCC_HCLK_Div1);

        /* PCLK1 = HCLK/2 ,APB1 CLOCK*/
        RCC_PCLK1Config(RCC_HCLK_Div1);

        /* PLLCLK = 8MHz * 9 = 72MHz */
        RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
        /* Enable PLL */
        RCC_PLLCmd(ENABLE);

        /* Wait till PLL is ready */
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }

        /* Select PLL as system clock source */
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

        /* Wait till PLL is used as system clock source */
        while(RCC_GetSYSCLKSource() != 0x08)
        {
        }
    }
}

#define UPDATE
void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);    

    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);    

}






/*******************************************************************************
* FunctionName   : void IWDG_Init(u8 prer, u16 rlr)
* Description    : 独立看门狗初始化函数
* Notice         : prer 分频   rlr重载值  主时钟 40K
* Param[in]      : 无
* Output         : NO.
* Return         : NO.
* LastUpdateDate : 2017/03/06---2017/03/06
*******************************************************************************/
void IWDG_Init(u8 prer, u16 rlr)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    IWDG_SetPrescaler(prer);

    IWDG_SetReload(rlr);

    IWDG_ReloadCounter();

    IWDG_Enable();
}
/*******************************************************************************
* FunctionName   : void IWDG_Feed(void)
* Description    : 独立看门狗喂狗
* Notice         : 无
* Param[in]      : 无
* Output         : NO.
* Return         : NO.
* LastUpdateDate : 2017/03/06---2017/03/06
*******************************************************************************/
void IWDG_Feed(void)
{
    IWDG_ReloadCounter();
}

/*****************************板子外设初始化***************************
*函数功能：电子板外设初始化
*入口参数：void
*返    回：void
*备    注：无
********************************************************************/
void  peripheral_init(void)
{   
 
    NVIC_Configuration();
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 
    IAP_Init(USART2,9600);
	
	
    USART_Cmd(UART4, DISABLE);
    USART_Cmd(USART1, DISABLE);
		
	DMA_Cmd(DMA1_Channel5, DISABLE);
	USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);
	USART_DMACmd(USART1,USART_DMAReq_Rx,DISABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, DISABLE);

    DMA_Cmd(DMA1_Channel3, DISABLE);
    USART_ITConfig(USART3, USART_IT_IDLE, DISABLE);
    USART_DMACmd(USART3,USART_DMAReq_Rx,DISABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, DISABLE);

    DMA_Cmd(DMA2_Channel3, DISABLE);
    USART_ITConfig(UART4, USART_IT_IDLE, DISABLE);
    USART_DMACmd(UART4,USART_DMAReq_Rx,DISABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, DISABLE);     

}
