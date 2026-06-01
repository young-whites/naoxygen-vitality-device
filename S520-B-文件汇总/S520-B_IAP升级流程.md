# S520-B IAP 升级流程

## 1. 系统概述

S520-B 采用 IAP（In-Application Programming）机制实现固件升级。系统包含两个固件：

1. **Bootloader**：位于 Flash 起始地址 (0x08000000)，负责固件升级
2. **APP**：位于 Flash 0x08004000，为实际应用固件

升级通过 Ymodem 协议从串口接收固件数据，支持主板（MAJOR）和子板（MINOR）两种设备的固件升级。

---

## 2. Flash 分区布局

### 2.1 主控板 Flash (STM32F103RBT6, 128KB)

```
0x08000000 ┌─────────────────────┐
           │   Bootloader (16KB)  │
           │   0x08000000~        │
           │   0x08003FFF         │
0x08004000 ├─────────────────────┤
           │                     │
           │   APP 应用程序       │
           │   (104KB)           │
           │                     │
0x0801DFFF ├─────────────────────┤
0x0801E000 │   IAP升级标志 (2B)  │
           │   FLASH_APP_UP_ADDR │
0x0801E001 ├─────────────────────┤
           │                     │
           │   参数存储区         │
           │                     │
0x0801FFF0 ├─────────────────────┤
0x0801FFF0 │   BL升级标志 (2B)   │
           │   FLASH_SAVE_ADDR   │
0x0801FFFF └─────────────────────┘
```

### 2.2 Flash 地址定义

| 宏定义 | 值 | 说明 |
|--------|-----|------|
| `FLASH_BASE_ADDR` | 0x08000000 | Flash 起始地址 |
| `IAP_ADDR` | 0x08004000 | APP 起始地址 |
| `FLASH_SAVE_ADDR` | 0x0801FFF0 | Bootloader 升级标志位地址 |
| `FLASH_APP_UP_ADDR` | 0x0801E000 | APP 端升级标志位地址 |
| `IAP_PARAM_ADDR` | 末尾1页 | 参数存储起始地址 |
| `FLASH_SIZE` | 0x20000 (128KB) | Flash 总大小 |
| `PAGE_SIZE` | 0x400 (1KB) | 页大小 |

---

## 3. Bootloader 启动流程

### 3.1 上电启动

```
上电/复位
  │
  ▼
NVIC_PriorityGroupConfig(4)      // 中断优先级分组
  │
  ▼
Sys_Clock_Init()                 // HSE 8MHz → PLL × 9 = 72MHz
  │
  ▼
Gpio_init()                      // GPIO 初始化（LED、串口、风扇等）
  │
  ▼
peripheral_init()                // 外设初始化
  │  ├─ NVIC 配置
  │  ├─ USART2 初始化 (9600bps)
  │  ├─ 禁用 USART1/3/UART4 及 DMA
  │  └─ IWDG 初始化
  │
  ▼
读取升级标志 (FLASH_SAVE_ADDR → UpgradeParams.uiCommandFlag)
  │
  ▼
┌─────────────────────────────┐
│ uiCommandFlag == 1 ?         │
│   ├─ YES → 进入升级模式      │
│   └─ NO  → 跳转到 APP       │
└─────────────────────────────┘
```

### 3.2 升级标志管理

| 标志地址 | 初始值 | 写入时机 | 含义 |
|----------|--------|----------|------|
| FLASH_SAVE_ADDR (0x0801FFF0) | 0xFFFF | 升级完成后写入 0 | 0=正常跳转APP |
| FLASH_SAVE_ADDR (0x0801FFF0) | — | APP跳转BL前写入 1 | 1=进入升级模式 |

**APP 端触发升级流程：**
```c
// main.c 中
if (Flag.update) {
    STMFLASH_Write(FLASH_APP_UP_ADDR, &Flag.update, 1); // 写入标志
    Flag.update = 0;
    IWDG_Feed();
    jump_sys_bootloader();  // 跳转到 Bootloader
}
```

