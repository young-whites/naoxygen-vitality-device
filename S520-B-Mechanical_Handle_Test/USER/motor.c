#include "motor.h"
#include "stm8s.h"
#include "gpio.h"
#include "Decode.h"

motor_state_t motor;

static volatile u8  accel_active = 0;
static volatile u8  accel_remaining = 0;
static volatile u8  accel_counter = 0;
static u32 accel_start_arr = 0;
static u32 accel_target_arr = 0;
static u8  accel_direction = 0;

void accel_start(u32 target_arr, u8 direction)
{
    accel_start_arr = MOTOR_SPEED_SLOW;
    accel_target_arr = target_arr;
    accel_direction = direction;
    accel_remaining = ACCEL_STEPS;
    accel_counter = 0;
    accel_active = 1;
}

void accel_update(void)
{
    u32 cur_arr, ccr2, ccr4;
    u32 step;

    if (!accel_active || accel_remaining == 0) return;
    if (++accel_counter < ACCEL_INTERVAL_MS) return;
    accel_counter = 0;

    step = ACCEL_STEPS - accel_remaining + 1;
    cur_arr = accel_start_arr
            + (u32)((s32)(accel_target_arr - accel_start_arr) * (s32)step / ACCEL_STEPS);

    TIM1->ARRH = (u8)(cur_arr >> 8);
    TIM1->ARRL = (u8)(cur_arr);

    if (accel_direction == 2) {
        ccr2 = cur_arr;
        ccr4 = cur_arr / 2;
    } else {
        ccr2 = cur_arr / 2;
        ccr4 = cur_arr;
    }
    TIM1->CCR2H = (u8)(ccr2 >> 8);
    TIM1->CCR2L = (u8)(ccr2);
    TIM1->CCR4H = (u8)(ccr4 >> 8);
    TIM1->CCR4L = (u8)(ccr4);

    if (--accel_remaining == 0) accel_active = 0;
}

void motor_hiz(void)
{
    accel_active = 0;
    GPIO_Init(IN1_PORT, IN1_PIN, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(IN2_PORT, IN2_PIN, GPIO_MODE_IN_FL_NO_IT);
}

void motor_stop(void)
{
    /* Disable TIM1 outputs directly, avoid pwm_init() CLK_DeInit */
    TIM1->CCER1 &= ~(u8)(TIM1_CCER1_CC2E | TIM1_CCER1_CC2NE);
    TIM1->CCER2 &= ~(u8)(TIM1_CCER2_CC4E);
    TIM1->BDTR &= ~(u8)(TIM1_BDTR_MOE);
    motor_hiz();
    motor.direction = 0;
}

void motor_forward(void)
{
    if (motor.limit_front) return;
    motor.direction = 2;
    GPIO_Init(IN1_PORT, IN1_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(IN2_PORT, IN2_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    pwm_init(START, MOTOR_SPEED_SLOW, 2);
    accel_start(MOTOR_SPEED_FAST, 2);
}

void motor_backward(void)
{
    if (motor.limit_rear) return;
    motor.direction = 1;
    GPIO_Init(IN1_PORT, IN1_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(IN2_PORT, IN2_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    pwm_init(START, MOTOR_SPEED_SLOW, 1);
    accel_start(MOTOR_SPEED_SLOW, 1);
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
