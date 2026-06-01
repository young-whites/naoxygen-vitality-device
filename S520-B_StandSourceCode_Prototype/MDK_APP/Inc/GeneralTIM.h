#ifndef __GENERALTIM_H
#define __GENERALTIM_H
#include "sys.h"






/**************通用定时器TIM参数定义，只限TIM2、3、4、5************/
// 当需要哪个定时器的时候，只需要把下面的宏定义改成1即可
#define GENERAL_TIM2    1
#define GENERAL_TIM3    0
#define GENERAL_TIM4    0

#if  		GENERAL_TIM2
#define            GENERAL_TIM_2                   TIM2
#define            GENERAL_TIM_2_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            GENERAL_TIM_2_CLK               RCC_APB1Periph_TIM2
#define            GENERAL_TIM_2_Period            999
#define            GENERAL_TIM_2_Prescaler         71
#define            GENERAL_TIM_2_IRQ               TIM2_IRQn
#define            GENERAL_TIM_2_IRQHandler        TIM2_IRQHandler
#endif /*GENERAL_TIM2*/











/**************************函数声明********************************/

void GENERAL_TIM_Init(void);
void Adjust_fre_duty_TIM3_CH3(uint32_t fre,uint8_t duty);
void Adjust_fre_duty_TIM3_CH2(uint32_t fre,uint8_t duty);
void Adjust_fre_duty_TIM3_CH1(uint32_t fre,uint8_t duty);
void GENERAL_TIM_3_PWM_Config(uint32_t arr, uint32_t psc);

#endif /*__GENERALTIM_H*/
