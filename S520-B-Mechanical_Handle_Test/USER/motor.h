#ifndef _MOTOR_H
#define _MOTOR_H

#include "stm8s_conf.h"
#include "timer.h"

#define MOTOR_SPEED_SLOW    2400
#define MOTOR_SPEED_FAST    300
#define MOTOR_SPEED_STOP    0

#define ACCEL_STEPS         20
#define ACCEL_INTERVAL_MS   10

void motor_forward(void);
void motor_backward(void);
void motor_stop(void);
void motor_hiz(void);
void accel_start(u32 target_arr, u8 direction);
void accel_update(void);
void check_limit(void);

typedef struct {
    volatile u8 direction;
    volatile u8 limit_rear;
    volatile u8 limit_front;
} motor_state_t;

extern motor_state_t motor;

#endif
