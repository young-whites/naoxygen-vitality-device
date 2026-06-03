# S520-B Ver3.2 主板代码分析报告

> **分析日期**：2025-06-01  
> **分析范围**：cubemx/Src/ + applications/ 全部源码  
> **分析方法**：基于源码静态分析，追踪实际调用链，确认函数初始化路径

---

## 1. 项目概述

| 项目 | 说明 |
|------|------|
| 产品名称 | S520-B 便携式氧疗设备（主板 Ver3.2） |
| MCU | STM32F103 系列（Cortex-M3，HAL 库） |
| 主频 | 72MHz（HSE 8MHz × PLL ×9） |
| RTOS | RT-Thread |
| Flash | 128KB 片内 Flash，FAL 分区管理 |
| 串口 | UART1~UART5 共 5 路串口 |
| 功能 | 蓝牙 APP 控制 + 机械手柄通信 + 气压传感器采集 + 比例阀 PWM 控制 + IAP 远程升级 |

---

## 2. 系统架构

### 2.1 RTOS 线程

| 线程名 | 优先级 | 栈大小 | 功能 | 初始化方式 |
|--------|--------|--------|------|-----------|
| main | 最高（裸机循环） | - | 系统主循环：蓝牙断连检测、按键电源管理、老化模式 | 系统入口 |
| uart2_thread_entry | 10 | 1024 | 蓝牙/APP 通信解码 | `uart2_decodeThread_Init()` 在 main() 中直接调用 |
| uart4_thread_entry | 10 | 1024 | 机械手柄通信解码 | `uart4_decodeThread_Init()` 在 main() 中直接调用 |
| uart1_thread_entry | 20 | 1024 | 压力传感器1（输出口气压）采集 | `INIT_APP_EXPORT` |
| uart3_thread_entry | 20 | 1024 | 压力传感器3（大气罐气压）采集 | `INIT_APP_EXPORT` |

### 2.2 软件定时器

| 定时器名 | 周期 | 功能 | 初始化方式 |
|----------|------|------|-----------|
| keyTimer | 10ms | 按键扫描 + 事件处理 | `keyTimer_Init()` 在 main() 第 121 行直接调用 |
| bluetoothTimer | 10ms | 蓝牙状态扫描（含 100ms 去抖） | `bluetoothTimer_Init()` 在 main() 第 263 行条件调用（开机时） |
| sysTimer | 1ms | 系统时基（1ms/10ms/50ms/500ms/1s 定时任务） | `INIT_APP_EXPORT` |
| ledTimer | 1ms | LED 渐变/闪烁扫描 | `INIT_APP_EXPORT` |
| iapTimer | 50ms | IAP 升级检测与执行 | `INIT_APP_EXPORT` |

### 2.3 串口分配

| 串口 | 波特率 | 方向 | 用途 | 配置 |
|------|--------|------|------|------|
| UART1 | 9600 | RX only | TK1300F 压力传感器（输出口） | CubeMX: `UART_MODE_RX` |
| UART2 | 9600 | TX+RX | 蓝牙模块（HC-05/06） | CubeMX: `UART_MODE_TX_RX` |
| UART3 | 9600 | RX only | TK1300F 压力传感器（大气罐） | CubeMX: `UART_MODE_RX` |
| UART4 | 9600 | TX+RX | 机械手柄通信 | CubeMX: `UART_MODE_TX_RX` |
| UART5 | 115200 | TX+RX | 调试串口 | CubeMX: `UART_MODE_TX_RX` |

### 2.4 GPIO 分配

| 引脚 | 方向 | 功能 |
|------|------|------|
| PA (LED_Orange, LED_White, LED_4G, LED_Status, Vavle_3_EN) | Output | LED 指示灯 + 电磁阀 |
| PA (PowerKey) | Input (Pull-Up) | 电源按键 |
| PA (Tk1300F_EN1) | Output | 压力传感器1使能 |
| PB (macBluetooth_EN, BEEP, Compressor_EN, SS6840_PHASE, SS6840_DECAY) | Output | 蓝牙使能/蜂鸣器/压缩机/电机驱动 |
| PB (Tk1300F_EN3) | Output | 压力传感器3使能 |
| PB (Power_EN, GPIO_PIN_5) | Output | 电源使能 |
| PC (LED_Message, FAN) | Output | LED + 风扇 |
| PD (macBluetooth_STA) | Input (Pull-Down) | 蓝牙连接状态 |

### 2.5 数据流

