# S520-B Mechanical Handle Control Logic Document

## 1. System Overview

This project is based on the STM8S microcontroller. It receives commands from the host via UART1 (9600bps) to control a DC motor's forward, backward, and stop operations, with travel protection via limit switches.

## 2. Hardware Pin Definitions

| Function | Pin | Description |
|----------|-----|-------------|
| ENA (Motor Enable) | PC6 | High level enables motor driver |
| IN1 (Direction Control 1) | PC4 | TIM1_CH2 PWM output |
| IN2 (Direction Control 2) | PC7 | TIM1_CH4 PWM output |
| PC1 (Rear Limit Switch) | PD2 | High level triggered |
| PC2 (Front Limit Switch) | PB4 | High level triggered |

## 3. Motor Drive Principle

Uses ENA + IN1/IN2 direction control scheme:

| ENA | IN1 | IN2 | Action |
|-----|-----|-----|--------|
| LOW | X | X | Stop (driver disabled) |
| HIGH | PWM | LOW | Forward |
| HIGH | LOW | PWM | Backward |
| HIGH | LOW | LOW | Brake |

### 3.1 PWM Configuration

- TIM1 clock: 16MHz, prescaler 100 → counter frequency 158.4kHz
- PWM mode: TIM1_OCMODE_TOGGLE (START) / TIM1_OCMODE_PWM2 (STOP)
- `speed_mode[0~3]`: Forward speed tiers (smaller value = faster)
- `speed_mode[4] = 0`: Stop placeholder (not used for PWM)
- `speed_mode[5] = 300`: Fixed backward speed

### 3.2 Speed Tiers

| Tier | Index | Speed Value (Example) | Description |
|------|-------|----------------------|-------------|
| Tier 0 | `speed_mode[0]` | -0.1094×25212 + 5740 | Slowest |
| Tier 1 | `speed_mode[1]` | -0.1094×25212 + 6200 | Slow |
| Tier 2 | `speed_mode[2]` | -0.1094×25212 + 5850 | Default |
| Tier 3 | `speed_mode[3]` | -0.1094×25212 + 5350 | Fastest |
| Backward | `speed_mode[5]` | 300 (fixed) | Backward only |

## 4. System Startup Sequence

```
Power On
 │
 ├─ disableInterrupts()
 ├─ CLK_HSIPrescalerConfig(HSIDIV1)    // 16MHz clock
 ├─ GPIO_Config()                       // Initialize all GPIOs
 ├─ USARTAPP_Config()                   // Initialize UART1 (9600, 8N1)
 ├─ TIM2_Config()                       // Initialize TIM2 (1ms interrupt)
 ├─ enableInterrupts()
 │
 ├─ motor init (direction=0, limit_rear=0, limit_front=0)
 ├─ Delay_ms(100)
 ├─ check_limit()                       // 【CRITICAL】Check limit switches immediately at startup
 ├─ Write_Option_Byte()
 ├─ EEPROM init
 ├─ Calculate speed_mode[0~3]
 ├─ senddata(REVISION_CMD, 75)         // Report version
 ├─ senddata(HANDLE_CHECK, 0)          // Report handle online
 │
 └─ Enter main loop: while(1) { check_limit(); }
```

### 4.1 Power-On Limit Detection

`check_limit()` is called immediately after power-on. If the motor's initial position is already pressing a limit switch:
- The corresponding limit flag is set immediately
- Subsequent motion commands in that direction are blocked
- Only reverse direction motion is allowed

## 5. Limit Protection Logic

### 5.1 Limit State Machine

