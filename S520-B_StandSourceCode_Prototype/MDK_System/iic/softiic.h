#ifndef __SOFTIIC_H
#define __SOFTIIC_H
#include "sys.h"




#if TEST_SOFT_IIC
    void _IIC_Start(void);
    void _IIC_Stop(void);
    uint8_t _IIC_Wait_Ack(void);
    void _IIC_Ack(void);
    void _IIC_NAck(void);
    void _IIC_Send_Byte(uint8_t txd);
    uint8_t _IIC_Read_Byte(unsigned char ack);
#endif /*TEST_SOFT_IIC*/













#endif /*__SOFTIIC_H*/