```
[APP (蓝牙)] ←UART2→ [主板 STM32] ←UART4→ [机械手柄]
                        ↑    ↑
                   UART1  UART3
                     ↓      ↓
              [TK1300F]  [TK1300F]
              (输出气压)  (大气罐气压)
```

---

## 3. 模块分析

### 3.1 核心入口 (main.c)

**功能**：系统启动、外设初始化、主循环状态机

**启动流程**：
1. `HAL_Init()` → `SystemClock_Config()` → `MX_GPIO_Init()` → 各 UART/TIM 初始化
2. `keyTimer_Init()` → `Power_EN()` → `uart2_decodeThread_Init()` → `uart4_decodeThread_Init()`
3. `PWM_Init()` → 比例阀/压缩机/风扇初始关闭 → `fal_init()` → 滤波器初始化
4. 进入 while(1) 主循环

**主循环逻辑**：
- 非老化模式：蓝牙断连处理、重连处理、按键电源管理、手柄回退、自检
- 老化模式：固定工作模式循环

**初始化确认**（基于实际调用链）：
- ✅ `keyTimer_Init()` — main() 第 121 行直接调用
- ✅ `PWM_Init()` — main() 第 128 行直接调用
- ✅ `bluetoothTimer_Init()` — main() 第 263 行条件调用
- ✅ `uart2_decodeThread_Init()` — main() 中直接调用
- ✅ `uart4_decodeThread_Init()` — main() 中直接调用
- ✅ `sysTimer_Init()` — `INIT_APP_EXPORT`
- ✅ `ledTimer_Init()` — `INIT_APP_EXPORT`
- ✅ `iapTimer_Init()` — `INIT_APP_EXPORT`
- ✅ `USART1_Init()` — `INIT_APP_EXPORT`
- ✅ `USART3_Init()` — `INIT_APP_EXPORT`
- ✅ `uart1_Thread_Init()` — `INIT_APP_EXPORT`
- ✅ `uart3_Thread_Init()` — `INIT_APP_EXPORT`

### 3.2 手柄通信 (rtt_uart4_Decode.c)

**功能**：通过 UART4 与机械手柄进行双向通信

**帧格式**（发送/接收一致）：
```
0x55 0xAA [DataLen] [DeviceID_H] [DeviceID_L] [CmdType] [CmdStatus] [Data...] [CRC16_H] [CRC16_L]
```

**协议宏**：
- `FRAME_TYPE_ACT` (0x31) — 动作指令
- `FRAME_TYPE_SET` (0x32) — 参数设置
- `FRAME_TYPE_GET` (0x33) — 参数获取

**手柄上报指令**：
| 指令码 | 宏名 | 功能 |
|--------|------|------|
| 0x2E | `FRAME_HAND_SET_CHECK_MOTOR_PROGRESS_CMD` | 电机位置进度 |
| 0x2F | `FRAME_HAND_SET_COME_BACK_CMD` | 回退状态 |
| 0xBB | `FRAME_HAND_SET_CHECK_HAND_CMD` | 手柄连接检测 |
| 0x19 | `FRAME_HAND_GET_VERSION_CMD` | 手柄版本号 |
| 0xF2 | `FRAME_HAND_GET_MOTOR_RUN_CMD` | 电机运行状态 |

**主板发送指令**：
| 指令码 | 宏名 | 功能 |
|--------|------|------|
| 0x2D | `FRAME_HAND_SET_DIRCTION_MOVE_CMD` | 推杆方向控制（0:停/1:退/2:进/3:复位/4:退停） |
| 0x17 | `FRAME_HAND_SET_WORK_MODE_CMD` | 设置工作模式 |
| 0x15 | `FRAME_HAND_SET_ADJUST_SPEED_CMD` | 调整速度 |
| 0x3D | `FRAME_HAND_SET_ATUO_CHECK_CMD` | 自检测 |
| 0x3E | `FRAME_HAND_SET_SAVE_STEP_PROGRESS` | 保存电机进度 |
| 0xF5 | `FRAME_HAND_SET_HAND_BACK_FORCE_CMD` | 强制回退 |

### 3.3 蓝牙/APP通信 (rtt_uart2_Decode.c)

**功能**：通过 UART2 + 蓝牙模块与 Android APP 通信

