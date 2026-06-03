#include "motor.h"
#include "stm8s.h"
#include "gpio.h"
#include "Decode.h"

motor_state_t motor;

void motor_hiz(void)
{
    GPIO_Init(IN1_PORT, IN1_PIN, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(IN2_PORT, IN2_PIN, GPIO_MODE_IN_FL_NO_IT);
}

void motor_stop(void)
{
    pwm_init(STOP, SPEED_MODE_0, 0);
    motor_hiz();
    GPIO_WriteLow(MOTOR_ENA_PORT, MOTOR_ENA_PIN); // ENA LOW
    motor.direction = 0;
}

void motor_forward(u8 mode)
{
    u32 speed_arr;

    if (motor.limit_front) return;

    /* Map mode to ARR value */
    switch (mode) {
        case 0:  speed_arr = SPEED_MODE_0; break;
        case 1:  speed_arr = SPEED_MODE_1; break;
        case 2:  speed_arr = SPEED_MODE_2; break;
        case 3:  speed_arr = SPEED_MODE_3; break;
        default: speed_arr = SPEED_MODE_2; break;
    }

    motor.direction = 2;
    /* Startup sequence: ENA LOW -> stop PWM -> ENA HIGH -> start PWM */
    GPIO_WriteLow(MOTOR_ENA_PORT, MOTOR_ENA_PIN);
    pwm_init(STOP, SPEED_MODE_0, 0);
    GPIO_WriteHigh(MOTOR_ENA_PORT, MOTOR_ENA_PIN);
    GPIO_Init(IN1_PORT, IN1_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(IN2_PORT, IN2_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    pwm_init(START, speed_arr, 2);
}

void motor_backward(void)
{
    if (motor.limit_rear) return;

    motor.direction = 1;
    /* Startup sequence: ENA LOW -> stop PWM -> ENA HIGH -> start PWM */
    GPIO_WriteLow(MOTOR_ENA_PORT, MOTOR_ENA_PIN);
    pwm_init(STOP, SPEED_MODE_0, 0);
    GPIO_WriteHigh(MOTOR_ENA_PORT, MOTOR_ENA_PIN);
    GPIO_Init(IN1_PORT, IN1_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(IN2_PORT, IN2_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    pwm_init(START, SPEED_REVERSE, 1);
}

void check_limit(void)
{
    u8 sw;

    sw = READ_PC1();
    if (sw && !motor.limit_rear) {
        motor.limit_rear = 1;
        motor_stop();
    } else if (!sw) {
        motor.limit_rear = 0;
    }

    sw = READ_PC2();
    if (sw && !motor.limit_front) {
        motor.limit_front = 1;
        motor_stop();
    } else if (!sw) {
        motor.limit_front = 0;
    }
}