---

## 4. Bootloader 主循环

```c
int main(void)
{
    // 初始化...
    STMFLASH_Read(FLASH_SAVE_ADDR, &UpgradeParams.uiCommandFlag, 1);

    while (1) {
        if (UpgradeParams.uiCommandFlag == 1) {
            // 升级模式
UPG:
            IAP_FirmwareUpgradeStep(USART2, &UpgradeParams.ucStep);
        } else {
            // 正常模式：跳转到APP
            __disable_irq();
            IAP_JumpToApplication();
            // 如果跳转失败，进入升级模式
            UpgradeParams.uiCommandFlag = 1;
            goto UPG;
        }
    }
}
```

---

## 5. 升级状态机

### 5.1 状态定义

```c
typedef enum {
    eStart = 0,           // 开始
    eCmdID,               // 等待设备命令
    eDeviceVerify,        // 设备验证
    eMajorUpdateGrade,    // 主板升级
    eMinorUpdateGrade     // 子板升级
} eUpgradeState;
```

### 5.2 状态转换流程

```
┌──────────┐
│  eStart   │ ──发送ACK──> ┌──────────┐
└──────────┘              │  eCmdID   │
                          └────┬─────┘
                               │ 接收 "MAJOR" 或 "MINOR"
                               ▼
                     ┌──────────────────┐
                     │  eDeviceVerify   │
                     └───┬──────────┬───┘
                         │          │
                  "MAJOR" │          │ "MINOR"
                         ▼          ▼
              ┌──────────────┐  ┌──────────────────┐
              │eMajorUpdate  │  │eMinorUpdateGrade │
              │   Grade      │  │                  │
              └──────┬───────┘  └────────┬─────────┘
                     │                   │
                     ▼                   ▼
              ┌──────────────┐  ┌──────────────────┐
              │DownloadFirm  │  │ 等待子板应答      │
              │  ware()      │  │ → DownloadFirm   │
              └──────┬───────┘  │    ware()        │
                     │          └────────┬─────────┘
                     ▼                   ▼
              ┌──────────────┐  ┌──────────────────┐
              │  系统复位     │  │   系统复位        │
              └──────────────┘  └──────────────────┘
```

---

## 6. Ymodem 协议实现细节

### 6.1 升级命令握手

Bootloader 启动后进入升级模式，会周期性发送 "UPGR" 字符串提醒上位机准备发送固件：

```
Bootloader                    上位机/APP
    │                              │
    │──── "UPGR" ─────────────────>│
    │──── "UPGR" ─────────────────>│
    │     ...                       │
    │<──── "MAJOR" ────────────────│  (5字节设备标识)
    │                              │
    │──── ACK(0x06) ──────────────>│
    │                              │
    │     开始 Ymodem 接收          │
```

### 6.2 设备标识

| 标识 | 值 | 说明 |
|------|-----|------|
| "MAJOR" | M,A,J,O,R | 主板固件升级 |
| "MINOR" | M,I,N,O,R | 子板固件升级 |

### 6.3 文件名校验

第一个数据包（SOH, 128字节）包含文件名和文件大小：

```
包0 数据域结构：
┌──────────────────────┬──────────┐
│ 文件名 (N字节, \0结尾) │ 文件大小 │
│ "MAJOR.bin"          │ "12345"  │
└──────────────────────┴──────────┘
```

- 主板升级：文件名必须匹配 "MAJOR.bin"
- 子板升级：文件名必须匹配 "MINOR.bin"
- 文件大小必须小于 Flash 可用空间

### 6.4 数据接收

