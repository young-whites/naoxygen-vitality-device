#include <applications/macAPP/Inc/rtt_uart2_Decode.h>
/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-06-05     zphu       the first version
 */

extern void Proportional_Flow_Mode_Config(Mode_StructType mode);
extern void Proportional_Takt_Mode_Config(Mode_StructType mode);
extern void Hand_Press_Mode_Config(Mode_StructType mode);


/*---------------------------------------------------------------------------------------------------------------*/
/* 以下是解码共用的函数                                                                                                                                                                                      */
/*---------------------------------------------------------------------------------------------------------------*/
/*****************************************************************************
* 功能:       CRC校验码计算（采用Crc16Modbus标准多项式）
* 说明:       校验步骤：
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
    uint16_t    CRC_index = 0xffff;
    uint16_t    buffer;
    volatile    uint8_t i = 0, j = 0;
    for(i = 0; i < len; i++){
        buffer = dat[i];                            // 把数据取出来放在缓冲区
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



#if VERSION_2point0_Method

/* 定义串口设备节点 */
rt_device_t  serial2;
/* 定义串口设备的名称 */
#define USART2_NAME "uart2"
/* 重定向串口配置结构体 */
struct serial_configure usart2Config = RT_SERIAL_CONFIG_DEFAULT;
/* 创建一个信号量用于接收单字节 */
rt_sem_t usart2_rec_sem = RT_NULL;
/* 创建串口2的缓冲区数组结构体 */
xUsart_Structure Uart2Buf;

/*---------------------------------------------------------------------------------------------------------------*/
/* 以下是串口1初始化的创建以及回调函数                                                                                                                                                          */
/*---------------------------------------------------------------------------------------------------------------*/
/**
  * @brief  串口2的中断回调函数
  * @retval rt_err_t
  */
rt_err_t Usart2_RX_Callback(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(usart2_rec_sem);
    return RT_EOK;
}


/**
  * @brief  串口2初始化函数
  * @retval int
  */
int USART2_Init(void)
{
    static rt_size_t sendNum = 0;

    // 创建动态信号量
    usart2_rec_sem = rt_sem_create("dynamic_sem2", 0, RT_IPC_FLAG_FIFO);
    if (usart2_rec_sem == RT_NULL){
        rt_kprintf("PRINTF:%d. create dynamic semaphore failed.\n",Record.kprintf_cnt++);
        return -1;
    }
    else{
        rt_kprintf("PRINTF:%d. create done. dynamic semaphore value = 0.\n",Record.kprintf_cnt++);
    }


    serial2 = rt_device_find(USART2_NAME);
    if(serial2 != RT_NULL){
        rt_kprintf("PRINTF:%d. Usart2 Device node created succeed! \r\n",Record.kprintf_cnt++);
        usart2Config.baud_rate = BAUD_RATE_9600;
        usart2Config.bufsz = 1024;
    }
    else {
        rt_kprintf("PRINTF:%d. Usart2 Device node created Failed! \r\n",Record.kprintf_cnt++);
        return RT_ERROR;
    }

    rt_device_control(serial2, RT_DEVICE_CTRL_CONFIG, &usart2Config);
    rt_device_open(serial2, RT_DEVICE_OFLAG_RDONLY | RT_DEVICE_FLAG_INT_RX);
    rt_device_set_rx_indicate(serial2, Usart2_RX_Callback);

    /* 初始化循环队列 */
    Uart2Buf.head = 0;
    Uart2Buf.tail = 0;
    Uart2Buf.lock = rt_mutex_create("uart2_buf_lock", RT_IPC_FLAG_FIFO);


    sendNum = rt_device_write(serial2, RT_NULL, "usart2 is opened!\r\n", 19);
    rt_kprintf("PRINTF:%d. The usart2 test send size : %d\r\n\n",Record.kprintf_cnt++,sendNum);

    return RT_EOK;
}



/*---------------------------------------------------------------------------------------------------------------*/
/* 以下是数据解析线程的创建以及回调函数                                                                                                                                                         */
/*---------------------------------------------------------------------------------------------------------------*/
/**
  * @brief  数据解码回调函数入口
  * @retval void
  */
