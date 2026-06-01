
#include "config.h"
#include "Ymodem.h"
#include "IAP.h"
#include "Download.h"

u8 tab_1024[1024] = {0};

/*************************************************************
  Function   : MAJOR_DownloadFirmware
  Description : 主板下载升级固件
  Input      : none
  return     : none
*************************************************************/
void DownloadFirmware(u8          device_id,USART_TypeDef* USARTx)
{
    u8 number[10]= "          ";        //文件的大小字符串
    s32 size = 0;
	u16 upg_flag = 0;
	

	size = YModem_Receive(device_id,USARTx,&tab_1024[0]);//开始接收升级程序
	
    if(device_id == MAJOR_DEVICE)
    {
        if(size<=0)     //数据传输异常,擦除已经写入数据
	    {
	       IAP_FlashEease(FLASH_SIZE + FLASH_BASE_ADDR - IAP_ADDR);//擦除Flash，升级处后面的flash空间
	    } 	
    }
	
	if(size > 0)
    {
        IAP_DisableFlashWPR();  //关闭写保护
		upg_flag = 0;
	    STMFLASH_Write(FLASH_SAVE_ADDR,&upg_flag,1);
		//upg_flag = STMFLASH_ReadHalfWord(FLASH_SAVE_ADDR);
		UpgradeParams.UP_STEP = 0;
		if(upg_flag == 0x8080)
		{
        //    IAP_SerialSendStr(USART3,"\r\n update grade flag wirte failure!\r\n");
		}
		else
		{
        //    IAP_SerialSendStr(USART3,"\r\n update grade flag wirte successfully!\r\n");
		}
		
		// IAP_SerialSendStr(USART3,"+-----------------------------------+\r\n");
        // IAP_SerialSendStr(USART3,"Proramming completed successfully!\r\nName: ");
        // IAP_SerialSendStr(USART3,file_name);//显示文件名

		if(size > 1)
        {
            YModem_Int2Str(number, size);
            // IAP_SerialSendStr(USART3,"\r\nSize: ");
            // IAP_SerialSendStr(USART3,number);//显示文件大小
            // IAP_SerialSendStr(USART3,"Bytes\r\n");
            UpgradeParams.UP_STEP = 1;
        }
        else
        {
            // IAP_SerialSendStr(USART3,"\r\nYmodem not transmit file size\r\n");
        }
        // IAP_SerialSendStr(USART3,"+-----------------------------------+\r\n");
    }
    else if(size == -1)//固件的大小超出处理器的flash空间
    {
        UpgradeParams.UP_STEP = 2;// IAP_SerialSendStr(USART3,"The image size is higher than the allowed space memory!\r\n");
    }
    else if(size == -2)//程序烧写不成功
    {
        UpgradeParams.UP_STEP = 3;// IAP_SerialSendStr(USART3,"Verification failed!\r\n");
    }
    else if(size == -3)//用户终止
    {
        UpgradeParams.UP_STEP = 4;// IAP_SerialSendStr(USART3,"Aborted by user!\r\n");
    }
    else if(size == -4) //CRC校验失败
    {
        UpgradeParams.UP_STEP = 5;// IAP_SerialSendStr(USART3,"CRC Check Failed!\r\n");
    }
    else if(size == -5) //序号和补码错误
    {
        UpgradeParams.UP_STEP = 6;// IAP_SerialSendStr(USART3,"The serial number complement failed!\r\n");
    }
    else if(size == -6) //文件名称错误
    {
        UpgradeParams.UP_STEP = 7;// IAP_SerialSendStr(USART3,"The file name failed!\r\n");
    }
    else //其他错误
    {
        UpgradeParams.UP_STEP = 8;// IAP_SerialSendStr(USART3,"Failed to receive the file!\r\n");
    }

}