```
YModem_Receive() 主循环：

  while(1) {
      // 接收一个数据包
      receive_status = YModem_RecvPacket(device_id, USARTx, packet_data, &packet_length, timeout);

      switch (receive_status) {
          case 0:  // 正常接收
              switch (packet_length) {
                  case -1:  // 发送端中止 → 回复 CA
                  case  0:  // 接收结束 → 回复 ACK
                  default:  // 数据包
                      // 校验序号
                      // CRC16-CCITT 校验
                      // 第0包：校验文件名+大小，擦除Flash
                      // 后续包：memcpy → IAP_UpdateProgram() 写入Flash
                      // 每包回复 ACK
              }
              break;
          case 1:  // 用户终止 → 回复 CA+CA
          default: // 超时/错误 → 发送 'C' 重试
      }

      if (file_done || session_done) break;
  }
```

### 6.5 LED 指示

升级过程中，PA1 引脚的 LED 会闪烁指示进度：

| 设备 | 闪烁间隔 | 说明 |
|------|----------|------|
| 主板 (MAJOR) | 50个包 闪烁一次 | 每接收50个数据包LED翻转 |
| 子板 (MINOR) | 100个包 闪烁一次 | 每接收100个数据包LED翻转 |

---

## 7. Flash 写入策略

### 7.1 擦除策略

```c
void IAP_FlashEease(u32 size)
{
    // 计算需要擦除的页数
    nbrOfPage = (size % PAGE_SIZE != 0) ? (size / PAGE_SIZE + 1) : (size / PAGE_SIZE);

    // 逐页擦除
    for (eraseCounter = 0; eraseCounter < nbrOfPage; eraseCounter++) {
        FLASH_ErasePage(IAP_ADDR + eraseCounter * PAGE_SIZE);
    }

    // 擦除参数页
    FLASH_ErasePage(IAP_PARAM_ADDR);
}
```

- 擦除范围：从 IAP_ADDR (0x08004000) 开始
- 擦除时机：收到第一个数据包后，根据文件大小计算擦除范围
- 擦除粒度：按页擦除（1KB/页）

### 7.2 编程策略

```c
s8 IAP_UpdateProgram(u32 addr, u32 size)
{
    for (ii = 0; ii < size; ii += 4) {
        // 按字(32位)编程
        FLASH_ProgramWord(flashptr, *(u32 *)addr);

        // 验证
        if (*(u32 *)flashptr != *(u32 *)addr) {
            // 失败：擦除该页 → 重写
            FLASH_ErasePage(flashptr);
            FLASH_ProgramWord(flashptr, flashptr1);

            // 再次验证
            if (*(u32 *)flashptr != flashptr1) {
                return -1;  // 彻底失败
            }
        }
        flashptr += 4;
        addr += 4;
    }
    return 0;  // 成功
}
```

**写入特点：**
- 按 4 字节（32位）对齐写入
- 每次写入后立即回读验证
- 验证失败时执行"擦除-重写-再验证"三步恢复
- 写入地址从 IAP_ADDR (0x08004000) 开始递增

### 7.3 写保护处理

```c
void IAP_DisableFlashWPR(void)
{
    blockNum = (IAP_ADDR - FLASH_BASE_ADDR) >> 12;  // 计算块号
    UserMemoryMask = ((u32)(~((1 << blockNum) - 1))); // 计算掩码

    if ((FLASH_GetWriteProtectionOptionByte() & UserMemoryMask) != UserMemoryMask) {
        FLASH_EraseOptionBytes();  // 擦除选项字节，关闭写保护
    }
}
```

---

## 8. APP 跳转机制

### 8.1 Bootloader → APP 跳转

```c
void IAP_JumpToApplication(void)
{
    // 校验栈指针合法性
    if (((*(__IO u32 *)IAP_ADDR) & 0x2FFE0000) == 0x20000000) {
        // 读取复位向量
        JumpAddress = *(__IO u32 *)(IAP_ADDR + 4);
        Jump_To_Application = (pFunction)JumpAddress;

        // 设置主栈指针
        __set_MSP(*(__IO u32 *)IAP_ADDR);

        // 跳转执行
        Jump_To_Application();
    } else {
        // APP 无效，写入升级标志
        IAP_DisableFlashWPR();
        upg_flag = 1;
        STMFLASH_Write(FLASH_SAVE_ADDR, &upg_flag, 1);
    }
}
```

