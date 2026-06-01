#include "Pressure_PID.h"



/*******************************************/
/*              PID算法                    */
/*-----------------------------------------*/
/*	本算法采用增量式PID算法进行气压控制
 *  u(k) = { Kp * [err(k) - err(k-1)] } + Ki * err(k) + { Kd * [err(k) - 2err(k-1) + err(k-2)] }
 * 
 *  其中，err(k)代表本次误差，err(k-1)代表上次误差 , err(k-2)代表上上次误差
 */




/* 气压的预设参数值 */
PRESSVALUE_StructTypeDef	PressPara;









#if ENABLE_POSITION_TYPE_PID

/* 位置式PID参数 */
PID_Position_StructType		PressPID;


/**
  * @brief  气压PID控制参数初始化
  * @param  None
  * @retval None
  */
void Press_PID_Init(void)
{
	PressPID.Kp = Press_Kp;
	PressPID.Ki = Press_Ki;
	PressPID.Kd = Press_Kd;
	PressPID.Ek1 = 0;
	PressPID.Ek  = 0;
	PressPID.ErrIntegral = 0;

	PressPara.TargetPress = 3200;
	PressPara.ErrPress = 500;
}




/**
  * @brief  位置式积分分离 -- PID计算
  * @param  None
  * @retval u(k) = { Kp * err(k)} + { Ki * ∑err(k) } + { Kd * [err(k)-err(k-1)]} 
  * @retval 气压传感器的实际值是1000~4000的数值（100Kpa~400Kpa）
  * @retval WaterFire协议格式："<any>:ch0,ch1,ch2,...,chN\n"
  */
void PID_Calculate_Press(void)
{
	float P, I, D;
	PressPara.CurrentPress = Record.PressDat;
	/* 计算本次偏差 */
	PressPID.Ek = PressPara.TargetPress - PressPara.CurrentPress;  

	
	/* 误差的积分项 */
	PressPID.ErrIntegral += PressPID.Ek;
	/* 位置式PID */
	P = PressPID.Kp * PressPID.Ek;
	I = PressPID.Ki * PressPID.ErrIntegral;
	D = PressPID.Kd * (PressPID.Ek - PressPID.Ek1);

	/* 传递误差 */
	PressPID.Ek1 = PressPID.Ek;

	/* 如果绝对误差大于规定值，使用PD控制 */
	if (abs(PressPID.Ek) > PressPara.ErrPress)
		I = 0;

	PressPID.Uk = P + I + D;

	/* 限幅 */
	//if (PressPID.Uk > 4000)	PressPID.Uk = 4000;
	//if (PressPID.Uk < 1000) PressPID.Uk = 1000;

	if (PressPID.Uk > 3400)
	{
		/* 压力值大了，就打开2号电磁阀泄气 */
		GPIO_ResetBits(macSolenoid_2_PORT, macSolenoid_2_PIN);
	}
	else
	{
		GPIO_SetBits(macSolenoid_2_PORT, macSolenoid_2_PIN);
	}

	/* VOFA+ WaterFire协议格式，用于PID参数整定 */
	printf("<any>:%f,%f,%f,%f\n", PressPID.Uk, PressPara.CurrentPress, PressPara.TargetPress, PressPID.ErrIntegral);

}
#endif /*ENABLE_INCREAMENTAL_PID*/









#if ENABLE_INCREMENTAL_PID

/* 增量式PID参数 */
PID_Incremental_StructType	PressPID;


/**
  * @brief  气压PID控制参数初始化
  * @param  None
  * @retval None
  */
void Press_PID_Init(void)
{
	PressPID.Kp = Press_Kp;
	PressPID.Ki = Press_Ki;
	PressPID.Kd = Press_Kd;
	PressPID.Ek2 = 0;
	PressPID.Ek1 = 0;
	PressPID.Ek = 0;
	PressPID.Uk1 = 0;
	PressPID.Uk = 0;

	PressPara.TargetPress = 3200;
}




/**
  * @brief  PID计算
  * @param  None
  * @retval u(k) = { Kp * [err(k) - err(k-1)] } + Ki * err(k) + { Kd * [err(k) - 2err(k-1) + err(k-2)] }
  * @retval 气压传感器的实际值是1000~4000的数值（100Kpa~400Kpa）
  * @retval WaterFire协议格式："<any>:ch0,ch1,ch2,...,chN\n"
  */
void PID_Calculate_Press(void)
{
	float P, I, D;

	/* 消除系统误差 */
	if (Record.PressDat > 4000)
	{
		Record.PressDat = 4000;
	}
	else if (Record.PressDat <= 1100)
	{
		Record.PressDat = 1100;
	}
	else
	{
		PressPara.CurrentPress = Record.PressDat;
	}

	/* 计算偏差 */
	PressPID.Ek = PressPara.TargetPress - PressPara.CurrentPress;  // 计算本次偏差
	

	P = PressPID.Kp * (PressPID.Ek - PressPID.Ek1);
	I = PressPID.Ki * (PressPID.Ek);
	D = PressPID.Kd * (PressPID.Ek - 2 * PressPID.Ek1 + PressPID.Ek2);

	if (PressPID.Ek > (20 * Press_Err_Gain) )	I = 0;		// 压差超过20Kpa，积分项不参与


	/* 更新 */
	PressPID.Uk  = PressPID.Uk1 + P + I + D;
	PressPID.Ek2 = PressPID.Ek1;								
	PressPID.Ek1 = PressPID.Ek;
	PressPID.Uk1 = PressPID.Uk;


	PressPara.CalculatePIDPress = PressPID.Uk + PressPara.CurrentPress;





	/* VOFA+ WaterFire协议格式，用于PID参数整定 */
	/* PID计算的气压值,当前实际的气压值，目标气压值，增量值 */
	printf("<any>:%f,%f,%f,%f\n", PressPara.CalculatePIDPress, PressPara.CurrentPress, PressPara.TargetPress,PressPID.Uk);

}
#endif /*ENABLE_INCREAMENTAL_PID*/
























