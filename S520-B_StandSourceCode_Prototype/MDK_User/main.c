#include <stdio.h>	 	 
#include <string.h>	
#include <stdarg.h>
#include "sys.h"




/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main ( void )
{	
    __disable_irq();
	ALL_Config();
    
    /*全局参数初始换*/
    SysFlag_Init();
	

    /* 开启所有中断 */
    __enable_irq();

    while ( 1 )
    {
        //-----------------------------------------------------------------------------------------------------------------
        delay_ms(2);        /* 串口2用于与蓝牙通讯 */
        while (USART2_QueueBuf.receive_signal_flag)
        {
            USART2_Decode_Receive_Command(UART_GetByte(&USART2_QueueBuf));
        }
        while (USART2_SendBuf.Send_Wait_DatNum)
        {
            USART2_DMA_Send();
        }
		
		delay_ms(2);        /* 串口4用于与手柄通讯 */
       while (UART4_QueueBuf.receive_signal_flag)
       {
           USART4_Decode_Receive_Command(UART_GetByte(&UART4_QueueBuf));
       }
       while (UART4_SendBuf.Send_Wait_DatNum)
       {
           UART4_DMA_Send();
       }
        
        //-----------------------------------------------------------------------------------------------------------------
        if(Flag.OldStart == 1){
            OldTest();
        }
        else if(Flag.OldStart == 0){

        }



	    /******************************升级指令检测*************************/
	    if (Flag.update)
	    {
            /* 这里必须先向Flash中写入标志位置1的值 */
		    STMFLASH_Write(FLASH_APP_UP_ADDR, &Flag.update, 1);
            /* 然后重新置0 */
            Flag.update = 0;
		    IWDG_Feed();
		    jump_sys_bootloader();
	    }
    }
}


/*********************************************END OF FILE**********************/