```
                    ┌─────────────────────────────────┐
                    │         Normal State             │
                    │   limit_front=0, limit_rear=0    │
                    │   Forward/Backward both allowed  │
                    └──────────┬──────────┬────────────┘
                               │          │
                   Front limit │          │ Rear limit
                    triggered  │          │ triggered
                               ▼          ▼
                ┌──────────────────┐  ┌──────────────────┐
                │  Front Limit Lock│  │  Rear Limit Lock │
                │  limit_front=1   │  │  limit_rear=1    │
                │  Forward blocked │  │  Backward blocked│
                │  Only backward   │  │  Only forward    │
                └────────┬─────────┘  └────────┬─────────┘
                         │                     │
              Motor moves│           Motor moves│
              backward   │           forward    │
              away       │           away       │
                         ▼                     ▼
                    ┌─────────────────────────────────┐
                    │       Return to Normal State     │
                    │   limit_front=0, limit_rear=0    │
                    └─────────────────────────────────┘
```

### 5.2 `check_limit()` Execution Logic

Called continuously in the main loop (after each TIM2 interrupt), detects PC1/PC2 limit switches:

| Condition | Action |
|-----------|--------|
| PC1=HIGH and `limit_rear=0` | Set `limit_rear=1`, call `motor_stop()` |
| PC1=LOW and `direction==2` (forward) | Clear `limit_rear=0` (motor moved away from rear limit) |
| PC2=HIGH and `limit_front=0` | Set `limit_front=1`, call `motor_stop()` |
| PC2=LOW and `direction==1` (backward) | Clear `limit_front=0` (motor moved away from front limit) |

**Key Rule**: Limit flags are only cleared when the motor moves in the **reverse direction AND the switch is released**. `motor_stop()` does NOT clear limit flags.

### 5.3 Limit Interception of Motion Commands

```c
motor_forward(mode):
    if (limit_front == 1) return;  // Front limit active → forward command ignored

motor_backward():
    if (limit_rear == 1) return;   // Rear limit active → backward command ignored
```

## 6. Motor Control Functions

### 6.1 `motor_stop()` — Stop

```c
void motor_stop(void)
{
    TIM1_DeInit();                    // Reset TIM1
    TIM1_Cmd(DISABLE);                // Disable TIM1
    TIM1_CtrlPWMOutputs(DISABLE);    // Disable PWM outputs
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, DISABLE);  // Turn off TIM1 clock

    GPIO_WriteLow(ENA);               // ENA = LOW (driver disabled)
    GPIO_Init(IN1, OUT_PP_LOW);       // IN1 = LOW
    GPIO_Init(IN2, OUT_PP_LOW);       // IN2 = LOW

    motor.direction = 0;
    // Note: does NOT clear limit_front / limit_rear
}
```

**Design Decision**: Directly disables TIM1 instead of calling `pwm_init(STOP, 0, 0)`, to avoid `speed_mode[4]=0` causing TIM1 period=0 which triggers a ~158kHz interrupt storm, blocking TIM2 serial parsing.

### 6.2 `motor_forward(mode)` — Forward

```c
void motor_forward(u8 mode)
{
    if (motor.limit_front) return;    // Front limit intercept

    motor.direction = 2;
    TIM1_DeInit();                    // Safely disable TIM1
    TIM1_Cmd(DISABLE);
    TIM1_CtrlPWMOutputs(DISABLE);

    GPIO_WriteLow(ENA);               // ENA LOW → HIGH (pulse start)
    GPIO_WriteHigh(ENA);
    pwm_init(START, speed_mode[mode], 2);  // Start PWM, direction=2
}
```

### 6.3 `motor_backward()` — Backward

```c
void motor_backward(void)
{
    if (motor.limit_rear) return;     // Rear limit intercept

    motor.direction = 1;
    TIM1_DeInit();                    // Safely disable TIM1
    TIM1_Cmd(DISABLE);
    TIM1_CtrlPWMOutputs(DISABLE);

    GPIO_WriteLow(ENA);               // ENA LOW → HIGH (pulse start)
    GPIO_WriteHigh(ENA);
    pwm_init(START, speed_mode[5], 1);  // Start PWM, direction=1, fixed speed
}
```

## 7. Serial Command Processing

### 7.1 Command Routing

```
Serial data → UART1_RX_IRQHandler (enqueue)
           → TIM2_IRQHandler (1ms) → CommunicationWithMainBoard()
           → ReceiveMessageDecode() → DecodeMessageHandle()
           → CMD_MessageHandle() / SENDDATA_STATUS_MessageHandle()
```

