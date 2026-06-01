#include "sys.h"
#include "bsp.h"

u8		Rest_State=0;

void Check_Rst(void)
{
    if(RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET ) // NRST 引脚复位
    {
        Rest_State|=0x01;
    }
    if(RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET ) // 上电掉电复位
    {
        Rest_State|=0x02;
    }
    if(RCC_GetFlagStatus(RCC_FLAG_SFTRST) != RESET ) //  软件复位
    {
        Rest_State|=0x04;
    }
    if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET ) // 独立看门狗复位
    {
        Rest_State|=0x08;
    }
    if(RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET ) // 窗口看门狗复位
    {
        Rest_State|=0x10;
    }
    if(RCC_GetFlagStatus(RCC_FLAG_LPWRRST) != RESET ) // 低功耗复位
    {
        Rest_State|=0x20;
    }
    RCC_ClearFlag();			//清除复位标志
}




int main(void)
{
	u16	test;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
	Sys_Clock_Init();
    Gpio_init();	
	peripheral_init();
	// Check_Rst();
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	

	STMFLASH_Read(FLASH_SAVE_ADDR,&UpgradeParams.uiCommandFlag,1); 
		
	while(1)
	{
		if(UpgradeParams.uiCommandFlag==1)
		{
			UPG:	 
			 IAP_FirmwareUpgradeStep(USART2,&UpgradeParams.ucStep); 

		}else
		{
			__disable_irq();
			IAP_JumpToApplication();  /*****跳转到升级代码执行*******/
			UpgradeParams.uiCommandFlag=1;
			goto UPG;               /******没有升级程序或者升级程序错误才会执行到这句，然后跳转到升级界面*******/
		}
	}	
}


