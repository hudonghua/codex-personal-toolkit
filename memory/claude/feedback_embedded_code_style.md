---
name: 嵌入式代码风格原则
description: 硬件项目代码要简单直接，避免指针/数组/枚举/结构体，方便全局搜索追踪变量生命周期
type: feedback
originSessionId: 8edb54a1-4546-41ef-a490-005cf2bf1242
---
## 规则

编写嵌入式硬件项目代码时：

### 默认原则
- **少用** enum、复杂数组索引、指针转发、结构体层层取成员
- **优先用** 可直接全局搜索的明确变量名和朴素 if/else 逻辑
- **重点不是做抽象**，而是把硬件场景、联动条件、控制动作写得直白、好追踪

### 边界处理
如果底层库、协议缓冲区、现有工程本来就必须用数组、指针、结构体：
- 把复杂性尽量留在**边界层**
- **不往应用层继续扩散**
- 在边界层做一次转换，转成简单变量供应用层使用

## Why（原因）

用户在2026-04-26给出了非常重要的工程经验：

> "我希望你尽量不要用到枚举，数组，指针等。为什么呢？打个比方：keil有全局搜索，我搜一个变量我就能知道这个变量的整个生命周期，如果是指针或者数组，或者结构体，我还得要看这个变量来源于那个变量里面的元素，程序观看体验不好。"

> "我们这种结合硬件的程序，不像大型软件，追求内存，追求要用多么高深的语言结构，我们就简单的逻辑判断。真正核心的不是程序实现，是结合硬件的场合经验，用最朴素的语言进行体现和控制。"

### 核心观点

1. **可追踪性比代码优雅更重要**
   - Keil全局搜索一个变量，能立刻看到整个生命周期
   - 如果用指针/数组/结构体，还要追踪来源，很麻烦
   - 调试硬件问题时，需要快速定位变量

2. **嵌入式项目不是大型软件**
   - 不需要追求内存优化
   - 不需要追求高深的语言结构
   - 不需要复杂的数据结构

3. **核心是硬件场合经验**
   - 真正重要的是：如何控制硬件
   - 真正重要的是：现场经验和调试技巧
   - 程序只是工具，要用最朴素的语言体现

4. **简单的逻辑判断就够了**
   - if/else判断
   - 简单的变量赋值
   - 直接的逻辑控制

### 实际问题举例

**不好的写法（难以追踪）：**
```c
// 使用结构体
typedef struct {
    uint8_t valve_status;
    uint8_t pump_status;
} SystemStatus;

SystemStatus sys;
sys.valve_status = 1;  // 搜索valve_status，还要看sys是什么

// 使用数组
uint8_t status[10];
status[3] = 1;  // 搜索status，不知道status[3]是什么含义

// 使用指针
uint8_t *p_valve = &valve_status;
*p_valve = 1;  // 搜索p_valve，还要追踪指向哪里
```

**好的写法（容易追踪）：**
```c
// 直接使用变量
uint8_t valve_status;
uint8_t pump_status;
uint8_t lubrication_status;
uint8_t sensitization_status;

valve_status = 1;  // Keil搜索valve_status，立刻看到所有使用的地方
pump_status = 0;
```

### 为什么这样更好

1. **调试方便**
   - 在Keil中搜索`valve_status`，立刻看到所有读写的地方
   - 不需要追踪结构体、数组索引、指针指向
   - 变量的整个生命周期一目了然

2. **现场修改方便**
   - 硬件工程师在现场调试时，能快速理解代码
   - 不需要理解复杂的数据结构
   - 改一个变量，立刻知道影响范围

3. **代码可读性强**
   - 变量名直接表达含义
   - 逻辑清晰，一眼看懂
   - 不需要查看结构体定义

4. **符合硬件项目特点**
   - 变量数量不多（几十个到几百个）
   - 不需要复杂的数据管理
   - 重点是控制逻辑，不是数据结构

## How to apply（如何应用）

### 编写嵌入式代码时

1. **优先使用简单变量**
   ```c
   // ✓ 好
   uint8_t valve_open;
   uint8_t valve_close;
   uint8_t pump_run;
   uint8_t pump_stop;
   
   // ✗ 避免
   typedef struct {
       uint8_t valve_open;
       uint8_t valve_close;
   } ValveStatus;
   ```

2. **避免数组（除非真的需要）**
   ```c
   // ✓ 好 - 每个变量独立，容易搜索
   uint8_t sensor1_value;
   uint8_t sensor2_value;
   uint8_t sensor3_value;
   
   // ✗ 避免 - 搜索sensor_value[1]很麻烦
   uint8_t sensor_value[3];
   ```

3. **避免指针（除非必须）**
   ```c
   // ✓ 好 - 直接操作变量
   valve_status = 1;
   
   // ✗ 避免 - 增加追踪难度
   uint8_t *p = &valve_status;
   *p = 1;
   ```

