#ifndef __COMPRESSOR_H
#define __COMPRESSOR_H
#include "sys.h"


/****************************** 压缩机 引脚配置参数定义***************************************/
#define             Compressor_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             Compressor_GPIO_CLK                        RCC_APB2Periph_GPIOB
#define             Compressor_GPIO_PORT                       GPIOB
#define             Compressor_GPIO_PIN                        GPIO_Pin_14



/****************************** 压缩机 函数宏定义***************************************/
#define            Compressor_OFF()                             GPIO_ResetBits ( Compressor_GPIO_PORT, Compressor_GPIO_PIN )
#define            Compressor_ON()                              GPIO_SetBits   ( Compressor_GPIO_PORT, Compressor_GPIO_PIN )




void Compressor_GPIO_Config(void);





#endif /*__COMPRESSOR_H*/
