
#include "timer.h"
#include "gpio.h"
#include "motor.h"
#include "usart.h"
#include "Decode.h"
#include "string.h"
#include "stm8s.h"


int main(void)
{
    disableInterrupts();
    CLK_HSIPrescalerConfig( CLK_PRESCALER_HSIDIV1 );
    GPIO_Config();
    USARTAPP_Config();
    TIM2_Config();
    enableInterrupts();

    motor.direction = 0;
    motor.limit_rear = 0;
    motor.limit_front = 0;

    Delay_ms(100);

    senddata(REVISION_CMD, REVISION);
    Delay_ms(50);
    senddata(HANDLE_CHECK, 0);
    Delay_ms(50);

    while(1)
    {
        accel_update();
        check_limit();
    }
}
