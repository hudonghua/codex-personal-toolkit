---
name: MC_LCD - 7Control_V1.2 项目完整框架
description: 隧道机械臂控制系统架构、模块分工、HMI交互、标定流程
type: project
---

# MC_LCD - 7Control_V1.2 隧道机械臂控制系统框架

## 项目概述

**硬件平台**：LPC17xx (ARM Cortex-M3)
**应用**：隧道掘进机三臂（左中右）正运动学解算、传感器标定、HMI显示控制
**核心功能**：
- 三臂独立运动学解算（正解）
- 传感器原始值→物理量转换（零位补偿）
- 20点加工误差标定（Levenberg-Marquardt优化）
- LCD显示和按键交互
- CAN通信（两条总线）

---

## 软件模块架构

### 核心模块 (Src/)

| 模块 | 文件 | 功能 | 职责 |
|------|------|------|------|
| **正解引擎** | Arm200A_Kine.c/h | 运动学计算 | 3臂的12点坐标解算、参数标定、LM优化 |
| **参数存储** | Arm200A_Store.c | EEPROM存储 | 零位配置、加工误差参数、样本数据持久化 |
| **HMI显示** | App_lcd.c | LCD控制显示 | 菜单页面、参数显示、标定界面、告警提示 |
| **按键输入** | App_KEY.c | 键盘处理 | 按键扫描、长按检测、菜单导航 |
| **用户逻辑** | App_usr.c | 业务逻辑 | 传感器读取、标定流程控制、运动决策 |
| **任务调度** | Task.c/Task.h | 任务框架 | 周期任务管理、优先级调度 |
| **扫描监测** | ScanCycle.c | 性能检测 | 10ms周期监测、延时统计 |
| **CAN通信** | can.c/can2.c | CAN1/CAN2驱动 | CANopen协议、帧接收发送 |
| **硬件接口** | GPIO.c / pwm.c / adc.c / uart.c / i2c.c / timer.c | 外设驱动 | DI/DO、PWM、ADC、串口、I2C、定时器 |
| **主程序** | main.c | 系统入口 | 初始化、主循环（10ms周期）、事件分发 |

---

## 程序流程图

### 系统启动流程

```
main()
  ├─ SystemInit()           // LPC17xx系统初始化
  ├─ Sys_Param_Init()       // 参数初始化
  ├─ IO_Init()              // GPIO初始化
  ├─ BD_Patam_Init()        // 驱动参数初始化
  ├─ PWM_Init() / PWM_Start()  // PWM初始化
  ├─ I2CInit()              // I2C初始化
  ├─ Sys_Read_Info()        // 读系统信息
  ├─ CAN_Init() / CAN2_Init()  // CAN初始化
  ├─ Sys_Read_BD()          // 读驱动参数
  ├─ init_timer()           // 定时器初始化 → 10ms周期
  ├─ UARTInit()             // 串口0(LCD) / 串口1(无线模块)
  ├─ ADCInit() / ADC_StartCnv()  // ADC初始化
  ├─ ReadRetain_()          // 读EEPROM保存的参数
  ├─ LCD_GO_Page(0)         // LCD初始化显示
  └─ 主循环 while(1)
```

### 主循环 (10ms周期)

```
while(1):
  ├─ if(gTimeFlg_10mS)      // 每10ms执行一次
  │  ├─ MyLogic_10ms()       // 用户逻辑处理
  │  └─ gTimeFlg_10mS = 0
  │
  ├─ if(gTimeFlg_1000mS)    // 每1000ms执行一次
  │  └─ 长周期任务
  │
  ├─ if(gUart1RcvFlg)       // UART1数据接收
  │  ├─ Uart1_WL_Rcv()       // 接收无线模块数据
  │  └─ Uart1_WL_Send()      // 回复无线模块
  │
  ├─ if(gLCDPage.vSave)     // 参数保存请求
  │  ├─ SaveRetain_()        // 保存到EEPROM
  │  ├─ Arm200A_StoreSave()  // 标定参数保存
  │  └─ Sys_Write_BD()
  │
  ├─ Can_ask_rx()           // CAN异常检测
  └─ Sys_Prog_While()       // 系统后台函数
```

### 10ms核心处理 (MyLogic_10ms)

```
MyLogic_10ms()
  ├─ 传感器采集
  │  ├─ 读ADC原始值（角度、位移）
  │  ├─ 读DI信号（限位、开关）
  │  └─ 读CAN数据（远程命令）
  │
  ├─ 零位补偿
  │  └─ arm_zero_convert_one()  // raw → 物理量
  │
  ├─ 运动学解算（三臂并行）
  │  ├─ Arm200A_CalcLeft()   // 左臂正解
  │  ├─ Arm200A_CalcMid()    // 中臂正解
  │  └─ Arm200A_CalcRight()  // 右臂正解
  │
  ├─ 标定流程（if需要）
  │  ├─ 收集样本点
  │  ├─ LM优化参数
  │  └─ 更新拟合精度
  │
  ├─ 运动控制
  │  ├─ 解析HMI命令
  │  ├─ 计算PWM输出
  │  └─ 发送DO信号
  │
  └─ 显示刷新
     └─ 更新LCD数据
```