**APP→主板指令**：
| 指令码 | 宏名 | 功能 |
|--------|------|------|
| 0x01 | `FRAME_APP_SET_WORK_SWITCH_CMD` | 开关机 |
| 0x02 | `FRAME_APP_SET_HAND_WORK_STATUS_CMD` | 运行/暂停 |
| 0x0D | `FRAME_APP_SET_FLOW_MODE_CMD` | 流量模式 |
| 0x0E | `FRAME_APP_SET_PRESSURE_MODE_CMD` | 压力模式 |
| 0x0F | `FRAME_APP_SET_TAKT_MODE_CMD` | 节拍模式 |
| 0x2D | `FRAME_APP_SET_MACHINE_HAND_CMD` | 推杆控制 |
| 0x29 | `FRAME_APP_SET_HAND_ADJUST_SPEED_CMD` | 速度调整 |
| 0xED | `FRAME_APP_SET_HAND_SPEED_SET` | 速度设置 |
| 0xAA | `FRAME_APP_SET_SYS_UPDATE_CMD` | 远程升级 |
| 0xEC | `FRAME_APP_SET_HAND_OLD_MODE_CMD` | 老化模式 |

**主板→APP指令**：
| 指令码 | 宏名 | 功能 |
|--------|------|------|
| 0x2E | `FRAME_APP_SET_MOTOR_LOCATION_CMD` | 电机位置 |
| 0x2F | `FRAME_APP_SET_HAND_RETURN_STATUS_CMD` | 回退状态 |
| 0x19 | `FRAME_APP_GET_HAND_VERSION_CMD` | 手柄版本 |
| 0x14 | `FRAME_APP_GET_REAL_PRESSURE_CMD` | 实时气压 |

### 3.4 系统工作逻辑 (rtt_system_work.c)

**功能**：sysTimer 1ms 回调中执行多速率定时任务

| 函数 | 周期 | 功能 |
|------|------|------|
| `Timing_1ms()` | 1ms | 电磁阀泄压控制（气压超限时开启泄气） |
| `Timing_10ms()` | 10ms | 休眠计数（工作180s/待机180s无操作进入休眠） |
| `Timing_50ms()` | 50ms | 关机泄气保持、比例阀微调（PID-like） |
| `Timing_500ms()` | 500ms | 定时发送实时气压到APP |
| `Timing_1s()` | 1s | 手柄位置轮询、倒计时、气压超限保护、到达前端自动回退 |

### 3.5 板级驱动

**蓝牙驱动 (bsp_bluetooth.c)**：
- `BLUETOOTH_SET_EN()` — 蓝牙模块使能/失能（EN 引脚，低电平有效）
- `BLUETOOTH_GetState()` — 读取 STA 引脚获取连接状态
- `BLUETOOTH_Scan()` — 10ms 周期扫描，100ms 去抖，首次上电设置蓝牙名称

**按键驱动 (bsp_key.c)**：
- 10ms 周期扫描，50ms 消抖
- 支持短按、长按 2s、长按 4s 事件
- 长按 4s 触发手柄自检（需已开机且手柄已连接）

**PWM 驱动 (bsp_pwm.c)**：
- TIM4_CH1 (PB6) — 比例阀 ENBL PWM（频率/占空比可调）
- TIM4_CH2 (PB7) — 比例阀 PEN PWM（节拍控制）
- SS6840H 电机驱动：PHASE + DECAY + PWM 三路控制

**蜂鸣器 (bsp_beep.c)**：
- `BEEP_Blink(num)` — 鸣叫 num 次，每次 150ms 亮 + 150ms 灭

**LED 驱动 (bsp_led.c)**：
- 支持常亮、渐变（呼吸）、闪烁、花式显示
- 1ms 周期扫描

**压缩机 (bsp_compressor.c)**：
- GPIO 控制，`Compressor_On()` / `Compressor_Off()`

**硬件初始化 (bsp_hard.c)**：
- 风扇、电磁阀、电源 LED、电源使能

### 3.6 压力传感器

**TK1300F 协议**：3 字节帧（高字节 + 低字节 + 校验和 = 高字节 + 低字节）

**UART1 (输出口气压)**：
- 上电校准：采集 10 个样本取平均作为零点基准，减去 50 作为偏移
- 无滤波处理

**UART3 (大气罐气压)**：
- 限幅保护：超过 `COMPRESSOR_AIR_PRESSURE_MAX` (3800) 时钳位
- 可选滑动平均滤波（当前 `USE_MOVING_AVERAGE_FILTER = 0`，已禁用）

### 3.7 存储与升级

**Flash 存储 (macFlash.c)**：
- FAL 框架适配，片内 Flash 128KB
- 支持按字（32bit）写入，按页擦除（1KB）

**IAP 升级 (rtt_iap_update_Timer.c + iap_upgrade.c)**：
- 50ms 周期检测 `Flag.update` 标志
- 升级流程：删除所有线程 → 停止定时器 → 擦除 Flash → 写入升级标志 → 跳转 Bootloader
- 跳转前执行 `RCC_DeInit()` + `NVIC` 清除 + `SCB->VTOR` 重设

