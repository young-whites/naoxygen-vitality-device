#include   "Decode.h"
#include   "crc16.h"
#include   "motor.h"
#include   "eeprom.h"

DecodeReceiveDataTypedef   Bluetooth_COM = DecodeReceiveDataTypedef_Defaults; //蓝牙解码缓冲区
u8  BlueToothDecode_COMStatu = FRAMEHEADER; //蓝牙解码步骤


void   SendDataPacketToPeripheral( u8 Datalength, u8 Cmdtype, u8 Cmdstatus, u8 *Datafile )
{
    u8  send_data[20];
    u8  i;
    u16 temp_crc16 = 0;
    send_data[0] = 0x55;
    send_data[1] = 0xAA;
    send_data[2] = 4 + Datalength;
    send_data[3] = DeviceID_H;
    send_data[4] = DeviceID_L;
    send_data[5] = Cmdtype;
    send_data[6] = Cmdstatus;

    for ( i = 0; i < Datalength; i++ )
    {
        send_data[7 + i] = Datafile[i];
    }
    temp_crc16 = CRC16_Check( send_data + 2, 5 + Datalength );
    send_data[6 + Datalength + 1] = ( u8 )( temp_crc16 >> 8 );
    send_data[6 + Datalength + 2] = ( u8 )( temp_crc16 );
    UART_SendData(send_data, 7 + Datalength+2);
}


void ReceiveMessageDecode( u8 Usart_Num, USART_ReceiveDataTypedef *Uart_Device_Rx, DecodeReceiveDataTypedef *Decode_Buffer, u8 *Decode_ComStatu )
{
    u8 data_temp = 0;
    static u8 rx_count = 0;

    if ( ( *Uart_Device_Rx ).receive_signal_flag )
    {
        switch ( *Decode_ComStatu )
        {
            case FRAMEHEADER:
            {
                if ( rx_count == 0 )
                {
                    ( *Decode_Buffer ).FrameHeader = UART_GetByte( Uart_Device_Rx );
                    ( *Decode_Buffer ).FrameHeader <<= 8;

                    if ( ( *Decode_Buffer ).FrameHeader == 0x5500 )
                    {
                        rx_count++;
                    }
                    else
                    {
                        ( *Decode_Buffer ).FrameHeader = 0;
                    }
                }
                else
                {
                    ( *Decode_Buffer ).FrameHeader |= UART_GetByte( Uart_Device_Rx );

                    if ( ( *Decode_Buffer ).FrameHeader == 0x55AA )
                    {
                        *Decode_ComStatu = DATALENGTH;
                    }
                    else
                    {
                        *Decode_ComStatu = FRAMEHEADER;
                    }
                    rx_count = 0;
                }
            }
            break;

            case DATALENGTH:
            {
                ( *Decode_Buffer ).DataLength = UART_GetByte( Uart_Device_Rx );
                ( *Decode_Buffer ).DataField[0] = ( *Decode_Buffer ).DataLength;
                *Decode_ComStatu = DEVICEID;
            }
            break;

            case DEVICEID:
            {
                if ( rx_count == 0 )
                {
                    ( *Decode_Buffer ).DeviceIDH = UART_GetByte( Uart_Device_Rx );
                    ( *Decode_Buffer ).DataField[1] = ( *Decode_Buffer ).DeviceIDH;

                    if ( ( *Decode_Buffer ).DeviceIDH == DeviceID_H )
                    {
                        rx_count++;
                    }
                    else
                    {
                        rx_count = 0;
                    }
                }
                else
                {
                    ( *Decode_Buffer ).DeviceIDL = UART_GetByte( Uart_Device_Rx );
                    ( *Decode_Buffer ).DataField[2] = ( *Decode_Buffer ).DeviceIDL;

                    if ( ( *Decode_Buffer ).DeviceIDL == DeviceID_L )
                    {
                        *Decode_ComStatu = CMDTYPE;
                    }
                    else
                    {
                        *Decode_ComStatu = FRAMEHEADER;
                    }
                    rx_count = 0;
                }
            }
            break;



            case CMDTYPE:
            {
                ( *Decode_Buffer ).CmdType = UART_GetByte( Uart_Device_Rx );
                ( *Decode_Buffer ).DataField[3] = ( *Decode_Buffer ).CmdType;
                *Decode_ComStatu = CMDSTATU;
            }break;

            case CMDSTATU:
            {
                ( *Decode_Buffer ).CmdStatu = UART_GetByte( Uart_Device_Rx );
                ( *Decode_Buffer ).DataField[4] = ( *Decode_Buffer ).CmdStatu;
                *Decode_ComStatu = DATAFIELD;
            }break;



            case DATAFIELD:
            {
                rx_count++;
                ( *Decode_Buffer ).DataField[rx_count + 4] = UART_GetByte( Uart_Device_Rx );
                if ( rx_count == ( *Decode_Buffer ).DataLength - 4 ){
                    rx_count = 0;
                    *Decode_ComStatu = CRC16;
                }
            }break;
            

            case CRC16:
            {
                data_temp = UART_GetByte( Uart_Device_Rx );
                if ( rx_count == 0 ){
                    ( *Decode_Buffer ).Crc16 = ( ( u16 )data_temp ) << 8;
                    rx_count++;
                }
                else{
                    ( *Decode_Buffer ).Crc16 |= ( ( u16 )data_temp );
                    DecodeMessageHandle( Usart_Num, Decode_Buffer );
                    rx_count = 0;
                    *Decode_ComStatu = FRAMEHEADER;
                }
            }break;

            default:
                break;
        }
    }
}