---

## HMI交互架构

### LCD显示系统

**硬件**：DGUS LCD屏
**通信**：UART0 (115200 bps)
**显示页面**：

| 页面ID | 名称 | 功能 | 交互 |
|--------|------|------|------|
| 0 | 主菜单 | 启动欢迎画面 | - |
| 1-2 | 工作界面 | 实时显示H点、参数、状态 | 按键导航 |
| 3-6 | 参数设置 | 调整传感器参数 | 上下左右键+确认 |
| 13-19 | 标定界面 | 零位采集、样本收集、拟合结果 | 标定流程引导 |
| 其他 | 告警/诊断 | 系统状态、错误信息 | - |

### 按键输入系统

**接口**：DI信号或CAN命令
**按键类型**：
- `Up/Down/Left/Right`：菜单导航、参数调整
- `OK/确认`：确认选择、启动操作
- `Return`：返回上一级
- `长按`：快速调整、切换模式

**处理流程**：
```
App_KEY_Scan()
  ├─ 扫描DI端口
  ├─ 去抖动处理（20ms）
  ├─ 检测上升沿
  ├─ 更新按键状态
  └─ 触发对应的LCD回调函数
```

---

## 标定流程完整说明

### 第1层：零位基准标定

**目标**：原始ADC值 → 物理量（角度/位移）

**采集过程**：
1. 机械臂移动到标准零位
2. 按键采集零位值→保存到`ARM200A_ZERO_CFG`
3. 记录该状态下10个ADC通道的原始值

**标定参数**（`ARM200A_ZERO_CFG`，共28个标量）：
- `root_zero_raw ~ body_pitch_zero_raw`：10个通道的零位ADC值
- `root_scale ~ body_pitch_scale`：10个通道的刻度因子（物理量/raw）
- `root_sign ~ body_pitch_sign`：10个通道的符号（±1）
- `cd_zero_mm, gg1_zero_mm`：两个伸缩段的初始长度

**实现函数**：
```c
Arm200A_ZeroInit()           // 初始化零位系统
Arm200A_ZeroAddSample()      // 添加一个零位样本
Arm200A_ZeroFitLeft/Mid/Right()  // 拟合三臂的零位参数
```

**验收标准**：20个样本最大误差 ≤ 3mm

---

### 第2层：加工误差标定

**目标**：修正设备固定的几何偏差

**采集过程**：
1. 机械臂移动到20个不同姿态
2. 每个姿态用全站仪实测H点真实坐标
3. 同时记录传感器输入（角度、位移）
4. 存储为`ARM200A_SAMPLE`（20个样本）

**标定参数**（`ARM200A_FIX`，共22个标量）：
- 11个3D向量偏移：o0o1, o1o, ob, ba, ac, d1e, ee1, e1f, fg, g1h, e1e2
- 1个长度修正：dd1_mm

**优化方法**：Levenberg-Marquardt算法
```
目标：最小化 Σ(H_calc - H_real)^2
迭代求解22个参数
```

**实现函数**：
```c
Arm200A_AddSampleByCurrentInput()   // 添加样本
Arm200A_RequestFit()                // 请求拟合
arm_fit_one()                       // 单臂LM拟合（内部）
arm_solve_linear()                  // Gauss消元求解线性方程
```

**验收标准**：
- 样本数：≥ 20个
- 最大残差：≤ 30mm
- RMS误差：≤ 10mm

---

### HMI标定流程（用户界面）

**菜单路径**：`主菜单 → 标定菜单 → 选择臂 → 选择标定类型`

#### 零位标定流程（LCD界面）

```
[零位标定菜单]
  ↓
按任意键开始收集零位
  ↓
[显示当前零位原始值]
  ↓
OK键确认 / 返回键返回
  ↓
拟合完成 → [显示拟合结果]
  ↓
保存参数 (gLCDPage.vSave = 1)
```

**代码调用链**：
```
IOFrame_17()  // LCD标定菜单
  ↓
Arm200A_ZeroAddSample(arm_id)
  ↓
Arm200A_ZeroFitLeft() / Mid() / Right()
  ↓
gLCDPage.vSave = 1  // 触发保存
```

#### 加工误差标定流程（LCD界面）

```
[样本收集菜单]
  ↓
移动臂到姿态1 → OK键采集 → [样本1/20]
  ↓
移动臂到姿态2 → OK键采集 → [样本2/20]
  ↓
... (重复20次)
  ↓
所有样本采集完毕
  ↓
拟合请求 (Arm200A_RequestFit)
  ↓
[LM优化过程中...] 显示迭代次数
  ↓
拟合完成 → [显示RMS误差、最大误差、样本数]
  ↓
保存参数
```

