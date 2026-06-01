#include "bsp_beep.h" 


static void     Beep_GPIO_Config                        ( void );



void Beep_Init ( void )
{		
 	Beep_GPIO_Config ();
  	BEEP_SetCycleDuty(200,100);
}


static void Beep_GPIO_Config ( void )
{		
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	
	/*开启GPIOB和GPIOF的外设时钟*/
	macBEEP_GPIO_APBxClock_FUN  ( macBEEP_GPIO_CLK, ENABLE ); 

	/*选择要控制的GPIOG引脚*/															   
	GPIO_InitStructure.GPIO_Pin = macBEEP_PIN;	

	/*设置引脚模式为下拉模式*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	/*设置引脚速率为50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*调用库函数，初始化GPIOG6*/
	GPIO_Init ( macBEEP_PORT, & GPIO_InitStructure );

}

/* END BEEP Config******************************************************************/



static	int16_t	beepClk=0;							// 蜂鸣器鸣叫时钟（单位：ms）
static	int16_t	beepCyc=0;							// 蜂鸣器单次鸣叫的周期（一次完整鸣叫+间隔的总时间，单位：ms）
static	int16_t	beepDty=0;							// 蜂鸣器单次鸣叫的时间（周期中鸣叫所占的时间，单位：ms）
static	int8_t	beepCnt=0;							// 蜂鸣器鸣叫计数
static	int8_t	beepCry=0;							// 蜂鸣器鸣叫次数（0表示不鸣叫）
static	int8_t	beepMut=0;							// 蜂鸣器停顿周期数（0表示不停顿）
static	int8_t	beepCct=0;							// 蜂鸣器重复计数
static	int8_t	beepRep=0;							// 蜂鸣器重复鸣叫次数（0表示不重复，100以上表示无限重复）



/*****************************************************************************
* 功能:		蜂鸣器关闭
*****************************************************************************/
void BEEP_Off(void)
{
	beepCry=0;										// 禁止（间隔）鸣叫
	macBEEP_OFF();
}



/*****************************************************************************
* 功能:		蜂鸣器长鸣
*****************************************************************************/
void BEEP_On(void)
{
	beepCry=0;										// 禁止（间隔）鸣叫
	macBEEP_ON();
}




/*****************************************************************************
* 功能:		设置蜂鸣器鸣叫周期及占空比
* 参数:		Cycle	周期（一次完整鸣叫+间隔的总时间，单位：ms）
* 			Duty	占空比（周期中鸣叫所占的时间，单位：ms）
*****************************************************************************/
void BEEP_SetCycleDuty(int16_t Cycle, int16_t Duty)
{
	beepCyc=Cycle;
	beepDty=Duty;
	beepClk=0;
}



/*****************************************************************************
* 功能:		蜂鸣器鸣叫（指定次数）
* 参数:		cry		蜂鸣器鸣叫次数（0表示不鸣叫）
* 			mute	蜂鸣器停顿周期数（0表示不停顿）
* 			repeat	蜂鸣器重复鸣叫次数（0表示不重复，100以上表示无限重复）
*****************************************************************************/
void BEEP_Blink(int8_t cry, int8_t mute, int8_t repeat)
{
	beepCry=cry;
	beepMut=mute;
	beepRep=repeat;
	beepCnt=0;
	beepCct=0;
}





/*****************************************************************************
* 功能:		蜂鸣器扫描
* 说明:		扫描周期：1ms。
*****************************************************************************/
void BEEP_DrvScan(void)
{
	if(beepCry){									// 需要鸣叫
		if(++beepClk >= beepCyc){ 					// 鸣叫小周期结束
			beepClk=0;
			if(++beepCnt >= (beepCry+beepMut)){		// 鸣叫中周期结束
				beepCnt=0;
				if(++beepCct >= beepRep){			// 鸣叫大周期结束
					beepCct=0;
					if(beepRep < 100)	beepCry=0;	// 重复次数到达，结束鸣叫
				}
			}
		}else if(beepClk >= beepDty){				// 后半小周 不叫
			macBEEP_OFF();
		}else if(beepCnt < beepCry){				// （仅非停顿期间的）前半小周 鸣叫
			macBEEP_ON();
		}
	}
}