void DecodeMessageHandle( u8 Usart_Num, DecodeReceiveDataTypedef *Decode_Buffer )
{
    switch ( ( *Decode_Buffer ).CmdType )
    {
        case SENDCMD://发送命令
            {
                CMD_MessageHandle( Usart_Num, ( *Decode_Buffer ).DataLength - 3, ( *Decode_Buffer ).DataField + 4 );
            }
            break;
        case SENDDATA_STATUS://发送数据或状态
            {
                SENDDATA_STATUS_MessageHandle( Usart_Num, ( *Decode_Buffer ).DataLength - 3, ( *Decode_Buffer ).DataField + 4 );
            }
            break;
        default:
            break;
    }
}

static void delay(u16 z)
{
    u16 x,y;
    for(x=1000;x>0;x--)
    for(y=z;y>0;y--);
} 



uint8_t adjust_mode = 0;
int adjust_Value = 0;
void CMD_MessageHandle( u8 Usart_Num, u8 data_length, u8 *data_buffer )
{
    char temp_buffer[20], i;

    for ( i = 0; i < data_length; i++ )
    {    
        temp_buffer[i] = data_buffer[i];
    }

    
    if ( Usart_Num == Channel )
    {
        if ( temp_buffer[0] == NORMAL_DATA_SEND ) //发送数据类型
        {
            switch ( temp_buffer[1] )
            {
                case MOTOR_MODE_CMD:
                {   
                    if((temp_buffer[2]==0)||(temp_buffer[2]==5))//停止
                    {
                        motor_stop();
                    }
                    else if((temp_buffer[2]==1)||(temp_buffer[2]==2)||(temp_buffer[2]==3)||(temp_buffer[2]==4))//前进
                    {
                        if(systemparameter.stop_flog!=2)
                        {
                            systemparameter.k_flag=0;//步骤前进
                            motor_forword(systemparameter.motor_mode);
                        }
                    }
                    break;
                }
                case CHECK_MOTOR_CMD:
                {   

                    systemparameter.k_flag=2;//校验电机前进
                    systemparameter.max_motor_count = MAX_MOTOR_COUNT;
                    systemparameter.motor_100 = MOTOR_100;
                    if(systemparameter.stop_flog!=2)
                    {
                        motor_forword(5);
                    }
                    else
                    {
                        motor_bank();
                    }
                    break;
                }
                case ACK_CMD:
                {
                    systemparameter.ack_flag=1;
                    senddata(HANDLE_CHECK,0);
                    break;
                }
                case MOVE_CMD:
                {
                    if(temp_buffer[2]==0)//停止
                    {
                        motor_stop();
                    }
                    else if(temp_buffer[2]==2)//前
                    {
                        systemparameter.k_flag=1;///手动前进
                        if(systemparameter.stop_flog!=2)
                        {
                            motor_forword(5);
                        }

                    } 
                    else if(temp_buffer[2]==1) // 退
                    {
                        if(systemparameter.stop_flog!=1)
                        {
                            motor_bank();
                        }
                    }                   
                    else if(temp_buffer[2]==3) // 复位
                    {
                        if(systemparameter.stop_flog!=1)
                        {
                            motor_bank();
                            SendRuturnFlag(1); // 通知主板，电机正在退后，APP发来的指令主板不理睬，并且主板通知APP电机正在退回。
                        }
                    }
                    else if (temp_buffer[2]==4) // 退回0.6s，再停这里是步骤到时间停止，主板里面还有一个手动停止
                    {
                        if(systemparameter.stop_flog!=1)
                        {
                            motor_bank();
                            if(systemparameter.IfRuturning==0)// 没有正在回退
                            {
                                systemparameter.tim1_count_cnt1 += 252;
                                
                                delay(1250);
                                motor_stop();
                            }
                        }
                    }
                    break;
                }
                case LITTLE_CMD:
                {
                    if(temp_buffer[2]==20)//9
                    {
                        systemparameter.motor_mode=2;
                        motor_forword(systemparameter.motor_mode);
                    }
                    else if((temp_buffer[2]>10)&&(temp_buffer[2]<14))//10
                    {
                        systemparameter.motor_mode=1;
                        motor_forword(systemparameter.motor_mode);
                    }
                    else if((temp_buffer[2]>26)&&(temp_buffer[2]<30))//8
                    {
                        systemparameter.motor_mode=3;
                        motor_forword(systemparameter.motor_mode);
                    }
                    else if(temp_buffer[2]==5)//11
                    {
                        systemparameter.motor_mode=0;
                        motor_forword(systemparameter.motor_mode);
                    }
                    break;
                }
                case MOTOR_STEP_SAVE:
                {
                    motor_step_save();
                    break;
                }       



                case Speed_Adjust_Cmd:
                {
                    adjust_mode = temp_buffer[2];
                    adjust_Value = temp_buffer[3];
                    /* 写Flash */
                    EEPROM_WriteData(ADJUST_SPEED_MODE_ADDR, adjust_mode);
                    EEPROM_WriteData(ADJUST_SPEED_VALUE_ADDR, adjust_Value);

                    if (adjust_mode == 1) {
                        adjust_Value = -adjust_Value;
                    }

                    speed_mode[0] = (u32)(-0.1094 * systemparameter.max_motor_count + adjust_Value + 5740);//11
                    speed_mode[1] = (u32)(-0.1094 * systemparameter.max_motor_count + adjust_Value + 6200);//10
                    speed_mode[2] = (u32)(-0.1094 * systemparameter.max_motor_count + adjust_Value + 5790);//9:35
                    speed_mode[3] = (u32)(-0.1094 * systemparameter.max_motor_count + adjust_Value + 5350);//8

                    break;
                }


                case 0xF5:
                {
                    systemparameter.IfRuturning = temp_buffer[2];
                    break;
                }

                
                
                 default:
                break;
            }
        }
    }
}
/**************************************************************************
//函数名称:void SENDDATA_STATUS_MessageHandle(u8 Usart_Num,u8 data_length,u8 *data_buffer)
//函数功能:发送数据状态消息处理
//入口参数:data_length==数据长度        data_buffer==数据缓冲区
//返回类型:void
//作者:daniel
//日期:20181217
***************************************************************************/
void SENDDATA_STATUS_MessageHandle(u8 Usart_Num,u8 data_length,u8 *data_buffer)
{
    u8 temp_buffer[20],i;
    for(i=0; i<data_length; i++)
    {
        temp_buffer[i]=data_buffer[i];
    }
    if(Usart_Num == Channel)
    {
        if(temp_buffer[0] == NORMAL_DATA_SEND)//发送数据类型
        {
            switch(temp_buffer[1])
            {  
                case 0X2e://获得获取电机位置
                    SendDjData();
		        break;

                case REVISION_CMD: //读出当前的版本号
                    senddata(REVISION_CMD,REVISION);
                break;

                default:
                    break;
            }
        }
    }
}

/****************************************************************
//函数名称:void   CommunicationWithMainBoard(void)
//函数功能:跟主板通信 (      USART3==蓝牙接口)
//入口 参数:void
//返回类型:void
//作者：daniel
//日期:20181221
*****************************************************************/
void   CommunicationWithMainBoard( void )
{
    ReceiveMessageDecode( Channel, &USART1_QueueBuf, &Bluetooth_COM, &BlueToothDecode_COMStatu );
}

void senddata(u8 type, u8 data)
{
    u8 temp_buffer[2];
    temp_buffer[0]=type;
    temp_buffer[1]=data;
    SendDataPacketToPeripheral(2,SENDCMD,NORMAL_DATA_SEND,temp_buffer);
}

