#include "GeneralTIM.h"

/*
 * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
 * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
 * 另外三个成员是通用定时器和高级定时器才有.
 *-----------------------------------------------------------------------------
 *typedef struct
 *{ TIM_Prescaler            都有
 *	TIM_CounterMode			     TIMx,x[6,7]没有，其他都有
 *  TIM_Period               都有
 *  TIM_ClockDivision        TIMx,x[6,7]没有，其他都有
 *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]才有
 *}TIM_TimeBaseInitTypeDef; 
 *-----------------------------------------------------------------------------
 */



// 中断优先级配置
static void GENERAL_TIM_2_NVIC_Config(void)
{
    NVIC_InitTypeDef 	NVIC_InitStructure; 
		// 设置中断组为4
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);		
		// 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = GENERAL_TIM_2_IRQ ;	
		// 设置抢占优先级为 6
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;	 
		// 设置子优先级为0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}







static void GENERAL_TIM_2_Mode_Config(void)
{
/*--------------------时基结构体初始化-------------------------*/

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

		// 开启定时器时钟,即内部时钟CK_INT=72M
    GENERAL_TIM_2_APBxClock_FUN(GENERAL_TIM_2_CLK, ENABLE);	
		// 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
    TIM_TimeBaseStructure.TIM_Period=GENERAL_TIM_2_Period;
		// 时钟预分频数
    TIM_TimeBaseStructure.TIM_Prescaler= GENERAL_TIM_2_Prescaler;	
		// 时钟分频因子 ，没用到不用管
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
		// 计数器计数模式，设置为向上计数
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 		
		// 重复计数器的值，没用到不用管
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
		// 初始化定时器
    TIM_TimeBaseInit(GENERAL_TIM_2, &TIM_TimeBaseStructure);
	
		// 清除计数器中断标志位
    TIM_ClearFlag(GENERAL_TIM_2, TIM_FLAG_Update);
	  
		// 开启计数器中断
    TIM_ITConfig(GENERAL_TIM_2,TIM_IT_Update,ENABLE);



	// 使能计数器
    TIM_Cmd(GENERAL_TIM_2, ENABLE);
}


/*  以上是通用定时器TIM2的相关配置 ****************************************************************************************************************************************/



void GENERAL_TIM_Init(void)
{
	GENERAL_TIM_2_NVIC_Config();
	GENERAL_TIM_2_Mode_Config();
}
/*   以上是通用定时器TIM3的相关配置 ****************************************************************************************************************************************/






