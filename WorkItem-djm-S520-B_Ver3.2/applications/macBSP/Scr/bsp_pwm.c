#include <applications/macBSP/Inc/bsp_pwm.h>
/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-06-12     zphu       the first version
 */


#define PWM_1_GPIO_PIN_NUM  23      /* PWM输出引脚编号，查看驱动文件drv_gpio.c确定 */
#define PWM_1_DEV_NAME      "pwm4"  /* 设备名称 */
#define PWM_1_DEV_CHANNEL   2       /* PWM输出通道 */


#define PWM_2_GPIO_PIN_NUM  22      /* PWM输出引脚编号，查看驱动文件drv_gpio.c确定 */
#define PWM_2_DEV_NAME      "pwm4"  /* 设备名称 */
#define PWM_2_DEV_CHANNEL   1       /* PWM输出通道 */

/**
  * @brief  Used to initialize PWM devices and channels
  * @retval None
  */
typedef struct {
    char pwm_dev1_name[16];             /* pwm设备名称 */
    int  pwm_dev1_channel;              /* 电机驱动的pwm输出通道 */
    int  pwm_dev1_out_pin;              /* 电机输出pwm的引脚编号 */

    char pwm_dev2_name[16];             /* pwm设备名称 */
    int  pwm_dev2_channel;              /* 电机驱动的pwm输出通道 */
    int  pwm_dev2_out_pin;              /* 电机输出pwm的引脚编号 */

    struct rt_device_pwm *pwm_dev;
}_pwm_init;



/**
  * @brief  TIM4_CH2 -- PB7 Initialization
  * @retval None
  */
static _pwm_init pwm_dev1   = {
        .pwm_dev1_name      =   PWM_1_DEV_NAME,
        .pwm_dev1_channel   =   PWM_1_DEV_CHANNEL,
        .pwm_dev1_out_pin   =   PWM_1_GPIO_PIN_NUM,
};



/**
  * @brief  TIM4_CH1 -- PB6 Initialization
  * @retval None
  */
static _pwm_init pwm_dev2   = {
        .pwm_dev2_name      =   PWM_2_DEV_NAME,
        .pwm_dev2_channel   =   PWM_2_DEV_CHANNEL,
        .pwm_dev2_out_pin   =   PWM_2_GPIO_PIN_NUM,
};


/**
  * @brief  PWM Device Initialization
  * @retval int
  */
int PWM_Init(void)
{

    /* 查找设备 */
    pwm_dev1.pwm_dev = (struct rt_device_pwm *)rt_device_find(pwm_dev1.pwm_dev1_name);
    if(pwm_dev1.pwm_dev != RT_NULL){
        rt_kprintf("PRINTF:%d. pwm1 device is created !! \r\n",Record.kprintf_cnt++);
        /* 初始化默认的周期和占空比
                  *   第三个参数是设置周期，    单位是ns，如果设置100*1000那么周期就是100us，取倒数得到频率为10Khz
                  *   第四个参数是设置占空比，单位是ns，如果设置100*1000那么占空比取50%，就需要设置为他的一半，即50*1000
                  *   注意：在CubeMX中设置的period本来是1000-1，也即设置的周期为1ms，经过此rt_pwm_set设置后会无效化，即重新设置了，增强了灵活性
                  *   此处，PWM设置的三要素，频率，周期，占空比就都可以使用该函数进行设置了
         */
        rt_pwm_set(pwm_dev1.pwm_dev, pwm_dev1.pwm_dev1_channel, 1000*1000,0*1000);
        rt_pwm_enable(pwm_dev1.pwm_dev, pwm_dev1.pwm_dev1_channel);

    }
    else {
        rt_kprintf("PRINTF:%d. pwm1 device created failed !! \r\n",Record.kprintf_cnt++);
        return RT_ERROR;
    }


    /* 查找设备 -- 这个通道用于控制节拍*/
    pwm_dev2.pwm_dev = (struct rt_device_pwm *)rt_device_find(pwm_dev2.pwm_dev2_name);
    if(pwm_dev2.pwm_dev != RT_NULL){
        rt_kprintf("PRINTF:%d. pwm2 device is created !! \r\n",Record.kprintf_cnt++);
        rt_pwm_enable(pwm_dev2.pwm_dev, pwm_dev2.pwm_dev2_channel);
        rt_pwm_set(pwm_dev2.pwm_dev, pwm_dev2.pwm_dev2_channel, 1000*1000, 0*1000);
    }
    else {
        rt_kprintf("PRINTF:%d. pwm2 device created failed !! \r\n",Record.kprintf_cnt++);
        return RT_ERROR;
    }

    return RT_EOK;
}
//INIT_APP_EXPORT(PWM_Init);
//MSH_CMD_EXPORT(PWM_Init,PWM_Init_Demo);


