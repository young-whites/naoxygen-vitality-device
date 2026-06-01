 #include "Bluetooth.h"
//----------------------------------------------------------------------------
int16_t	btPowerOnCnt = 0;							// 蓝牙电源开启计时
int16_t	bBtCnct = 0;								// 蓝牙连接状态
//----------------------------------------------------------------------------


void Bluetooth_GPIO_Config ( void )
{		
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	macBluetooth_EN_GPIO_APBxClock_FUN  ( macBluetooth_EN_GPIO_CLK, ENABLE ); 														   
	GPIO_InitStructure.GPIO_Pin = macBluetooth_EN_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init ( macBluetooth_EN_PORT, & GPIO_InitStructure );			
	GPIO_ResetBits(macBluetooth_EN_PORT,macBluetooth_EN_PIN);

	macBluetooth_STA_GPIO_APBxClock_FUN  ( macBluetooth_STA_GPIO_CLK, ENABLE ); 														   
	GPIO_InitStructure.GPIO_Pin = macBluetooth_STA_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   
	GPIO_Init ( macBluetooth_STA_PORT, & GPIO_InitStructure );

}





/*****************************************************************************
* 功能:		蓝牙模块 扫描 （10ms扫描一次）
*****************************************************************************/
uint8_t ATNameBuf[20] = "AT+NAMEX04\r\n";
void BLUETOOTH_Scan(void)
{
	static	int8_t	send=1;
	static	int8_t	cnt=0;
	int8_t	curr;

	if(btPowerOnCnt < 0xFF){								// 首次上电开机后0.5秒，设置一次名称
		btPowerOnCnt ++;
		// 首次上电开机后0.5秒，设置一次名称
		if(send && (btPowerOnCnt == 50)){
			send = 0;
			USART2_Printf_String(USART2, ATNameBuf);
		}
	}
	
	curr = BLUETOOTH_GetState();							// 获取蓝牙模块的连接状态
	if(curr != bBtCnct)
	{
		/* 状态改变维持100ms就认为真的改变 */
		if(++cnt >= 10)
		{	
			cnt = 0;
			bBtCnct = curr;									// 保存新的蓝牙连接状态
			if(bBtCnct)
			{	
				BEEP_Blink(2,0,0);							// 蓝牙刚联通时，蜂鸣器鸣叫2声
			}
		}
	}else{
		cnt = 0;
	}
}

/*****************************************************************************
* 功能:		获取蓝牙模块的连接状态
* 返回:		0：未连接；1：已连接
*****************************************************************************/
int8_t BLUETOOTH_GetConnect(void)
{
	return bBtCnct;
}



/*****************************************************************************
* 功能:		获取蓝牙模块的连接状态
* 返回:		0：未连接；1：已连接
*****************************************************************************/
int8_t BLUETOOTH_GetState(void)
{
	return GPIO_ReadInputDataBit(macBluetooth_STA_PORT, macBluetooth_STA_PIN) ? 1 : 0;
}



/**
  * @brief  This is a function that setting the bluetooth's work-station.
  * @retval 0: disconnect
  *         1: connect
  */
void BLUETOOTH_SET_EN(FunctionalState status)
{
    if(status == ENABLE){
        GPIO_SetBits(macBluetooth_EN_PORT,macBluetooth_EN_PIN);
    }
    else if(status == DISABLE){
        GPIO_ResetBits(macBluetooth_EN_PORT,macBluetooth_EN_PIN);
    }
}

