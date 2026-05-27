---
name: qdn-project-vision
description: >-
  QDN / 全电脑模型工业视觉项目上下文。涉及四点 LED 光斑、solvePnP 位姿、K/D 内参、
  T_shell_cam 标定、QT+OpenCV C++ 工控端视觉时必读。与通用 opencv skill 配合使用。
---

# QDN 项目视觉（全电脑模型）

在编写或修改 **本用户** 的工业视觉、标定、定位代码前，先加载项目记忆，再套用 `opencv` / `terminalskills-opencv` 的通用写法。

## 何时启用

- 四点光源 + `solvePnP` 位姿解算
- 相机内参 K/D、外壳标定 `T_shell_cam`
- QT + OpenCV 工控机实时视觉（约 12–16 Hz）
- 炮孔图、现场标定、倾角补偿相关视觉链路

## 必读记忆（按任务选读）

根目录：`C:\Users\DELL\.claude\projects\C--Users-DELL\memory\`

| 文件 | 内容 |
|------|------|
| `project_qdn_vision_architecture.md` | 硬件、算法流程、延迟与实时性 |
| `project_qdn_vision_decisions.md` | 定位、靶标、相机、标定链决策 |
| `project_qdn_vision_calibration.md` | 内参标定：筛帧、清晰度、样本管理 |
| `project_qdn_t_shell_cam_implementation.md` | T_shell_cam C++/Python 实现与现场流程 |
| `project_qdn_calibration_strategy.md` | 两层标定、自学习修正、倾角补偿 |
| `project_qdn_work_scenario.md` | 自动定位钻孔、炮孔图、操作界面 |

整合导航：`C:\Users\DELL\.cursor\memory\CURSOR_AGENT_MEMORY.md` §5

## 技术栈约束（勿偏离除非用户明确要求）

- **语言**：工控端以 **C++ OpenCV** 为主；标定脚本可用 **Python OpenCV**
- **算法**：传统 CV（阈值、轮廓、质心/光斑）+ **solvePnP**；非默认上 YOLO/深度相机
- **颜色**：OpenCV 默认 **BGR**；显示或导出时注意与 RGB 工具链转换
- **实时**：视觉周期约 60–80 ms；慢速微调与钻孔静止工况可接受，快速移动需外推或融合策略
- **编码**：若改嵌入式 C/H（GBK），改走 `embedded-c-safe-edit` / `safe-edit-gbk`，勿用本 skill 代替

## 推荐工作流

1. Read 上表 1–2 个最相关的 `project_*` 文件
2. 确认当前任务是 **检测 / 标定 / 位姿 / 坐标变换** 中的哪一段
3. 用 `opencv`（基础 API）或 `terminalskills-opencv`（管线、轮廓、视频、DNN 示例）
4. 改 C++ 或 Python 后给出可复现的验证步骤（样本图、标定板、重投影误差等）

## 常见任务速查

### 四点光斑检测

- 灰度 → 高斯/中值滤波 → 阈值（固定或自适应）→ 轮廓 → 按面积/圆度筛选 → 质心
- 注意曝光与反光；与 `project_qdn_vision_architecture` 中四点刚体假设一致

### solvePnP

- 图像点与物方四点坐标一一对应；使用已标定的 K/D
- 输出位姿用于后续坐标变换链（见架构文档）

### 标定

- K/D：见 `project_qdn_vision_calibration.md`
- T_shell_cam：见 `project_qdn_t_shell_cam_implementation.md`（出厂一次，现场测外壳点）

## 不要默认做的事

- 不要引入 RealSense / 点云 / Robot Perception 栈，除非用户明确换硬件
- 不要把训练式检测（YOLO 等）当作默认方案替代四点光斑
- 不要凭函数名猜定时周期；须追到调用链或定时器配置
