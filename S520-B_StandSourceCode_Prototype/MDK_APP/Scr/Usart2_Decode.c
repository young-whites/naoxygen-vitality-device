#include "Usart2_Decode.h"
#include "iap_upgrade.h"


/*****************************************************************************
* 功能:		CRC校验码计算（采用Crc16Modbus标准多项式）
* 说明:		校验步骤：
			1、 设置 CRC 寄存器， 并给其赋值 FFFF(hex)
			2、 将数据的第一个 8-bit 字符与 16 位 CRC 寄存器的低 8 位进行异或， 并把结果 存入 CRC 寄存器
			3、 CRC 寄存器向右移一位， MSB 补零， 移出并检查 LSB
			4、 如果 LSB 为 0， 重复第三步； 若 LSB 为 1， CRC 寄存器与多项式码相异或
			5、 重复第 3 与第 4 步直到 8 次移位全部完成。 此时一个 8-bit 数据处理完毕
			6、 重复第 2 至第 5 步直到所有数据全部处理完成
			7、 最终 CRC 寄存器的内容即为 CRC 值
			8、 CRC(16 位)多项式为 X16+X15+X2+1， 其对应校验二进制位列为 1 1000 0000 0000 0101
*****************************************************************************/
uint16_t CrcCalc_Crc16Modbus(uint8_t *dat,uint8_t len)
{
	uint16_t	CRC_index = 0xffff;
	uint16_t	buffer;
	volatile	uint8_t	i = 0, j = 0;
	for(i = 0; i < len; i++){
		buffer = dat[i];							// 把数据取出来放在缓冲区
		CRC_index ^= buffer;
		for(j = 0; j < 8; j++){
			if(CRC_index & 0x0001){
				CRC_index >>= 1;
				CRC_index ^= 0xa001;
			}else{
				CRC_index >>= 1;
			}
		}
	}
	return CRC_index;
}





static  unsigned    char    Data_count=0;
static  unsigned    char    Decode_buffer[30];
/********************************************************************
*函数功能：接收数据解码处理。
*入口参数：unsigned  char data。
*返    回：无。
*备    注：无。
********************************************************************/
void USART2_Decode_Receive_Command(unsigned  char data)
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
                USART2_Deal_correct_data(Decode_buffer + 1, Data_len);
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
void USART2_Send_Command_to_Principal(u8 Datalength,u8 Cmdtype,u8 Cmdstatus,u8 *Datafile)
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
		
	 Uart_Send_Upat(&USART2_SendBuf,send_data,7+Datalength+2);
}










/*****************************向APP发送信息***************************
*函数功能：向APP发送信息
*入口参数：void
*返    回：void
*备    注：无
********************************************************************/
void USART2_Order_to_Andriod(uint8_t Order)
{
    uint8_t temp_buffer[20] = { 0 };
    switch (Order)
    {

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
void USART2_Deal_correct_data(unsigned  char  *buf, unsigned char len)
{
    if(len == 0)    return;                             //设备ID2个字节已判断过,此处忽略
    switch (buf[2])
    {
        case FRAME_TYPE_ACT: 
        {
           if(buf[3] == 0x02)                           //发送的指令或响应
            {
                switch(buf[4])
                {    

                    // APP控制设备启动指令--------------------------------------------------------------------------------------------- 
                    case FRAME_APP_SET_WORK_SWITCH_CMD:
                    {
                        if(buf[5] == ENABLE){
                            Flag.WorkStatus = 1;
                            macValve_3_ON();
                            Compressor_ON();
                        }
                        else if(buf[5] == DISABLE){
                            Flag.WorkStatus = 0;
                        }
                    }break;


                    // APP控制机械手柄的推杆运动指令------------------------------------------------------------------------------------
                    case FRAME_APP_SET_MACHINE_HAND_CMD:
                    {
                        if(Flag.Hand_ReturnStatus == 0){
                            switch(buf[5]){
                            case Dirction_back:     USART4_Order_to_Mechanical_Handle(Order_HAND_ACT_CTRL_BACK);break;
                            case Dirction_forward:  USART4_Order_to_Mechanical_Handle(Order_HAND_ACT_CTRL_FORWARD);break;
                            case Dirction_reset:    USART4_Order_to_Mechanical_Handle(Order_HAND_ACT_CTRL_RESET);break;
                            case Dirction_stop:     USART4_Order_to_Mechanical_Handle(Order_HAND_ACT_CTRL_STOP);break;
                            case Dirction_back_stop:USART4_Order_to_Mechanical_Handle(Order_HAND_ACT_CTRL_BACK_STOP);break;
                            default:break;
                            }
                        }
                    }break;                    


                    default:
                        break;
                }
            }
        }
        break;



        case FRAME_TYPE_SET: 
        {
            if(buf[3] == 0x02)
            {
                switch(buf[4])
                {

                }
            }
        }
        break;   



        case FRAME_TYPE_GET: 
        {
           if(buf[3] == 0x02)                         
           {
                switch(buf[4])
                {   

                }
           }  
        }
        break;

        default:    break;  
   }
}