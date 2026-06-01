#ifndef __SYS_H
#define __SYS_H	
#include "stm32f10x.h"

/* 标准库头文件 */
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>


/* 自定义头文件 */
#include "bsp_led.h"
#include "usart.h"
#include "core_delay.h"
#include "systick_delay.h"
#include "bsp_beep.h" 
#include "TimingSet.h"
#include "GeneralTIM.h"
#include "Bluetooth.h"
#include "bsp_beep.h"
#include "MyTypedef.h"
#include "bsp_Key.h"
#include "Compressor.h"
#include "bsp_Fan.h"
#include "softiic.h"
#include "Pressure_PID.h"
#include "sys_status.h"
#include "Tk1300F.h"
#include "oldTest.h"
#include "fire_usart.h"
#include "iap_upgrade.h"
#include "iwdg.h"
#include "bsp_flash.h" 
#include "bsp_hard.h"
#include "Usart2_Decode.h"
#include "Usart4_Decode.h"

/* 自定义全局宏 */
#define			TEST                             1   // 全局测试用例宏
#define			TEST_PRINTF_PRESSURE			 0   // 用于WaterFire协议打印气压数据的宏



void ALL_Config(void);

																	    
	
	
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 


#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n) 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n) 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n) 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n) 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  


void WFI_SET(void);		
void INTX_DISABLE(void);
void INTX_ENABLE(void);	
void MSR_MSP(u32 addr);	

#endif
