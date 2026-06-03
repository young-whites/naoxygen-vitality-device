#include "motor.h" 
#include "stm8s.h"
#include "gpio.h"
#include "Decode.h"
#include "eeprom.h"





   
u32 speed_mode[6] = {3600,3200,2800,2400,0,300};//值越小，速度越快


systempara systemparameter;
uint8_t Location;

static void delay(u16 z)
{  
    u16 x,y;
    for(x=1000;x>0;x--)
    for(y=z;y>0;y--);
} 


void SendDjData(void)
{
    int number;
    number = 100 - systemparameter.tim1_count_cnt1/systemparameter.motor_100;
    Location = number;
    if(number>100)number=100;
    else if(number<=0)number=0;
    
    senddata(MOTOR_STEP_CMD,number);
}


void SendRuturnFlag(u8 flag)
{
    senddata(RUTURN_FLAG_CMD,flag);
}

/* ---- Acceleration ramp ---- */
#define ACCEL_STEPS     20      /* number of ramp steps */
#define ACCEL_INTERVAL  10      /* ms between each step */

static volatile u8  accel_active = 0;
static volatile u8  accel_steps_remaining = 0;
static volatile u8  accel_counter = 0;
static u32 accel_start_arr = 0;  /* ARR at first step (slow) */
static u32 accel_target_arr = 0; /* ARR at target speed */

void accel_start(u32 target_arr)
{
    accel_start_arr = speed_mode[5];  /* always start from slowest */
    accel_target_arr = target_arr;
    accel_steps_remaining = ACCEL_STEPS;
    accel_counter = 0;
    accel_active = 1;
}

/*
 * Linear interpolation: ramp ARR from accel_start_arr down to accel_target_arr.
 * Update CCR2/CCR4 proportionally so the effective duty cycle ratio stays constant.
 *
 * Forward: CCR2 = ARR (full, inverted), CCR4 = ARR/2 (half, inverted)
 * Backward: CCR2 = ARR/2, CCR4 = ARR
 */
void accel_update(void)
{
    u32 cur_arr;
    u32 step;
    u32 ccr2, ccr4;

    if (!accel_active || accel_steps_remaining == 0)
        return;

    if (++accel_counter < ACCEL_INTERVAL)
        return;
    accel_counter = 0;

    step = ACCEL_STEPS - accel_steps_remaining + 1;  /* 1 .. ACCEL_STEPS */
    /* Linear interpolation: cur_arr = start + (target - start) * step / STEPS */
    cur_arr = accel_start_arr
            + (u32)((s32)(accel_target_arr - accel_start_arr) * (s32)step / ACCEL_STEPS);

    /* Update ARR */
    TIM1->ARRH = (u8)(cur_arr >> 8);
    TIM1->ARRL = (u8)(cur_arr);

    /* Update CCR2/CCR4 proportionally */
    if (systemparameter.currt_mode == 2) {
        /* Forward: OC2=full, OC4=half */
        ccr2 = cur_arr;
        ccr4 = cur_arr / 2;
    } else {
        /* Backward: OC2=half, OC4=full */
        ccr2 = cur_arr / 2;
        ccr4 = cur_arr;
    }
    TIM1->CCR2H = (u8)(ccr2 >> 8);
    TIM1->CCR2L = (u8)(ccr2);
    TIM1->CCR4H = (u8)(ccr4 >> 8);
    TIM1->CCR4L = (u8)(ccr4);

    if (--accel_steps_remaining == 0)
        accel_active = 0;
}

void motor_hiz(void)
{
    accel_active = 0;  /* cancel any running ramp */
    // Set IN1/IN2 to high-impedance (floating input)
    GPIO_Init(IN1_PORT, IN1_PIN, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(IN2_PORT, IN2_PIN, GPIO_MODE_IN_FL_NO_IT);
}

void motor_stop(void)
{
    pwm_init( STOP,speed_mode[4],0);
    motor_hiz(); // IN1/IN2 to Hi-Z, ENA stays HIGH
    systemparameter.currt_mode=0;
}

void motor_step_save(void)
{
EEPROM_WriteData(MOTOR_DATA_ADDR_LOW8,systemparameter.tim1_count_cnt1%256);
EEPROM_WriteData(MOTOR_DATA_ADDR_HIGH8,systemparameter.tim1_count_cnt1/256);
}


void motor_forword(u8 mode)   
{
    systemparameter.currt_mode=2;
    // Reconfigure IN1/IN2 as push-pull output for timer PWM control
    GPIO_Init(IN1_PORT, IN1_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(IN2_PORT, IN2_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    pwm_init( START,speed_mode[5],2);  /* start from slowest speed */
    accel_start(speed_mode[mode]);     /* accelerate to target */
}

void motor_bank(void)
{
    systemparameter.currt_mode=1;
    // Reconfigure IN1/IN2 as push-pull output for timer PWM control
    GPIO_Init(IN1_PORT, IN1_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(IN2_PORT, IN2_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    pwm_init( START,speed_mode[5],1);  /* start from slowest speed */
    accel_start(speed_mode[5]);         /* backward: stay at slow speed */
}



void check_stop(void)
{

    static char read_pc1_r=0,read_pc2_r=0;//,i=0,j=0;
    char read_pc1,read_pc2;

    read_pc1=READ_PC1();//后端
    systemparameter.read_pcc1=read_pc1;

    if(read_pc1_r!=read_pc1)
    {
        read_pc1_r=read_pc1;
        if(read_pc1==1) 
        {
            motor_stop();
            if(systemparameter.k_flag==2)//校验电机退回
            {
                senddata(HANDLE_CHECK,0);//让主板响beep
                EEPROM_WriteData(MOTOR_CHECK_DATA_ADDR_LOW8,systemparameter.tim1_count_cnt1%256);
                EEPROM_WriteData(MOTOR_CHECK_DATA_ADDR_HIGH8,systemparameter.tim1_count_cnt1/256);
            }
            else
            {
                systemparameter.tim1_count_cnt1=systemparameter.max_motor_count;//对电机数据进行后端校准
                motor_step_save();
            }
            senddata(MOTOR_STEP_CMD,0);
            systemparameter.stop_flog=1;//到达后端
            systemparameter.IfRuturning=0;
            delay(20);
            SendRuturnFlag(0);//通知主板，电机以退回，如果之前不理睬APP，现在要开始理睬了。并且主板通知APP电机以退回。//这里测试偶尔丢失这个通知，？？，所以加延时。
        }
        else
        { 
            systemparameter.stop_flog=0;//离开后端
        }    
    }

    read_pc2=READ_PC2();//前端
    systemparameter.read_pcc2=read_pc2;
    if(read_pc2_r!=read_pc2)
    {
        read_pc2_r=read_pc2;
        if(read_pc2==1)
        {
            systemparameter.stop_flog=2;//到达前端
            systemparameter.tim1_count_cnt1=0;//对电机数据进行前端校准

            if(systemparameter.k_flag==1)//手动前进，到达前端，停止
            {
                motor_stop();
            }
            else if(systemparameter.k_flag==0)//步骤前进，到达前端，自动退回
            {
                motor_bank();
                systemparameter.IfRuturning=1;
                SendRuturnFlag(1);//通知主板，电机正在退后，APP发来的指令主板不理睬，并且主板通知APP电机正在退回。
            }
            else if(systemparameter.k_flag==2)//校验电机退回
            {
                motor_bank();
            }
        }
        else
        {
            systemparameter.stop_flog=0;//离开前端
        }
    }


    if(systemparameter.SendDjData_flag==1)
    {
        SendDjData();
        systemparameter.SendDjData_flag=0;
    }
}

     

