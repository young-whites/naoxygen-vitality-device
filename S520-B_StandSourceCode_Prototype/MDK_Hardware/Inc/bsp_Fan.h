#ifndef __BSP_FAN_H
#define __BSP_FAN_H
#include "sys.h"



/****************************** 散热风扇 引脚配置参数定义***************************************/
#define             macFAN_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macFAN_GPIO_CLK                        RCC_APB2Periph_GPIOC
#define             macFAN_GPIO_PORT                       GPIOC
#define             macFAN_GPIO_PIN                        GPIO_Pin_6

/****************************** FAN 函数宏定义***************************************/
#define             macFAN_OFF()                           GPIO_ResetBits ( macFAN_GPIO_PORT, macFAN_GPIO_PIN )
#define             macFAN_ON()                            GPIO_SetBits ( macFAN_GPIO_PORT, macFAN_GPIO_PIN )



#define FAN_GPIO_CTRL		1


void BSP_FAN_GPIO_Config(void);








#endif /*__BSP_FAN_H*/