void uart2_thread_entry(void* parameter)
{
    char recDat = 0;
    rt_size_t sizeValue = 0;
    uint8_t decodeStatus = 0;
    uint8_t usartID = 2;
    while(1)
    {
        sizeValue = rt_device_read(serial2, RT_NULL, &recDat, 1);
        if(sizeValue == 1){
            rt_sem_take(usart2_rec_sem, RT_WAITING_FOREVER);
            /* 加锁保护队列操作 */
            rt_mutex_take(Uart2Buf.lock, RT_WAITING_FOREVER);
            /* 计算下一个尾指针位置 */
            uint16_t next_tail = (Uart2Buf.tail + 1) % MAX_DATA_LENGTH;
            /* 队列未满 */
            if(next_tail != Uart2Buf.head) {
                Uart2Buf.rxBuffer[Uart2Buf.tail] = recDat;
                Uart2Buf.tail = next_tail;
            }
            else {
                rt_kprintf("UART2 Queue Full! Data Lost: 0x%02X\n", recDat);
            }
            /* 释放互斥锁 */
            rt_mutex_release(Uart2Buf.lock);
            /* 触发协议解析 */
            decodeStatus = USART2_Portocol_Get_Command(serial2,usartID);
            if(decodeStatus == CMD_TRUE) {
            }
        }
        rt_thread_mdelay(10);
    }
}





/**
  * @brief  初始化数据解码函数
  * @retval int
  */
rt_thread_t uart2_decodeThread_Handle;
int uart2_decodeThread_Init(void)
{
    uart2_decodeThread_Handle = rt_thread_create("uart2_thread_entry", uart2_thread_entry, RT_NULL, 1024, 10, 200);
    if(uart2_decodeThread_Handle != RT_NULL){
        rt_kprintf("PRINTF:%d. uart2 Thread is created!!\r\n",Record.kprintf_cnt++);
        USART2_Init();
        rt_thread_startup(uart2_decodeThread_Handle);
    }
    else {
        rt_kprintf("PRINTF:%d. Thread is not created!!\r\n",Record.kprintf_cnt++);
    }
    return RT_EOK;
}



/**
 * @brief 尝试获取一条指令
 * @param command 指令存放指针
 * @return 获取的指令长度
 * @retval 0 没有获取到指令
 */
