#include "bsp_Key.h"



void KEY_GPIO_Config(void)
{

	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;
	

	/* 开关按键 */
	macKEY_Power_GPIO_APBxClock_FUN  ( macKEY_Power_GPIO_CLK, ENABLE ); 														   
	GPIO_InitStructure.GPIO_Pin = macKEY_Power_GPIO_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   
	GPIO_Init ( macKEY_Power_GPIO_PORT, & GPIO_InitStructure );

}


















/* 以下是按键驱动（视情况使用）**************************************************************************************************************************************************************/

extern uint8_t  KEY_GetState(uint8_t keyName);
extern uint8_t  KEY_GetNumber(void);
//----------------------------------------------------------------------------
#define		KEY_MAX						(4)			// 最大支持按键数量
//----------------------------------------------------------------------------
#define		KEY_VAL_BUF_SIZE			(KEY_MAX+3)	// 键值缓冲区大小
#define		KEY_SCAN_FILTER_TIMES		(5)			// 按键扫描滤波计数（滤波消抖时间=计数值*扫描周期；比如：扫描周期为10ms的滤波时间为50ms；以此类推）
#define		KEY_SCAN_LONG_PRESS_2S		(200)		// 按键扫描长按计数（按键长按时间=计数值*扫描周期；比如：扫描周期为10ms的长按时间为 2 s；以此类推）
#define		KEY_SCAN_LONG_PRESS_4S		(400)		// 按键扫描长按计数（按键长按时间=计数值*扫描周期；比如：扫描周期为10ms的长按时间为 4 s；以此类推）
//----------------------------------------------------------------------------
#define		KEY_Val_First				(0x01)		// port_key.c中的首个按键值应与此值一致，且其他按键值须在此基础上连续递增
//----------------------------------------------------------------------------
static	uint8_t	_keyValBuf[KEY_VAL_BUF_SIZE];		// 键值缓冲区
static	uint8_t	_keyValBufR=0;						// 键值缓冲区读取索引（读取缓冲区时，读写索引号相等表示缓冲区为空）
static	uint8_t	_keyValBufW=0;						// 键值缓冲区写入索引（写入缓冲区时，读写索引号相等表示缓冲区已满）
static	uint8_t	_keyValBufCnt=0;					// 键值缓冲区待读取键值计数
//----------------------------------------------------------------------------


/*****************************************************************************
* 功能:		键值写入
* 参数:		value：键值
*****************************************************************************/
void KEY_Write(uint8_t value)
{
	if(_keyValBufCnt<KEY_VAL_BUF_SIZE)	{			// 缓冲区尚未写满（已满则丢弃当前待写入键值）
		_keyValBufCnt++;							// 每写入一个键值，计数加一
		_keyValBuf[_keyValBufW]=value;				// 写入键值到缓冲区
		if(++_keyValBufW>=KEY_VAL_BUF_SIZE)	{		// 调整写入位置
			_keyValBufW=0;							// 卷绕
		}
	}
}


/*****************************************************************************
* 功能:		键值读取
* 返回:		键值（0表示没有键值）
*****************************************************************************/
uint8_t KEY_Read(void)
{
	uint8_t	value=0;
	if(_keyValBufCnt)	{							// 缓冲区有尚未读取的键值
		_keyValBufCnt--;							// 每读出一个键值，计数减一
		value=_keyValBuf[_keyValBufR];				// 读取缓冲区键值
		if(++_keyValBufR>=KEY_VAL_BUF_SIZE)	{		// 调整读取位置
			_keyValBufR=0;							// 卷绕
		}
	}
	return value;
}





