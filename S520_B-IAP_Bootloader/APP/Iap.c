
#include "config.h"

UpgradeParams_t  UpgradeParams = UpgradeParamsDefaults; 

/*************************************************************
  Function   : IAP_Init 
  Description : IAP初始化函数，及对应波特率
  Input      : 串口号，波特率        
  return     : none    
*************************************************************/
void IAP_Init(USART_TypeDef* USARTx,u32 BaudRate)
{
	USART_InitTypeDef USART_InitStructure;
	
    USART_InitStructure.USART_BaudRate = BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    /* Configure USARTz */
    USART_Init(USARTx, &USART_InitStructure);
    /* Enable USARTz Receive and Transmit interrupts */
    USART_ITConfig(USARTx, USART_IT_RXNE, DISABLE);
    /* Enable the USART */
    USART_Cmd(USARTx, ENABLE);
}

/*************************************************************
  Function   : IAP_SerialSendByte 
  Description : 串口发送字节
  Input      : 串口号，c-要发送的字节        
  return     : none    
*************************************************************/
void IAP_SerialSendByte(USART_TypeDef* USARTx,u8 c)
{
	
	USART_SendData(USARTx, c);
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET) 
	{
	}
}

void IAP_SerialSendByte1(USART_TypeDef* USARTx,u8 *s,u8 len)
{
	u8 i =0;
	for(i = 0;i<len;i++)
	{
	USART_SendData(USARTx, s[i]);
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET) {}
	}
}
/*************************************************************
  Function   : IAP_SerialSendStr 
  Description : 串口发送字符串
  Input      : 串口号及发送的字符串        
  return     : none    
*************************************************************/
void IAP_SerialSendStr(USART_TypeDef* USARTx,u8 *s)
{
	while(*s != '\0')
	{
        IAP_SerialSendByte(USARTx,*s);
		s++;
	}
}

/*************************************************************
  Function   : IAP_SerialSendData 
  Description : 串口发送数据
  Input      : 串口号及发送的数据和数据长度        
  return     : none    
*************************************************************/
void  IAP_SerialSendData(USART_TypeDef* USARTx,u8 *buffer,u8 size)
{
    u8 i = 0;
	
    for(i=0;i<size;i++)
    {
        /* Write a character to the USART */
        USART_SendData(USARTx, * buffer);
        /* Loop until the end of transmission */
       while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
	   buffer++;
    }  
}

/*************************************************************
  Function   : IAP_SerialGetByte 
  Description : 接收一个字节数据
  Input      : 串口号，接收的字符串        
  return     : 返回结果值，0-没有接收到数据；1-接收到数据    
*************************************************************/
u8 IAP_SerialGetByte(USART_TypeDef* USARTx,u8 *c)
{
	if(USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) != RESET)
	{
		*c = USART_ReceiveData(USARTx);
		return 1;
	}  
	return 0;
}

/*************************************************************
  Function   : IAP_DisableFlashWPR 
  Description : 关闭flash的写保护
  Input      : none        
  return     : none    
*************************************************************/
void IAP_DisableFlashWPR(void)
{
	u32 blockNum = 0, UserMemoryMask = 0;

    blockNum = (IAP_ADDR - FLASH_BASE_ADDR) >> 12;   //计算flash块
	UserMemoryMask = ((u32)(~((1 << blockNum) - 1)));//计算掩码

	if((FLASH_GetWriteProtectionOptionByte() & UserMemoryMask) != UserMemoryMask)//查看块所在区域是否写保护
	{
		FLASH_EraseOptionBytes ();//擦除选择位
	}
}

/*************************************************************
  Function   : IAP_UpdateParam(s32 *param) 
  Description : 更新参数 
  Input      : none        
  return     : none    
*************************************************************/
s8 IAP_UpdateParam(s32 *param)
{
	u32 i;
	u32 flashptr = IAP_PARAM_ADDR;

	FLASH_Unlock();//flash解锁
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清除flash相关标志位
	for(i = 0; i < IAP_PARAM_SIZE; i++)
	{
		FLASH_ProgramWord(flashptr + 4 * i, *param);
		if(*(u32 *)(flashptr + 4 * i) != *param)
		{
			return -1;
		}	
		param++;
	}
	FLASH_Lock();//flash上锁
	return 0;
}

