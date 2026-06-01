#ifndef __USART4_DECODE_H
#define __USART4_DECODE_H
#include "sys.h"









void USART4_Decode_Receive_Command(unsigned  char data);
void USART4_Send_Command_to_Principal(u8 Datalength,u8 Cmdtype,u8 Cmdstatus,u8 *Datafile);
void USART4_Order_to_Mechanical_Handle(uint8_t Order);
void USART4_Deal_correct_data(unsigned  char  *buf, unsigned char len);









#endif
