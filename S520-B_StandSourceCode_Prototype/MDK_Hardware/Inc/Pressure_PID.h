#ifndef __PRESSURE_PID_H
#define __PRESSURE_PID_H
#include "sys.h"


#define ENABLE_POSITION_TYPE_PID		0		// 位置式PID算法
#define ENABLE_INCREMENTAL_PID			0		// 增量式PID算法




#if ENABLE_POSITION_TYPE_PID

typedef struct
{
	float	Kp;			/* 比例系数 */
	float   Ki;			/* 积分系数 */
	float   Kd;			/* 微分系数 */
	float   Ek;			/* 当前偏差值 */
	float   Ek1;		/* 上一次的偏差值 */
	float   ErrIntegral;/* 误差积分值 */
	float   Uk;			/* 当前输出值 */
}  PID_Position_StructType;
extern   PID_Position_StructType PressPID;
#endif /*ENABLE_POSITION_TYPE_PID*/










#if ENABLE_INCREMENTAL_PID
typedef struct
{
	float	Kp;			/* 比例系数 */
	float   Ki;			/* 积分系数 */
	float   Kd;			/* 微分系数 */
	float   Ek;			/* 当前偏差值 */
	float	Ek1;		/* 上次偏差值 */
	float	Ek2;		/* 上上次偏差 */
	float   Uk;			/* 当前输出的增量值 */
	float	Uk1;		/* 上次输出的增量值 */
}  PID_Incremental_StructType;
extern   PID_Incremental_StructType PressPID;
#endif /*ENABLE_INCREMENTAL_PID*/






typedef struct {
	float	CurrentPress;						// 当前气压值
	float	TargetPress;						// 目标气压值
	float	MaxPress;							// 设置上限气压幅值
	float	MiniPress;							// 设置下限气压幅值
	float   ErrPress;							// 允许最大误差
	float	CalculatePIDPress;					// 计算出的PID调节值
}PRESSVALUE_StructTypeDef;
extern   PRESSVALUE_StructTypeDef PressPara;




/* 宏定义初始化PID系数的值 */
#define		Press_Kp			(0.01)							// 气压控制 Kp 系数（放大1倍）
#define		Press_Ki			(0.001)							// 气压控制 Ki 系数（放大1倍）
#define		Press_Kd			(0.1)							// 气压控制 Kd 系数（放大1倍）
#define		Press_Err_Gain		(10)						// 偏差放大倍数





void Press_PID_Init(void);
void PID_Calculate_Press(void);



#endif /*__PRESSURE_PID_H*/
