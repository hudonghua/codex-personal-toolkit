---
name: feedback_bf_variable_pattern
description: BF变量的双重数据源模式：正常运行从CAN读取，调试时手动修改
type: feedback
originSessionId: ac6c94d3-c880-407f-b00d-6ecabb89f1b9
---
# BF变量的双重数据源模式

**规则：** 嵌入式项目中，BF0-BF7这类全局变量有双重数据源

## 正常运行模式
```c
BF0 = gRunInfo.vRcvWL[0];  // 从CAN总线接收的数据
BF1 = gRunInfo.vRcvWL[1];
BF2 = gRunInfo.vRcvWL[2];
...
BF7 = gRunInfo.vRcvWL[7];
```

每次刷新时，先从CAN数据更新到BF变量，然后用BF变量显示。

## 调试模式
在Keil调试器中，可以直接修改BF0-BF7的值来测试显示效果，不需要真的发CAN数据。

例如：
- 用户说"比如我给BF0和BF1的数据，BF6==1 第一个压力值会显示出来吗？"
- 意思是：在调试器里手动设置BF0=0x22, BF1=0x02, BF6=0x01，看屏幕是否显示27.3

## Why: 为什么这样设计？

1. **正常运行**：BF变量从CAN数据自动更新
2. **调试方便**：不需要真的发CAN数据，直接改BF变量就能测试显示
3. **变量名清晰**：BF0-BF7对应CAN数据字节，方便追踪

## How to apply: 如何应用

**错误理解1：** BF变量是从1C8来的，不需要赋值
- ❌ 删除 `BF0 = gRunInfo.vRcvWL[0];`
- ✅ 保留赋值语句，正常运行时需要

**错误理解2：** BF和vRcvWL是不同功能的变量
- ❌ 认为BF是独立的数据源
- ✅ BF是vRcvWL的副本，方便调试

**错误理解3：** 只用BF或只用vRcvWL
- ❌ 直接用vRcvWL显示
- ✅ 先赋值到BF，再用BF显示

## 正确的代码模式

```c
// 每次刷新时
BF0 = gRunInfo.vRcvWL[0];  // 从CAN数据更新
BF1 = gRunInfo.vRcvWL[1];
BF2 = gRunInfo.vRcvWL[2];
BF3 = gRunInfo.vRcvWL[3];
BF4 = gRunInfo.vRcvWL[4];
BF5 = gRunInfo.vRcvWL[5];
BF6 = gRunInfo.vRcvWL[6];
BF7 = gRunInfo.vRcvWL[7];

// 用BF变量显示
if(BF6 == 1) {
    vPress = BF0 + BF1*256;
    LCD_Disp_Intx(...);
}
```

## 用户的原话

"BF跟gRunInfo.vRcvWL 这个是同样功能的。只是数据来源不同。"

"为什么gRunInfo.vRcvWL[7]不读取？一样的要读取啊。我说了很多回了，我只是能修改BF的数据来源，然后便于调试。"

---

**教训：** 不要自作聪明删除赋值语句。BF变量既要从CAN读取（正常运行），又要支持手动修改（调试）。两者都需要！

**最后更新：** 2026-04-30
