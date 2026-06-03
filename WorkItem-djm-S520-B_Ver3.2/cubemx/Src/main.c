/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_UART5_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_UART4_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */
extern void Proportional_Flow_Mode_Config(Mode_StructType mode);
extern void Proportional_Takt_Mode_Config(Mode_StructType mode);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
__WEAK int main(void)
{

  /* USER CODE BEGIN 1 */
  static rt_uint8_t BluetoothInitCnt = 0;
  static rt_uint8_t AutoHandCheckCnt = 0;
  static rt_uint8_t PowerLEDToggleCnt = 0;
  static rt_uint8_t OldModeCnt = 0;
  static rt_uint8_t HandStopCmdCnt = 0;
  static rt_uint32_t OldModeTick = 0;
  static rt_uint8_t SleepCnt = 0;

  rt_tick_t time_tick;
  extern uint8_t nowLocationCnt;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  Flag.Solenoid_Valve3_Limit = 0;
  Flag.OldMode = 0;
  Flag.APPWorkStatus = 0;
  Flag.SysConnectStatus = 0;

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_UART5_Init();
  MX_USART2_UART_Init();
  MX_UART4_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM4_Init();
  keyTimer_Init();
  Power_EN();


  uart2_decodeThread_Init();
  uart4_decodeThread_Init();
  /* USER CODE BEGIN 2 */
  PWM_Init();
  Record.KeyPowerCnt = 0;
  // 所有的硬件上电初始化在此进行 -------------------------------------------------------------------------------------------------
  /*! 上电初始化 -- 主气路初始化 */
  Main_Gas_Circuit_Off();
  Compressor_Off();
  /*! 风扇初始化 -- 关闭风扇 */
  Fan_Off();
  /*! SS6840H比例阀工作模式初始化 */
  SS6840H_Mode_Config(H_decay,clockwise);
  /* 关闭比例阀 */
  Proportional_Flow_Mode_Config(MODE_ZERO);
  /* 关闭比例阀使能 */
  Proportional_Takt_Mode_Config(MODE_FIVE);
  fal_init();
#if USE_MOVING_AVERAGE_FILTER
  /*! 初始化滤波器 */
  Init_filter(&Mov_Filtre);
#endif
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // 这是非老化模式运行的程序--------------------------------------------------------------------------------
    if(Flag.OldMode == 0)
    {
        if(Record.HandReset == 1){
              Proportional_Flow_Mode_Config(MODE_ZERO);
              Proportional_Takt_Mode_Config(MODE_FIVE);
        }


        if(Flag.Action == 1){
            Record.Hand_Location = 0x64;
            USART2_Order_to_Andriod(Order_Android_ACT_HAND_LOCATION_CMD);
            rt_thread_mdelay(50);
            Flag.Hand_ReturnStatus = 1;
            Record.HandReset = 1;
            USART2_Order_to_Andriod(Order_Android_ACT_HAND_IS_RETURNING_STATUS);
            rt_thread_mdelay(50);
            /* 控制手柄回退 */
            USART4_Order_to_Mechanical_Handle(Order_HAND_ACT_CTRL_BACK);
            Flag.Action = 0;
        }



     /* 蓝牙连接检测  -- 断开关闭所有功能 */
     if((((Flag.SysConnectStatus == 1) && (Record.KeyPowerCnt == 1)) && (BLUETOOTH_GetState() == 0)) || (Flag.SleepMode == 1)){
         /* 关闭比例阀 */
         Proportional_Flow_Mode_Config(MODE_ZERO);
         /* 关闭比例阀使能 */
         Proportional_Takt_Mode_Config(MODE_FIVE);
         /* 断开压缩机和风扇 */
         Compressor_Off();
         LED_Off(LED_Name_Status);
         if(Record.HandReset == 0 && Flag.APPWorkStatus != 2 && Flag.SleepMode != 1){
             /* 控制推杆停止运动 */
             USART4_Order_to_Mechanical_Handle(Order_HAND_ACT_CTRL_STOP);
         }
         Fan_On();
         /* 标志位复位 */
         if(Flag.SleepMode == 0){
             Flag.SysConnectStatus = 0;
             SleepCnt = 0;
         }
         /* 清除参数，避免开始 */
         Flag.Solenoid_Valve3_Limit = 0;
         Record.Takt_Mode = 5;
         Record.Flow_mode = 0;
         Record.BluetoothConnectCnt = 0;
         Flag.HandDirction = 0;
         for(; SleepCnt < 1; SleepCnt = 2){
             Main_Gas_Circuit_On();
             time_tick = rt_tick_get();
             rt_thread_delay_until(&time_tick, 5000);
         }
     }
     else if((((Flag.SysConnectStatus == 0) && (Record.KeyPowerCnt == 1)) && (BLUETOOTH_GetState() == 1))&&(Flag.SleepMode == 0) && (Flag.APPWorkStatus !=2 )){

#if 1
         /* 唤醒时先延时2s*/
         time_tick = rt_tick_get();
         rt_thread_delay_until(&time_tick, 2000);
#endif
         Compressor_On();
         Fan_On();
         Flag.SysConnectStatus = 1;
         Flag.Solenoid_Valve3_Limit = 1;

         if(Flag.Hand_ReturnStatus == 1){
             USART2_Order_to_Andriod(Order_Android_ACT_HAND_RETURN_STATUS);
         }
     }

       //-----------------------------------------------------------------------------------------------
       /* 按键计数检测 -- 按键次数表征电源开关 */
       /* KeyPowerPressCnt：0 , it indicates that the device is powered off */
       if((Record.KeyPowerCnt == 0) && (Record.KeyLong4sCnt != 1))
       {
           /* 关闭比例阀 */
           Proportional_Flow_Mode_Config(MODE_ZERO);
           /* 关闭比例阀使能 */
           Proportional_Takt_Mode_Config(MODE_FIVE);
           /* 发送手柄停止指指令 */
           for(; HandStopCmdCnt < 2; HandStopCmdCnt++){
               USART4_Order_to_Mechanical_Handle(Order_HAND_ACT_WORK_MODE_ZERO_CMD);
           }
           /* 标志位复位 */
           Flag.APPWorkStatus = 0;
           Flag.SysConnectStatus = 0;
           Flag.Solenoid_Valve3_Limit = 0;
           Flag.HandDirction = 0;
           BluetoothInitCnt = 0;
           BLUETOOTH_SET_EN(DISABLE);
           macTk1300f_Uart1_EN(DISABLE);
           macTk1300f_Uart3_EN(DISABLE);
           Power_Off_LED();
           Fan_Off();
           Compressor_Off();
       }
       /* KeyPowerPressCnt：1 , it indicates that the device is powered on */
       else if(((Record.KeyPowerCnt == 1) && (Record.KeyLong4sCnt != 1))&&(Flag.SleepMode == 0))
       {
           SleepCnt = 0;
           HandStopCmdCnt = 0;
           /* 按键开机初始化 */
           Power_On_LED();
           AutoHandCheckCnt = 0;
           for(;BluetoothInitCnt < 1; BluetoothInitCnt = 2){
               bluetoothTimer_Init();
               macTk1300f_Uart1_Init();
               macTk1300f_Uart3_Init();
               macTk1300f_Uart1_EN(ENABLE);
               macTk1300f_Uart3_EN(ENABLE);
           }
       }
       else if((Record.KeyPowerCnt == 2) && (Record.KeyLong4sCnt != 1))
       {
           Record.KeyPowerCnt = 0;
       }


       //-------------------------------------------------------------------------------------------------
       /* KeyLong4sCnt：1 , it indicates that the device is going to check itself */
       if(Record.KeyLong4sCnt == 1)
       {
           /*! 发送手柄自检测指令 */
           for(;AutoHandCheckCnt < 1; AutoHandCheckCnt = 2){
               BEEP_Blink(3);
               USART4_Order_to_Mechanical_Handle(Order_HAND_ACT_AUTO_CHECK);
               nowLocationCnt = 0;
               Flag.HandDirction = 0;
               /*! 关闭蓝牙的电源，防止在手柄自检过程中响应蓝牙的其他动作指令  */
               BLUETOOTH_SET_EN(DISABLE);
               Compressor_Off();
               /*! 设置机器状态为自检状态 */
               Flag.APPWorkStatus = 2;
               Flag.SysConnectStatus = 0;
               Flag.Solenoid_Valve3_Limit = 0;
               Proportional_Flow_Mode_Config(MODE_ZERO);
               Proportional_Takt_Mode_Config(MODE_FIVE);
           }

           if(PowerLEDToggleCnt == 0){
               Power_Off_LED();
               PowerLEDToggleCnt++;
           }
           else if(PowerLEDToggleCnt == 1){
               PowerLEDToggleCnt = 0;
               Power_On_LED();
           }
       }
       rt_thread_mdelay(500);
      }
      // 这是老化模式运行的程序--------------------------------------------------------------------------------
      else if(Flag.OldMode == 1)
      {
          for(;OldModeCnt < 1; OldModeCnt = 2){
              rt_kprintf("LOG:%d. Now is in old mode.\r\n",++Flag.log_cnt);
          }
          for(;OldModeTick < 1; OldModeTick = 2){
              macTk1300f_Uart1_Init();
              macTk1300f_Uart3_Init();
              macTk1300f_Uart1_EN(ENABLE);
              macTk1300f_Uart3_EN(ENABLE);
              Proportional_Flow_Mode_Config(MODE_ZERO);
              Proportional_Takt_Mode_Config(MODE_FIVE);
              Fan_On();
              Compressor_On();
          }

          if(Flag.Old_WorkMode == 1){
              Proportional_Flow_Mode_Config(MODE_FIVE);
              Proportional_Takt_Mode_Config(MODE_ONE);
              Flag.Solenoid_Valve3_Limit = 1;
          }
          else if(Flag.Old_WorkMode == 2){
              Proportional_Flow_Mode_Config(MODE_FOUR);
              Proportional_Takt_Mode_Config(MODE_TWO);
              Flag.Solenoid_Valve3_Limit = 1;
          }
          else if(Flag.OldMode == 3){
              Proportional_Flow_Mode_Config(MODE_THREE);
              Proportional_Takt_Mode_Config(MODE_THREE);
              Flag.Solenoid_Valve3_Limit = 1;
          }
          else if(Flag.OldMode == 4){
              Proportional_Flow_Mode_Config(MODE_TWO);
              Proportional_Takt_Mode_Config(MODE_FOUR);
              Flag.Solenoid_Valve3_Limit = 1;
          }
       rt_thread_mdelay(500);
      }

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.Prediv1Source = RCC_PREDIV1_SOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the Systick interrupt time
  */
  __HAL_RCC_PLLI2S_ENABLE();
}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 9600;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief UART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 115200;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */

  /* USER CODE END UART5_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED_Orange_Pin|LED_White_Pin|LED_4G_Pin|LED_Status_Pin
                          |Vavle_3_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LED_Message_Pin|FAN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, macBluetooth_EN_Pin|BEEP_Pin|Compressor_EN_Pin|SS6840_PHASE_Pin
                          |SS6840_DECAY_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PowerKey_Pin */
  GPIO_InitStruct.Pin = PowerKey_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(PowerKey_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_Orange_Pin LED_White_Pin LED_4G_Pin LED_Status_Pin
                           Vavle_3_EN_Pin */
  GPIO_InitStruct.Pin = LED_Orange_Pin|LED_White_Pin|LED_4G_Pin|LED_Status_Pin
                          |Vavle_3_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_Message_Pin FAN_Pin */
  GPIO_InitStruct.Pin = LED_Message_Pin|FAN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : macBluetooth_STA_Pin */
  GPIO_InitStruct.Pin = macBluetooth_STA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(macBluetooth_STA_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : macBluetooth_EN_Pin BEEP_Pin Compressor_EN_Pin SS6840_PHASE_Pin
                           SS6840_DECAY_Pin */
  GPIO_InitStruct.Pin = macBluetooth_EN_Pin|BEEP_Pin|Compressor_EN_Pin|SS6840_PHASE_Pin
                          |SS6840_DECAY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */



#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
