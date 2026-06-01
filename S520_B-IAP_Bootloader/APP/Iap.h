
#ifndef __IAP_H__
#define __IAP_H__

#include "bsp.h"

#define FLASH_BASE_ADDR  (u32)0x08000000      //Flash基地址
#define IAP_ADDR         (u32)0x08004000      //升级代码地址
#define FLASH_SAVE_ADDR  (u32)0x0801FFF0 	 //升级标志位数据保存地址

#define IAP_PARAM_SIZE  4
#define IAP_PARAM_ADDR  (FLASH_BASE_ADDR + FLASH_SIZE - PAGE_SIZE) //Flash空间最后1页地址开始处存放参数

//////////////////////////////////////////////////////////////////////////////////////////////////////
//用户根据自己的需要设置
#define STM32_FLASH_SIZE 128 	 		//所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN 1                     //使能FLASH写入(0，不是能;1，使能)
//////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)
 #define PAGE_SIZE          (0x400)    //页的大小1K
 #define FLASH_SIZE         (0x20000)  //Flash空间128K
#elif defined STM32F10X_CL
 #define PAGE_SIZE          (0x400)    //页的大小2K
 #define FLASH_SIZE         (0x20000)  //Flash空间256K
#elif defined STM32F10X_HD || defined (STM32F10X_HD_VL)
 #define PAGE_SIZE          (0x800)    //页的大小2K
 #define FLASH_SIZE         (0x80000)  //Flash空间512K
#elif defined STM32F10X_XL
 #define PAGE_SIZE          (0x800)    //页的大小2K
 #define FLASH_SIZE         (0x100000) //Flash空间1M 
#else 
 #error "Please select first the STM32 device to be used (in stm32f10x.h)"    
#endif

typedef struct{
         u16   uiCommandFlag;           
         u8    ucStep;
		 u8    ucUart2ReceiveFlag;
         u8    ucUart3ReceiveFlag;
		 u8    UP_STEP;
}UpgradeParams_t;

#define UpgradeParamsDefaults  {0,\
	                            0,\
	                            0,\
	                            0}

extern UpgradeParams_t UpgradeParams; 

typedef enum{
	eStart = 0,
	eCmdID,
	eDeviceVerify,
	eMajorUpdateGrade,
    eMinorUpdateGrade
}eUpgradeState;

typedef enum{
   eUPG_Cmd = 0,
   eUPG_Reply,
   eUPG_Running
}eMinorComState;

void IAP_Init(USART_TypeDef* USARTx,u32 BaudRate);
void IAP_SerialSendByte(USART_TypeDef* USARTx,u8 c);
void IAP_SerialSendStr(USART_TypeDef* USARTx,u8 *s);
void  IAP_SerialSendData(USART_TypeDef* USARTx,u8 *buffer,u8 size);
u8 IAP_GetKey(void);
u8 IAP_SerialGetByte(USART_TypeDef* USARTx,u8 *c);
void IAP_JumpToApplication(void);
s8 IAP_UpdateParam(s32 *param);
s8 IAP_UpdateProgram(u32 addr, u32 size);
void IAP_ShowMenu(void);
void  Delay_ms(u16 ms);
void IAP_WaitForChoose(void);
void IAP_FlashEease(u32 size);
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   	
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);
void  IAP_FirmwareUpgradeStep(USART_TypeDef* USARTx,u8 *step);

void IAP_DisableFlashWPR(void);
void FLASH_WriteMoreData(u32 startAddress,u16 *writeData,u16 countToWrite);
u16 STMFLASH_ReadHalfWord(u32 faddr);
void IAP_SerialSendByte1(USART_TypeDef* USARTx,u8 *s,u8 len);
#endif
