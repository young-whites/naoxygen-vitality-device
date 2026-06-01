#include "crc16.h"


u16 CRC16_Check(u8 *data, u8 len)
{
    u16 CRC_index = 0xffff;
    u16 buffer;
    volatile u8 i = 0, j = 0;
    for(i = 0; i < len; i++)
    {
        buffer = data[i];
        CRC_index ^= buffer;
        for(j = 0; j < 8; j++)
        {
            if(CRC_index & 0x0001)
            {
                CRC_index >>= 1;
                CRC_index ^= CRC16_value;
            }
            else
            {
                CRC_index >>= 1;
            }
        }
    }
    return  CRC_index;
}

