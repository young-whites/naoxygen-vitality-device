#include <applications/macAPP/Inc/rtt_uart4_Decode.h>
/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-11-14     teati       the first version
 */





/* 定义串口设备节点 */
rt_device_t  serial4;
/* 定义串口设备的名称 */
#define USART4_NAME "uart4"
/* 重定向串口配置结构体 */
struct serial_configure usart4Config = RT_SERIAL_CONFIG_DEFAULT;
/* 创建一个信号量用于接收单字节 */
rt_sem_t usart4_rec_sem = RT_NULL;
/* 创建串口4的缓冲区数组结构体 */
xUsart_Structure Uart4Buf;

/*---------------------------------------------------------------------------------------------------------------*/
/* 以下是串口4初始化的创建以及回调函数                                                                                                                                                          */
/*---------------------------------------------------------------------------------------------------------------*/
/**
  * @brief  串口2的中断回调函数
  * @retval rt_err_t
  */
rt_err_t Usart4_RX_Callback(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(usart4_rec_sem);
    return RT_EOK;
}


/**
  * @brief  串口2初始化函数
  * @retval int
  */
int USART4_Init(void)
{
    static rt_size_t sendNum = 0;

    // 创建动态信号量
    usart4_rec_sem = rt_sem_create("dynamic_sem4", 0, RT_IPC_FLAG_FIFO);
    if (usart4_rec_sem == RT_NULL){
        rt_kprintf("PRINTF:%d. create dynamic semaphore failed.\n",Record.kprintf_cnt++);
        return -1;
    }
    else{
        rt_kprintf("PRINTF:%d. create done. dynamic semaphore value = 0.\n",Record.kprintf_cnt++);
    }


    serial4 = rt_device_find(USART4_NAME);
    if(serial4 != RT_NULL){
        rt_kprintf("PRINTF:%d. Usart4 Device node created succeed! \r\n",Record.kprintf_cnt++);
        usart4Config.baud_rate = BAUD_RATE_9600;
        usart4Config.bufsz = 1024;
    }
    else {
        rt_kprintf("PRINTF:%d. Usart4 Device node created Failed! \r\n",Record.kprintf_cnt++);
        return RT_ERROR;
    }

    rt_device_control(serial4, RT_DEVICE_CTRL_CONFIG, &usart4Config);
    rt_device_open(serial4, RT_DEVICE_OFLAG_RDONLY | RT_DEVICE_FLAG_INT_RX);
    rt_device_set_rx_indicate(serial4, Usart4_RX_Callback);

    /* 初始化循环队列 */
    Uart4Buf.head = 0;
    Uart4Buf.tail = 0;
    Uart4Buf.lock = rt_mutex_create("uart4_buf_lock", RT_IPC_FLAG_FIFO);


    sendNum = rt_device_write(serial4, RT_NULL, "usart4 is opened!\r\n", 19);
    rt_kprintf("PRINTF:%d. The usart4 test send size : %d\r\n\n",Record.kprintf_cnt++,sendNum);

    return RT_EOK;
}



/*---------------------------------------------------------------------------------------------------------------*/
/* 以下是数据解析线程的创建以及回调函数                                                                                                                                                         */
/*---------------------------------------------------------------------------------------------------------------*/
/**
  * @brief  数据解码回调函数入口
  * @retval void
  */
