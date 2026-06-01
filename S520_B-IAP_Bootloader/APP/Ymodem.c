
#include "config.h"

u8 file_name[FILE_NAME_LENGTH];

/*************************************************************
  Function   : Int2Str
  Description : 整型转化成字符串
  Input      : str-字符串指针 intnum-转换值
  return     : none
*************************************************************/
void YModem_Int2Str(u8* str, s32 intnum)
{
    u32 i, Div = 1000000000, j = 0, Status = 0;

    for (i = 0; i < 10; i++)
    {
        str[j++] = (intnum / Div) + '0';//数字转化成字符

        intnum = intnum % Div;
        Div /= 10;
        if ((str[j-1] == '0') & (Status == 0))//忽略最前面的'0'
        {
            j = 0;
        }
        else
        {
            Status++;
        }
    }
}

/*************************************************************
  Function   : YModem_RecvByte
  Description : ymodem接收一个字节
  Input      : c-存放接收到的字节 timeout-超时时间
  return     : none
*************************************************************/
s8 YModem_RecvByte(USART_TypeDef* USARTx,u8 *c, u32 timeout)
{
    while(timeout-- > 0)
    {
        if(IAP_SerialGetByte(USARTx,c) == 1)
        {
            return 0;
        }
    }
    return -1;
}

/*************************************************************
  Function   : YModem_SendByte
  Description : 发送一个字节
  Input      : c-要发送的字节
  return     : none
*************************************************************/
void YModem_SendByte(USART_TypeDef* USARTx,u8 c)
{
    IAP_SerialSendByte(USARTx,c);
}

/*************************************************************
  Function   : UpdateCRC16
  Description : 计算一个字节的CRC16校验码(CRC16-CCITT欧洲标准)
  Input      : crcIn-上一次的CRC码
               byte-一个字节
  return     : 返回crc码
*************************************************************/
u16 UpdateCRC16(uint16_t crcIn, uint8_t byte)
{
    uint32_t crc = crcIn;
    uint32_t in = byte|0x100;
    do
    {
        crc <<= 1;
        in <<= 1;
        if (in&0x100)
            ++crc;      //crc |= 0x01
        if (crc&0x10000)
            crc ^= 0x1021;
    }
    while (!(in&0x10000));
    return crc&0xffffu;
}

/*************************************************************
  Function   : Cal_CRC16
  Description : 计算数据的CRC码
  Input      : data-要计算的数据
               size-数据的大小
  return     : 返回计算出的CRC码
*************************************************************/
u16 Cal_CRC16(const uint8_t* data, uint32_t size)
{
    uint32_t crc = 0;
    const uint8_t* dataEnd = data+size;
    while (data<dataEnd)
        crc = UpdateCRC16(crc,*data++);

    crc = UpdateCRC16(crc,0);
    crc = UpdateCRC16(crc,0);
    return crc&0xffffu;
}