---

## 4. 手柄通信兼容性确认

### 4.1 帧格式验证

**主板发送帧格式**（`USART4_Send_Command_to_Principal`）：
```
SendDat[0] = 0x55  (FRAME_HEAD1)
SendDat[1] = 0xAA  (FRAME_HEAD2)
SendDat[2] = 4 + DataLen  (DataLen = CmdType + CmdStatus + Data 的长度)
SendDat[3] = 0x00  (DEVICE_ID_H)
SendDat[4] = 0x01  (DEVICE_ID_L)
SendDat[5] = CmdType
SendDat[6] = CmdStatus
SendDat[7..] = Data
CRC16 = CrcCalc_Crc16Modbus(SendDat+2, 5+DataLen)  // 从 DataLen 字节开始计算
SendDat[6+DataLen+1] = CRC16_H
SendDat[6+DataLen+2] = CRC16_L
```

**结论**：帧格式正确，CRC 计算范围覆盖 DataLen + DeviceID + CmdType + CmdStatus + Data。

### 4.2 CRC16-Modbus 算法验证

```c
uint16_t CrcCalc_Crc16Modbus(uint8_t *dat, uint8_t len) {
    uint16_t CRC_index = 0xffff;
    // 标准 CRC16-Modbus 实现
    // 多项式: 0xA001 (反转的 0x8005)
}
```

**结论**：CRC 算法为标准 CRC16-Modbus，与协议一致。

### 4.3 接收解码验证

接收端（`USART4_Portocol_Get_Command`）的状态机：
1. 检测帧头 0x55 0xAA
2. 读取 DataLen
3. 验证 DeviceID (0x00 0x01)
4. 读取 CmdType + CmdStatus + Data
5. 读取 CRC16_H + CRC16_L
6. 校验 CRC

**结论**：解码流程正确，与发送格式匹配。

### 4.4 手柄数据处理

| 手柄上报 | 主板处理 | 转发到APP |
|----------|----------|-----------|
| 0x2E 位置 | 更新 `Record.Hand_Location` | ✅ 发送 `Order_Android_ACT_HAND_LOCATION_CMD` |
| 0x2F 回退状态 | 更新 `Flag.Hand_ReturnStatus` | ✅ 发送 `Order_Android_ACT_HAND_RETURN_STATUS` |
| 0xBB 连接检测 | 设置 `Flag.HandConnect = 1` | ❌ 不转发（仅本地处理） |
| 0x19 版本号 | 更新 `Record.Hand_Version` | ✅ 发送 `Order_Android_GET_HAND_VERSION` |
| 0xF2 电机运行 | 调整流量模式 | ❌ 不转发 |

---

## 5. 问题清单

### P0 — 严重（可能导致系统死机/数据损坏）

#### P0-1: IAP 升级定时器回调中在临界区内执行阻塞操作

**文件**：`rtt_iap_update_Timer.c` 第 28~45 行

```c
rt_enter_critical();                          // 关闭调度器
macNorFlash_Erase_Page(0x0801FFF0, 1);       // Flash 擦除（耗时 ~20ms）
rt_thread_delay_until(&erase_tick, 100);     // ⚠️ 在临界区内调用 delay！
macNorFlash_Write_Word(0x0801FFF0, 1);       // Flash 写入
rt_thread_delay_until(&jump_tick, 1000);     // ⚠️ 在临界区内调用 delay！
jump_sys_bootloader();                       // 跳转
rt_exit_critical();                          // 永远不会执行到
```

**问题**：
1. `rt_enter_critical()` 会关闭调度器和中断，在此期间调用 `rt_thread_delay_until` 会导致系统死锁（delay 依赖定时器中断，但中断已关闭）
2. Flash 擦写在临界区内执行，所有线程和中断被阻塞数十毫秒
3. 此代码在软件定时器回调中执行，软件定时器回调不应包含阻塞操作

**影响**：系统在执行 IAP 升级时极大概率死机

**修复建议**：
- 删除 `rt_enter_critical()` / `rt_exit_critical()`
- 使用 `rt_thread_create` 创建专门的升级线程，而非在定时器回调中执行
- 或使用 `rt_event_send` 通知主线程执行升级

---

#### P0-2: 老化模式条件判断 Bug — 模式 3 和 4 永远不会执行

**文件**：`cubemx/Src/main.c` 第 282~295 行

