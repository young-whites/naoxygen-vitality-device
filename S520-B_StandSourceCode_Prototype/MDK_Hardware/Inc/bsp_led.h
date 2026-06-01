#ifndef __BSP_LED_H_
#define	__BSP_LED_H_



#include "stm32f10x.h"




#define		LED_NUM			(5)					// LED数量

	/*LED名称类型*/
	typedef	enum	
	{								
		LED_Name_4G	= (0x01),					// 测试指示 	LED名称值
		LED_Name_Block,							// 通讯指示 	LED名称值
		LED_Name_Circle,						// 运行指示 	LED名称值
		LED_Name_Orange,						// 关机指示灯	LED名称值
		LED_Name_White,							// 开机指示灯	LED名称值
	}LED_Name_TypeDef;
	//------------------------------------------------------------------------
	




/****************************** LED_Orange 引脚配置参数定义***************************************/
#define             macLED_Orange_GPIO_APBxClock_FUN            RCC_APB2PeriphClockCmd
#define             macLED_Orange_GPIO_CLK                      RCC_APB2Periph_GPIOA
#define             macLED_Orange_GPIO_PORT                     GPIOA
#define             macLED_Orange_GPIO_PIN                      GPIO_Pin_1



/****************************** LED_White 引脚配置参数定义***************************************/
#define             macLED_White_GPIO_APBxClock_FUN             RCC_APB2PeriphClockCmd
#define             macLED_White_GPIO_CLK                       RCC_APB2Periph_GPIOA
#define             macLED_White_GPIO_PORT                      GPIOA
#define             macLED_White_GPIO_PIN                       GPIO_Pin_4



 
/****************************** LED_4G 引脚配置参数定义***************************************/
#define             macLED_4G_GPIO_APBxClock_FUN                RCC_APB2PeriphClockCmd
#define             macLED_4G_GPIO_CLK                          RCC_APB2Periph_GPIOA
#define             macLED_4G_GPIO_PORT                         GPIOA
#define             macLED_4G_GPIO_PIN                          GPIO_Pin_6



/****************************** LED_Block 引脚配置参数定义***************************************/
#define             macLED_Block_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macLED_Block_GPIO_CLK                        RCC_APB2Periph_GPIOA
#define             macLED_Block_GPIO_PORT                       GPIOA
#define             macLED_Block_GPIO_PIN                        GPIO_Pin_7


/****************************** LED_Circle 引脚配置参数定义***************************************/
#define             macLED_Circle_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macLED_Circle_GPIO_CLK                        RCC_APB2Periph_GPIOC
#define             macLED_Circle_GPIO_PORT                       GPIOC
#define             macLED_Circle_GPIO_PIN                        GPIO_Pin_4




/****************************** LED 函数宏定义***************************************/
#define             macLED_4G_OFF()                             GPIO_ResetBits ( macLED_4G_GPIO_PORT, macLED_4G_GPIO_PIN )
#define             macLED_4G_ON()                              GPIO_SetBits ( macLED_4G_GPIO_PORT, macLED_4G_GPIO_PIN )
#define             macLED_4G_TOGGLE()                          GPIO_ReadOutputDataBit ( macLED_4G_GPIO_PORT, macLED_4G_GPIO_PIN ) ? \
                                                                GPIO_ResetBits ( macLED_4G_GPIO_PORT, macLED_4G_GPIO_PIN ) : GPIO_SetBits ( macLED_4G_GPIO_PORT, macLED_4G_GPIO_PIN )

#define             macLED_Block_OFF()                          GPIO_ResetBits ( macLED_Block_GPIO_PORT, macLED_Block_GPIO_PIN )
#define             macLED_Block_ON()                           GPIO_SetBits ( macLED_Block_GPIO_PORT, macLED_Block_GPIO_PIN )
#define             macLED_Block_TOGGLE()                       GPIO_ReadOutputDataBit ( macLED_Block_GPIO_PORT, macLED_Block_GPIO_PIN ) ? \
                                                                GPIO_ResetBits ( macLED_Block_GPIO_PORT, macLED_Block_GPIO_PIN ) : GPIO_SetBits ( macLED_Block_GPIO_PORT, macLED_Block_GPIO_PIN )

#define             macLED_Circle_OFF()                         GPIO_ResetBits ( macLED_Circle_GPIO_PORT, macLED_Circle_GPIO_PIN )
#define             macLED_Circle_ON()                          GPIO_SetBits ( macLED_Circle_GPIO_PORT, macLED_Circle_GPIO_PIN )
#define             macLED_Circle_TOGGLE()                      GPIO_ReadOutputDataBit ( macLED_Circle_GPIO_PORT, macLED_Circle_GPIO_PIN ) ? \
                                                                GPIO_ResetBits ( macLED_Circle_GPIO_PORT, macLED_Circle_GPIO_PIN ) : GPIO_SetBits ( macLED_Circle_GPIO_PORT, macLED_Circle_GPIO_PIN )

#define             macLED_Orange_OFF()                         GPIO_ResetBits ( macLED_Orange_GPIO_PORT, macLED_Orange_GPIO_PIN )
#define             macLED_Orange_ON()                          GPIO_SetBits ( macLED_Orange_GPIO_PORT, macLED_Orange_GPIO_PIN )
#define             macLED_Orange_TOGGLE()                      GPIO_ReadOutputDataBit ( macLED_Orange_GPIO_PORT, macLED_Orange_GPIO_PIN ) ? \
                                                                GPIO_ResetBits ( macLED_Orange_GPIO_PORT, macLED_Orange_GPIO_PIN ) : GPIO_SetBits ( macLED_Orange_GPIO_PORT, macLED_Orange_GPIO_PIN )

#define             macLED_White_OFF()                          GPIO_ResetBits ( macLED_White_GPIO_PORT, macLED_White_GPIO_PIN )
#define             macLED_White_ON()                           GPIO_SetBits ( macLED_White_GPIO_PORT, macLED_White_GPIO_PIN )
#define             macLED_White_TOGGLE()                       GPIO_ReadOutputDataBit ( macLED_White_GPIO_PORT, macLED_White_GPIO_PIN ) ? \
                                                                GPIO_ResetBits ( macLED_White_GPIO_PORT, macLED_White_GPIO_PIN ) : GPIO_SetBits ( macLED_White_GPIO_PORT, macLED_White_GPIO_PIN )

/************************** PAD 函数声明********************************/
void LED_Init(void);
void LED_Out(int8_t ledName, int8_t ledState);
int8_t LED_GetNumber(void);
void LED_Off(int8_t ledName);
void LED_On(int8_t ledName);
void LED_Toggle(int8_t ledName);
void LED_Grad(int8_t ledName);
void LED_BlinkSetCycleDuty(int8_t ledName, int8_t Cycle, int8_t Duty);
void LED_Blink(int8_t ledName, int8_t cry, int8_t mute, int8_t repeat);
void LED_Fancy(int8_t mode);
void LED_DrvScan(void);

void Power_Off_LED(void);
void Power_On_LED(void);




#endif /* __LED_H_ */