/*************************************************************
  Function   : IAP_UpdateProgram 
  Description : 升级程序
  Input      : addr-烧写的地址 size-大小        
  return     : 0-OK 1-error    
*************************************************************/
	u32 ii=0;
	// static u32 flashptr = IAP_ADDR;
    u32 flashptr = IAP_ADDR;
	u32 flashptr1;
	u32 flashptr2;
s8 IAP_UpdateProgram(u32 addr, u32 size)
{


	FLASH_Unlock();//flash解锁
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清除flash相关标志位
	for(ii = 0; ii < size; ii += 4)
	{	
		FLASH_ProgramWord(flashptr, *(u32 *)addr);//烧写1个字
		flashptr1 =  *(u32 *)addr;
		flashptr2 =  *(u32 *)flashptr;
		if(*(u32 *)flashptr != *(u32 *)addr)//判断是否烧写成功
		{
			FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清除flash相关标志位
			FLASH_ErasePage(flashptr);
			FLASH_ProgramWord(flashptr,flashptr1);//烧写1个字
			if(*(u32 *)flashptr != flashptr1)//判断是否烧写成功
			{
			return -1;
			}
		}
		flashptr += 4;
		addr += 4;
	}
	FLASH_Lock();//flash上锁
	return 0;
}
/*************************************************************
  Function   : IAP_FlashEease 
  Description : 擦除Flash
  Input      : size-擦除的大小        
  return     : none    
*************************************************************/
void IAP_FlashEease(u32 size)
{
	u16 eraseCounter = 0;
	u16 nbrOfPage = 0;
	FLASH_Status FLASHStatus = FLASH_COMPLETE;	  

	if(size % PAGE_SIZE != 0)//计算需要擦写的页数
	{										  
		nbrOfPage = size / PAGE_SIZE + 1; 
	}
	else
	{
		nbrOfPage = size / PAGE_SIZE;
	}

	FLASH_Unlock();//解除flash擦写锁定
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清除flash相关标志位
	for(eraseCounter = 0; (eraseCounter < nbrOfPage) && ((FLASHStatus == FLASH_COMPLETE)); eraseCounter++)//开始擦除
	{
		FLASHStatus = FLASH_ErasePage(IAP_ADDR + (eraseCounter * PAGE_SIZE));//擦除
	}
	FLASH_ErasePage(IAP_PARAM_ADDR);//擦除参数所在的flash页
	FLASH_Lock();//flash擦写锁定
}


typedef  void (*pFunction)(void); 
pFunction Jump_To_Application;

/*************************************************************
  Function   : IAP_JumpToApplication 
  Description : 跳转到升级程序处
  Input      : none        
  return     : none    
*************************************************************/
void IAP_JumpToApplication(void)
{
	u32 JumpAddress;//跳转地址
    u16 upg_flag = 1;
	if(((*(__IO u32 *)IAP_ADDR) & 0x2FFE0000) == 0x20000000)//有升级代码，IAP_ADDR地址处理应指向主堆栈区，即0x20000000
	{      
		JumpAddress = *(__IO u32 *)(IAP_ADDR + 4);//获取复位地址	
		Jump_To_Application = (pFunction)JumpAddress;//函数指针指向复位地址
		__set_MSP(*(__IO u32*)IAP_ADDR);//设置主堆栈指针MSP指向升级机制IAP_ADDR
		Jump_To_Application();//跳转到升级代码处
	}
	else {
        IAP_DisableFlashWPR();  //关闭写保护
		upg_flag = 1;
	    STMFLASH_Write(FLASH_SAVE_ADDR,&upg_flag,1);
	}
}

/*************************************************************
  Function   : Delay_ms 
  Description : 延时函数
  Input      : none        
  return     : none    
*************************************************************/
void  Delay_ms(u16 ms)
{
     int a,b;
	 
	 for(a=ms;a>0;a--)
	 {
        for(b=8000;b>0;b--);
	 }
}

/*---------------------------------------------------------------------------
* 函数名  : STMFLASH_ReadHalfWord(u32 faddr)
* 描述    : 读取指定地址的半字(16位数据)
* 输入    : faddr:读地址(此地址必须为2的倍数!!)
* 输出    : 无
* 返回    : 返回值:对应数据.
---------------------------------------------------------------------------*/
u16 STMFLASH_ReadHalfWord(u32 faddr)
{ 
    return *(vu16*)faddr; 
}

#if STM32_FLASH_WREN	//如果使能了写   