**跳转前校验：**
- 读取 IAP_ADDR 处的值（APP的栈指针初始值）
- 检查高3位是否为 0x2FFE（即值在 0x20000000~0x2001FFFF 范围内）
- 若校验失败，认为 APP 无效，写入升级标志

### 8.2 APP → Bootloader 跳转

```c
void jump_sys_bootloader(void)
{
    uint32_t boot_address = 0x08000000;

    // 关闭全局中断
    __disable_irq();

    // 复位 SysTick
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

    // 复位 RCC 到 HSI
    RCC_DeInit();

    // 清除所有 NVIC 中断
    for (i = 0; i < 8; i++) {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }

    // 使能全局中断
    __enable_irq();

    // 读取 Bootloader 的复位向量
    p_sys_bootloader_run = (void (*)(void))(*((uint32_t*)(boot_address + 4)));

    // 设置 MSP
    __set_MSP(*(uint32_t*)boot_address);

    // 清除 CONTROL 寄存器（确保特权模式+MSP）
    __set_CONTROL(0);

    // 跳转到 Bootloader
    p_sys_bootloader_run();
}
```

### 8.3 跳转安全保障

| 检查项 | 说明 |
|--------|------|
| 栈指针校验 | 高3位必须为 0x2FFE（SRAM范围） |
| 中断关闭 | 跳转前关闭所有 NVIC 中断使能和挂起 |
| SysTick 复位 | 清零 CTRL/LOAD/VAL 寄存器 |
| RCC 复位 | 恢复到 HSI 默认时钟 |
| MSP 重新设置 | 确保栈指针指向正确位置 |
| CONTROL 清零 | 确保特权模式 + 使用 MSP |

---

## 9. 子板（MINOR）升级流程

子板升级由主板中转，流程略有不同：

```
Bootloader                    主板                    子板(手柄)
    │                          │                        │
    │<─── "MINOR" ────────────│                        │
    │──── ACK ────────────────>│                        │
    │                          │─── "UPGR" ───────────>│
    │                          │<── 应答 ──────────────│
    │                          │                        │
    │                          │─── ACK(0x06) ────────>│ (告诉Bootloader子板在线)
    │                          │                        │
    │     Ymodem 数据包 ───────│─── 转发 ──────────────>│
    │                          │<── ACK ───────────────│
    │                          │                        │
    │     ...                  │     ...                │
    │                          │                        │
    │     接收完成              │                        │
    │──── 系统复位 ───────────>│                        │
```

---

## 10. 升级参数结构

```c
typedef struct {
    u16  uiCommandFlag;         // 升级命令标志 (1=升级, 0=正常)
    u8   ucStep;                // 升级步骤 (eUpgradeState)
    u8   ucUart2ReceiveFlag;    // USART2 接收标志
    u8   ucUart3ReceiveFlag;    // USART3 接收标志
    u8   UP_STEP;               // 升级结果步骤
} UpgradeParams_t;
```

---

## 11. 升级结果码

| UP_STEP | 说明 |
|---------|------|
| 0 | 升级成功 |
| 1 | 固件过大（超出Flash空间） |
| 2 | Flash 烧写错误 |
| 3 | 用户终止 |
| 4 | CRC 校验失败 |
| 5 | 序号和补码错误 |
| 6 | 文件名错误 |
| 7 | 其他错误 |
| 8 | 接收超时 |

---

## 变更记录

| 版本 | 日期 | 改动摘要 |
|------|------|----------|
| V1.0 | 2026-06-01 | 初始版本，基于全部源码分析生成 |

---

*文档生成工具: OpenClaw AI Agent*
