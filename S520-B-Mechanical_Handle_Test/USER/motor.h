#ifndef _MOTOR_H
#define _MOTOR_H

#include "stm8s_conf.h"
#include "timer.h"

/* Speed modes: smaller ARR = faster */
#define SPEED_MODE_0    3600    /* slowest */
#define SPEED_MODE_1    3200
#define SPEED_MODE_2    2800
#define SPEED_MODE_3    2400    /* fastest */
#define SPEED_REVERSE   300     /* backward speed */

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
