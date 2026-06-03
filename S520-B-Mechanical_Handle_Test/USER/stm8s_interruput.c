#include "stm8s.h"
#include "stm8s_uart1.h"
#include "stm8s_uart3.h"
#include "stm8s_tim1.h"
#include "gpio.h"
#include "usart.h"
#include "motor.h"
#include "Decode.h"
#include "stm8s_tim2.h"


void Timing_10ms(void)
{


}



void Timing_1ms(void)
{
    /* protocol transmission */
      CommunicationWithMainBoard();
}



// TIM2 更新中断服务程序
#pragma vector = 0x0F
__interrupt void TIM2_IRQHandler(void)
{
    static uint32_t msCnt = 0;
    msCnt++;
    /*计数用于取余运算来判断时间事件*/
    if (++msCnt >= 60000)
    {
        msCnt = 0;
    }

    /*1s扫秒时间，取余为整数说明时间到进入执行函数*/
    if ((msCnt % 10) == 0)      Timing_10ms();
    if ((msCnt % 1) == 0)       Timing_1ms();


    TimingDelay_Decrement();
    /* Cleat Interrupt Pending bit */
    TIM2_ClearITPendingBit(TIM2_IT_UPDATE);
}


#pragma vector=0xD
__interrupt void TIM1_UPD_OVF_TRG_BRK_IRQHandler( void )
{
    /* TIM1 update interrupt - kept for PWM operation.
       Step counting and position tracking removed in refactor. */
    TIM1_ClearITPendingBit( TIM1_IT_UPDATE );
}

#pragma vector=0x14
__interrupt void UART1_RX_IRQHandler( void )
{
     u8 data = 0;
    if ( UART1_GetITStatus( UART1_IT_RXNE ) )
    {
        UART1_ClearITPendingBit( UART1_IT_RXNE );
        data = ( u8 )UART1_ReceiveData8();
        UART_Receive( &USART1_QueueBuf, data );
    }
}
