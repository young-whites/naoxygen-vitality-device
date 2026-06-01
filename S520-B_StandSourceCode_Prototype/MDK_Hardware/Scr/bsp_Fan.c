#include "bsp_Fan.h"




#if FAN_GPIO_CTRL


/**
 * @brief  配置 散热风扇 的 GPIO 功能
 * @param  无
 * @retval 无
 */
void BSP_FAN_GPIO_Config(void)
{
    /*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;

    // 风扇 GPIO 初始化 -- PC6
    macFAN_GPIO_APBxClock_FUN(macFAN_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = macFAN_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(macFAN_GPIO_PORT, &GPIO_InitStructure);

}

#endif /*FAN_GPIO_CTRL*/





