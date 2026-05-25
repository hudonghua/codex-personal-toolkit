---
name: McgsPro项目分析参考
description: 华矿中型/小型机MCP文件分析的完整案例，包含提取方法和结果
type: reference
originSessionId: d97fb6f0-1fc7-4973-88ab-4a0bd7bccc89
---
## 项目信息
- **项目名称：** 华矿中型/小型机装药车HMI系统
- **文件：** 10寸中型小型机.MCP (9.74 MB)
- **平台：** McgsPro组态软件（昆仑通态）
- **文件位置：** D:\小谭交接\work\华矿中\

## 成功提取的数据

### 1. CAN通信协议（完整）
```
Write(0x3A1, 0, 0, 8, ID3A1_01~08) - DO数字输出控制
Write(0x3D8, 0, 0, 8, ID3D8_01~08) - PWM输出控制1
Write(0x3D9, 0, 0, 8, ID3D9_01~08) - PWM输出控制2
Write(0x3D1, 0, 0, 8, ID3D1_01~08) - PWM输出控制3
Write(0x3FF, 0, 0, 8, 0,0,0,0...) - 心跳/保留
```

### 2. 变量清单（28个）
ID3A1_01~08, ID3D8_01~08, ID3D9_01~08, ID3D1_01~08

### 3. 脚本逻辑统计
- 自增操作(++): 182次 → 计数器、状态机
- 自减操作(--): 194次 → 定时器、倒计时
- 加赋值(+=): 24次 → 数据累加
- 减赋值(-=): 2次
- 除赋值(/=): 8次 → 平均值计算
- Loop循环: 3次 → 主循环、子循环、定时循环

### 4. 数据库对象
- CStrategyBlock（策略块）
- CStgyConnIF（策略连接接口）
- CDevDealMethodGCZH（设备处理方法）

## 提取方法（Python脚本）

```python
import re

with open('10寸中型小型机.MCP', 'rb') as f:
    content = f.read()
    
    # 提取CAN函数
    can_writes = re.findall(rb'Write\(0x[0-9A-F]+[^)]+\)', content)
    
    # 提取变量
    id_vars = re.findall(rb'ID3[A-F0-9]{2}_\d{2}', content)
    
    # 统计操作符
    count_inc = content.count(b'++')
    count_dec = content.count(b'--')
```

## 系统架构
- **角色：** 上位机HMI（人机界面）
- **通信：** CAN总线
- **协议：** 与华旷二代纯液压/半液压完全相同
- **兼容性：** 可配套使用

## 文档位置
D:\小谭交接\work\华矿中\程序交接文档.html

包含完整的CAN协议、脚本逻辑分析、典型场景示例。
