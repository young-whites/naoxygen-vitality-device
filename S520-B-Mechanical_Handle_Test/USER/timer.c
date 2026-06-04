#include "timer.h"
#include "stm8s_tim1.h"
#include "stm8s.h"
#include "stm8s_tim2.h"

int pwm_init(TIM1_OCMode_TypeDef TIM1_OCMode,u32 TIM1_Peri0d,u8 direction)
{    
    /* Clock already configured in main(), only enable TIM1 peripheral clock */
    CLK_PeripheralClockConfig( CLK_PERIPHERAL_TIMER1, ENABLE );

    TIM1_DeInit(); /* Time Base configuration */
    /*Tim1 freq: 16mhz/TIM1_Prescaler/TIM1_Period*/
    TIM1_TimeBaseInit(100, TIM1_COUNTERMODE_UP, TIM1_Peri0d, 0); /* Channel 1, 2,3 and 4 Configuration in PWM mode */

    if(direction==0)//停止
    {
        TIM1_OC2Init(TIM1_OCMode, TIM1_OUTPUTSTATE_ENABLE, TIM1_OUTPUTNSTATE_DISABLE, 0 , TIM1_OCPOLARITY_LOW, TIM1_OCNPOLARITY_HIGH, TIM1_OCIDLESTATE_SET, TIM1_OCIDLESTATE_RESET); /*TIM1_Pulse = CCR1_Val*/
        TIM1_OC4Init(TIM1_OCMode, TIM1_OUTPUTSTATE_ENABLE, 0 , TIM1_OCPOLARITY_LOW, TIM1_OCIDLESTATE_RESET); /*TIM1_Pulse = CCR2_Val*/
    }
    else if(direction==1)//backward
    {
        TIM1_OC2Init(TIM1_OCMode, TIM1_OUTPUTSTATE_ENABLE, TIM1_OUTPUTNSTATE_DISABLE, TIM1_Peri0d, TIM1_OCPOLARITY_LOW, TIM1_OCNPOLARITY_HIGH, TIM1_OCIDLESTATE_SET, TIM1_OCIDLESTATE_RESET); /*TIM1_Pulse = CCR1_Val*/
        TIM1_OC4Init(TIM1_OCMode, TIM1_OUTPUTSTATE_ENABLE, TIM1_Peri0d/2, TIM1_OCPOLARITY_LOW, TIM1_OCIDLESTATE_RESET); /*TIM1_Pulse = CCR2_Val*/
    }
    else if(direction==2)//forward
    {
        TIM1_OC2Init(TIM1_OCMode, TIM1_OUTPUTSTATE_ENABLE, TIM1_OUTPUTNSTATE_DISABLE, TIM1_Peri0d/2, TIM1_OCPOLARITY_LOW, TIM1_OCNPOLARITY_HIGH, TIM1_OCIDLESTATE_SET, TIM1_OCIDLESTATE_RESET); /*TIM1_Pulse = CCR1_Val*/
        TIM1_OC4Init(TIM1_OCMode, TIM1_OUTPUTSTATE_ENABLE, TIM1_Peri0d, TIM1_OCPOLARITY_LOW, TIM1_OCIDLESTATE_RESET);
    }

    TIM1_ARRPreloadConfig(ENABLE);    //使能自动重装载
    TIM1_ITConfig( TIM1_IT_UPDATE, ENABLE );//中断
    TIM1_Cmd(ENABLE); /* TIM1 Main Output Enable */
    TIM1_CtrlPWMOutputs(ENABLE);
}
    




__IO uint32_t TimingDelay = 0;




void TIM2_Config(void)
{

    /* TIM2 寄存器初始化默认值 */
    TIM2_DeInit();

    /* 使能TIM2时钟 */
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, ENABLE);

    /* 配置时钟分频系数为16，设置为向上计数模式，计数重装载值为1000 */
    TIM2_TimeBaseInit(TIM2_PRESCALER_16, 1000);

    /* 使能自动重装载 */
    TIM2_ARRPreloadConfig(ENABLE);

    /* 使能 TIM2 中断 */
    TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);

    /* 使能TIM1时钟 */
    TIM2_Cmd(ENABLE);
}




/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in subtle.
  * @retval None
  */
void Delay_ms(__IO uint32_t nTime)
{
    TimingDelay = nTime;

    while (TimingDelay != 0);
}




/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
    if (TimingDelay != 0x00)
    {
        TimingDelay--;
    }
}


