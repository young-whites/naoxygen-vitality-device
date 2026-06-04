#include "motor.h"
#include "stm8s.h"
#include "stm8s_tim1.h"
#include "gpio.h"
#include "Decode.h"

motor_state_t motor;

/* Global speed array init: values correspond to motor speeds */
u32 speed_mode[6] = {0, 0, 0, 0, 0, 300}; // smaller = faster

void motor_hiz(void)
{
    GPIO_Init(IN1_PORT, IN1_PIN, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(IN2_PORT, IN2_PIN, GPIO_MODE_IN_FL_NO_IT);
}

void motor_stop(void)
{
    /* Disable TIM1 immediately to prevent interrupt storm (speed_mode[4]=0 causes period=0) */
    TIM1_DeInit();
    TIM1_Cmd(DISABLE);
    TIM1_CtrlPWMOutputs(DISABLE);
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, DISABLE);

    /* Set motor control pins to safe state: IN1=LOW, IN2=LOW, ENA=LOW */
    GPIO_WriteLow(MOTOR_ENA_PORT, MOTOR_ENA_PIN);
    GPIO_Init(IN1_PORT, IN1_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(IN2_PORT, IN2_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);

    motor.direction = 0;
    /* Do NOT clear limit flags here - they persist until motor moves away from switch */
}

void motor_forward(u8 mode)
{
    if (motor.limit_front) return;

    /* If rear limit was active, set override so check_limit won't re-trigger it */
    if (motor.limit_rear) {
        motor.override_rear = 1;
    }
    motor.limit_rear = 0;

    motor.direction = 2;

    /* Safely stop TIM1 first */
    TIM1_DeInit();
    TIM1_Cmd(DISABLE);
    TIM1_CtrlPWMOutputs(DISABLE);

    /* Then start forward PWM */
    GPIO_WriteLow(MOTOR_ENA_PORT, MOTOR_ENA_PIN);
    GPIO_WriteHigh(MOTOR_ENA_PORT, MOTOR_ENA_PIN);
    pwm_init(START, speed_mode[mode], 2);
}

void motor_backward(void)
{
    if (motor.limit_rear) return;

    /* If front limit was active, set override so check_limit won't re-trigger it */
    if (motor.limit_front) {
        motor.override_front = 1;
    }
    motor.limit_front = 0;

    motor.direction = 1;

    /* Safely stop TIM1 first (avoid interrupt storm from period=0) */
    TIM1_DeInit();
    TIM1_Cmd(DISABLE);
    TIM1_CtrlPWMOutputs(DISABLE);

    /* Then start backward PWM */
    GPIO_WriteLow(MOTOR_ENA_PORT, MOTOR_ENA_PIN);
    GPIO_WriteHigh(MOTOR_ENA_PORT, MOTOR_ENA_PIN);
    pwm_init(START, speed_mode[5], 1);
}

void check_limit(void)
{
    u8 sw;

    /* PC1 = rear limit switch */
    sw = READ_PC1();
    if (sw && !motor.limit_rear && !motor.override_rear) {
        /* New limit trigger - stop motor */
        motor.limit_rear = 1;
        motor_stop();
    } else if (!sw && motor.override_rear) {
        /* Switch released after override - clear override */
        motor.override_rear = 0;
    } else if (!sw && motor.limit_rear && motor.direction == 2) {
        /* Motor moving forward away from rear switch - clear limit */
        motor.limit_rear = 0;
    }

    /* PC2 = front limit switch */
    sw = READ_PC2();
    if (sw && !motor.limit_front && !motor.override_front) {
        /* New limit trigger - stop motor */
        motor.limit_front = 1;
        motor_stop();
    } else if (!sw && motor.override_front) {
        /* Switch released after override - clear override */
        motor.override_front = 0;
    } else if (!sw && motor.limit_front && motor.direction == 1) {
        /* Motor moving backward away from front switch - clear limit */
        motor.limit_front = 0;
    }
}