```c
else if(Flag.OldMode == 1)  // 此分支内 Flag.OldMode 恒为 1
{
    // ...
    else if(Flag.OldMode == 3){   // ⚠️ 应为 Flag.Old_WorkMode == 3
        Proportional_Flow_Mode_Config(MODE_THREE);
    }
    else if(Flag.OldMode == 4){   // ⚠️ 应为 Flag.Old_WorkMode == 4
        Proportional_Flow_Mode_Config(MODE_TWO);
    }
}
```

**问题**：在 `Flag.OldMode == 1` 的分支内，使用 `Flag.OldMode == 3` 和 `Flag.OldMode == 4` 作为条件，由于外层已限定 `Flag.OldMode == 1`，这两个条件永远为假。

**影响**：老化模式的第 3、4 阶段永远不会执行，比例阀和节拍参数不会切换

**修复建议**：将 `Flag.OldMode` 改为 `Flag.Old_WorkMode`

---

#### P0-3: UART 接收线程 rt_device_read 与信号量使用顺序颠倒

**文件**：`rtt_uart2_Decode.c` 第 87~100 行，`rtt_uart4_Decode.c` 第 82~95 行

```c
while(1)
{
    sizeValue = rt_device_read(serial4, RT_NULL, &recDat, 1);  // 先读
    if(sizeValue == 1){
        rt_sem_take(usart4_rec_sem, RT_WAITING_FOREVER);       // 后等信号量
        // ...
    }
    rt_thread_mdelay(10);
}
```

**问题**：RT-Thread 串口 INT_RX 模式的标准使用方式是 **先 sem_take（阻塞等待数据）→ 再 rt_device_read（读取数据）**。当前代码颠倒了顺序：
- `rt_device_read` 在没有数据时返回 0，跳过 sem_take — 正确
- 但如果 `rt_device_read` 返回 1（读到数据），此时 sem_take 会立即成功（因为中断已释放信号量），消耗一个信号量计数
- 如果在 `rt_device_read` 和 `rt_sem_take` 之间有新字节到达，信号量计数会累积，导致后续循环中 `rt_device_read` 在无数据时也能通过 sem_take

**影响**：可能导致串口数据丢失或重复读取

**修复建议**：
```c
while(1)
{
    rt_sem_take(usart4_rec_sem, RT_WAITING_FOREVER);   // 先等信号量
    sizeValue = rt_device_read(serial4, RT_NULL, &recDat, 1);  // 再读数据
    if(sizeValue == 1){
        // 处理数据
    }
}
```

---

#### P0-4: CRC16_H 高字节为 0x00 时有效指令被静默丢弃

**文件**：`rtt_uart2_Decode.c` 第 170~175 行，`rtt_uart4_Decode.c` 第 162~167 行

```c
if((Decode_Step == Decode_Step_0) && (CRC16_H != 0))
{
    CRC16_Value = CrcCalc_Crc16Modbus(CMD_buffer, CMD_Length + 1);
    if(((CRC16_H << 8) | CRC16_L) == CRC16_Value)
    {
        // 有效指令处理
    }
}
```

**问题**：当 CRC16 的高字节恰好为 0x00 时（概率约 1/256），`CRC16_H != 0` 条件为假，整个 CRC 校验被跳过，有效指令被静默丢弃。

**影响**：约 0.4% 的有效指令会丢失，表现为手柄/APP 偶尔无响应

**修复建议**：使用独立的标志位判断是否接收到完整帧，而非依赖 CRC16_H 的值：
```c
static uint8_t frame_complete = 0;
// 在 Decode_Step_7 中设置:
frame_complete = 1;
// CRC 校验条件改为:
if((Decode_Step == Decode_Step_0) && (frame_complete))
{
    frame_complete = 0;
    // ... CRC 校验
}
```

---

### P1 — 高（影响可靠性/稳定性）

#### P1-1: 全局共享变量 Record/Flag 无任何同步保护

**涉及文件**：所有模块

`Record` 和 `Flag` 结构体被以下上下文同时访问：
- `main()` 主循环
- `uart2_thread_entry`（蓝牙通信线程）
- `uart4_thread_entry`（手柄通信线程）
- `uart1_thread_entry`（压力传感器1线程）
- `uart3_thread_entry`（压力传感器3线程）
- `sysTimer_callback`（系统定时器回调）
- `keyTimer_callback`（按键定时器回调）
- `bluetoothTimer_callback`（蓝牙定时器回调）
- `iapTimer_callback`（IAP 定时器回调）

**问题**：虽然 ARM Cortex-M3 单次 32bit 读写是原子的，但以下操作不是原子的：
- `Record.KeyPowerCnt++`（读-改-写）
- `Flag.SysConnectStatus = 1; Flag.APPWorkStatus = 1;`（连续赋值，非原子）
- `Record.Output_PressDat = (H << 8) | L`（16bit 写入，可能被中断打断）