/*---------------------------------------------------------------------------
* 函数名  : STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)  
* 描述    : 不检查的写入
* 输入    :WriteAddr:起始地址      pBuffer:数据指针     NumToWrite:半字(16位)数   
* 输出    : 无
* 返回    : 返回值:对应数据.
---------------------------------------------------------------------------*/ 
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
    u16 i;
    for(i=0;i<NumToWrite;i++)
    {
        FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
        WriteAddr+=2;//地址增加2.
    }  
} 

#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //字节
#else 
#define STM_SECTOR_SIZE	2048
#endif		 

u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//最多是2K字节

/*---------------------------------------------------------------------------
* 函数名  : STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	  
* 描述    : 从指定地址开始写入指定长度的数据
* 输入    :WriteAddr:起始地址(此地址必须为2的倍数!!)               pBuffer:数据指针     NumToWrite:半字(16位)数(就是要写入的16位数据的个数.)
* 输出    : 无
* 返回    : 无
---------------------------------------------------------------------------*/
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
	u32 secpos;	   //扇区地址
	u16 secoff;	   //扇区内偏移地址(16位字计算)
	u16 secremain; //扇区内剩余地址(16位字计算)	   
 	u16 i;    
	u32 offaddr;   //去掉0X08000000后的地址
	
	if(WriteAddr<FLASH_BASE_ADDR||(WriteAddr>=(FLASH_BASE_ADDR+1024*STM32_FLASH_SIZE)))return;//非法地址
	
	FLASH_Unlock();						//解锁
	
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清除flash相关标志位
	
	offaddr=WriteAddr-FLASH_BASE_ADDR;		//实际偏移地址.
	secpos=offaddr/STM_SECTOR_SIZE;			//扇区地址  0~127 for STM32F103RBT6
	secoff=(offaddr%STM_SECTOR_SIZE)/2;		//在扇区内的偏移(2个字节为基本单位.)
	secremain=STM_SECTOR_SIZE/2-secoff;		//扇区剩余空间大小   
	if(NumToWrite<=secremain)secremain=NumToWrite;//不大于该扇区范围
	
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+FLASH_BASE_ADDR,STMFLASH_BUF,STM_SECTOR_SIZE/2);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
	        if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//需要擦除  	  
		}
		
		if(i<secremain)//需要擦除
		{
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE+FLASH_BASE_ADDR);//擦除这个扇区
			for(i=0;i<secremain;i++)//复制
			{
			    STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+FLASH_BASE_ADDR,STMFLASH_BUF,STM_SECTOR_SIZE/2);//写入整个扇区
			
		}else STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;				//扇区地址增1
			secoff=0;				//偏移位置为0 	 
		   	pBuffer+=secremain;  	//指针偏移
			WriteAddr+=secremain;	//写地址偏移	   
		   	NumToWrite-=secremain;	//字节(16位)数递减
			if(NumToWrite>(STM_SECTOR_SIZE/2))secremain=STM_SECTOR_SIZE/2;//下一个扇区还是写不完
			else secremain=NumToWrite;//下一个扇区可以写完了
		}	 
	};	
	FLASH_Lock();//上锁
}
#endif

/*---------------------------------------------------------------------------
* 函数名  : STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)  
* 描述    : 从指定地址开始读出指定长度的数据
* 输入    : ReadAddr:起始地址      pBuffer:数据指针 16位数个数
* 输出    : 无
* 返回    : 无
---------------------------------------------------------------------------*/
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.
		ReadAddr+=2;//偏移2个字节.	
	}
}

