#ifndef __TK130F_H
#define __TK130F_H
#include "sys.h"


#define             TK130F_GPIO_APBxClock_FUN           RCC_APB2PeriphClockCmd
#define             TK130F_GPIO_CLK                     RCC_APB2Periph_GPIOB
#define             TK130F_Ctrl_GPIO_PORT               GPIOB
#define             TK130F_Ctrl_GPIO_PIN                GPIO_Pin_10

#define				TK1300F_ON							GPIO_SetBits(TK130F_Ctrl_GPIO_PORT, TK130F_Ctrl_GPIO_PIN)
#define				TK1300F_OFF							GPIO_ResetBits ( TK130F_Ctrl_GPIO_PORT, TK130F_Ctrl_GPIO_PIN )

void Tk1300F_GPIO_Config(void);
void Tk1300F_Calculate(unsigned  char data);





#endif /*__TKAP100E_H*/