**影响**：竞态条件可能导致状态不一致，极端情况下触发误动作

**修复建议**：对关键状态变量使用 `rt_mutex` 保护，或使用 `rt_enter_critical()` / `rt_exit_critical()` 保护临界区（但需确保临界区内无耗时操作）

---

#### P1-2: BEEP_Blink 在定时器回调上下文中被调用

**文件**：`bsp_beep.c`，被 `bsp_bluetooth.c` 的 `BLUETOOTH_Scan()` 间接调用

```c
// bsp_bluetooth.c
if(bBtCnct){
    Flag.BlueBeepCnt = 1;
    BEEP_Blink(2);           // ⚠️ 在 bluetoothTimer 回调中调用
    Flag.BlueBeepCnt = 2;
}

// bsp_beep.c
void BEEP_Blink(uint8_t num) {
    for(uint8_t i = 0; i < num; i++){
        macBEEP_ON();
        tickk = rt_tick_get();
        rt_thread_delay_until(&tickk, 150);  // ⚠️ 阻塞操作
        macBEEP_OFF();
        tickk = rt_tick_get();
        rt_thread_delay_until(&tickk, 150);  // ⚠️ 阻塞操作
    }
}
```

**问题**：`bluetoothTimer` 是软件定时器（`RT_TIMER_FLAG_SOFT_TIMER`），其回调在 timer 线程中执行。`BEEP_Blink` 使用 `rt_thread_delay_until` 阻塞 150ms×2×num，会阻塞整个 timer 线程，导致所有其他软件定时器（sysTimer、keyTimer、iapTimer、ledTimer）暂停。

**影响**：蓝牙连接时所有定时器暂停 300ms（2 次鸣叫），可能导致按键响应延迟、气压控制中断

**修复建议**：
- 方案 A：将蜂鸣器控制改为非阻塞方式（状态机 + 定时器计数）
- 方案 B：使用 `rt_timer_start` 创建一次性定时器控制蜂鸣器时序

---

#### P1-3: main() 中 `for(; variable < N; variable = M)` 模式的脆弱性

**文件**：`cubemx/Src/main.c` 多处

```c
for(; BluetoothInitCnt < 1; BluetoothInitCnt = 2){
    bluetoothTimer_Init();
    macTk1300f_Uart1_Init();
    // ...
}
```

**问题**：这种 `for` 循环被用作"只执行一次"的机制，但存在以下风险：
1. 如果 `BluetoothInitCnt` 被外部修改为 >= 2，操作永远不会执行
2. 如果 `BluetoothInitCnt` 被意外重置为 0，操作会重复执行
3. 代码可读性差，意图不明确

**影响**：维护困难，潜在的初始化重复或遗漏

**修复建议**：使用 `static bool initialized = false; if(!initialized) { ... initialized = true; }` 替代

---

#### P1-4: 压力传感器 UART1/UART3 为 RX-only 但尝试发送数据

**文件**：`rtt_uart1_tk1300f.c` 第 62 行，`rtt_uart3_tk1300f.c` 第 62 行

```c
// CubeMX 配置: huart1.Init.Mode = UART_MODE_RX;
sendNum = rt_device_write(serial1, RT_NULL, "usart1 is opened!\r\n", 19);
```

**问题**：UART1 和 UART3 在 CubeMX 中配置为 RX-only 模式，但初始化函数中调用 `rt_device_write` 尝试发送调试字符串。RT-Thread 的串口驱动在 RX-only 模式下调用 write 会失败或产生总线错误。

**影响**：初始化时可能产生 UART 总线错误，不影响核心功能但浪费 CPU 周期

**修复建议**：删除 RX-only 串口的 `rt_device_write` 调用，或在写入前检查串口模式

---

#### P1-5: main() 函数声明为 __WEAK

**文件**：`cubemx/Src/main.c` 第 60 行

```c
__WEAK int main(void)
```

**问题**：`main` 函数使用 `__WEAK` 属性声明，意味着可以被其他模块的 `main` 覆盖。对于系统入口函数，这是不寻常的，可能导致启动到错误的 `main` 实现。

**影响**：如果其他 .c 文件中定义了 `main`，系统会执行错误的入口

**修复建议**：移除 `__WEAK` 属性

---

#### P1-6: Flash 写入未检查地址对齐

**文件**：`macFlash.c` `write()` 函数