/*******************************************************************************
* Function Name  : IAP_Firmware_Upgrade_Step(USART_TypeDef* USARTx,u8 *step)
* Description	 : 固件升级步骤
* Input 		 : 升级串口 ，升级步骤
* Output		 : None
* Return		 : None
*******************************************************************************/
void  IAP_FirmwareUpgradeStep(USART_TypeDef* USARTx,u8 *step)
{
	 u8  i,j,k = 0;
	 u8  major[5] = {'M','A','J','O','R'};
	 u8  minor[5] = {'M','I','N','O','R'};
	 u8  minor_step = 0;
     
	 static u8  file_cmd[5] = {0,0,0,0,0};
     
	 switch(*step)
     {
        case eStart:		
	        *step = eCmdID;
			//  IAP_SerialSendStr(USART3,"\r\n start firemware update grade.\r\n");
			YModem_SendByte(USARTx,ACK);
		break;
		case eCmdID:
			do
			{
                if(YModem_RecvByte(USARTx,file_cmd,NAK_TIMEOUT) == 0)//接收数据包的第一个字节
				{
				   if(file_cmd[0]=='M')
				   {
                        for(i=1;i<5;i++)
                       	{
							if(YModem_RecvByte(USARTx,file_cmd+i,NAK_TIMEOUT) != 0)
							{
								//GPIO_SetBits(GPIOB,GPIO_Pin_1);
			                    //  IAP_SerialSendStr(USART3,"\r\n app command receive error.\r\n");
								break;
							}
						}						
                      *step = eDeviceVerify;
					//    IAP_SerialSendStr(USART3,"\r\n app command receive complete.\r\n");	
					   break;
				   	}
                }
				else
				{
					
                    //   IAP_SerialSendStr(USART3,"\r\n waiting app command \r\n");	
				      IAP_SerialSendStr(USARTx,"UPGR");  /*****提醒APP或上位机需要升级****/
				}
			}
			while (1);
		break;
		case eDeviceVerify:
			for(i=0;i<5;i++)
			{
			   if(file_cmd[i] == major[i])
			   {
	              j++; 
			   }
			   
			   if(file_cmd[i] == minor[i])
			   {
	              k++;
			   }
			}
			
			if(j == 5)
			{
	           *step = eMajorUpdateGrade;
			    //  IAP_SerialSendStr(USART3,"\r\n app command receive correct into major board update grade.\r\n");
			    // YModem_SendByte(USARTx,ACK);
				YModem_SendByte(USARTx,MAJOR);
			}
			else if(k == 5)
			{
	           *step = eMinorUpdateGrade;
			    // IAP_SerialSendStr(USART3,"\r\n app command receive correct into minor board update grade.\r\n");
			}
			else 
			{
               *step = eCmdID;
			//    IAP_SerialSendStr(USART3,"\r\n app command receive error.\r\n"); 
			}
		break;
		case eMajorUpdateGrade: 
		    //  IAP_SerialSendStr(USART3,"\r\n Ymode step1 \r\n");
		
		    DownloadFirmware(MAJOR_DEVICE,USARTx);  //开始升级
				*step = eStart;
				STMFLASH_Read(FLASH_SAVE_ADDR,&UpgradeParams.uiCommandFlag,1); 
		    NVIC_SystemReset();//重启	
		break;
		case eMinorUpdateGrade:
		do
		{
			switch(minor_step)	
			{
               case eUPG_Cmd:
			//    IAP_SerialSendStr(USART3,"\r\n major board send update grade command to minor board \r\n");
			   minor_step = eUPG_Reply;
			//    IAP_SerialSendStr(USART1,"UPGR");
			//    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

			    // __enable_irq(); 
			   UpgradeParams.ucUart2ReceiveFlag = 0;
			   UpgradeParams.ucUart3ReceiveFlag = 0;
			   Delay_ms(3000);
			   break;
			   case eUPG_Reply:
               if(UpgradeParams.ucUart2ReceiveFlag)
               {
                //    IAP_SerialSendStr(USART1,":"); 
			   }
               else
               {
                //   IAP_SerialSendStr(USART3,"\r\n hand shank a mabay not connected \r\n"); 
			   }
			   
			   if(UpgradeParams.ucUart3ReceiveFlag)
               {
                //    IAP_SerialSendStr(USART1,":"); 
			   }
               else
               {
                //    IAP_SerialSendStr(USART3,"\r\n hand shank b mabay not connected \r\n"); 
			   }
			//    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
			//     __disable_irq(); 
			   if((UpgradeParams.ucUart2ReceiveFlag == 1)||(UpgradeParams.ucUart3ReceiveFlag == 1))
			   {
                  minor_step = eUPG_Running;
				  YModem_SendByte(USARTx,ACK);
			   }
			   else
			   {
				  YModem_SendByte(USARTx,NAK);
				  NVIC_SystemReset();//重启	
			   }
			   
			   break;
			   case eUPG_Running:
			   DownloadFirmware(MINOR_DEVICE,USARTx);
		       NVIC_SystemReset();//重启	
			   break;
			   default:break;
			}
	    }
		while(1);
		break;
		default : break;
	 }
         IWDG_Feed();
}


