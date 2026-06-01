#ifndef __BSP_HARD_H
#define __BSP_HARD_H
#include "sys.h"




/******************************电源引脚配置参数定义***************************************/
#define             macPower_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macPower_GPIO_CLK                        RCC_APB2Periph_GPIOB
#define             macPower_GPIO_PORT                       GPIOB
#define             macPower_GPIO_PIN                        GPIO_Pin_5

/****************************** FAN 函数宏定义***************************************/
#define             macPower_OFF()                           GPIO_ResetBits ( macPower_GPIO_PORT, macPower_GPIO_PIN )
#define             macPower_ON()                            GPIO_SetBits ( macPower_GPIO_PORT, macPower_GPIO_PIN )


/******************************主气路电磁阀配置参数定义***************************************/
#define             macValve_3_GPIO_APBxClock_FUN            RCC_APB2PeriphClockCmd
#define             macValve_3_GPIO_CLK                      RCC_APB2Periph_GPIOA
#define             macValve_3_GPIO_PORT                     GPIOA
#define             macValve_3_GPIO_PIN                      GPIO_Pin_15

/****************************** FAN 函数宏定义***************************************/
#define             macValve_3_OFF()                         GPIO_ResetBits ( macValve_3_GPIO_PORT, macValve_3_GPIO_PIN )
#define             macValve_3_ON()                          GPIO_SetBits ( macValve_3_GPIO_PORT, macValve_3_GPIO_PIN )



void Power_GPIO_Config(void);
void Main_Gas_Circuit_GPIO_Config(void);






#endif /* __BSP_HARD_H */
