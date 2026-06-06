
#include "timer.h"
#include "gpio.h"
#include "motor.h"
#include "usart.h"
#include "Decode.h"
#include "string.h"
#include "eeprom.h"
#include "stm8s.h"


extern uint8_t adjust_mode;
extern int adjust_Value;

void Write_Option_Byte(void)
{
    static u16 Option;
    FLASH_DeInit();
    FLASH_Unlock(FLASH_MEMTYPE_DATA);
    FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);
    Option=FLASH_ReadOptionByte(0x4803);
    if(Option != 0x817e)
    {
        FLASH_ProgramOptionByte(0x4803,0x81);
        WWDG->CR = 0x80;
    }
}


int main(void)
{
    uint8_t Start_Flag = 0;
    uint8_t Progress_H , Progress_L;
    
    
    disableInterrupts();
    CLK_HSIPrescalerConfig( CLK_PRESCALER_HSIDIV1 );
    GPIO_Config();
    USARTAPP_Config();
    TIM2_Config();
    enableInterrupts();

    systemparameter.currt_mode=0;
    systemparameter.stop_flog=0;
    systemparameter.k_flag=0;
    systemparameter.ack_flag=0;
    systemparameter.read_pcc1=0;
    systemparameter.SendDjData_flag=0;
    systemparameter.IfRuturning=0;
    systemparameter.motor_mode=2;
    systemparameter.test_state=TEST_IDLE;
    systemparameter.test_forward_cnt=0;
    systemparameter.test_target=0;
    Delay_ms(100);
    Write_Option_Byte();


    Start_Flag = EEPROM_ReadData(START_FLAG_ADDR);

    if (Start_Flag == 0x01) {

        adjust_mode = EEPROM_ReadData(ADJUST_SPEED_MODE_ADDR);
        Delay_ms(10);
        adjust_Value = EEPROM_ReadData(ADJUST_SPEED_VALUE_ADDR);
        Delay_ms(10);

        /* �������������������� */
        Progress_H = EEPROM_ReadData(MOTOR_DATA_ADDR_HIGH8);
         Delay_ms(10);
        Progress_L = EEPROM_ReadData(MOTOR_DATA_ADDR_LOW8);
         Delay_ms(10);
        systemparameter.tim1_count_cnt1 = (Progress_H << 8) | Progress_L;

        systemparameter.max_motor_count = MAX_MOTOR_COUNT;
        systemparameter.motor_100 = MOTOR_100;
    }
    else {
        adjust_mode = 0;
        adjust_Value = 0;
        EEPROM_WriteData(START_FLAG_ADDR, 0x01);
        Delay_ms(10);
        EEPROM_WriteData(MOTOR_DATA_ADDR_LOW8, 0);
        Delay_ms(10);
        EEPROM_WriteData(MOTOR_DATA_ADDR_HIGH8, 0);
        Delay_ms(10);
        EEPROM_WriteData(ADJUST_SPEED_MODE_ADDR, adjust_mode);
        Delay_ms(10);
        EEPROM_WriteData(ADJUST_SPEED_VALUE_ADDR, adjust_Value);
        Delay_ms(10);

        systemparameter.max_motor_count = MAX_MOTOR_COUNT;
        systemparameter.motor_100 = MOTOR_100;
    }

  
    if (adjust_mode == 1) {
        adjust_Value = -adjust_Value;
    }

    speed_mode[0]=(u32)(-0.1094*systemparameter.max_motor_count + adjust_Value + 5740);//11
    speed_mode[1]=(u32)(-0.1094*systemparameter.max_motor_count + adjust_Value + 6200);//10
    speed_mode[2]=(u32)(-0.1094*systemparameter.max_motor_count + adjust_Value + 5850);//9:35
    speed_mode[3]=(u32)(-0.1094*systemparameter.max_motor_count + adjust_Value + 5350);//8

    SendDjData();
    Delay_ms(50);
    senddata(REVISION_CMD,REVISION);
    Delay_ms(50);
    senddata(HANDLE_CHECK,0);
    Delay_ms(50);
    
    while(1)
    {
       check_stop();
       motor_limit_test_update();
    }
    
}