//以下是流量控制的代码----------------------------------------------------------------------------------------------------------

void Proportional_Flow_Mode_Config(Mode_StructType mode);
void Proportional_Takt_Mode_Config(Mode_StructType mode);

/***
   * 流量设定表
 * */
uint16_t const SetFlow_tabe[2][5]=
{
    {2200,2200 ,2200 ,2200 ,2200 },
    {1600,1900 ,2200 ,2600 ,3000 }
};



/**
  * @brief  Mode引脚硬件置“1”，输出模式由ENBL，PHASE，DECAY三个引脚共同控制
  * @param  ENBL  -> 芯片使能输出引脚（1：使能输出     0：失能输出）（PWM）
  *         PHASE -> 控制电流方向
  *         DECAY -> 可以实现刹车和滑行状态控制
  *         PEN   -> 总的使能控制（PWM）
  * @retval void
  */

void SS6840H_Mode_Config(SS6840_DECAY decayMode,SS6840_DIR Dir)
{

    // nDECAY控制----------------------------------------------
    if(decayMode == H_decay){
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        __HAL_RCC_GPIOB_CLK_ENABLE();
        GPIO_InitStruct.Pin = SS6840_DECAY_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        HAL_GPIO_WritePin(SS6840_DECAY_GPIO_Port, SS6840_DECAY_Pin, GPIO_PIN_SET);
    }
    else if(decayMode == L_decay){
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        __HAL_RCC_GPIOB_CLK_ENABLE();
        GPIO_InitStruct.Pin = SS6840_DECAY_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        HAL_GPIO_WritePin(SS6840_DECAY_GPIO_Port, SS6840_DECAY_Pin, GPIO_PIN_RESET);
    }
    else if(decayMode == X_decay){
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        __HAL_RCC_GPIOB_CLK_ENABLE();
        GPIO_InitStruct.Pin = SS6840_DECAY_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(SS6840_DECAY_GPIO_Port, &GPIO_InitStruct);
    }
    // 电流方向控制----------------------------------------------
    if(Dir == clockwise){
        HAL_GPIO_WritePin(SS6840_PHASE_GPIO_Port, SS6840_PHASE_Pin, GPIO_PIN_SET);
    }
    else if(Dir == anticlockwise){
        HAL_GPIO_WritePin(SS6840_PHASE_GPIO_Port, SS6840_PHASE_Pin, GPIO_PIN_RESET);
    }
}




/**
  * @brief  Proportional_Valve PWM Output Ctrl
  * @retval void
  */
void Proportional_Valve_ENBL_PWM_Set(rt_uint32_t Period, rt_uint32_t Dutyfactor)
{
    rt_pwm_set(pwm_dev1.pwm_dev, pwm_dev1.pwm_dev1_channel, Period * 1000, Dutyfactor * 1000);
}


/**
  * @brief  关闭比例阀的PWM使能
  * @retval void
  */
void Proportional_Valve_ENBL_PWM_Disable(void)
{
    rt_pwm_disable(pwm_dev1.pwm_dev, pwm_dev1.pwm_dev1_channel);
}