4. **避免枚举（用宏定义代替）**
   ```c
   // ✓ 好 - 直接搜索VALVE_OPEN能看到所有使用
   #define VALVE_OPEN  1
   #define VALVE_CLOSE 0
   
   valve_status = VALVE_OPEN;
   
   // ✗ 避免 - 枚举类型增加理解成本
   typedef enum {
       VALVE_OPEN = 1,
       VALVE_CLOSE = 0
   } ValveState;
   ```

5. **使用简单的if/else逻辑**
   ```c
   // ✓ 好 - 逻辑清晰
   if(valve_open_signal == 1)
   {
       valve_status = 1;
       pump_status = 1;
   }
   else
   {
       valve_status = 0;
       pump_status = 0;
   }
   
   // ✗ 避免 - 过度封装
   void SetSystemStatus(SystemStatus *sys, uint8_t signal)
   {
       sys->valve = signal;
       sys->pump = signal;
   }
   ```

### 边界层处理示例

**场景：CAN协议必须用结构体**

```c
// ========== 边界层（复杂性留在这里）==========
typedef struct {
    uint8_t data[8];
    uint16_t id;
} CAN_Message;

CAN_Message can_rx_msg;  // 接收缓冲区

// 边界层函数：把复杂结构转成简单变量
void CAN_Parse_To_SimpleVars(void)
{
    // 从结构体中提取，转成简单变量
    valve_open_signal = can_rx_msg.data[0];
    pump_run_signal = can_rx_msg.data[1];
    pressure_value = can_rx_msg.data[2];
}

// ========== 应用层（只用简单变量）==========
uint8_t valve_open_signal;
uint8_t pump_run_signal;
uint8_t pressure_value;

// 应用层逻辑：直白、好追踪
void Application_Logic(void)
{
    if(valve_open_signal == 1)
    {
        valve_status = 1;
        LCD_Display_Valve_Open();
    }
    else
    {
        valve_status = 0;
        LCD_Display_Valve_Close();
    }
    
    if(pump_run_signal == 1)
    {
        pump_status = 1;
        Motor_Start();
    }
}
```

**关键点：**
- 复杂性（结构体、数组）留在边界层
- 应用层只看到简单变量
- Keil搜索`valve_open_signal`能看到整个生命周期
- 硬件场景、联动条件写得直白

### 什么时候可以用复杂结构

只有在以下情况才考虑：
1. **协议数据包**：CAN/串口数据包，必须打包发送（留在边界层）
2. **底层库要求**：HAL库等必须使用结构体（留在边界层）
3. **现有工程已有**：不要强行改造，但不往应用层扩散

**原则：复杂性留在边界，不往应用层扩散**

### 代码审查标准

审查嵌入式代码时，问自己：
1. 能否用Keil全局搜索快速追踪变量？
2. 现场工程师能否快速理解？
3. 是否用了最朴素的语言？
4. 是否过度设计了？

### 核心理念

> "真正核心的不是程序实现，是结合硬件的场合经验，用最朴素的语言进行体现和控制。"

**三个重点：**
1. **不是做抽象** - 不追求代码优雅、不过度设计
2. **写得直白** - 硬件场景、联动条件、控制动作一目了然
3. **好追踪** - Keil全局搜索能看到变量完整生命周期

**边界原则：**
- 复杂性留在边界层（协议解析、底层库接口）
- 不往应用层扩散
- 应用层只用简单变量和朴素逻辑

- 程序是工具，不是目的
- 简单直接，胜过优雅复杂
- 可追踪性，胜过代码美观
- 现场经验，胜过编程技巧

## 适用场景

- 嵌入式硬件项目（单片机、PLC等）
- 现场调试频繁的项目
- 需要多人协作维护的项目
- 硬件工程师也要看代码的项目

## 不适用场景

- 大型软件系统
- 需要复杂数据管理的项目
- 内存严重受限的项目
- 纯软件算法实现

## 相关记忆

- [feedback_hardware_project_rigor.md](feedback_hardware_project_rigor.md) — 硬件项目严谨性
- [project_200a_screen_engineering.md](project_200a_screen_engineering.md) — 200A屏项目工程经验

## 用户原话

> "我希望你尽量不要用到枚举，数组，指针等。为什么呢？打个比方：keil有全局搜索，我搜一个变量我就能知道这个变量的整个生命周期，如果是指针或者数组，或者结构体，我还得要看这个变量来源于那个变量里面的元素，程序观看体验不好。"

> "我们这种结合硬件的程序，不像大型软件，追求内存，追求要用多么高深的语言结构，我们就简单的逻辑判断。真正核心的不是程序实现，是结合硬件的场合经验，用最朴素的语言进行体现和控制。"

这是非常宝贵的工程经验。
