#include "iap_upgrade.h"

/*******************************************/
/*          IAP  --  IROM空间分配          */
/*-----------------------------------------*/
/*	
 *  
 *  
 *  
 *  
 */

iapfun jumpapp;


void jump_sys_bootloader(void)
{
    uint32_t i = 0;

    /* 声明一个函数指针 */
    void (*p_sys_bootloader_run)(void);

    /* STM32F1030RBT6 的系统 BootLoader 地址, 来自手册 */
    __IO uint32_t boot_address = 0x08000000;

    /* 关闭全局中断 */
    __disable_irq();

    /* 关闭滴答定时器，复位到默认值 */
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

    /* 设置所有时钟到默认状态，使用HSI时钟 */
    RCC_DeInit();

    /* 关闭所有中断，清除所有中断挂起标志 */
    for (i = 0; i < 8; i++)
    {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }

    /* 使能全局中断 */
    __enable_irq();

    /* 跳转到系统 BootLoader ，首地址是 MSP ，地址 +4 是复位中断服务程序地址 */
    p_sys_bootloader_run = (void (*)(void)) (*((uint32_t*)(boot_address + 4)));

    /* 设置主堆栈指针 */
    __set_MSP(*(uint32_t*)boot_address);

    /* 在 RTOS 工程，这条语句很重要，设置为特权级模式，使用 MSP 指针 */
    __set_CONTROL(0);

    /* 跳转到 系统 BootLoader 代码 运行 */
    p_sys_bootloader_run();

		
    /* 跳转不成功，软件复位 */
    while (1);
}






typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
void IAP_JumpToApplication(void)
{
    u8 i = 0;
    u32 JumpAddress;//跳转地址
    __disable_irq();//关闭全局中断
    /* 关闭滴答定时器，复位到默认值 */
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;


   /* 关闭所有中断，清除所有中断挂起标志 */
    for (i = 0; i < 8; i++)
    {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }
    if (((*(__IO u32*)0x08000000) & 0x2FFE0000) == 0x20000000)//有升级代码，IAP_ADDR地址处理应指向主堆栈区，即0x20000000
    {
        JumpAddress = *(__IO u32*)(0x08000000 + 4);//获取复位地址	
        Jump_To_Application = (pFunction)JumpAddress;//函数指针指向复位地址
        __set_MSP(*(__IO u32*)0x08000000);//设置主堆栈指针MSP指向升级机制IAP_ADDR
        Jump_To_Application();//跳转到升级代码处
    }
}





//跳转到应用程序段
//appxaddr:用户代码起始地址.
void Iap_Jump_app(uint32_t appxaddr)
{
    // printf("%02x\r\n",(((*(vu32*)appxaddr)&0x2FFE0000)));

    if (((*(vu32*)appxaddr) & 0x2FFE0000) == 0x20000000)	//检查栈顶地址是否合法.
    {
        __disable_irq();                            //关闭全局中断
        jumpapp = (iapfun) * (vu32*)(appxaddr + 4);	//用户代码区第二个字为程序开始地址(复位地址)		
        __set_MSP(*(vu32*)appxaddr);				//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
        jumpapp();									//跳转到APP
    }
}



void soft_reset(void)
{
    // 关闭所有中断
    __set_FAULTMASK(1);
    // 复位
    NVIC_SystemReset();
}



void Update_CommandReceive(uint8_t data)
{
    static uint8_t upg_count = 0;
    static uint8_t upg_cmd[8] = { 'F','W','U','P','D','A','T','E' };

    if (data == upg_cmd[upg_count])
    {
        if (++upg_count == 8)
        {
            upg_count = 0;
            Flag.update = 1;
        }
    }
    else
    {
        upg_count = 0;
    }
}

