uint8_t USART2_Portocol_Get_Command(rt_device_t dev, uint8_t USART_ID)
{
    uint8_t decode_result = CMD_ERROR;
    static uint8_t  Decode_Step = 0;
    static uint8_t  CMD_Length = 0;
    static uint8_t  CMD_buffer[64] = {0};
    static uint8_t  CMD_DataCnt = 0;
    static uint8_t  CRC16_H,CRC16_L = 0;
    static uint16_t CRC16_Value = 0;



    while(Uart2Buf.head != Uart2Buf.tail)
    {

        uint8_t DecodeDat = Uart2Buf.rxBuffer[Uart2Buf.head];
        Uart2Buf.head = (Uart2Buf.head + 1) % MAX_DATA_LENGTH;

        /* 然后可以进行正常的数据解析流程 */
        /*--------------------------------*/
        switch(Decode_Step)
        {
            /*****************                第一步数据解析               ****************************/
            case Decode_Step_0:
            {
                if(DecodeDat == 0x55){
                    Decode_Step = Decode_Step_1;
                }
            }break;

            /*****************                第二步数据解析               ****************************/
            case Decode_Step_1:
            {
                if(DecodeDat == 0xAA) {
                    Decode_Step = Decode_Step_2;
                }
                else {
                    Decode_Step = Decode_Step_0;
                }
            }break;

            /*****************                第三步数据解析               ****************************/
            case Decode_Step_2:
            {
                /* 获取指令长度数据（除指令包头的2个字节，长度1字节以及CRC校验的2字节以外的长度） */
                CMD_Length = DecodeDat;
                CMD_buffer[CMD_DataCnt++] = CMD_Length;
                Decode_Step = Decode_Step_3;
            }break;

            /*****************                第四步数据解析               ****************************/
            case Decode_Step_3:
            {
                if(DecodeDat == DEVICE_ID_H){
                    CMD_buffer[CMD_DataCnt++] = DecodeDat;
                    Decode_Step = Decode_Step_4;
                }
                else{
                    Decode_Step = Decode_Step_0;
                    CMD_DataCnt = 0;
                }
            }break;

            /*****************                第五步数据解析               ****************************/
            case Decode_Step_4:
            {
                if(DecodeDat == DEVICE_ID_L){
                    CMD_buffer[CMD_DataCnt++] = DecodeDat;
                    Decode_Step = Decode_Step_5;
                }
                else{
                    Decode_Step = Decode_Step_0;
                    CMD_DataCnt = 0;
                }
            }break;

            /*****************                第六步数据解析               ****************************/
            case Decode_Step_5:
            {
                CMD_buffer[CMD_DataCnt++] = DecodeDat;
                if(CMD_DataCnt >= (CMD_Length + 1)){
                    Decode_Step = Decode_Step_6;
#if USART2_REC_TEST_PRINTF
                    for(uint8_t d = 0; d < CMD_Length + 1;d++){
                        rt_kprintf("%x ",CMD_buffer[d]);
                    }
#endif
                }
            }break;

            /*****************                第七步数据解析               ****************************/
            case Decode_Step_6:
            {
                CRC16_H = DecodeDat;
#if USART2_REC_TEST_PRINTF
                rt_kprintf("%x ",CRC16_H);
#endif
                Decode_Step = Decode_Step_7;
            }break;

            /*****************                第八步数据解析               ****************************/
            case Decode_Step_7:
            {
                CRC16_L = DecodeDat;
#if USART2_REC_TEST_PRINTF
                rt_kprintf("%x ",CRC16_L);
#endif
                Decode_Step = Decode_Step_0;
            }break;

            default:{
                Decode_Step = Decode_Step_0;
            }break;
        }



        /* CRC校验处理 */
        if((Decode_Step == Decode_Step_0) && (CRC16_H != 0))
        {
            CRC16_Value = CrcCalc_Crc16Modbus(CMD_buffer, CMD_Length + 1);

            if(((CRC16_H << 8) | CRC16_L) == CRC16_Value)
            {
                if(USART_ID == 2)
                {
                    Protocol_Operation_USART2(dev,CMD_buffer);
                    #if USART2_REC_CMD_PRINTF
                        rt_kprintf("Valid   Command: ");
                        for(uint8_t i = 0; i < CMD_Length + 5; i++) {
                            rt_kprintf("%02X ", Uart2Buf.rxBuffer[(Uart2Buf.head - (CMD_Length + 5) + i) % MAX_DATA_LENGTH]);
                        }
                        rt_kprintf("\n");
                    #endif
                        decode_result = CMD_TRUE;
                }
            }
            else
            {
                rt_kprintf("InValid Command: ");
                for(uint8_t i = 0; i < CMD_Length + 5; i++) {
                    rt_kprintf("%02X ", Uart2Buf.rxBuffer[(Uart2Buf.head - (CMD_Length + 5) + i) % MAX_DATA_LENGTH]);
                }
                rt_kprintf("\n");
                rt_kprintf("CRC Error: Calc=0x%04X , Recv=0x%02X%02X\n",CRC16_Value, CRC16_H, CRC16_L);
            }
            /* 重置校验相关变量 */
            CRC16_H = 0;
            CRC16_L = 0;
            CMD_DataCnt = 0;
        }
    }
    return decode_result;
}





/**
 * @brief   STM32通过串口2发送指令到上位机进行应答
 * @param   CmdBuf      数据域存放的指针
 *          USART_ID    串口号
 * @retval  None
 */
