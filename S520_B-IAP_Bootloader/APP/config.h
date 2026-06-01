#ifndef _CONFIG_H
#define _CONFIG_H

#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include <string.h>

#include "IAP.h"
#include "Ymodem.h"
#include "Download.h"

#define MAJOR_MCU  "MAJOR.bin"
#define MINOR_MCU  "MINOR.bin"

#define  UPG_INDICATE_LED_PIN     GPIO_Pin_1
#define  UPG_INDICATE_LED_PORT    GPIOA

#define  USART1_EN   1
#define  USART2_EN   1
#define  USART3_EN   1
#define  UART4_EN    0

void IWDG_Feed(void);

#endif
