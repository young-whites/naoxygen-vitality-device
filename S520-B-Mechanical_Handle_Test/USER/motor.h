#ifndef _MOTOR_H
#define _MOTOR_H

#include "stm8s_conf.h"
#include "timer.h"

/* Motor parameters */
#define MAX_MOTOR_COUNT  25212
#define MOTOR_100        252

/* Speed array: smaller value = faster */
extern u32 speed_mode[6];

void motor_forward(u8 mode);
void motor_backward(void);
void motor_stop(void);
void motor_hiz(void);
void check_limit(void);

typedef struct {
    volatile u8 direction;
    volatile u8 limit_rear;
    volatile u8 limit_front;
} motor_state_t;

extern motor_state_t motor;

#endif
