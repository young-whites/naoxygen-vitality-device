#include <applications/macBSP/Inc/bsp_hard.h>
/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-06-04     zphu       the first version
 */



/**
  * @brief  This Function is used to ctrl the Fan off.
  *         GPIOC Pin6
  * @retval void
  */
void Fan_Off(void)
{
    HAL_GPIO_WritePin(FAN_GPIO_Port, FAN_Pin, GPIO_PIN_RESET);
}


/**
  * @brief  This Function is used to ctrl the Fan on.
  *         GPIOC Pin6
  * @retval void
  */
void Fan_On(void)
{
    HAL_GPIO_WritePin(FAN_GPIO_Port, FAN_Pin, GPIO_PIN_SET);
}





/**
  * @brief  3电磁阀失能控制 -- 用于控制主气路
  *         GPIOA Pin15
  * @retval void
  */
void Main_Gas_Circuit_Off(void)
{
    HAL_GPIO_WritePin(Vavle_3_EN_GPIO_Port, Vavle_3_EN_Pin, GPIO_PIN_RESET);
}


/**
  * @brief  3电磁阀使能控制 -- 用于控制主气路
  *         GPIOA Pin15
  * @retval void
  */
void Main_Gas_Circuit_On(void)
{
    HAL_GPIO_WritePin(Vavle_3_EN_GPIO_Port, Vavle_3_EN_Pin, GPIO_PIN_SET);

}


/**
  * @brief  电源开启的LED状态
  * @retval void
  */
void Power_Off_LED(void)
{
    LED_Off(LED_Name_White);
    LED_On(LED_Name_Orange);
}






/**
  * @brief  电源关闭的LED状态
  * @retval void
  */
void Power_On_LED(void)
{
    LED_On(LED_Name_White);
    LED_Off(LED_Name_Orange);
}






void Power_EN(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5, GPIO_PIN_SET);

    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

