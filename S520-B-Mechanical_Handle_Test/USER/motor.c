#include "motor.h"
#include "stm8s.h"
#include "gpio.h"
#include "Decode.h"
#include "eeprom.h"






u32 speed_mode[6] = {3600,3200,2800,2400,0,300};//值越小,速度越快


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

void motor_stop(void)
{
    GPIO_WriteLow(MOTOR_ENA_PORT, MOTOR_ENA_PIN);//带上拉,推挽输出低电平
    pwm_init( STOP,speed_mode[4],0);
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
    GPIO_WriteLow(MOTOR_ENA_PORT, MOTOR_ENA_PIN);//带上拉,推挽输出低电平
    pwm_init( STOP,speed_mode[4],0);
    GPIO_WriteHigh(MOTOR_ENA_PORT, MOTOR_ENA_PIN);//带上拉,推挽输出低电平
    pwm_init( START,speed_mode[mode],2);
}

void motor_bank(void)
{
    systemparameter.currt_mode=1;
    GPIO_WriteLow(MOTOR_ENA_PORT, MOTOR_ENA_PIN);//带上拉,推挽输出低电平
    pwm_init( STOP,speed_mode[4],0);
    GPIO_WriteHigh(MOTOR_ENA_PORT, MOTOR_ENA_PIN);//带上拉,推挽输出低电平
    pwm_init( START,speed_mode[5],1);
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
            SendRuturnFlag(0);//通知主板,电机以退回,如果之前不理睬APP,现在要开始理睬了。并且主板通知APP电机以退回。//这里测试偶尔丢失这个通知,??,所以加延时。
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

            if(systemparameter.k_flag==1)//手动前进,到达前端,停止
            {
                motor_stop();
            }
            else if(systemparameter.k_flag==0)//步骤前进,到达前端,自动退回
            {
                motor_bank();
                systemparameter.IfRuturning=1;
                SendRuturnFlag(1);//通知主板,电机正在退后,APP发来的指令主板不理睬,并且主板通知APP电机正在退回。
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


/*
 * motor_limit_test_start - trigger the limit switch test
 * @forward_pulses: number of pulses to move forward after rear limit hit
 *
 * Call this once from UART command handler to start the test.
 * Non-blocking: state machine runs in motor_limit_test_update().
 */
void motor_limit_test_start(u16 forward_pulses)
{
    if (systemparameter.stop_flog == 2) {
        /* At front limit - cannot test, need to back first manually */
        senddata(DATA_CMD, 0xEE);
        return;
    }
    systemparameter.test_target = forward_pulses;
    systemparameter.test_forward_cnt = 0;
    if (systemparameter.stop_flog == 1) {
        /* Already at rear limit - skip backing, go directly to forward */
        systemparameter.test_state = TEST_WAIT_AFTER_STOP;
    } else {
        /* Normal case - start backing */
        systemparameter.test_state = TEST_BACKING;
        motor_bank();
    }
}

/*
 * motor_limit_test_update - non-blocking state machine for limit test
 * Call this in the main loop (after check_stop).
 *
 * Sequence:
 *   1. BACKING: move backward until rear limit hit -> stop -> WAIT
 *   2. WAIT_AFTER_STOP: brief delay -> start forward
 *   3. FORWARDING: move forward target pulses -> DONE
 *   4. DONE: send result, return to IDLE
 */
void motor_limit_test_update(void)
{
    switch (systemparameter.test_state)
    {
        case TEST_IDLE:
            break;

        case TEST_BACKING:
            /* Waiting for rear limit (stop_flog==1) triggered by check_stop() */
            if (systemparameter.stop_flog == 1) {
                motor_stop();
                systemparameter.test_state = TEST_WAIT_AFTER_STOP;
                systemparameter.test_forward_cnt = 0;
            }
            break;

        case TEST_WAIT_AFTER_STOP:
            /* Small delay using counter, then start forward */
            systemparameter.test_forward_cnt++;
            if (systemparameter.test_forward_cnt >= 200) {
                systemparameter.test_forward_cnt = systemparameter.tim1_count_cnt1;
                systemparameter.test_state = TEST_FORWARDING;
                systemparameter.k_flag = 1; /* manual forward mode */
                motor_forword(5); /* forward at default speed */
            }
            break;

        case TEST_FORWARDING:
            /* Check if we moved forward enough pulses */
            /* Critical section: read tim1_count_cnt1 atomically on 8-bit MCU */
            disableInterrupts();
            {
                u16 cur_pos = systemparameter.tim1_count_cnt1;
                u16 start_pos = systemparameter.test_forward_cnt;
                u16 target = systemparameter.test_target;
                enableInterrupts();
                /* Guard against underflow: only check if start_pos >= target */
                if ((start_pos >= target && cur_pos <= start_pos - target)
                    || systemparameter.stop_flog == 2) {
                    /* Target reached or front limit hit */
                    motor_stop();
                    systemparameter.test_state = TEST_DONE;
                }
            }
            break;

        case TEST_DONE:
            /* Report result: final position */
            senddata(MOTOR_STEP_CMD, (u8)(systemparameter.tim1_count_cnt1 / systemparameter.motor_100));
            senddata(DATA_CMD, 0x01); /* test complete flag */
            systemparameter.test_state = TEST_IDLE;
            break;
    }
}
     