void uart4_thread_entry(void* parameter)
{
    char recDat = 0;
    rt_size_t sizeValue = 0;
    uint8_t decodeStatus = 0;
    uint8_t usartID = 4;
    while(1)
    {
        sizeValue = rt_device_read(serial4, RT_NULL, &recDat, 1);
        if(sizeValue == 1){
            rt_sem_take(usart4_rec_sem, RT_WAITING_FOREVER);
            /* 加锁保护队列操作 */
            rt_mutex_take(Uart4Buf.lock, RT_WAITING_FOREVER);
            /* 计算下一个尾指针位置 */
            uint16_t next_tail = (Uart4Buf.tail + 1) % MAX_DATA_LENGTH;
            /* 队列未满 */
            if(next_tail != Uart4Buf.head) {
                Uart4Buf.rxBuffer[Uart4Buf.tail] = recDat;
                Uart4Buf.tail = next_tail;
            }
            else {
                rt_kprintf("UART4 Queue Full! Data Lost: 0x%02X\n", recDat);
            }
            /* 释放互斥锁 */
            rt_mutex_release(Uart4Buf.lock);
            /* 触发协议解析 */
            decodeStatus = USART4_Portocol_Get_Command(serial4,usartID);
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
rt_thread_t uart4_decodeThread_Handle;
int uart4_decodeThread_Init(void)
{
    uart4_decodeThread_Handle = rt_thread_create("uart4_thread_entry", uart4_thread_entry, RT_NULL, 1024, 10, 200);
    if(uart4_decodeThread_Handle != RT_NULL){
        rt_kprintf("PRINTF:%d. uart4 Thread is created!!\r\n",Record.kprintf_cnt++);
        USART4_Init();
        rt_thread_startup(uart4_decodeThread_Handle);
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
uint8_t USART4_Portocol_Get_Command(rt_device_t dev, uint8_t USART_ID)
{
    uint8_t decode_result = CMD_ERROR;
    static uint8_t  Decode_Step = 0;
    static uint8_t  CMD_Length = 0;
    static uint8_t  CMD_buffer[64] = {0};
    static uint8_t  CMD_DataCnt = 0;
    static uint8_t  CRC16_H,CRC16_L = 0;
    static uint16_t CRC16_Value = 0;



    while(Uart4Buf.head != Uart4Buf.tail)
    {

        uint8_t DecodeDat = Uart4Buf.rxBuffer[Uart4Buf.head];
        Uart4Buf.head = (Uart4Buf.head + 1) % MAX_DATA_LENGTH;

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
                if(USART_ID == 4)
                {
                    Protocol_Operation_USART4(dev,CMD_buffer);
                    #if USART4_REC_CMD_PRINTF
                        rt_kprintf("Valid   Command: ");
                        for(uint8_t i = 0; i < CMD_Length + 5; i++) {
                            rt_kprintf("%02X ", Uart4Buf.rxBuffer[(Uart4Buf.head - (CMD_Length + 5) + i) % MAX_DATA_LENGTH]);
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
                    rt_kprintf("%02X ", Uart4Buf.rxBuffer[(Uart4Buf.head - (CMD_Length + 5) + i) % MAX_DATA_LENGTH]);
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
 * @brief   STM32通过串口4发送指令到上位机进行应答
 * @param   CmdBuf      数据域存放的指针
 *          USART_ID    串口号
 * @retval  None
 */
void USART4_Send_Command_to_Principal(uint8_t DataLen, uint8_t CmdType, uint8_t CmdStatus, uint8_t* DataBuf)
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
    dev = rt_device_find("uart4");
    rt_enter_critical();
    rt_device_write(dev, RT_NULL, SendDat, 9+DataLen);
#if USART4_SEND_CMD_INFO_PRINTF
    rt_kprintf("LOG:%d. USART4 send to Hand :: 55 aa ",Record.Log_Uart_cnt++);
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
 * @brief   解析串口4数据域指令，执行响应的函数(小手柄的通讯执行函数)
 * @param   CmdBuf      数据域存放的指针
 *          USART_ID    串口号
 * @retval  None
 */
int8_t Hand_Location_Diff = 0;
rt_uint8_t location_cnt = 0;
rt_uint8_t location_forcnt = 0;
rt_uint8_t location_autocnt = 0;
rt_uint8_t beepStart = 0;
extern void Proportional_Flow_Mode_Config(Mode_StructType mode);
void Protocol_Operation_USART4(rt_device_t dev,uint8_t* CmdBuf)
{
    /*以 06 00 61 31 02 01 01 数据域指令为例*/
    /*长度 + 设备ID_H + 设备ID_L + 指令类型 + 指令状态 + 实际指令宏 + 指令数据 */
    switch(*(CmdBuf + 3))
    {

        /*********************************************************************************/
        /*    控制指令码 0x31  */
        /*********************************************************************************/
        case FRAME_TYPE_ACT:
        {
            switch(*(CmdBuf + 5))
            {
                // 机械手柄发送位置信息，主板接收再发送给APP---------------------------------------------------------------------------------------------------------------
                case FRAME_HAND_SET_CHECK_MOTOR_PROGRESS_CMD:
                {
                    Record.Hand_Location = *(CmdBuf + 6);
                    USART2_Order_to_Andriod(Order_Android_ACT_HAND_LOCATION_CMD);

                    if(Record.Hand_Location == 0){
                        if(Flag.APPWorkStatus == 2){
                            Flag.Hand_ReturnStatus = 0;
                            Flag.APPWorkStatus = 0;
                            Record.KeyPowerCnt = 0;
                            Record.KeyLong4sCnt = 0;
                        }

                        for(; location_cnt < 1; location_cnt = 2){
                            Flag.Hand_ReturnStatus = 0;
                            USART2_Order_to_Andriod(Order_Android_ACT_HAND_RETURN_STATUS);
                            Record.HandReset = 0;
                            if(Flag.StartCnt != 0 && Flag.BlueBeepCnt != 2 && Flag.BlueBeepCnt != 0){
                                BEEP_Blink(1);
                            }
                        }
                    }
                    else{
                        location_cnt = 0;
                    }

                    if(Record.Hand_Location == 0x64 && Flag.Action== 1 && Record.WorkMode == 1){
                        for(;location_forcnt < 1; location_forcnt = 2){
                            if(Flag.StartCnt != 0 && Flag.BlueBeepCnt != 2 ){
//                                BEEP_Blink(1);
                            }
                        }
                    }
                    else{
                        location_forcnt = 0;
                    }

                    if(Record.Hand_Location == 0x64 && Record.WorkMode == 0){
                        for(;location_autocnt < 1; location_autocnt = 2){
                            if(Flag.StartCnt != 0 && Flag.BlueBeepCnt != 2 ){
                                BEEP_Blink(1);
                            }
                        }
                    }
                    else{
                        location_autocnt = 0;
                    }

                }break;

                // 送给手柄版本号给APP----------------------------------------------------------------------------------------------------
                case FRAME_HAND_GET_VERSION_CMD:
                {
                    Record.Hand_Version = *(CmdBuf + 6);
                    USART2_Order_to_Andriod(Order_Android_GET_HAND_VERSION);
                }break;
                // 接收到手柄连接指令---------------------------------------------------------------------------------------------------------------
                case FRAME_HAND_SET_CHECK_HAND_CMD:
                {
                    Flag.HandConnect = 1;
                    if(Record.KeyLong4sCnt != 1 && Record.tickStart > 45){
                        if(Flag.BlueBeepCnt >= 2){
                            beepStart++;
                            BEEP_Blink(1);
                        }
                        if(beepStart >= 1){
//                            BEEP_Blink(1);
                        }
                    }
                    /*! 如果处于回退状态 */
                    if(Flag.Hand_ReturnStatus == 1 && Flag.APPWorkStatus != 2){
                        USART4_Order_to_Mechanical_Handle(Order_HAND_ACT_CTRL_BACK);
                    }
                }break;
                // 接收到手柄的电机工作指令---------------------------------------------------------------------------------------------------------------
                case FRAME_HAND_GET_MOTOR_RUN_CMD:
                {
                    /* 如果电机开始运动，但是流量不对，就进行流量请求 */
                    if((Record.Flow_mode == MODE_ZERO) && (Flag.APPWorkStatus == 1) && (Record.StartCnt >= 2)){
                          Proportional_Flow_Mode_Config(2);
                    }
                }break;
                // 机械手柄给主板发送电机正在回退的状态（0：已回退到目标点  1：正在回退）----------------------------------------------------------------------------------------------------
                case FRAME_HAND_SET_COME_BACK_CMD:
                {
                    /*! 已经退回到指定点，给APP发送退回完毕标志 */
                    if(*(CmdBuf + 6) == 0){
                        Flag.Hand_ReturnStatus = 0;
                        Record.HandReset = 0;
                    }
                    if(*(CmdBuf + 6) == 1){
                        Flag.Hand_ReturnStatus = 1;
                        Record.HandReset = 1;
                        USART2_Order_to_Andriod(Order_Android_ACT_HAND_RETURN_STATUS);
                        rt_kprintf("LOG:%d. HAND MOTOR IS BACKING!\r\n",++Record.Log_cnt);
                    }
                }break;

                default:    break;
            }
        }break;



        /*********************************************************************************/
        /*    控制指令码 0x32  */
        /*********************************************************************************/



        default:    break;
    }
}







/**
 * @brief   STM32通过串口2向安卓屏发送指令
 * @param   order   指令码
 * @retval  None
 */
void USART4_Order_to_Mechanical_Handle(uint8_t order)
{
    uint8_t emptyBuf[20] = {0};
    switch(order)
    {
        // 0x31指令集-----------------------------------------------------------------------------------------------------------------
        // 设置机械手柄的工作模式：模式0---------------------------------------------------------------------------------------------------
        case Order_HAND_ACT_WORK_MODE_ZERO_CMD:
        {
            if(Record.HandReset == 0){
                rt_memset(emptyBuf, 0, sizeof(emptyBuf));
                emptyBuf[0] = FRAME_HAND_SET_WORK_MODE_CMD;
                emptyBuf[1] = MODE_ZERO;
                USART4_Send_Command_to_Principal(2, FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf);
            }
        }break;
        // 设置机械手柄的工作模式：模式x---------------------------------------------------------------------------------------------------
        case Order_HAND_ACT_WORK_MODE_CMD:
        {
            if(Record.HandReset == 0){
                rt_memset(emptyBuf, 0, sizeof(emptyBuf));
                emptyBuf[0] = FRAME_HAND_SET_WORK_MODE_CMD;
                emptyBuf[1] = Record.Hand_Press_Mode;
                USART4_Send_Command_to_Principal(2, FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf);
            }
        }break;
        // 发送是否是机械手柄的指令------------------------------------------------------------------------------
        case Order_HAND_ACT_WHETHER_HAVE_HAND:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_HAND_SET_WHETHER_HAVE_HAND_CMD;
            USART4_Send_Command_to_Principal(1, FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf);
        }break;
        // 发送保存电机进度的指令------------------------------------------------------------------------------
        case Order_HAND_ACT_SAVE_STEP_PROGRESS:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_HAND_SET_SAVE_STEP_PROGRESS;
            USART4_Send_Command_to_Principal(1, FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf);
        }break;
        // 发送手柄自检测指令------------------------------------------------------------------------------
        case Order_HAND_ACT_AUTO_CHECK:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_HAND_SET_ATUO_CHECK_CMD;
            USART4_Send_Command_to_Principal(1, FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf);
        }break;
        // 发送手柄调整速度的指令------------------------------------------------------------------------------
        case Order_HAND_ACT_ADJUST_SPEED:
        {
            if(Record.HandReset == 0){
                rt_memset(emptyBuf, 0, sizeof(emptyBuf));
                emptyBuf[0] = FRAME_HAND_SET_ADJUST_SPEED_CMD;
                emptyBuf[1] = Record.HandSpeed;
                USART4_Send_Command_to_Principal(2, FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf);
            }
        }break;
        // 发送机械手柄推杆退回步进值指令------------------------------------------------------------------------------
        case Order_HAND_ACT_CTRL_BACK:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_HAND_SET_DIRCTION_MOVE_CMD;
            emptyBuf[1] = 0x01;
            USART4_Send_Command_to_Principal(2, FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf);
        }break;
        // 发送机械手柄推杆前进步进值指令------------------------------------------------------------------------------
        case Order_HAND_ACT_CTRL_FORWARD:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_HAND_SET_DIRCTION_MOVE_CMD;
            emptyBuf[1] = 0x02;
            USART4_Send_Command_to_Principal(2, FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf);
        }break;
        // 发送机械手柄推杆停止步进值指令------------------------------------------------------------------------------
        case Order_HAND_ACT_CTRL_STOP:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_HAND_SET_DIRCTION_MOVE_CMD;
            emptyBuf[1] = 0x00;
            USART4_Send_Command_to_Principal(2, FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf);
        }break;
        // 发送机械手柄推杆复位指令------------------------------------------------------------------------------
        case Order_HAND_ACT_CTRL_RESET:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_HAND_SET_DIRCTION_MOVE_CMD;
            emptyBuf[1] = 0x03;
            USART4_Send_Command_to_Principal(2, FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf);
        }break;
        // 发送机械手柄推杆先退回0.6s再停止步进值指令------------------------------------------------------------------------------
        case Order_HAND_ACT_CTRL_BACK_STOP:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_HAND_SET_DIRCTION_MOVE_CMD;
            emptyBuf[1] = 0x04;
            USART4_Send_Command_to_Principal(2, FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf);
        }break;

        // 向手柄发送设置手柄速度------------------------------------------------------------------------------------------------------
        case Order_HAND_SEND_HAND_Speed:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_APP_SET_HAND_SPEED_SET;
            emptyBuf[1] = Record.HandSpeedAdjust_Mode;
            emptyBuf[2] = Record.HandSpeed;
            USART4_Send_Command_to_Principal(3, FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf);
        }break;

        // 最高优先级的回退,手柄不可以进行任何其他操作在回退过程中------------------------------------------------------------------------------
        case Order_HAND_ACT_CTRL_BACK_STOP_FORCE:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_HAND_SET_HAND_BACK_FORCE_CMD;
            emptyBuf[1] = 1;
            USART4_Send_Command_to_Principal(2, FRAME_TYPE_ACT, FRAME_STATE_ASK, emptyBuf);
        }break;


        // 0x32指令集-----------------------------------------------------------------------------------------------------------------
        // 向手柄发送获取手柄位置信息------------------------------------------------------------------------------------------------------
        case Order_HAND_SEND_HAND_LOCATION:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_HAND_SET_CHECK_MOTOR_PROGRESS_CMD;
            USART4_Send_Command_to_Principal(1, FRAME_TYPE_SET, FRAME_STATE_ASK, emptyBuf);
        }break;
        // 向手柄发送获取手柄版本号指令------------------------------------------------------------------------------------------------------
        case Order_HAND_SEND_HAND_VERSION:
        {
            rt_memset(emptyBuf, 0, sizeof(emptyBuf));
            emptyBuf[0] = FRAME_HAND_GET_VERSION_CMD;
            USART4_Send_Command_to_Principal(1, FRAME_TYPE_SET, FRAME_STATE_ASK, emptyBuf);
        }break;



        default: break;
    }
}