**代码调用链**：
```
IOFrame_13() / IOFrame_14()  // 样本收集菜单
  ↓
Arm200A_AddSampleByCurrentInput(arm_id, hx, hy, hz)
  ↓
Arm200A_RequestFit(arm_id)
  ↓
Arm200A_FitLeft() / Mid() / Right()  // 触发拟合
  ↓
arm_fit_one()  // LM优化（约50-200次迭代）
  ↓
gLCDPage.vSave = 1  // 保存结果
```

---

## 关键数据结构

### ARM200A_INPUT（输入：物理量）

```c
typedef struct {
    float root_deg;       // 根部旋转（绕X轴）
    float base_deg;       // 基座旋转（绕Z轴）
    float boom_deg;       // 臂架摆动（绕Y轴）
    float cd_mm;          // 伸缩段1长度
    float d1_deg;         // D1转动
    float e1_deg;         // E1转动
    float f_deg;          // F转动
    float gg1_mm;         // 伸缩段2长度
    float body_roll_deg;  // 车体侧倾
    float body_pitch_deg; // 车体俯仰
} ARM200A_INPUT;
```

### ARM200A_RESULT（输出：12个铰接点坐标）

```c
typedef struct {
    ARM200A_POINT3 o0;   // 根部参考点
    ARM200A_POINT3 o1;   // 旋转中心
    ARM200A_POINT3 o;    // 臂起点
    ARM200A_POINT3 b;    // 基座转动中心
    ARM200A_POINT3 a;    // 臂架转动中心
    ARM200A_POINT3 c;    // 伸缩段起点
    ARM200A_POINT3 d;    // D点
    ARM200A_POINT3 d1;   // D1点
    ARM200A_POINT3 e;    // E点
    ARM200A_POINT3 e1;   // E1点
    ARM200A_POINT3 f;    // F点
    ARM200A_POINT3 g;    // G点
    ARM200A_POINT3 g1;   // G1点
    ARM200A_POINT3 h;    // H点（末端执行器）
    unsigned char ok;    // 计算状态
} ARM200A_RESULT;
```

### ARM200A_FIX（加工误差参数，共22个标量）

```c
typedef struct {
    ARM200A_POINT3 o0o1, o1o, ob, ba, ac;  // 5个向量
    ARM200A_POINT3 d1e, ee1, e1f, fg, g1h, e1e2;  // 6个向量
    float dd1_mm;  // 1个长度
} ARM200A_FIX;  // 共 5×3 + 6×3 + 1 = 22个标量
```

---

## 函数调用关键路径

### 正解计算路径

```
MyLogic_10ms()
  ├─ arm_convert_all_input()  // 原始值→物理量
  ├─ Arm200A_CalcLeft()
  │  └─ arm_calc_from_fix()
  │     └─ arm_calc_one()  // 矩阵变换计算12个点
  ├─ Arm200A_CalcMid()
  │  └─ 同左臂
  └─ Arm200A_CalcRight()
     └─ 同左臂
```

### 标定参数保存路径

```
main() 主循环
  ├─ if(gLCDPage.vSave == 1)
  │  ├─ SaveRetain_()  // 保存常规参数到EEPROM
  │  ├─ Arm200A_StoreSave()  // 保存标定参数
  │  │  ├─ Arm200A_FixToArray()  // FIX结构→数组
  │  │  └─ eeprom_write()  // 写EEPROM
  │  └─ gLCDPage.vSave = 0
```

---

## 文件大小和代码行数

| 模块 | 行数 | 功能密度 |
|------|------|--------|
| Arm200A_Kine.c | 718 | 高（算法核心） |
| App_usr.c | ~500 | 中（业务逻辑） |
| App_lcd.c | ~1500 | 低（页面多） |
| can.c / can2.c | ~300 | 中（CAN驱动） |
| main.c | 193 | 低（框架简） |
| 其他驱动 | ~2000 | 低（硬件适配） |

---

## 性能指标

**主循环周期**：10ms
**正解计算时间**：< 1ms（三臂并行）
**LM优化迭代**：50-200次/次
**内存占用**：~150KB
**EEPROM参数**：~20KB

---

## 与codex.md的关系

MC_LCD - 7Control_V1.2是实验室验证工程，验证：
- ✓ 第1层：零位基准转换（arm_zero_convert_one）
- ✓ 第2层：加工误差标定（arm_fit_one）
- ✓ 正解运动学（arm_calc_one）

不包含：
- ✗ 第3层：四点坐标系变换（QT电脑端实现）
- ✗ 第4层：视觉融合修正（QT电脑端实现）

实际工程（MC_LCD - 7Control_V2.2）会集成前两层成熟的标定流程。
