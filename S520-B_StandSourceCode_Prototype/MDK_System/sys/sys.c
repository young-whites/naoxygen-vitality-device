#include "sys.h"


/*******************************************/
/*              中断优先级分配             */
/*-----------------------------------------*/
/*	USART1  -> 主优先级：1  子优先级：0
 *  USART2  -> 主优先级：7  子优先级：0
 *  USART3  -> 主优先级：4  子优先级：0
 *  TIM2    -> 主优先级：6  子优先级：0
 *  TIM3    -> 主优先级：6  子优先级：0
 */



void ALL_Config()
{
	/*设置中断优先级分组为组4：4位抢占优先级*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	CPU_TS_TmrInit();
	delay_init(72);
//	NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x08008000);
	/*通用定时器初始化,仅用于定时中断*/
	GENERAL_TIM_Init();
	/*串口初始化*/
	USART2_Config();
	UART4_Config();
	FireUSART_3_Config();
	/*DMA配置初始化*/
	USART2_DMA_Config();
	UART4_DMA_Config();
	/*数据队列初始化*/
	USART2_SEND_RECEIVE_Queue_Init();
	UART4_SEND_RECEIVE_Queue_Init();
	
	/*外设硬件初始化*/
	LED_Init();
	Beep_Init();
	KEY_GPIO_Config();
	Bluetooth_GPIO_Config();
	BSP_FAN_GPIO_Config();
	Compressor_GPIO_Config();
	Tk1300F_GPIO_Config();
	Main_Gas_Circuit_GPIO_Config();
}


/**
  * @brief  系统开机之后或者关机之前第一个要执行的准备工作函数
  * @retval int
  */
void System_Work_CTRL(FunctionalState status)
{
    if(status == DISABLE)
    {

        /*! 电机未处于回退状态 */
        if(Flag.Hand_ReturnStatus == 0){
            USART4_Order_to_Mechanical_Handle(Order_HAND_ACT_WORK_MODE_ZERO_CMD);
        }
    }
    /*! 系统正常可以开机 */
    else{

    }


    while(1)
	{
	
	}


}













//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//关闭所有中断
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
//开启所有中断
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}