/**
  * @brief  开启比例阀的PWM使能
  * @retval void
  */
void Proportional_Valve_ENBL_PWM_Enable(void)
{
    rt_pwm_enable(pwm_dev1.pwm_dev, pwm_dev1.pwm_dev1_channel);
}



/**
  * @brief  Proportional_Valve MOS PWM Output Ctrl
  * @retval void
  */
void Proportional_Valve_PEN_PWM_Set(rt_uint32_t Period, rt_uint32_t Dutyfactor)
{
    rt_pwm_set(pwm_dev2.pwm_dev, pwm_dev2.pwm_dev2_channel, Period * 1000, Dutyfactor * 1000);
}


/**
  * @brief  开启比例阀的MOS使能
  * @retval void
  */
void Proportional_Valve_PEN_PWM_Enable(void)
{
    rt_pwm_enable(pwm_dev2.pwm_dev, pwm_dev2.pwm_dev2_channel);
}


/**
  * @brief  开启比例阀的MOS失能
  * @retval void
  */
void Proportional_Valve_PEN_PWM_Disable(void)
{
    rt_pwm_disable(pwm_dev2.pwm_dev, pwm_dev2.pwm_dev2_channel);
}





#if 1
/**
  * @brief  比例阀的流量控制模式
  * @retval void
  */
void Proportional_Flow_Mode_Config(Mode_StructType mode)
{
    switch (mode)
    {
        /*! 设定目标流量值 -- 关闭*/
        case MODE_ZERO:{
            Proportional_Valve_ENBL_PWM_Set(1000, 0);
        }break;

        /*! 设定目标流量值 */
        case MODE_ONE:{
            Proportional_Valve_ENBL_PWM_Set(1000, 805);
        }break;

        /*! 设定目标流量值 */
        case MODE_TWO:{
            Proportional_Valve_ENBL_PWM_Set(1000, 820);
//            Proportional_Valve_ENBL_PWM_Set(1000, 1000);
        }break;

        /*! 设定目标流量值 */
        case MODE_THREE:{
            Proportional_Valve_ENBL_PWM_Set(1000, 835);
//            Proportional_Valve_ENBL_PWM_Set(1000, 1000);
        }break;

        /*! 设定目标流量值 */
        case MODE_FOUR:{
            Proportional_Valve_ENBL_PWM_Set(1000, 900);
//            Proportional_Valve_ENBL_PWM_Set(1000, 1000);
        }break;

        /*! 设定目标流量值 */
        case MODE_FIVE:{
            Proportional_Valve_ENBL_PWM_Set(1000, 1000);
        }break;

        default:{
        }break;
    }
}




/**
  * @brief  比例阀的输出节拍控制模式
  * @retval void
  */
