#include "bsp_hard.h"



/**
 * @brief  设备供电引脚初始化
 * @param  无
 * @retval 无
 */
void Power_GPIO_Config(void)
{
    /*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;

    // 电源 GPIO 初始化 -- PB5
    macFAN_GPIO_APBxClock_FUN(macPower_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = macPower_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(macPower_GPIO_PORT, &GPIO_InitStructure);
}


/**
 * @brief  主气路的电磁阀引脚初始化
 * @param  无
 * @retval 无
 */
void Main_Gas_Circuit_GPIO_Config(void)
{
    /*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;

    // 电源 GPIO 初始化 -- PA15
    macFAN_GPIO_APBxClock_FUN(macValve_3_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = macValve_3_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(macValve_3_GPIO_PORT, &GPIO_InitStructure);
}



