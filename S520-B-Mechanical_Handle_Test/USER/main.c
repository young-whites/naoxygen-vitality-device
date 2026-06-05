
#include "timer.h"
#include "gpio.h"
#include "motor.h"
#include "usart.h"
#include "Decode.h"
#include "string.h"
#include "eeprom.h"
#include "stm8s.h"


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


    disableInterrupts();
    CLK_HSIPrescalerConfig( CLK_PRESCALER_HSIDIV1 );
    GPIO_Config();
    USARTAPP_Config();
    TIM2_Config();
    enableInterrupts();

    motor.direction = 0;
    motor.limit_rear = 0;
    motor.limit_front = 0;
    motor.override_front = 0;
    motor.override_rear = 0;

    Delay_ms(100);
    /* Check limit switches immediately at startup to set correct flags */
    // check_limit();  // temporarily disabled: full command control
    Write_Option_Byte();

    Start_Flag = EEPROM_ReadData(START_FLAG_ADDR);
    if (Start_Flag != 0x01) {
        EEPROM_WriteData(START_FLAG_ADDR, 0x01);
        Delay_ms(10);
    }

    speed_mode[0]=(u32)(-0.1094*MAX_MOTOR_COUNT + 5740);//11
    speed_mode[1]=(u32)(-0.1094*MAX_MOTOR_COUNT + 6200);//10
    speed_mode[2]=(u32)(-0.1094*MAX_MOTOR_COUNT + 5850);//9:35
    speed_mode[3]=(u32)(-0.1094*MAX_MOTOR_COUNT + 5350);//8

    senddata(REVISION_CMD, REVISION);
    Delay_ms(50);
    senddata(HANDLE_CHECK, 0);
    Delay_ms(50);

    while(1)
    {
        // check_limit();  // temporarily disabled: full command control
    }
}
