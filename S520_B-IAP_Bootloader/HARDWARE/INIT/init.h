#ifndef __INIT_H
#define __INIT_H
#include"sys.h"
#include "bsp.h"

void Sys_Clock_Init(void);
void NVIC_Configuration(void);
void IWDG_Init(u8 prer, u16 rlr);
void  peripheral_init(void);
void IWDG_Feed(void);
void    Power_OnOff(u8  state);

#endif
