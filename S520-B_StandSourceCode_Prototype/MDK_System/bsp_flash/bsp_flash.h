#ifndef __BSP_FLASH_H
#define __BSP_FLASH_H
#include "sys.h"



#define STM32_FLASH_SIZE    128 	 		//ROM SIZE
#define STM32_FLASH_BASE    0x08000000 	    //STM32 FLASH    STARTARR

#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //×Ö½Ú
#else 
#define STM_SECTOR_SIZE	2048
#endif	

u16  STMFLASH_ReadHalfWord(u32 faddr);
u32  STMFLASH_ReadProgramWord(u32 faddr);
void STMFLASH_Write_OptionByteData(u32 WriteAddr, u16* pBuffer, u16 NumToWrite);
void STMFLASH_Write_NoCheck(u32 WriteAddr, u16* pBuffer, u16 NumToWrite);
void STMFLASH_Write_ProgramWord(u32 WriteAddr, u32* pBuffer, u16 NumToWrite);
void STMFLASH_Write(u32 WriteAddr, u16* pBuffer, u16 NumToWrite);
void STMFLASH_Read(u32 ReadAddr, u16* pBuffer, u16 NumToRead);
void Test_Write(u32 WriteAddr, u16 WriteData);










#endif /*__BSP_FLASH_H*/