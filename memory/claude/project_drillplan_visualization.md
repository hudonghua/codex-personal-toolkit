---
name: 炮孔图生成工作记录
description: 从XML和Excel数据生成多视角炮孔图的完整流程和坐标系定义
type: project
originSessionId: 30f472f9-211f-4b91-8d73-bb2efa60d3ec
---
# 炮孔图生成工作记录

## 项目背景
为全电脑钻孔系统生成炮孔图，用于HMI显示屏和电脑端分析。

## 坐标系定义

### 标准坐标系（用户定义）
- **X+**：隧道方向（向前）
- **Y+**：操作手左侧
- **Z+**：向上

### 数据源坐标系

**XML文件（ToJumbo格式）**
- X：左右方向
- Y：隧道方向（向前）
- Z：上下方向

**Excel文件（243孔坐标计算结果）**
- X：隧道方向（向前）← 与用户定义一致
- Y：左右方向
- Z：向上
- 包含字段：HoleId, Xstart, Ystart, Zstart, Xend, Yend, Zend, Type, Length, Dip_deg, Azimuth_deg

## 生成的炮孔图类型

### 1. 静态PNG图（用于HMI显示屏）

**从XML生成（6个隧道段）**
- 文件位置：`F:\工作\AI_Claude_模型\生成炮孔图\CRCHI\`
- 分辨率：1024×768
- 视角：Y-Z平面（操作手视角）
- 文件：
  - bisizddlc_drill_plan.png（第一隧道段-下层）
  - bisizdxlc_drill_plan.png（第一隧道段-上层）
  - biwuzddlc_drill_plan.png（第二隧道段-下层）
  - biwuzdxlc_drill_plan.png（第二隧道段-上层）
  - bisanzddlc_drill_plan.png（第三隧道段-下层）
  - bisanzdxlc_drill_plan.png（第三隧道段-上层）

**从Excel生成（243孔）**
- 文件位置：`F:\工作\AI_Claude_模型\生成炮孔图\`
- 多视角：
  - `243孔_驾驶室操作手视角.png` - Y-Z平面（主要使用）
  - `243孔_YZ平面_操作手视角.png` - Y-Z平面
  - `243孔_XZ平面_侧视图.png` - X-Z平面
  - `243孔_XY平面_俯视图.png` - X-Y平面（俯视）

### 2. 交互式3D HTML（用于电脑端）
- 文件：`F:\工作\AI_Claude_模型\生成炮孔图\drillplans_3d_new.html`
- 技术：Plotly 3D可视化
- 功能：
  - 下拉菜单切换6个隧道段
  - 复选框显示/隐藏起点终点
  - 可旋转、缩放、平移
  - 专业设计风格（深色主题）

## 孔类型颜色映射

| 孔类型 | 颜色 | RGB | 说明 |
|--------|------|-----|------|
| 轮廓孔 (Contour) | 黄色 | (255, 200, 0) | 隧道轮廓 |
| 掏槽孔 (CutHole) | 红色 | (255, 100, 100) | 中心爆破 |
| 底部孔 (Bottom) | 青色 | (0, 200, 255) | 底部 |
| 二次轮廓孔 (SecondContour) | 橙色 | (255, 150, 0) | 光爆 |
| 卸压孔 (EasersHole) | 绿色 | (150, 255, 150) | 减压 |
| 参考孔 (Reference) | 紫色 | (200, 100, 255) | 定位参考 |
| 辅助孔 | 青色 | (0, 200, 255) | 辅助 |

## 生成脚本

### 1. generate_drill_plan_hmi.py
- 功能：从XML生成PNG静态图
- 输入：XML钻孔计划文件
- 输出：1024×768 PNG图片
- 特点：深灰背景，显示钻孔方向箭头

### 2. generate_3d_drillplan.py
- 功能：从XML生成3D HTML
- 输入：XML钻孔计划文件
- 输出：交互式HTML文件
- 特点：基于参考文件结构，只替换PLANS数据

### 3. generate_excel_drillplan.py
- 功能：从Excel生成单视角PNG
- 输入：Excel坐标文件
- 输出：Y-Z平面PNG图片

### 4. generate_all_views.py
- 功能：从Excel生成三视角PNG
- 输入：Excel坐标文件
- 输出：Y-Z、X-Z、X-Y三个平面的PNG图片

### 5. generate_driver_view.py
- 功能：从Excel生成驾驶室操作手视角
- 输入：Excel坐标文件
- 输出：Y-Z平面PNG（Y+在左侧）
- 特点：添加方向标注（左Y+、右Y-、上Z+）

## 关键技术点

### 坐标转换
- XML的Y轴 → 用户的X轴（隧道方向）
- XML的X轴 → 用户的Y轴（左右方向）
- Excel坐标系与用户定义一致，无需转换

### 视角映射
**驾驶室操作手视角（朝前看掌子面）**
- 显示平面：Y-Z
- Y+在左侧（图的左边）
- Z+在上方
- 坐标转换：`px = 显示宽 - y_偏移 - (y - y_min) * 缩放因子`

### 显示特性
- 孔半径：`max(6, int(缩放因子 * 0.048))`
- 方向箭头：显示钻孔方向
- 等比例缩放：保持X、Y、Z的真实比例
- 深灰背景：(40, 40, 40)
- 白色边框和文字

## 数据统计（243孔示例）

| 项目 | 数值 |
|------|------|
| 总孔数 | 243个 |
| 掏槽孔 | 131个 |
| 二次轮廓孔 | 64个 |
| 辅助孔 | 48个 |
| Y范围 | -2.78 ~ 2.87 m |
| Z范围 | 0.00 ~ 4.14 m |
| X范围 | -4.89 ~ 7.30 m |

## 使用场景

1. **HMI显示屏**：使用PNG静态图，1024×768分辨率
2. **电脑端分析**：使用3D HTML，可交互查看
3. **现场操作**：驾驶室操作手视角，清晰显示钻孔位置和方向
4. **技术分析**：多视角PNG，从不同角度分析钻孔布置

## 文件路径

**数据源**
- XML：`C:\Users\DELL\xwechat_files\...\ToJumbo\prod\`
- Excel：`F:\工作\实际进展与验证\模型确定\Claude_记录\243 孔坐标计算结果 (1).xlsx`

**输出**
- PNG图片：`F:\工作\AI_Claude_模型\生成炮孔图\`
- 3D HTML：`F:\工作\AI_Claude_模型\生成炮孔图\drillplans_3d_new.html`

**脚本**
- `C:\Users\DELL\generate_*.py`

## Why（为什么这样做）

1. **多视角**：不同场景需要不同视角（操作手、技术分析、俯视）
2. **颜色区分**：快速识别不同类型的孔
3. **方向箭头**：清晰显示钻孔方向和角度
4. **等比例**：保持真实空间关系
5. **深灰背景**：工程图纸专业风格，对比度高

## How to apply（如何应用）

- 需要生成新的炮孔图时，运行对应的Python脚本
- HMI显示屏使用PNG图片
- 电脑端分析使用3D HTML
- 坐标系转换时注意XML和Excel的差异
- 操作手视角必须确保Y+在左侧
