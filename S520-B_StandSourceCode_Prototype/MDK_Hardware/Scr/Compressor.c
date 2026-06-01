#include "Compressor.h"




 /**
  * @brief  配置 压缩机 的 GPIO 功能
  * @param  无
  * @retval 无
  */
void Compressor_GPIO_Config(void)
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;


	/* 配置 压缩机 引脚 */
	RCC_APB2PeriphClockCmd ( Compressor_GPIO_CLK, ENABLE ); 															   
	GPIO_InitStructure.GPIO_Pin = Compressor_GPIO_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init ( Compressor_GPIO_PORT, & GPIO_InitStructure );	

	Compressor_OFF();
}