/*****************************************************************************
* 功能:		按键扫描
* 说明:		扫描周期：10ms；滤波消抖时间：50ms；长按时间：2s。
*****************************************************************************/
void KEY_DrvScan(void)
{
	static	uint8_t	    step[KEY_MAX]={0};							// 扫描状态
	static	uint16_t	holdingCnt[KEY_MAX]={0};					// 按住计时
	static	uint8_t	    filterCnt[KEY_MAX]={0};						// 滤波计时
	static	uint8_t	    pressLast[KEY_MAX]={0};					    // 上次按下状态
	uint8_t	i,num,pressCurr;										// 当前按下状态
	
	num=KEY_GetNumber();											// 获取按键个数
	if(num>KEY_MAX)	num=KEY_MAX;									// 按键数量不能超范围
	for(i=0;i<num;i++){												// 逐个按键扫描
		pressCurr = KEY_GetState(KEY_Val_First+i);					// 获取按键当前按下状态
		switch(step[i]){											// 扫描状态分为：稳定状态和滤波状态
		case 0:														// 稳定状态（持续松开或持续按住状态）
			if(pressCurr!=pressLast[i]){							// 按键状态有变化
				pressLast[i]=pressCurr;								// 按键状态一旦改变，更新上次状态值
				filterCnt[i]=0;										// 开始消抖计时
				step[i]=1;											// 扫描进入滤波消抖状态
			}else if(pressCurr){									// 按键持续被按住
				if(holdingCnt[i]!=0xffff){							// 防止计数器回零重复响应
					holdingCnt[i]++;
					if(holdingCnt[i]==KEY_SCAN_LONG_PRESS_2S){		// 长按2S达到
						KEY_Write((KEY_Val_First+i)+KEY_Evt_Long2S);// 键值写入到缓冲区
					}
					if(holdingCnt[i]==KEY_SCAN_LONG_PRESS_4S){		// 长按4S达到
						KEY_Write((KEY_Val_First+i)+KEY_Evt_Long4S);// 键值写入到缓冲区
					}
				}
			}
			break;
		case 1:														// 滤波状态（消除抖动）
			if(pressCurr==pressLast[i]){							 
				if(++filterCnt[i]>=KEY_SCAN_FILTER_TIMES){			// 连续N次状态改变，才认为按键状态确实已改变
					if(pressCurr){									// 确实已按下
						KEY_Write((KEY_Val_First+i)+KEY_Evt_Press); // 键值写入到缓冲区
						holdingCnt[i]=0;							// 开始计时
					}else{											// 确实已松开
						KEY_Write((KEY_Val_First+i)+KEY_Evt_Release);// 键值写入到缓冲区
					}
					step[i]=0;										// 回到稳态继续监测
				}
			}else{													// 消抖期间，一旦发现本次与上次状态不同，则认为是干扰
				pressLast[i]=pressCurr;								// 按键状态一旦改变，更新上次状态值
				step[i]=0;											// 回到稳态继续监测
			}
			break;
		default:
			step[i]=0;												// 非法状态则立即回到稳态
			break;
		}
	}
}


/* 以下是按键事件处理函数***************************************************************************************************************************************/



/*****************************************************************************
* 功能:		获取按键按下状态
* 返回:		0：未按；非0：按下
*****************************************************************************/
uint8_t KEY_GetState(uint8_t keyName)
{
	uint8_t	stat=0;
	switch(keyName)
	{
		case KEY_Val_Power: stat = (GPIO_ReadInputDataBit(macKEY_Power_GPIO_PORT, macKEY_Power_GPIO_PIN) ?  1 : 0);	break;	// 按键管脚 检测低电平表示导通
	}
	return stat;
}

/*****************************************************************************
* 功能:		获取按键数量
*****************************************************************************/
uint8_t KEY_GetNumber(void)
{
	return KEY_NUM;
}



/*****************************************************************************
* 功能:		电源按键 按下处理
*****************************************************************************/
static void _KeyPower_Press(void)
{
    if(Record.KeyLong4sCnt != 1){
        Record.KeyPowerCnt++;
        BEEP_Blink(1, 0, 0);
    }
}



/*****************************************************************************
* 功能:       KEYA按键 长按下处理
*****************************************************************************/
static void _keyLong4s_Press(void)
{
    /*! 开机时才允许自检 */
    if(Record.KeyPowerCnt == 1){
        Record.KeyLong4sCnt++;
    }

}




/*****************************************************************************
* 功能:		按键处理
*****************************************************************************/
void KEY_Scan(void)
{
	uint8_t	key,event;
	
	for( key=KEY_Read(); key; key=KEY_Read())
	{
		event=key&0xf0;													// 事件
		key  =key&0x0f;													// 键值
		switch(key)
		{
			/* 电源按键 */
			case KEY_Val_Power:	
			{
				switch (event)
				{
					case KEY_Evt_Release:		_KeyPower_Press();		break;
					case KEY_Evt_Long4S:        _keyLong4s_Press();     break;
				}
			}break;

		}
	}
}



