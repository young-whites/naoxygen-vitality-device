#include "Usart4_Decode.h"





static  unsigned    char    Data_count=0;
static  unsigned    char    Decode_buffer[30];
/********************************************************************
*函数功能：接收数据解码处理。
*入口参数：unsigned  char data。
*返    回：无。
*备    注：无。
********************************************************************/
void USART4_Decode_Receive_Command(unsigned  char data)
{
    static  unsigned    char    Decode_step = 0;
    static  unsigned    char    Data_len = 0;        //数据长度
    static  unsigned    char    CRC16_H = 0, CRC16_L = 0; //CRC校验位的高低字节
    static  unsigned    int     CRC16_Value = 0;     //CRC16校验位		

    switch(Decode_step)
    {
		
		//比对帧头第一个字节 -- 0x55  -----------------------------------------------------
        case 0:
        {
            if(data==0x55){
                Decode_step = 1;
            }
        }
        break;

		//比对帧头第一个字节 -- 0xAA  -----------------------------------------------------
        case 1:
        {
            if(data == 0xAA){
                Decode_step = 2;
            }
            else{
                Decode_step = 0;
            }
        }
        break;
		
		//设定数据长度 -- Data_len  -------------------------------------------------------
        case 2:                                    
        {
            Data_len = data;
            Data_count = 0;
            Decode_buffer[Data_count] = Data_len;
            Data_count++;
            Decode_step = 3;
        }
        break;
		//设备ID高位 -- DEVICE_ID_H  -------------------------------------------------------
        case 3:                                  
        {  
            if(data == DEVICE_ID_H){
                Decode_step = 4;
                Decode_buffer[Data_count] = data;
                Data_count++;
            }
            else{
                Decode_step = 0;
            }
        }
        break;
		
		//设备ID低位 -- DEVICE_ID_L  -------------------------------------------------------
        case 4:  
        {
            if(data == DEVICE_ID_L){
                Decode_step = 5;
                Decode_buffer[Data_count] = data;
                Data_count++;
            }
            else{
                Decode_step = 0;
            }
        }
        break;
		
		//命令类型  -------------------------------------------------------------------------
        case 5:
        {
            Decode_step = 6;
            Decode_buffer[Data_count] = data;
            Data_count++;
        }
        break;
		
		//发送命令状态及数据域并发送CRC高位  -------------------------------------------------
        case 6:
        {	
            if(Data_count < Data_len + 1){
                Decode_buffer[Data_count] = data;
                Data_count++;
            }
            else{
                CRC16_H = data;
                Decode_step = 7;
            }
        }
        break;
		
		//对比校验CRC低位  -------------------------------------------------------------------
        case 7:                              
        {
            CRC16_L = data;
            CRC16_Value = CrcCalc_Crc16Modbus(Decode_buffer, Data_len + 1);
            if(((CRC16_H << 8) | CRC16_L) == CRC16_Value){
                USART4_Deal_correct_data(Decode_buffer + 1, Data_len);
            }
            Decode_step = 0;
        }
        break;
    }
}




/**
 * @brief  发送数据到APP
 * @param  Datalength ： 数据长度
 *         Cmdtype    ： 命令类型
 *         Cmdstatus  ： 命令状态
 *         Datafile   ： 数据内容
 * @retval 无
 */
void USART4_Send_Command_to_Principal(u8 Datalength,u8 Cmdtype,u8 Cmdstatus,u8 *Datafile)
{
    uint8_t  send_data[60];
    u8  i;
    u16 temp_crc16 = 0;
    send_data[0]=0x55;
    send_data[1]=0xAA;
    send_data[2] = 4 + Datalength;
    send_data[3] =DEVICE_ID_H;
    send_data[4] =DEVICE_ID_L;
    send_data[5] =Cmdtype;
    send_data[6] =Cmdstatus;
    for(i=0; i<Datalength; i++){
        send_data[7+i]=Datafile[i];
    }
     temp_crc16=CrcCalc_Crc16Modbus(send_data+2, 5 + Datalength);
     send_data[6 + Datalength + 1] =(u8)(temp_crc16 >> 8);
     send_data[6 + Datalength + 2] =(u8)(temp_crc16);
		
	 Uart_Send_Upat(&UART4_SendBuf,send_data,7+Datalength+2);
}