```c
static int write(long offset, const uint8_t *buf, size_t size) {
    uint32_t addr = stm32f1_onchip_flash.addr + offset;
    for (i = 0; i < size; i++, buf++) {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, *buf);  // 每次写 4 字节
        // 但 *buf 是 uint8_t，实际只写入 1 字节的数据到 4 字节位置
        addr += 4;
    }
}
```

**问题**：
1. `FLASH_TYPEPROGRAM_WORD` 写入 32bit，但 `*buf` 是 uint8_t，高 24 位为 0，会覆盖原有数据
2. `size` 参数语义不明确（是字数还是字节数？）
3. 未检查 offset 是否 4 字节对齐

**影响**：如果 Flash 用于存储升级标志以外的数据，可能导致数据损坏

**修复建议**：明确 size 语义，添加对齐检查，或改用 `FLASH_TYPEPROGRAM_HALFWORD`

---

#### P1-7: AdjustValue 无边界保护

**文件**：`rtt_system_work.c` `Timing_50ms()` 函数

```c
int AdjustValue = 0;
// ...
if(Record.Output_PressDat < 2150){
    AdjustValue--;
    Proportional_Valve_ENBL_PWM_Set(1000, 820 - AdjustValue);
}
if(Record.Output_PressDat > 2250){
    AdjustValue++;
    Proportional_Valve_ENBL_PWM_Set(1000, 820 - AdjustValue);
}
```

**问题**：`AdjustValue` 是 `int` 类型，无上下限保护。如果气压持续偏低或偏高，`AdjustValue` 会持续递增/递减，最终导致：
- 整数溢出（未定义行为）
- PWM 占空比参数超出合理范围（负值或超大值）

**影响**：极端情况下比例阀输出异常

**修复建议**：添加边界检查：
```c
#define ADJUST_MAX 100
#define ADJUST_MIN -100
if(AdjustValue > ADJUST_MIN) AdjustValue--;
// ...
```

---

### P2 — 中（代码质量/可维护性）

#### P2-1: LED 宏定义中 MESSAGE 和 STATUS 引脚反转

**文件**：`bsp_led.h`

```c
// 在设备上对应方块的灯
#define macLED_MESSAGE_OFF()  HAL_GPIO_WritePin(LED_Status_GPIO_Port, LED_Status_Pin, ...)  // ⚠️ 用的是 Status 引脚
// 在设备上对应圆圈的灯
#define macLED_STATUS_OFF()   HAL_GPIO_WritePin(LED_Message_GPIO_Port, LED_Message_Pin, ...)  // ⚠️ 用的是 Message 引脚
```

**问题**：MESSAGE 宏控制的是 Status 引脚，STATUS 宏控制的是 Message 引脚，命名与实际引脚相反。

**影响**：LED 显示逻辑混乱，但不影响功能（只要调用方使用正确的宏名）

**修复建议**：统一宏名与引脚的对应关系，或添加注释说明

---

#### P2-2: 串口发送函数每次调用都执行 rt_device_find

**文件**：`rtt_uart2_Decode.c` `USART2_Send_Command_to_Principal()`，`rtt_uart4_Decode.c` `USART4_Send_Command_to_Principal()`

```c
void USART2_Send_Command_to_Principal(...) {
    // ...
    dev = rt_device_find("uart2");  // ⚠️ 每次发送都查找设备
    rt_enter_critical();
    rt_device_write(dev, RT_NULL, SendDat, 9+DataLen);
    rt_exit_critical();
}
```

**问题**：`rt_device_find` 每次调用都遍历设备链表查找设备，效率低。应在初始化时缓存设备句柄。

**影响**：每次通信增加不必要的 CPU 开销

**修复建议**：在初始化时将设备句柄存入全局变量，发送时直接使用

---

#### P2-3: 发送函数在临界区内执行串口写入

**文件**：`rtt_uart2_Decode.c` `USART2_Send_Command_to_Principal()`，`rtt_uart4_Decode.c` `USART4_Send_Command_to_Principal()`

```c
rt_enter_critical();
rt_device_write(dev, RT_NULL, SendDat, 9+DataLen);  // ⚠️ 临界区内执行 I/O
rt_exit_critical();
```

**问题**：`rt_device_write` 在中断模式下可能需要等待发送完成，耗时较长。在临界区内执行会阻塞所有中断和调度。

**影响**：发送数据期间系统响应延迟

**修复建议**：使用 `rt_mutex` 替代临界区保护，或使用 DMA + 完成量机制

---

#### P2-4: 卡尔曼滤波模块为空文件

**文件**：`macMATH/Scr/karman.c`