void USART2_Send_Command_to_Principal(uint8_t DataLen, uint8_t CmdType, uint8_t CmdStatus, uint8_t* DataBuf)
{
    uint8_t SendDat[30] = {0};
    uint16_t Empty_CRC = 0;
    rt_device_t dev = RT_NULL;

    SendDat[0] = FRAME_HEAD1;
    SendDat[1] = FRAME_HEAD2;
    SendDat[2] = 4 + DataLen;
    SendDat[3] = DEVICE_ID_H;
    SendDat[4] = DEVICE_ID_L;
    SendDat[5] = CmdType;
    SendDat[6] = CmdStatus;
    for(uint8_t i = 0; i < DataLen; i++)
    {
        SendDat[7+i] = DataBuf[i];
    }
    Empty_CRC = CrcCalc_Crc16Modbus(SendDat + 2, 5 + DataLen);
    SendDat[6 + DataLen + 1] = (uint8_t)(Empty_CRC >> 8);
    SendDat[6 + DataLen + 2] = (uint8_t)(Empty_CRC);
    dev = rt_device_find("uart2");
    rt_enter_critical();
    rt_device_write(dev, RT_NULL, SendDat, 9+DataLen);
#if USART2_SEND_CMD_INFO_PRINTF
    rt_kprintf("LOG:%d. USART2 send to Android :: 55 aa ",Record.Log_Uart_cnt++);
    for(uint8_t i = 0; i<9+DataLen; i++){
        if(SendDat[i] <= 0x0F){
            rt_kprintf("0%x ",SendDat[i]);
        }
        else{
            rt_kprintf("%x ",SendDat[i]);
        }
    }
    rt_kprintf("\r\n");
#endif
    rt_exit_critical();
}




/**
 * @brief   解析串口2数据域指令，执行响应的函数(小手柄的通讯执行函数)
 * @param   CmdBuf      数据域存放的指针
 *          USART_ID    串口号
 * @retval  None
 */
