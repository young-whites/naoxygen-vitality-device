#include "bsp_flash.h" 



/**
  * @brief  读取指定地址的半字(16位数据)
  * @param  faddr : 读地址,此地址必须为2的倍数
  * @retval 地址下的对应数据
  */
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr;
}




/**
  * @brief  读取指定地址的一个字(16位数据)
  * @param  faddr : 读地址,此地址必须为2的倍数
  * @retval 地址下的对应数据
  */
u32 STMFLASH_ReadProgramWord(u32 faddr)
{
	return *(vu32*)faddr;
}






/**
  * @brief  不进行校验的写数据操作
  * @param  WriteAddr		:起始地址
  *			pBuffer			:数组指针
  *			NumToWrite		:半字(8位)数   
  * @retval None
  */
void STMFLASH_Write_OptionByteData(u32 WriteAddr, u16* pBuffer, u16 NumToWrite)
{
	u16 i;

	for (i = 0; i < NumToWrite; i++)
	{
		FLASH_ProgramOptionByteData(WriteAddr, pBuffer[i]);
		WriteAddr += 2;//地址增加2
	}
}





/**
  * @brief  不进行校验的写数据操作
  * @param  WriteAddr		:起始地址
  *			pBuffer			:数组指针
  *			NumToWrite		:半字(16位)数据
  * @retval None
  */
void STMFLASH_Write_NoCheck(u32 WriteAddr, u16* pBuffer, u16 NumToWrite)
{
	u16 i;

	for (i = 0; i < NumToWrite; i++)
	{
		FLASH_ProgramHalfWord(WriteAddr, pBuffer[i]);
		WriteAddr += 2;//地址增加2.
	}
}



/**
  * @brief  不进行校验的写数据操作
  * @param  WriteAddr		:起始地址
  *			pBuffer			:数组指针
  *			NumToWrite		:全字(32位)数
  * @retval None
  */
void STMFLASH_Write_ProgramWord(u32 WriteAddr, u32* pBuffer, u16 NumToWrite)
{
	u16 i;
	FLASH_Unlock();	//解锁
	for (i = 0; i < NumToWrite; i++)
	{
		FLASH_ProgramWord(WriteAddr, pBuffer[i]);
		WriteAddr += 4;//地址增加2
	}
	FLASH_Lock();//上锁
}




/**
  * @brief  从指定地址开始写入指定长度的数据
  * @param  WriteAddr		:起始地址
  *			pBuffer			:数组指针
  *			NumToWrite		:半字(16位)数(就是要写入的16位数据的个数)
  * @retval None
  */
u16 STMFLASH_BUF[STM_SECTOR_SIZE / 2];//最多是2K字节
u32 STMFLASH_BUF_32[STM_SECTOR_SIZE / 4];//最多是2K字节
void STMFLASH_Write(u32 WriteAddr, u16* pBuffer, u16 NumToWrite)
{
	u32 secpos;	   //扇区地址
	u16 secoff;	   //扇区内偏移地址(16位字计算)
	u16 secremain; //扇区内剩余地址(16位字计算)	   
	u16 i;
	u32 offaddr;   //去掉0X08000000后的地址

	if (WriteAddr < 0x08000000 || (WriteAddr >= (0x08000000 + 1024 * STM32_FLASH_SIZE)))return;//非法地址

	FLASH_Unlock();						//解锁

	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清除flash相关标志位

	offaddr = WriteAddr - 0x08000000;		//实际偏移地址.
	secpos = offaddr / STM_SECTOR_SIZE;			//扇区地址  0~127 for STM32F103RBT6
	secoff = (offaddr % STM_SECTOR_SIZE) / 2;		//在扇区内的偏移(2个字节为基本单位.)
	secremain = STM_SECTOR_SIZE / 2 - secoff;		//扇区剩余空间大小   
	if (NumToWrite <= secremain)secremain = NumToWrite;//不大于该扇区范围

	while (1)
	{
		STMFLASH_Read(secpos * STM_SECTOR_SIZE + 0x08000000, STMFLASH_BUF, STM_SECTOR_SIZE / 2);//读出整个扇区的内容
		for (i = 0; i < secremain; i++)//校验数据
		{
			if (STMFLASH_BUF[secoff + i] != 0XFFFF)break;//需要擦除  	  
		}

		if (i < secremain)//需要擦除
		{
			FLASH_ErasePage(secpos * STM_SECTOR_SIZE + 0x08000000);//擦除这个扇区
			for (i = 0; i < secremain; i++)//复制
			{
				STMFLASH_BUF[i + secoff] = pBuffer[i];
			}
			STMFLASH_Write_NoCheck(secpos * STM_SECTOR_SIZE + 0x08000000, STMFLASH_BUF, STM_SECTOR_SIZE / 2);//写入整个扇区

		}
		else STMFLASH_Write_NoCheck(WriteAddr, pBuffer, secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if (NumToWrite == secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;				//扇区地址增1
			secoff = 0;				//偏移位置为0 	 
			pBuffer += secremain;  	//指针偏移
			WriteAddr += secremain;	//写地址偏移	   
			NumToWrite -= secremain;	//字节(16位)数递减
			if (NumToWrite > (STM_SECTOR_SIZE / 2))secremain = STM_SECTOR_SIZE / 2;//下一个扇区还是写不完
			else secremain = NumToWrite;//下一个扇区可以写完了
		}
	};
	FLASH_Lock();//上锁
}




/**
  * @brief  从指定地址开始读取指定长度的数据
  * @param  WriteAddr		:起始地址
  *			pBuffer			:数组指针
  *			NumToRead		:半字(16位)数(就是要写入的16位数据的个数)
  * @retval None
  */
void STMFLASH_Read(u32 ReadAddr, u16* pBuffer, u16 NumToRead)
{
	u16 i;
	for (i = 0; i < NumToRead; i++)
	{
		pBuffer[i] = STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.
		ReadAddr += 2;//偏移2个字节
	}
}




/**
  * @brief 测试写入一个字的函数
  * @param  WriteAddr		:起始地址
  *			WriteData		:要写入的数据
  * @retval None
  */
void Test_Write(u32 WriteAddr, u16 WriteData)
{
	STMFLASH_Write(WriteAddr, &WriteData, 1);//写入一个字 
}


















