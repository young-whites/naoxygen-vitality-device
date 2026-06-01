/**
******************************************************************************
* @文件：       GPIO.c
* @作者：       kelvCdb
* @版本：       V0.1
* @时间：       21-4-2018
* @说明：       
******************************************************************************
*/
#define   _GPIO_H_C
#include "sys.h"
#include "bsp.h"

GPIO_InitTypeDef GPIO_InitStructure;
/*****************************LED引脚***************************
*函数功能：LED引脚设为推挽输出   PA0    PA1    开关机指示灯
*入口参数：void
*返    回：void
*备    注：无
********************************************************************/
void  LED_GPIO_INIT(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
}

/*****************************串口设置***************************
*函数功能：串口引脚初始化
*入口参数：void
*返    回：void
*备    注：无
********************************************************************/
void  USART_GPIO_INIT(void)
{ 
    //USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    //USART1_RX	  GPIOA.10初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    //USART2_TX   GPIOA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    //USART2_RX	  GPIOA.3初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure); 

    //USART3_TX   GPIOB.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    //USART3_RX	  GPIOB.11初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);    

    //USART4_TX   GPIOC.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    //USART4_RX	  GPIOC.11初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);            
}

/*****************************风扇使能引脚***************************
*函数功能：风扇使能引脚设为推挽输出   PB5
*入口参数：void
*返    回：void
*备    注：无
********************************************************************/
void  FAN_PWR_GPIO_INIT(void)
{
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC,GPIO_Pin_6);
}
/*****************************蜂鸣器控制引脚***************************
*函数功能：蜂鸣器控制引脚设为推挽输出   PB13
*入口参数：void
*返    回：void
*备    注：无
********************************************************************/
void  BEEP_GPIO_INIT(void)
{
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB,GPIO_Pin_1);
}

/*****************************蓝牙使能控制引脚***************************
*函数功能：蓝牙使能引脚设为推挽输出   PB13
*入口参数：void
*返    回：void
*备    注：无
********************************************************************/
void  BLUETOOTH_GPIO_INIT(void)
{
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);
}



/*****************************主气路控制引脚***************************
*函数功能：主气路引脚设为推挽输出   PB13
*入口参数：void
*返    回：void
*备    注：无
********************************************************************/
void  Main_Gas_GPIO_INIT(void)
{
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA,GPIO_Pin_15);
}



void  Gpio_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|\
    RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOG, ENABLE);
    BLUETOOTH_GPIO_INIT();
    BEEP_GPIO_INIT();
    FAN_PWR_GPIO_INIT();
    USART_GPIO_INIT();
    LED_GPIO_INIT();
	Main_Gas_GPIO_INIT();
	
}