void Protocol_Operation_USART2(rt_device_t dev,uint8_t* CmdBuf)
{
#if 1
    static uint8_t modeSet = 0;
    uint8_t DataBuf[20] = {0};
    switch(*(CmdBuf + 3))
    {
        case FRAME_TYPE_ACT:
        {
            switch(*(CmdBuf + 5))
            {
                case FRAME_APP_SET_WORK_SWITCH_CMD:
                {
                    if(*(CmdBuf + 6) == 0){
                        Flag.SysConnectStatus = 0;
                        Compressor_Off();
                        Fan_Off();
                        if(Flag.Hand_ReturnStatus == 0){
                            for(;modeSet < 1; modeSet = 2){
                                USART4_Order_to_Mechanical_Handle(Order_HAND_ACT_WORK_MODE_ZERO_CMD);
                            }
                        }
                    }
                    else if(*(CmdBuf + 6) == 1){
                        /* 蓝牙连接标志,并启动压缩机的标志 */
                        Flag.SysConnectStatus = 1;
                        rt_tick_t delay_tick = rt_tick_get();
                        rt_thread_delay_until(&delay_tick, 1000);
                        Compressor_On();
                        modeSet = 0;
                        USART4_Order_to_Mechanical_Handle(Order_HAND_ACT_SAVE_STEP_PROGRESS);
                    }
                }break;




                // APP控制手柄工作休眠状态指令----------------------------------------------------------------------------------------------------
                case FRAME_APP_SET_HAND_WORK_STATUS_CMD:
                {
                    LED_Blink(LED_Name_Message, 1, 0, 0);
                    // 接收上位机的暂停指令
                    if(*(CmdBuf + 6) == 0){
                        Record.WorkMode = 0;
                        /* 暂停指令标志位 */
                        Flag.APPWorkStatus = 0;
                        /* 暂停时也需要把电磁阀关闭,否则常开会导致发热 */
                        Main_Gas_Circuit_Off();
                        /* 暂停时,如果电机不处于回退状态,也发指令暂停手柄 */
                        if(Flag.Hand_ReturnStatus == 0){
                            USART4_Order_to_Mechanical_Handle(Order_HAND_ACT_WORK_MODE_ZERO_CMD);
                            /* 暂停电机运动 */
                            USART4_Order_to_Mechanical_Handle(Order_HAND_ACT_CTRL_STOP);
                        }
                        /* 设置输出的节拍模式和流量模式 */
                        Proportional_Flow_Mode_Config(MODE_ZERO);
                        Proportional_Takt_Mode_Config(MODE_FIVE);
                        if(Record.StartCnt == 1  && Record.SecondDown <= 10){
                            Record.StartCnt = 0;
                        }
                        LED_Off(LED_Name_Status);
                        /* 暂停时保存电机进度 */
                        USART4_Order_to_Mechanical_Handle(Order_HAND_ACT_SAVE_STEP_PROGRESS);
                    }
                    else if(*(CmdBuf + 6) == 1){
                        /* 启动时先把电磁阀开启,避免压力过大损伤压缩机(电磁阀在定时器中常闭,只有在气压超过限制时才会开启泄气) */
                        Record.WorkMode = 1;
                        Flag.Solenoid_Valve3_Limit = 1;
                        Flag.APPWorkStatus = 1;
                        Flag.SleepMode = 0;
                        Record.System_Sleep_Tick = 0;
                        Record.System_Sleep_Time = 0;
                        Fan_On();
                        Compressor_On();
                        LED_On(LED_Name_Status);
                        Record.StartCnt++;
                        /* 启动时电机处于回退状态,就给上位机发送处于回退状态指令 */
                        if(Flag.Hand_ReturnStatus == 1){
#if DEBUG_LOG
                            rt_kprintf("The hand is backing !\r\n");
#endif
                            USART2_Order_to_Andriod(Order_Android_ACT_HAND_RETURN_STATUS);
                        }
                        /* 设置手柄的工作模式 */
                        else if(Flag.Hand_ReturnStatus == 0){
                            USART4_Order_to_Mechanical_Handle(Order_HAND_ACT_WORK_MODE_CMD);
                        }
                    }
                }break;




                // APP配置流量模式指令------------------------------------------------------------------------------------------------------------------------
                case FRAME_APP_SET_FLOW_MODE_CMD:
                {
                    if(Record.HandReset == 0){
                        LED_Blink(LED_Name_Message, 1, 0, 0);
                        Record.Flow_mode = *(CmdBuf + 6);
                        Proportional_Flow_Mode_Config(Record.Flow_mode);
                        Proportional_Takt_Mode_Config(Record.Takt_Mode);
                    }

                }break;

                // APP配置PWM输出节拍模式------------------------------------------------------------------------------------------------------------
                case FRAME_APP_SET_TAKT_MODE_CMD:
                {
                    if(Record.HandReset == 0){
                        Record.Takt_Mode = *(CmdBuf + 6);
                        Proportional_Takt_Mode_Config(Record.Takt_Mode);
                    }
                }break;


                // APP配置喷射力度模式指令------------------------------------------------------------------------------------------------------------------------
                case FRAME_APP_SET_PRESSURE_MODE_CMD:
                {
                    LED_Blink(LED_Name_Message, 1, 0, 0);
                    Record.Hand_Press_Mode = *(CmdBuf + 6);
                    if(Flag.Hand_ReturnStatus == 0){
                        USART4_Order_to_Mechanical_Handle(Order_HAND_ACT_WORK_MODE_CMD);
                    }
#if DEBUG_LOG
                    rt_kprintf("LOG:%d. The Hand_Press_Mode is %d \r\n",Record.Log_Uart_cnt++,Record.Hand_Press_Mode);
#endif
                }break;


                // APP设置机械手柄速度调整指令
                case FRAME_APP_SET_HAND_ADJUST_SPEED_CMD:
                {
                    Record.HandSpeed = *(CmdBuf + 6);
                    USART4_Order_to_Mechanical_Handle(Order_HAND_ACT_ADJUST_SPEED);
                }break;

                // APP控制机械手柄的推杆运动指令----------------------------------------------------------------------------------------------------------------
                case FRAME_APP_SET_MACHINE_HAND_CMD:
                {
                    switch(*(CmdBuf + 6))
                    {
                        case Dirction_back:
                        {
                           Flag.StartCnt++;
                           Flag.BlueBeepCnt++;
                           USART4_Order_to_Mechanical_Handle(Order_HAND_ACT_CTRL_BACK);
                           LED_Blink(LED_Name_Message, 1, 0, 0);
                           Flag.HandDirction = 0;
                        }break;

                        case Dirction_forward:
                        {
                            Flag.StartCnt++;
                            Flag.BlueBeepCnt++;
                            if( Record.HandReset == 0){
                                USART4_Order_to_Mechanical_Handle(Order_HAND_ACT_CTRL_FORWARD);
                                LED_Blink(LED_Name_Message, 1, 0, 0);
                                Flag.HandDirction = 1;
                            }
                        }break;

                        case Dirction_reset:
                        {
                            Flag.StartCnt++;
                            Flag.BlueBeepCnt++;
                            LED_Blink(LED_Name_Message, 1, 0, 0);
                            Record.HandReset = 1;
                            USART4_Order_to_Mechanical_Handle(Order_HAND_SEND_HAND_LOCATION);
                            rt_thread_delay(50);
                            USART2_Order_to_Andriod(Order_Android_ACT_HAND_LOCATION_CMD);
                            rt_thread_delay(50);
                            USART4_Order_to_Mechanical_Handle(Order_HAND_ACT_CTRL_RESET);
                            Flag.HandDirction = 0;
                        }break;

                        case Dirction_stop:
                        {
                            Flag.StartCnt++;
                            Flag.BlueBeepCnt++;
                            if(Record.HandReset == 0){
                                LED_Blink(LED_Name_Message, 1, 0, 0);
                                USART4_Order_to_Mechanical_Handle(Order_HAND_ACT_CTRL_STOP);
                                Flag.HandDirction = 0;
                            }
                        }break;


                        case Dirction_back_stop:
                        {
                            Flag.StartCnt++;
                            Flag.BlueBeepCnt++;
                            if(Record.HandReset == 0){
                                LED_Blink(LED_Name_Message, 1, 0, 0);
                                USART4_Order_to_Mechanical_Handle(Order_HAND_ACT_CTRL_BACK_STOP);
                                Flag.HandDirction = 0;
                            }
                        }break;

                        default:break;
                    }
                }break;

                // APP控制设备升级指令---------------------------------------------------------------------------------------------------------------
                case FRAME_APP_SET_SYS_UPDATE_CMD:
                {
                    Flag.update = 1;
                }break;

                case FRAME_APP_SET_HAND_OLD_MODE_CMD:
                {
                    Flag.OldMode = 1;
                    BLUETOOTH_SET_EN(DISABLE);
                }break;


                // APP设置机械手柄的速度
                case FRAME_APP_SET_HAND_SPEED_SET:
                {
                    Record.HandSpeedAdjust_Mode = *(CmdBuf + 6);
                    Record.HandSpeed = *(CmdBuf + 7);

                    USART4_Order_to_Mechanical_Handle(Order_HAND_SEND_HAND_Speed);
                }break;


                default:    break;
            }
        }break;

        /*********************************************************************************/
        /*    控制指令码 0x32  */
        /*********************************************************************************/
        case FRAME_TYPE_SET:
        {
            switch(*(CmdBuf + 5))
            {
                // APP获取推杆电机的位置进度----------------------------------------------------------------------------------------------------------------
                case FRAME_APP_SET_MOTOR_LOCATION_CMD:
                {
                    USART4_Order_to_Mechanical_Handle(Order_HAND_SEND_HAND_LOCATION);
                }break;


                default:
                    break;
            }
        }break;





        /*********************************************************************************/
        /*    控制指令码 0x33  */
        /*********************************************************************************/
        case FRAME_TYPE_GET:
        {

            switch (*(CmdBuf + 5))
            {
                // APP获取当前流量模式----------------------------------------------------------------------------------------------------------------
                case FRAME_APP_GET_FLOW_MODE_CMD:
                {
                    DataBuf[0] = FRAME_APP_GET_FLOW_MODE_CMD;
                    DataBuf[1] = Record.Flow_mode;
                    USART2_Send_Command_to_Principal(2, FRAME_TYPE_GET, FRAME_STATE_ACK, DataBuf);
                }break;

                // APP获取压力模式信息----------------------------------------------------------------------------------------------------------------
                case FRAME_APP_GET_PRESS_MODE_CMD:
                {
                    DataBuf[0] = FRAME_APP_GET_PRESS_MODE_CMD;
                    DataBuf[1] = Record.Hand_Press_Mode;
                    USART2_Send_Command_to_Principal(2, FRAME_TYPE_GET, FRAME_STATE_ACK, DataBuf);
                }break;

                // APP获取节拍模式信息----------------------------------------------------------------------------------------------------------------
                case FRAME_APP_GET_TAKT_MODE_CMD:
                {
                    DataBuf[0] = FRAME_APP_GET_TAKT_MODE_CMD;
                    DataBuf[1] = Record.Takt_Mode;
                    USART2_Send_Command_to_Principal(2, FRAME_TYPE_GET, FRAME_STATE_ACK, DataBuf);
                }break;

                // APP获取主板版本号----------------------------------------------------------------------------------------------------------------
                case FRAME_APP_GET_BOARD_VERSION_CMD:
                {
                    DataBuf[0] = FRAME_APP_GET_BOARD_VERSION_CMD;
                    DataBuf[1] = DEVICE_VERSION;
                    USART2_Send_Command_to_Principal(2, FRAME_TYPE_GET, FRAME_STATE_ACK, DataBuf);
                }break;

                // APP获取手柄版本号----------------------------------------------------------------------------------------------------------------
                case FRAME_APP_GET_HAND_VERSION_CMD:
                {
                    USART4_Order_to_Mechanical_Handle(Order_HAND_SEND_HAND_VERSION);
                }break;

                // APP获取实时气压----------------------------------------------------------------------------------------------------------------
                case FRAME_APP_GET_REAL_PRESSURE_CMD:
                {
                    Record.PressReadEnable = *(CmdBuf + 6);

                }break;

                default:
                    break;
            }


        }break;

        default:    break;
    }
#endif
}




