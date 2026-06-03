
#ifndef __CRC_16_H__
#define __CRC_16_H__

#include "stm8s.h"
#define CRC16_value  0xa001

u16 CRC16_Check(u8 *data, u8 len);

#endif