**问题**：文件仅包含头文件引用，无任何实现代码。

**影响**：无功能影响，但表明卡尔曼滤波功能未实现

---

#### P2-5: CheckStartPress 校准使用浮点除法

**文件**：`rtt_uart1_tk1300f.c`

```c
uint16_t CheckStartPress = 0;
// ...
CheckStartPress /= 10.0;  // ⚠️ uint16_t 除以 float
```

**问题**：`uint16_t` 除以 `10.0`（double）会被隐式转换为 double 运算，结果再截断为 uint16_t。在 Cortex-M3 上浮点运算较慢（软件模拟）。

**影响**：仅在启动校准时执行一次，影响可忽略

**修复建议**：改用整数除法 `CheckStartPress /= 10;`

---

#### P2-6: 系统休眠超时可能被意外重置

**文件**：`rtt_system_work.c` `Timing_10ms()`

```c
if((Flag.APPWorkStatus == 1) && (Flag.SysConnectStatus == 1)){
    Record.System_Sleep_Time++;
    if(Record.System_Sleep_Time > 18000){
        Flag.SleepMode = 1;
    }
}
else{
    Record.System_Sleep_Time = 0;
    Flag.SleepMode = 0;  // ⚠️ 会清除已设置的休眠标志
}
```

**问题**：当 `APPWorkStatus != 1` 或 `SysConnectStatus != 1` 时，`Flag.SleepMode` 被强制清零。如果系统在特定时序下短暂切换状态，休眠计数器会被重置。

**影响**：系统可能无法正常进入休眠

---

#### P2-7: 无看门狗配置

**问题**：系统未配置 IWDG 或 WWDG。如果程序跑飞或死锁，无自动恢复机制。

**影响**：系统异常时需要手动复位

**修复建议**：配置独立看门狗（IWDG），超时时间 2~4 秒

---

#### P2-8: Flash 操作未关闭中断

**文件**：`macFlash.c` `write()` 和 `erase()` 函数

**问题**：Flash 擦写期间未关闭中断。STM32F1 的 Flash 操作期间如果发生中断，可能导致 Bus Fault。

**影响**：极低概率下可能导致 HardFault

**修复建议**：在 Flash 操作前后使用 `__disable_irq()` / `__enable_irq()` 保护

---

## 6. 修复建议汇总

| 优先级 | 编号 | 问题 | 修复方案 | 工作量 |
|--------|------|------|----------|--------|
| P0 | P0-1 | IAP 临界区内阻塞 | 改用专用升级线程，删除临界区 | 中 |
| P0 | P0-2 | 老化模式条件 Bug | `Flag.OldMode` → `Flag.Old_WorkMode` | 小 |
| P0 | P0-3 | 串口接收信号量顺序 | sem_take 移到 rt_device_read 之前 | 小 |
| P0 | P0-4 | CRC16_H=0 丢弃指令 | 使用独立帧完成标志位 | 小 |
| P1 | P1-1 | 共享变量无同步 | 关键变量加 mutex 或临界区 | 中 |
| P1 | P1-2 | 蜂鸣器阻塞定时器 | 改为非阻塞状态机 | 中 |
| P1 | P1-3 | for 循环一次性模式 | 改用 static bool 标志 | 小 |
| P1 | P1-4 | RX-only 串口发送 | 删除 write 调用 | 小 |
| P1 | P1-5 | main __WEAK | 移除 __WEAK | 小 |
| P1 | P1-6 | Flash 写入对齐 | 添加对齐检查，明确 size 语义 | 小 |
| P1 | P1-7 | AdjustValue 无边界 | 添加上下限保护 | 小 |
| P2 | P2-1 | LED 宏引脚反转 | 统一命名或添加注释 | 小 |
| P2 | P2-2 | 串口设备重复查找 | 缓存设备句柄 | 小 |
| P2 | P2-3 | 临界区内 I/O | 改用 mutex | 小 |
| P2 | P2-4 | 卡尔曼滤波空文件 | 实现或删除 | - |
| P2 | P2-5 | 浮点除法 | 改用整数除法 | 小 |
| P2 | P2-6 | 休眠计数重置 | 重构休眠状态机 | 中 |
| P2 | P2-7 | 无看门狗 | 配置 IWDG | 小 |
| P2 | P2-8 | Flash 操作未关中断 | 添加 __disable_irq 保护 | 小 |

---

## 7. 变更记录

| 版本 | 日期 | 变更内容 | 作者 |
|------|------|----------|------|
| v1.0 | 2025-06-01 | 初版完整代码分析报告 | 龙虾团队 |

---

*报告结束*
