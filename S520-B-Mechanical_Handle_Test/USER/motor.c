#include "motor.h"
#include "stm8s.h"
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
    GPIO_WriteLow(MOTOR_ENA_PORT, MOTOR_ENA_PIN);
    pwm_init(STOP, speed_mode[4], 0);
    motor.direction = 0;
}

void motor_forward(u8 mode)
{
    if (motor.limit_front) return;

    motor.direction = 2;
    /* Startup sequence: ENA LOW -> stop -> ENA HIGH -> start */
    GPIO_WriteLow(MOTOR_ENA_PORT, MOTOR_ENA_PIN);
    pwm_init(STOP, speed_mode[4], 0);
    GPIO_WriteHigh(MOTOR_ENA_PORT, MOTOR_ENA_PIN);
    pwm_init(START, speed_mode[mode], 2);
}

void motor_backward(void)
{
    if (motor.limit_rear) return;

    motor.direction = 1;
    /* Startup sequence: ENA LOW -> stop -> ENA HIGH -> start */
    GPIO_WriteLow(MOTOR_ENA_PORT, MOTOR_ENA_PIN);
    pwm_init(STOP, speed_mode[4], 0);
    GPIO_WriteHigh(MOTOR_ENA_PORT, MOTOR_ENA_PIN);
    pwm_init(START, speed_mode[5], 1);
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
