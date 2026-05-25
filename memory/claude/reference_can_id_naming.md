---
name: CAN ID命名规则
description: CAN ID与变量名的对应关系，用于理解通信协议
type: reference
originSessionId: d97fb6f0-1fc7-4973-88ab-4a0bd7bccc89
---
## CAN ID命名规则

**格式：** `EA05` 对应 CAN ID `0x2EA.05`

**解析规则：**
- 前缀 `EA` → CAN ID 的后两位 `0x2EA`
- 后缀 `05` → 字节索引或子ID `.05`

**示例：**
```
变量名     CAN ID
EA05   →  0x2EA.05
EA01   →  0x2EA.01
D801   →  0x3D8.01
A101   →  0x3A1.01
```

**推断规则：**
- `ID3A1_01` → CAN ID `0x3A1`, Byte 1
- `ID3D8_05` → CAN ID `0x3D8`, Byte 5
- `EA05` → CAN ID `0x2EA`, Byte/Sub-ID 5

**Why：**
这种命名方式将CAN ID编码到变量名中，便于：
1. 快速识别变量对应的CAN帧
2. 理解数据在CAN报文中的位置
3. 调试时追踪数据流向

**How to apply：**
当看到类似 `XXYY` 格式的变量名时：
- `XX` 部分可能是CAN ID的一部分（通常是后2位）
- `YY` 部分是字节索引或子ID
- 完整CAN ID可能需要加前缀（如 `0x2` 或 `0x3`）

在分析通信协议时，注意这种命名模式，可以快速建立变量与CAN帧的映射关系。