### 7.2 Downlink Command Processing

#### 7.2.1 Motor Mode `0x17` (MOTOR_MODE_CMD)

| DataField[2] | Action |
|--------------|--------|
| 0 or 5 | `motor_stop()` |
| 1~4 | `motor_forward(motor_mode)` |

#### 7.2.2 Motor Control `0x2D` (MOVE_CMD)

| DataField[2] | Action | Description |
|--------------|--------|-------------|
| 0 | `motor_stop()` | Stop |
| 1 | `motor_backward()` | Backward |
| 2 | `motor_forward(motor_mode)` | Forward (using current speed tier) |
| 3 | `motor_backward()` + `senddata(0x2F, 1)` | Reset (backward + notify host) |
| 4 | `motor_backward()` + `Delay_ms(600)` + `motor_stop()` | Backward 0.6s then stop |

#### 7.2.3 Speed Switch `0x15` (LITTLE_CMD)

| DataField[2] | motor_mode | Description |
|--------------|------------|-------------|
| 5 | 0 | Slowest |
| 11~13 | 1 | Slow |
| 20 | 2 | Default |
| 27~29 | 3 | Fastest |

#### 7.2.4 Acknowledge `0x1C` (ACK_CMD)

Reply `HANDLE_CHECK(0xBB)` + data `0x00`.

### 7.3 Uplink Data

| Function Code | Description |
|---------------|-------------|
| `0x19` | Version number (current: 75) |
| `0xBB` | Handle online response |
| `0x2F` | Return flag (0=returned, 1=returning) |

## 8. Interrupt Architecture

| Interrupt Source | Frequency | Function |
|-----------------|-----------|----------|
| TIM2 Update | 1ms | Calls `CommunicationWithMainBoard()` to parse serial data |
| UART1 RX | Per byte | Receives serial data into queue |
| TIM1 Update | PWM period | PWM operation (triggers during normal operation) |

## 9. Key Design Decisions

### 9.1 Why doesn't `motor_stop()` call `pwm_init(STOP, 0, 0)`?

`speed_mode[4] = 0`, calling `pwm_init(STOP, 0, 0)` sets TIM1 period to 0, causing:
- TIM1 counter overflows every clock cycle
- TIM1 update interrupt fires at ~158kHz continuously
- TIM2 interrupt (serial parsing) is completely blocked
- System cannot receive any serial commands

Solution: Directly `TIM1_DeInit()` + `TIM1_Cmd(DISABLE)` to disable TIM1.

### 9.2 Why doesn't `motor_stop()` clear limit flags?

Limit flags are used to lock the motion direction. If `motor_stop()` cleared limit flags:
- Limit triggered → stop → flags cleared → forward command executes again → motor hits limit again
- The "only allow reverse direction after limit" protection logic would be broken

Correct behavior: Limit flags are cleared by `check_limit()` only when the motor moves in reverse direction AND the switch is released.

### 9.3 Why call `check_limit()` at power-on?

At power-on, `limit_front = 0`, `limit_rear = 0`. If the motor's initial position is already pressing a limit switch:
- Without `check_limit()`: flags are 0 → forward/backward commands execute → motor may hit limit
- With `check_limit()`: flags set immediately → corresponding direction is locked

## 10. State Variables

| Variable | Type | Description |
|----------|------|-------------|
| `motor.direction` | `volatile u8` | 0=stopped, 1=backward, 2=forward |
| `motor.limit_front` | `volatile u8` | Front limit flag (1=locked, 0=normal) |
| `motor.limit_rear` | `volatile u8` | Rear limit flag (1=locked, 0=normal) |
| `motor_mode` | `u8` | Current speed tier (0~3, default 2) |
| `speed_mode[6]` | `u32` | Speed array (index 0~3 forward tiers, 4=0 stop placeholder, 5=300 backward) |

## Change Log

| Version | Date | Changes |
|---------|------|---------|
| v1.0 | 2026-06-04 | Initial version, control logic documented from code analysis |