/*****************************向手柄发送信息***************************
*函数功能：向APP发送信息
*入口参数：void
*返    回：void
*备    注：无
********************************************************************/
void USART4_Order_to_Mechanical_Handle(uint8_t Order)
{
    uint8_t temp_buffer[20] = { 0 };
    switch (Order)
    {
        case Order_HAND_ACT_WORK_MODE_ZERO_CMD:  // Stop motor
            temp_buffer[0] = 0x17;  // MOTOR_MODE_CMD
            temp_buffer[1] = 0x02;  // NORMAL_DATA_SEND
            temp_buffer[2] = 0x00;  // Stop
            USART4_Send_Command_to_Principal(3, 0x31, 0x02, temp_buffer);
            break;

        case Order_HAND_ACT_CTRL_FORWARD:  // Manual forward
            temp_buffer[0] = 0x2D;  // MOVE_CMD
            temp_buffer[1] = 0x02;  // NORMAL_DATA_SEND
            temp_buffer[2] = 0x02;  // Forward
            USART4_Send_Command_to_Principal(3, 0x31, 0x02, temp_buffer);
            break;

        case Order_HAND_ACT_CTRL_BACK:  // Manual back
            temp_buffer[0] = 0x2D;  // MOVE_CMD
            temp_buffer[1] = 0x02;  // NORMAL_DATA_SEND
            temp_buffer[2] = 0x01;  // Back
            USART4_Send_Command_to_Principal(3, 0x31, 0x02, temp_buffer);
            break;

        case Order_HAND_ACT_CTRL_STOP:  // Stop
            temp_buffer[0] = 0x2D;  // MOVE_CMD
            temp_buffer[1] = 0x02;  // NORMAL_DATA_SEND
            temp_buffer[2] = 0x00;  // Stop
            USART4_Send_Command_to_Principal(3, 0x31, 0x02, temp_buffer);
            break;

        case Order_HAND_ACT_CTRL_RESET:  // Reset to rear
            temp_buffer[0] = 0x2D;  // MOVE_CMD
            temp_buffer[1] = 0x02;  // NORMAL_DATA_SEND
            temp_buffer[2] = 0x03;  // Reset
            USART4_Send_Command_to_Principal(3, 0x31, 0x02, temp_buffer);
            break;

        case Order_HAND_ACT_CTRL_BACK_STOP:  // Back 0.6s then stop
            temp_buffer[0] = 0x2D;  // MOVE_CMD
            temp_buffer[1] = 0x02;  // NORMAL_DATA_SEND
            temp_buffer[2] = 0x04;  // Back-stop
            USART4_Send_Command_to_Principal(3, 0x31, 0x02, temp_buffer);
            break;

        case Order_HAND_ACT_AUTO_CHECK:  // Calibrate motor
            temp_buffer[0] = 0x3D;  // CHECK_MOTOR_CMD
            temp_buffer[1] = 0x02;  // NORMAL_DATA_SEND
            temp_buffer[2] = 0x00;
            USART4_Send_Command_to_Principal(3, 0x31, 0x02, temp_buffer);
            break;

        case Order_HAND_ACT_SAVE_STEP_PROGRESS:  // Save position
            temp_buffer[0] = 0x3E;  // MOTOR_STEP_SAVE
            temp_buffer[1] = 0x02;  // NORMAL_DATA_SEND
            temp_buffer[2] = 0x00;
            USART4_Send_Command_to_Principal(3, 0x31, 0x02, temp_buffer);
            break;

        case Order_HAND_SEND_HAND_LOCATION:  // Request position
            temp_buffer[0] = 0x2E;  // MOTOR_STEP_CMD
            temp_buffer[1] = 0x02;  // NORMAL_DATA_SEND
            USART4_Send_Command_to_Principal(2, 0x32, 0x02, temp_buffer);
            break;

        case Order_HAND_SEND_HAND_VERSION:  // Request version
            temp_buffer[0] = 0x19;  // REVISION_CMD
            temp_buffer[1] = 0x02;  // NORMAL_DATA_SEND
            USART4_Send_Command_to_Principal(2, 0x32, 0x02, temp_buffer);
            break;

        default:
            break;
    }
}






/********************************************************************
*函数功能：处理CRC校验正确的数据并处理
*入口参数：unsigned  char *buf，unsigned char len
*返    回：无
*备    注：无
********************************************************************/
void USART4_Deal_correct_data(unsigned  char  *buf, unsigned char len)
{
    if(len == 0)    return;                             //Device ID 2 bytes already checked
    switch (buf[2])
    {
        case FRAME_TYPE_ACT:  // 0x31
        {
           if(buf[3] == 0x02)                          // Command or response
            {
                switch(buf[4])
                {
                    case 0x2E:  // MOTOR_STEP_CMD -- Handle position report
                    {
                        uint8_t location = buf[5];
                        // TODO: Update display / sync to APP
                        // Location is 0-100 percent
                    } break;

                    case 0x2F:  // RUTURN_FLAG_CMD -- Return status
                    {
                        if(buf[5] == 0) {
                            Flag.Hand_ReturnStatus = 0;  // Returned / timeout
                        } else if(buf[5] == 1) {
                            Flag.Hand_ReturnStatus = 1;  // Returning, ignore APP
                        }
                    } break;

                    case 0xBB:  // HANDLE_CHECK -- Handle detect / beep
                    {
                        // Beep briefly to confirm handle detected
                        BEEP_On();
                        Delay_ms(50);
                        BEEP_Off();
                    } break;

                    case 0x19:  // REVISION_CMD -- Version
                    {
                        uint8_t version = buf[5];
                        // TODO: Store/display handle version
                    } break;

                    default:
                        break;
                }
            }
        }
        break;

        case FRAME_TYPE_SET:  // 0x32
        {
            if(buf[3] == 0x02)
            {
                // Handle response to GET commands
            }
        }
        break;

        case FRAME_TYPE_GET:  // 0x33
        {
           if(buf[3] == 0x02)
           {
                // Handle proactive report data
           }
        }
        break;

        default:    break;
   }
}



