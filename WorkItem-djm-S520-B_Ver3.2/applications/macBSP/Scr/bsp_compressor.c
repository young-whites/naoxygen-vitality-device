#include <applications/macBSP/Inc/bsp_compressor.h>
/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-11-19     teati       the first version
 */



/**
  * @brief  This Function is used to ctrl the compressor on.
  *         GPIOB Pin14
  * @retval void
  */
void Compressor_On(void)
{
    HAL_GPIO_WritePin(Compressor_EN_GPIO_Port, Compressor_EN_Pin, GPIO_PIN_SET);
}


/**
  * @brief  This Function is used to ctrl the compressor off.
  *         GPIOB Pin14
  * @retval void
  */
void Compressor_Off(void)
{
    HAL_GPIO_WritePin(Compressor_EN_GPIO_Port, Compressor_EN_Pin, GPIO_PIN_RESET);
}






