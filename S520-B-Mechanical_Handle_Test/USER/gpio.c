#include "gpio.h"
#include "motor.h"
#include "usart.h"


void GPIO_Config(void)
{
    GPIO_Init(MOTOR_ENA_PORT, MOTOR_ENA_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(IN1_PORT, IN1_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(IN2_PORT, IN2_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(PC1_PORT,PC1_PIN,GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(PC2_PORT,PC2_PIN,GPIO_MODE_IN_PU_NO_IT);
    GPIO_WriteLow(MOTOR_ENA_PORT, MOTOR_ENA_PIN); // ENA LOW at startup (motor stopped)
}