void Proportional_Takt_Mode_Config(Mode_StructType mode)
{
    switch (mode)
    {
        /*! 常开模式，节拍占空比100%，周期1ms，设定目标流量值 */
        case MODE_ZERO:{
            Proportional_Valve_PEN_PWM_Set(1000, 1000);
            Proportional_Valve_PEN_PWM_Enable();
            Record.TankPress = 3600;
            Record.Set_FlowValue = SetFlow_tabe[0][0];
        }break;
        /*! 模式1，节拍占空比50%，周期100ms，设定目标流量值 */
        case MODE_ONE:{
            Proportional_Valve_PEN_PWM_Set(100000, 50000);
            Proportional_Valve_PEN_PWM_Enable();

            if(Record.Flow_mode == MODE_ONE){
                Record.TankPress = 2400;    // 主气缸100Kpa时，手柄输出50Kpa左右
            }
            else if(Record.Flow_mode == MODE_TWO){
                Record.TankPress = 2600;    // 主气缸100Kpa时，手柄输出 90Kpa左右
            }
            else if(Record.Flow_mode == MODE_THREE){
                Record.TankPress = 2800;   // 主气缸200Kpa时，手柄输出120Kpa左右
            }
            else if(Record.Flow_mode == MODE_FOUR){
                Record.TankPress = 3000;   // 主气缸250Kpa时，手柄输出150Kpa左右
            }
            else if(Record.Flow_mode == MODE_FIVE)
            {
                Record.TankPress = 3300;   // 主气缸300Kpa时，手柄输出220Kpa左右
            }
            Record.Set_FlowValue = SetFlow_tabe[0][1];
        }break;
        /*! 模式2，节拍占空比50%，周期200ms，设定目标流量值 */
        case MODE_TWO:{
            Proportional_Valve_PEN_PWM_Set(200000, 100000);
            Proportional_Valve_PEN_PWM_Enable();

            if(Record.Flow_mode == MODE_ONE){
                Record.TankPress = 2400;
            }
            else if(Record.Flow_mode == MODE_TWO){
                Record.TankPress = 2600;
            }
            else if(Record.Flow_mode == MODE_THREE){
                Record.TankPress = 2800;
            }
            else if(Record.Flow_mode == MODE_FOUR){
                Record.TankPress = 3000;
            }
            else if(Record.Flow_mode == MODE_FIVE)
            {
                Record.TankPress = 3300;
            }
            Record.Set_FlowValue = SetFlow_tabe[0][2];
        }break;
        /*! 模式3，节拍占空比50%，周期400ms，设定目标流量值 */
        case MODE_THREE:{
            Proportional_Valve_PEN_PWM_Set(400000, 200000);
            Proportional_Valve_PEN_PWM_Enable();

            if(Record.Flow_mode == MODE_ONE){
                Record.TankPress = 2400;
            }
            else if(Record.Flow_mode == MODE_TWO){
                Record.TankPress = 2600;
            }
            else if(Record.Flow_mode == MODE_THREE){
                Record.TankPress = 2800;
            }
            else if(Record.Flow_mode == MODE_FOUR){
                Record.TankPress = 3000;
            }
            else if(Record.Flow_mode == MODE_FIVE)
            {
                Record.TankPress = 3300;
            }
            Record.Set_FlowValue = SetFlow_tabe[0][3];
        }break;
        /*! 模式4，节拍占空比50%，周期500ms，设定目标流量值 */
        case MODE_FOUR:{
            Proportional_Valve_PEN_PWM_Set(500000, 250000);
            Proportional_Valve_PEN_PWM_Enable();

            if(Record.Flow_mode == MODE_ONE){
                Record.TankPress = 2400;
            }
            else if(Record.Flow_mode == MODE_TWO){
                Record.TankPress = 2600;
            }
            else if(Record.Flow_mode == MODE_THREE){
                Record.TankPress = 2800;
            }
            else if(Record.Flow_mode == MODE_FOUR){
                Record.TankPress = 3000;
            }
            else if(Record.Flow_mode == MODE_FIVE)
            {
                Record.TankPress = 3300;
            }
            Record.Set_FlowValue = SetFlow_tabe[0][4];
        }break;
        /*! 模式5，节拍占空比0%，周期1ms，设定目标流量值 */
        case MODE_FIVE:{
            Proportional_Valve_PEN_PWM_Set(1000, 0);
            Proportional_Valve_PEN_PWM_Disable();
            if(Flag.APPWorkStatus == 0){
                Record.TankPress = 3400;
            }

            if(Record.Flow_mode == MODE_ONE){
                Record.TankPress = 3000;
            }
            else if(Record.Flow_mode == MODE_TWO){
                Record.TankPress = 3100;
            }
            else if(Record.Flow_mode == MODE_THREE){
                Record.TankPress = 3200;
            }
            else if(Record.Flow_mode == MODE_FOUR){
                Record.TankPress = 3300;
            }
            else if(Record.Flow_mode == MODE_FIVE)
            {
                Record.TankPress = 3400;
            }

        }break;

        default:
            break;
    }
}
#endif
