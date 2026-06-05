#include   "Decode.h"
#include   "crc16.h"
#include   "motor.h"

DecodeReceiveDataTypedef   Bluetooth_COM = DecodeReceiveDataTypedef_Defaults; //Bluetooth decode buffer
u8  BlueToothDecode_COMStatu = FRAMEHEADER; //Bluetooth decode step


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
        case SENDCMD://Send command
            {
                CMD_MessageHandle( Usart_Num, ( *Decode_Buffer ).DataLength - 3, ( *Decode_Buffer ).DataField + 4 );
            }
            break;
        case SENDDATA_STATUS://Send data or status
            {
                SENDDATA_STATUS_MessageHandle( Usart_Num, ( *Decode_Buffer ).DataLength - 3, ( *Decode_Buffer ).DataField + 4 );
            }
            break;
        default:
            break;
    }
}


u8 motor_mode = 2;  /* default speed mode */

void CMD_MessageHandle( u8 Usart_Num, u8 data_length, u8 *data_buffer )
{
    char temp_buffer[20], i;

    for ( i = 0; i < data_length; i++ )
    {    
        temp_buffer[i] = data_buffer[i];
    }

    
    if ( Usart_Num == Channel )
    {
        if ( temp_buffer[0] == NORMAL_DATA_SEND )
        {
            switch ( temp_buffer[1] )
            {
                case MOTOR_MODE_CMD:
                {
                    if (temp_buffer[2] == 0 || temp_buffer[2] == 5)  /* stop */
                    {
                        motor_stop();
                    }
                    else if (temp_buffer[2] >= 1 && temp_buffer[2] <= 4)  /* step forward */
                    {
                        motor_forward(motor_mode);
                    }
                    break;
                }
                case MOVE_CMD:
                {
                    if (temp_buffer[2] == 0)       /* stop */
                    {
                        motor_stop();
                    }
                    else if (temp_buffer[2] == 2)  /* forward */
                    {
                        motor_forward(5);
                    }
                    else if (temp_buffer[2] == 1)  /* backward */
                    {
                        motor_backward();
                    }
                    else if (temp_buffer[2] == 3)  /* reset: backward + notify */
                    {
                        motor_backward();
                        senddata(0x2F, 1);  // RETURN_FLAG_CMD = 1, motor returning
                    }
                    else if (temp_buffer[2] == 4)  /* backward 0.6s then stop */
                    {
                        motor_backward();
                        Delay_ms(600);
                        motor_stop();
                    }
                    break;
                }
                case LITTLE_CMD:
                {
                    /* Speed mode selection:
                       data[2]==5       -> mode 0 (slowest)
                       data[2]==11~13   -> mode 1
                       data[2]==20      -> mode 2 (default)
                       data[2]==27~29   -> mode 3 (fastest) */
                    if (temp_buffer[2] == 5)
                        motor_mode = 0;
                    else if (temp_buffer[2] > 10 && temp_buffer[2] < 14)
                        motor_mode = 1;
                    else if (temp_buffer[2] == 20)
                        motor_mode = 2;
                    else if (temp_buffer[2] > 26 && temp_buffer[2] < 30)
                        motor_mode = 3;
                    break;
                }
                case ACK_CMD:
                {
                    senddata(HANDLE_CHECK, 0);
                    break;
                }
                default:
                    break;
            }
            /* ACK: echo back function code and action value */
            send_ack(temp_buffer[1], temp_buffer[2]);
        }
    }
}

/**************************************************************************
//Function: void SENDDATA_STATUS_MessageHandle(u8 Usart_Num,u8 data_length,u8 *data_buffer)
//Purpose:  Send data/status message handler
//Params:   data_length -- data length, data_buffer -- data buffer
//Return:   void
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
        if(temp_buffer[0] == NORMAL_DATA_SEND)
        {
            switch(temp_buffer[1])
            {  
                case REVISION_CMD:
                    senddata(REVISION_CMD,REVISION);
                    break;

                default:
                    break;
            }
            /* ACK: echo back function code and action value */
            send_ack(temp_buffer[1], temp_buffer[2]);
        }
    }
}

/****************************************************************
//Function: void CommunicationWithMainBoard(void)
//Purpose:  Communicate with main board (USART3 == Bluetooth)
//Params:   void
//Return:   void
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

void send_ack(u8 func_code, u8 action)
{
    u8 temp_buffer[4];
    temp_buffer[0]=func_code;
    temp_buffer[1]=action;
    temp_buffer[2]=motor.limit_front | (motor.limit_rear<<1) | (motor.override_front<<2) | (motor.override_rear<<3);
    temp_buffer[3]=motor.direction;
    SendDataPacketToPeripheral(4,SENDCMD,NORMAL_DATA_SEND,temp_buffer);
}
