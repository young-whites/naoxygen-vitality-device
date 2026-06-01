#ifndef _TIMER_H
#define _TIMER_H

#include "stm8s_conf.h"
#include "stm8s_tim1.h"


#define   START  TIM1_OCMODE_TOGGLE
#define   STOP   TIM1_OCMODE_PWM2

 int pwm_init(TIM1_OCMode_TypeDef TIM1_OCMode,u32 TIM1_Prescaler,u8 direction);

 void TIM2_Config(void);
 void Delay_ms(__IO uint32_t nTime);
 void TimingDelay_Decrement(void);



#endif