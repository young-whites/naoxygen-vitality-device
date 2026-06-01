/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h" 
#include "sys.h"

 
 
/**********自定义的全局变量*******************/
volatile uint32_t TimeCnt_ms = 0; // ms 计时变量
 
 
void NMI_Handler(void)
{
}
 
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

 
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}
 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
 
void SVC_Handler(void)
{
}
 
void DebugMon_Handler(void)
{
}
 
void PendSV_Handler(void)
{
}
 
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{

}



















/**
  * @brief  This function handles Basic TIM6 interrupt request.
  * @param  None
  * @retval None
  */
void  GENERAL_TIM_2_IRQHandler (void)
{
	static int msCnt=0;
	
	
	if ( TIM_GetITStatus( GENERAL_TIM_2, TIM_IT_Update) != RESET ) 
	{	
		TimeCnt_ms++;
		
		/*计数用于取余运算来判断时间事件*/
		if(++msCnt>=36000000)
		{
			msCnt=0;
		}
		
		/*1s扫秒时间，取余为整数说明时间到进入执行函数*/
		if((msCnt%15)==0)	 	  Timing_1ms();
		if((msCnt%150) == 0)      Timing_10ms();
		if((msCnt%750) == 0)      Timing_50ms();
		if((msCnt%7500)== 0)      Timing_500ms();
    	if((msCnt%15000)== 0)     Timing_1s();
		
	
		TIM_ClearITPendingBit(GENERAL_TIM_2 , TIM_FLAG_Update);  		 
	}		 	
}










/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
