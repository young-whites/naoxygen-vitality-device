/*
 * bsp_led.h
 *
 *  Created on: May 16, 2024
 *      Author: zphu
 */

#ifndef MACBSP_INC_BSP_LED_H_

#define MACBSP_INC_BSP_LED_H_
#include <applications/macSYS/Inc/macSYS.h>

#define             LED_NUM             (4)
/****************************** LED 函数宏定义***************************************/
#define             macLED_WHITE_OFF()                          HAL_GPIO_WritePin ( LED_White_GPIO_Port, LED_White_Pin , GPIO_PIN_RESET )
#define             macLED_WHITE_ON()                           HAL_GPIO_WritePin ( LED_White_GPIO_Port, LED_White_Pin , GPIO_PIN_SET )

#define             macLED_ORANGE_OFF()                         HAL_GPIO_WritePin ( LED_Orange_GPIO_Port, LED_Orange_Pin , GPIO_PIN_RESET)
#define             macLED_ORANGE_ON()                          HAL_GPIO_WritePin ( LED_Orange_GPIO_Port, LED_Orange_Pin , GPIO_PIN_SET )

// 在设备上对应方块的灯
#define             macLED_MESSAGE_OFF()                        HAL_GPIO_WritePin ( LED_Status_GPIO_Port, LED_Status_Pin , GPIO_PIN_RESET)
#define             macLED_MESSAGE_ON()                         HAL_GPIO_WritePin ( LED_Status_GPIO_Port, LED_Status_Pin , GPIO_PIN_SET )

// 在设备上对应圆圈的灯
#define             macLED_STATUS_OFF()                         HAL_GPIO_WritePin ( LED_Message_GPIO_Port, LED_Message_Pin , GPIO_PIN_RESET)
#define             macLED_STATUS_ON()                          HAL_GPIO_WritePin ( LED_Message_GPIO_Port, LED_Message_Pin , GPIO_PIN_SET )

#define             macLED_4G_OFF()                             HAL_GPIO_WritePin ( LED_4G_GPIO_Port, LED_4G_Pin , GPIO_PIN_RESET)
#define             macLED_4G_ON()                              HAL_GPIO_WritePin ( LED_4G_GPIO_Port, LED_4G_Pin , GPIO_PIN_SET )

/*LED名称类型*/
typedef	enum
{
    LED_Name_Orange = (0x01),
	LED_Name_White,
	LED_Name_Status,
	LED_Name_Message,
	LED_Name_4G,
}LED_Name_TypeDef;



/************************** PAD 函数声明********************************/
void LED_Init(void);
void LED_Out(int8_t ledName, int8_t ledState);
int8_t 	LED_GetNumber(void);
void LED_Off(int8_t ledName);
void LED_On(int8_t ledName);
void LED_Toggle(int8_t ledName);
void LED_Grad(int8_t ledName);
void LED_BlinkSetCycleDuty(int8_t ledName, int8_t Cycle, int8_t Duty);
void LED_Blink(int8_t ledName, int8_t cry, int8_t mute, int8_t repeat);
void LED_Fancy(int8_t mode);
void LED_DrvScan(void);

#endif /* MACBSP_INC_BSP_LED_H_ */
