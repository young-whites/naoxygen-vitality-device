#include "iwdg.h"




/*******************************************************************************
* FunctionName   : void IWDG_Init(uint8_t prer, uint16_t rlr)
* Description    : 独立看门狗初始化函数
* Notice         : prer 分频   rlr重载值  主时钟 40K
* Param[in]      : None
* Output         : None
* Return         : None
* LastUpdateDate : None
*******************************************************************************/
void IWDG_Init(uint8_t prer, uint16_t rlr)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    IWDG_SetPrescaler(prer);

    IWDG_SetReload(rlr);

    IWDG_ReloadCounter();

    IWDG_Enable();
}
/*******************************************************************************
* FunctionName   : void IWDG_Feed(void)
* Description    : 独立看门狗喂狗
* Notice         : None
* Param[in]      : None
* Output         : None
* Return         : None
* LastUpdateDate : None
*******************************************************************************/
void IWDG_Feed(void)
{
    IWDG_ReloadCounter();
}