/*************************************************************
  Function   : YModem_RecvPacket
  Description : 接收一个数据包
  Input      : data-存放接收到的数据
               length-数据包的长度
               timeout-超时时间
  return     : 0 -正常接收  -1 -接收错误
*************************************************************/
s8 YModem_RecvPacket(u8 device_id,USART_TypeDef* USARTx,u8 *data, s32 *length, u32 timeout)
{
    u16 i, packet_size;
    u8 c;
    static u8 update_led_count = 0;
    u8  indicate_time = 0;
	
    *length = 0;
    if(YModem_RecvByte(USARTx,&c, timeout) != 0)//接收数据包的第一个字节
    {
        // IAP_SerialSendStr(USART3,"\r\n ur1 \r\n");
        return -1;
    }
    switch(c)
    {
        case SOH:               //128字节数据包
            packet_size = PACKET_SIZE;  //记录数据包的长度
			break;
        case STX:               //1024字节数据包
            packet_size = PACKET_1K_SIZE;   //记录数据包的长度
            break;
        case EOT:               //数据接收结束字符
            // IAP_SerialSendStr(USART3,"\r\n ur2 \r\n");
            return 0;                       //接收结束
        case CA:                //接收中止标志
            if((YModem_RecvByte(USARTx,&c, timeout) == 0) && (c == CA))//等待接收中止字符
            {
                *length = -1;       //接收到中止字符
                // IAP_SerialSendStr(USART3,"\r\n Ymode step4 \r\n");
                return 0;
            }
            else                //接收超时
            {
                return -1;
            }
        case ABORT1:                //用户终止，用户按下'A'
        case ABORT2:                //用户终止，用户按下'a'
            // IAP_SerialSendStr(USART3,"\r\n ur3 \r\n");
            return 1;                       //接收终止
        default:
            // IAP_SerialSendStr(USART3,"\r\n ur4 \r\n");
            return -1;                      //接收错误
    }
    *data = c;                          //保存第一个字节
    for(i = 1; i < (packet_size + PACKET_OVERHEAD); i++)//接收数据
    {
        if(YModem_RecvByte(USARTx,data + i, timeout) != 0)
        {
            // IAP_SerialSendStr(USART3,"\r\n ur5 \r\n");
            return -1;
        }
        else  //正常接收
        {    
            if(device_id == MAJOR_DEVICE)
            {
                indicate_time = 50;
            }
            else if(device_id == MINOR_DEVICE)
            {
                indicate_time = 100;
            }
			
            if(++update_led_count==indicate_time)
            {
                GPIO_SetBits(UPG_INDICATE_LED_PORT,UPG_INDICATE_LED_PIN);
            }
            else if(update_led_count==(indicate_time+indicate_time))
            {
                GPIO_ResetBits(UPG_INDICATE_LED_PORT,UPG_INDICATE_LED_PIN);
                update_led_count = 0;
            }
        }
    }
    if(data[PACKET_SEQNO_INDEX] != ((data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff))  //判断序号是否正确
    {
        return -1;                        //接收错误
    }
    *length = packet_size;               //保存接收到的数据长度
    return 0;                            //正常接收
}

/*************************************************************
  Function   : YModem_Receive
  Description : ymodem接收
  Input      : buf-存放接收到的数据
  return     :  0 -发送端传输中止
               -1 -固件过大
               -2 -flash烧写错误
               -3 -用户终止
*************************************************************/
u8 *file_ptr, *buf_ptr;
u8  *upgfile = NULL;
u8 packet_data[PACKET_SIZE + PACKET_OVERHEAD], file_size[FILE_SIZE_LENGTH];
u8 code_len[1];
s32 packet_length = 0, size = 0;
u32 i = 0,RamSource = 0;

u32 asfasf_code;
s32 YModem_Receive(u8 device_id,USART_TypeDef* USARTx,u8 *buf)
{
   
    u8 session_done, file_done, session_begin, errors;
    u16 packets_received;
    
    u16 tempCRC = 0,packet_size_tmep = 0;
    s8  receive_status = 0;
    u16 RecCRCData = 0;
   
    u8  check_sum = 0;
    u8  rx_data = 0;
    u8  rx_flag = 0;

    session_done = 0;
    errors = 0;
    session_begin = 0;
    packets_received = 0;
    file_done = 0;
    buf_ptr = buf;

    while(1) //进入等待接收
    {
        receive_status = YModem_RecvPacket(device_id,USARTx,packet_data, &packet_length, NAK_TIMEOUT);
        IWDG_Feed();
        switch(receive_status)
        {
            case 0:     //正常接收
                errors = 0;
				if(device_id == MINOR_DEVICE)
                {
                    if(rx_flag)
                    {
						if(packet_length>0)
						{
		                //    IAP_SerialSendStr(USART1,":");
						   Delay_ms(5);
						}
                    }
				}
				
                switch(packet_length)
                {
                    case -1: //发送端中止传输
                        //YModem_SendByte(USARTx,ACK);//回复ACK
                        YModem_SendByte(USARTx,CA);//回复ACK
                        return 0;
                    case  0:    //接收结束
                        YModem_SendByte(USARTx,ACK);
                        file_done = 1;//接收完成
                        rx_flag = 0;
                        if(size == 0)size = 1;
						
                        if(device_id == MINOR_DEVICE)
                        {
                           Delay_ms(5);
						//    IAP_SerialSendStr(USART1,"#");
						//     IAP_SerialSendStr(USART3,"\r\n minor update grade successful.\r\n");
                        }
						else 
						{
                            // IAP_SerialSendStr(USART3,"\r\n major update grade successful.\r\n"); 
						}
                        break;
                    default: //接收数据中
                        if((packet_data[PACKET_SEQNO_INDEX] & 0xff) != (packets_received & 0xff)) //判断序号和补码是否正确
                        {
                            // IAP_SerialSendStr(USART3,"\r\n code_err nck \r\n");
                            YModem_SendByte(USARTx,NAK);//接收错误的数据，回复NAK
                            return -5;
                        }
                        else//接收到正确的数据
                        {
                            if(packet_length>=PACKET_1K_SIZE)
                            {
                                packet_size_tmep = PACKET_1K_SIZE;
                                RecCRCData = ((u16)packet_data[1027]<<8)|packet_data[1028];
                            }
                            else
                            {
                                packet_size_tmep = PACKET_SIZE;
                                RecCRCData = ((u16)packet_data[131]<<8)|packet_data[132];
                            }

                            tempCRC = Cal_CRC16(&packet_data[3],packet_size_tmep);//计算校验码

                            if(tempCRC != RecCRCData)
                            {
                                // IAP_SerialSendStr(USART3,"\r\n crc_err \r\n");
                                YModem_SendByte(USARTx,CA);
                                YModem_SendByte(USARTx,CA);//连续发送2次中止符CA
                                return -4;         //CRC校验失败
                            }

                            if(packets_received == 0)//接收第一帧数据
                            {
                                if(device_id == MAJOR_DEVICE)
                                {
                                    upgfile = MAJOR_MCU;
                                }
                                else if(device_id == MINOR_DEVICE)
                                {
                                    upgfile = MINOR_MCU;
                                }

                                if(packet_data[PACKET_HEADER] != 0)//包含文件信息：文件名，文件长度等
                                {
                                    for(i = 0, file_ptr = packet_data + PACKET_HEADER; (*file_ptr != 0) && (i < FILE_NAME_LENGTH); )
                                    {
                                        if(*upgfile != *file_ptr)
                                        {
                                            YModem_SendByte(USARTx,CA);
                                            YModem_SendByte(USARTx,CA);//连续发送2次中止符CA
                                            return -6;  //文件名称错误
                                        }
                                        file_name[i++] = *file_ptr++;//保存文件名
                                        upgfile++;
                                    }

                                    file_name[i++] = '\0';//文件名以'\0'结束

                                    for(i = 0, file_ptr++; (*file_ptr != ' ') && (i < FILE_SIZE_LENGTH); )
                                    {
                                        file_size[i++] = *file_ptr++;//保存文件大小
                                    }

                                    file_size[i++] = '\0';//文件大小以'\0'结束
                                    size = atoi((const char *)file_size);//将文件大小字符串转换成整型

                                    if(size > (FLASH_SIZE -1))//升级固件过大
                                    {
                                        YModem_SendByte(USARTx,CA);
                                        YModem_SendByte(USARTx,CA);//连续发送2次中止符CA
                                        return -1;//返回
                                    }
                                    if(device_id == MAJOR_DEVICE)
                                    {
                                        IAP_FlashEease(size);//擦除相应的flash空间
                                        IAP_UpdateParam(&size);//将size大小烧写进Flash中Parameter区
                                        //  IAP_SerialSendStr(USART3,"\r\n Ymode step5\r\n");
                                    }
                                    YModem_SendByte(USARTx,ACK);//回复ACk
									YModem_SendByte(USARTx,C);//发送'C',询问数据
                                }
                                else//文件名数据包为空，结束传输
                                {
                                    YModem_SendByte(USARTx,ACK);//回复ACK
                                    file_done = 1;      //停止接收
                                    session_done = 1;   //结束对话
                                    break;
                                }
                            }
                            else //收到数据包
                            {
                                if(device_id == MAJOR_DEVICE)
                                {
                                    memcpy(buf_ptr, packet_data + PACKET_HEADER, packet_length);//拷贝数据
                                    RamSource = (u32)buf;//8位强制转化成32位数据
                                    if(IAP_UpdateProgram(RamSource, packet_length) != 0)        //烧写升级数据
                                    {
                                        YModem_SendByte(USARTx,CA);
                                        YModem_SendByte(USARTx,CA);//flash烧写错误，连续发送2次中止符CA
                                        return -2;//烧写错误
                                    }
									YModem_SendByte(USARTx,ACK);//flash烧写成功，回复ACK
                                }
                                else if(device_id == MINOR_DEVICE)
                                {
    
                                }
								
                            }
                            packets_received++;//收到数据包的个数
                            session_begin = 1;//设置接收中标志
                        }
                }
                break;
            case 1:    //用户终止
                YModem_SendByte(USARTx,CA);
                YModem_SendByte(USARTx,CA);    //连续发送2次中止符CA
                return -3;        //烧写中止
                break;
            default:  //接收超时或接收错误
                if(session_begin > 0)   //传输过程中发生错误
                {
                    errors++;
                }
                if(errors > MAX_ERRORS) //错误超过上限
                {
                    YModem_SendByte(USARTx,CA);
                    YModem_SendByte(USARTx,CA);//连续发送2次中止符CA
                    return 0;   //传输过程发生过多错误
                }
                YModem_SendByte(USARTx,C); //发送'C',继续接收
                break;
                break;
        }
        if((file_done != 0)|| (session_done != 0))//文件接收完毕或对话结束，退出循环
        {
            break;
        }
    }
    return (s32)size;//返回接收到文件的大小
}

