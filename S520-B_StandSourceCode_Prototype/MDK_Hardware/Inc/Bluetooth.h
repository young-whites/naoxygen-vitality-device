#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H
#include "stm32f10x.h"
#include "sys.h"




/******************** BT_EN 蓝牙电源使能引脚配置参数定义 **************************/
#define             macBluetooth_EN_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macBluetooth_EN_GPIO_CLK                        RCC_APB2Periph_GPIOB
#define             macBluetooth_EN_PORT                            GPIOB
#define             macBluetooth_EN_PIN                             GPIO_Pin_0

/******************** BT_STA 蓝牙状态引脚配置参数定义 **************************/
#define             macBluetooth_STA_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macBluetooth_STA_GPIO_CLK                        RCC_APB2Periph_GPIOC
#define             macBluetooth_STA_PORT                            GPIOC
#define             macBluetooth_STA_PIN                             GPIO_Pin_5



void Bluetooth_GPIO_Config ( void );
int8_t BLUETOOTH_GetState(void);
void BLUETOOTH_Scan(void);
int8_t BLUETOOTH_GetConnect(void);
void BLUETOOTH_SET_EN(FunctionalState status);


#endif /*__BLUETOOTH_H*/