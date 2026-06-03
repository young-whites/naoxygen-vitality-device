#ifndef _MOTOR_H
#define _MOTOR_H

#include "stm8s_conf.h"
#include "timer.h"

void motor_stop(void);
void motor_hiz(void);
void motor_forword(u8 mode);
void motor_bank(void);
void accel_start(u32 target_arr);
void accel_update(void);
void check_stop(void);
void SendDjData(void);
void motor_step_save(void);

//#define MAX_MOTOR_COUNT  26056
//#define MOTOR_100  274

#define MAX_MOTOR_COUNT  25212
#define MOTOR_100  252


extern u32 speed_mode[6] ;//值越小，速度越快

typedef struct system
{
    volatile char currt_mode;
    char stop_flog;
    char SendDjData_flag;
    char work_flag;//如果电机走到最前端以后，没有退回到底端，或者没有进行一次手动调节，APP下发的指令接不起作用
    char k_flag;
    char IfRuturning;
    char ack_flag;
    unsigned int  tim1_count_cnt1;
    unsigned int  max_motor_count;
    int  motor_100;
    int  motor_mode;
    volatile char read_pcc1;
    volatile char read_pcc2;
}systempara;

extern systempara systemparameter;
void SendRuturnFlag(u8 flag);


#endif