/**
 * @brief   STM32通过串口2向安卓屏发送指令
 * @param   order   指令码
 * @retval  None
 */
extern uint16_t CheckStartPress;
void USART2_Order_to_Andriod(uint8_t order)
{
    uint8_t emptyBuf[20] = {0};
    switch(order)
    {
        // 0x31指令集-----------------------------------------------------------------------------------------------------------------
        // 向安卓发送手柄电机位置信息------------------------------------------------------------------------------------------------------
        case Order_Android_ACT_HAND_LOCATION_CMD:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_APP_SET_MOTOR_LOCATION_CMD;
            emptyBuf[1] = 0;
            emptyBuf[2] = Record.Hand_Location;
            USART2_Send_Command_to_Principal(3, FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf);
        }break;

        // 向安卓发送手柄电机位置 100%------------------------------------------------------------------------------------------------------
        case Order_Android_ACT_HAND_LOCATION_FULL_CMD:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_APP_SET_MOTOR_LOCATION_CMD;
            emptyBuf[1] = 0;
            emptyBuf[2] = 0;
            USART2_Send_Command_to_Principal(3, FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf);
        }break;
        // 向安卓发送手柄电机回退状态信息------------------------------------------------------------------------------------------------------
        case Order_Android_ACT_HAND_RETURN_STATUS:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_APP_SET_HAND_RETURN_STATUS_CMD;
            emptyBuf[1] = 0;
            emptyBuf[2] = Flag.Hand_ReturnStatus;
            USART2_Send_Command_to_Principal(3, FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf);
        }break;

        // 向安卓发送手柄电机正在回退信息------------------------------------------------------------------------------------------------------
        case Order_Android_ACT_HAND_IS_RETURNING_STATUS:
        {
            macBEEP_ON();
            rt_thread_mdelay(150);
            macBEEP_OFF();

            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_APP_SET_HAND_RETURN_STATUS_CMD;
            emptyBuf[1] = 0;
            emptyBuf[2] = 1;
            USART2_Send_Command_to_Principal(3, FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf);
        }break;

        // 0x33指令集-----------------------------------------------------------------------------------------------------------------
        // 向安卓屏发送手柄的版本信息------------------------------------------------------------------------------------------------------
        case Order_Android_GET_HAND_VERSION:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_APP_GET_HAND_VERSION_CMD;
            emptyBuf[1] = Record.Hand_Version;
            rt_kprintf("The Hand Version is %x\r\n",Record.Hand_Version);
            USART2_Send_Command_to_Principal(2, FRAME_TYPE_GET, FRAME_STATE_ACK, emptyBuf);
        }break;

        case Order_Android_GET_Real_Press:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_APP_GET_REAL_PRESSURE_CMD;
            emptyBuf[1] = (Record.Atmospheric_Tank_PressDat - CheckStartPress) >> 8;
            emptyBuf[2] = Record.Atmospheric_Tank_PressDat - CheckStartPress;
            emptyBuf[3] = 0;
            emptyBuf[4] = 0;
            emptyBuf[5] = (Record.Output_PressDat - CheckStartPress) >> 8;
            emptyBuf[6] = Record.Output_PressDat - CheckStartPress;
            USART2_Send_Command_to_Principal(7, FRAME_TYPE_GET, FRAME_STATE_ACK, emptyBuf);
        }break;

        default: break;
    }
}


#endif














