#include "Tk1300F.h"
#include <stdio.h>






void Tk1300F_GPIO_Config(void)
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* 配置 Tk1300F 的使能引脚 */
	TK130F_GPIO_APBxClock_FUN(TK130F_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = TK130F_Ctrl_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TK130F_Ctrl_GPIO_PORT, &GPIO_InitStructure);
	TK1300F_ON;
}




















