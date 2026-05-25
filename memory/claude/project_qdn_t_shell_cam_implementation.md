---
name: T_shell_cam标定实现
description: T_shell_cam标定的完整C++实现、文档和使用方法
type: project
originSessionId: current
---

# T_shell_cam 标定实现

**日期**: 2026-05-06  
**状态**: 已完成K/D标定，T_shell_cam实现完成，待现场标定

---

## 一、项目进展

### 已完成
1. ✅ K/D内参标定（用户已完成）
2. ✅ T_shell_cam标定流程文档（HTML）
3. ✅ T_shell_cam C++函数库（完整实现）
4. ✅ 使用示例代码

### 下一步
1. ⏳ 准备硬件（外壳基准点、全站仪、四点光源）
2. ⏳ 执行出厂标定
3. ⏳ 验证标定精度
4. ⏳ 现场测试

---

## 二、标定策略

### 用户选择：出厂固化流程

**原因**：
- 标定一次T_shell_cam，以后每次现场只需要测外壳3个点
- 节省现场时间 50%（从30分钟 → 16分钟）
- 不需要每次携带四点光源靶标
- 操作简单，不易出错

**流程**：
```
出厂标定（一次）：
1. K/D内参标定 ✅ 已完成
2. T_shell_cam标定 ← 当前阶段
3. 保存到YAML文件

现场使用（每次）：
1. 全站仪测外壳3个点
2. 加载T_shell_cam.yaml
3. 自动计算相机位姿
```

---

## 三、已实现的功能

### 1. 标定流程文档

**文件**: `T_shell_cam标定流程.html`  
**位置**: `F:\工作\实际进展与验证\s视觉\Claude视觉\T_shell_cam\`

**内容**：
- 标定目标和意义
- 准备工作检查清单
- 7个详细步骤（含代码）
- 验证方法
- 后续使用说明
- 常见问题解答
- 完整Python标定脚本

---

### 2. C++函数库

**文件**: 
- `T_shell_cam_functions.h` - 头文件
- `T_shell_cam_functions.cpp` - 实现文件

**位置**: `F:\工作\实际进展与验证\s视觉\Claude视觉\T_shell_cam\`

**包含5个核心函数**：

#### (1) ComputeT_world_shell()
```cpp
bool ComputeT_world_shell(
    const cv::Point3d& Q1_world,
    const cv::Point3d& Q2_world,
    const cv::Point3d& Q3_world,
    cv::Mat& R_world_shell,
    cv::Mat& t_world_shell);
```
- 功能：根据外壳3个基准点计算外壳位姿
- 输入：Q1、Q2、Q3的世界坐标（全站仪测量）
- 输出：T_world_shell（R和t）

#### (2) LoadT_shell_cam()
```cpp
bool LoadT_shell_cam(
    const std::string& yaml_path,
    cv::Mat& R_shell_cam,
    cv::Mat& t_shell_cam);
```
- 功能：从YAML文件加载出厂标定结果
- 输入：YAML文件路径
- 输出：T_shell_cam（R和t）

#### (3) ComputeT_world_cam()
```cpp
void ComputeT_world_cam(
    const cv::Mat& R_world_shell,
    const cv::Mat& t_world_shell,
    const cv::Mat& R_shell_cam,
    const cv::Mat& t_shell_cam,
    cv::Mat& R_world_cam,
    cv::Mat& t_world_cam);
```
- 功能：计算相机在世界坐标系的位姿
- 输入：T_world_shell 和 T_shell_cam
- 输出：T_world_cam（R和t）
- 公式：T_world_cam = T_world_shell × T_shell_cam

#### (4) QuickLocateCamera() - 推荐使用
```cpp
bool QuickLocateCamera(
    const cv::Point3d& Q1_world,
    const cv::Point3d& Q2_world,
    const cv::Point3d& Q3_world,
    const std::string& yaml_path,
    cv::Mat& R_world_cam,
    cv::Mat& t_world_cam);
```
- 功能：一步到位，现场快速定位相机位姿
- 输入：外壳3个点 + YAML文件路径
- 输出：相机位姿
- 封装了完整流程

#### (5) PrintTransform()
```cpp
void PrintTransform(
    const cv::Mat& R,
    const cv::Mat& t,
    const std::string& name);
```
- 功能：打印变换矩阵（调试用）

---

### 3. 使用示例

**文件**: `T_shell_cam_usage_example.cpp`  
**位置**: `F:\工作\实际进展与验证\s视觉\Claude视觉\T_shell_cam\`

**包含4个示例**：
1. 示例1：一步到位（推荐）
2. 示例2：分步调用（理解流程）
3. 示例3：从文件读取坐标
4. 示例4：手动输入坐标（交互式）

---

## 四、使用方法

### 最简单的调用方式

```cpp
#include "T_shell_cam_functions.h"

// 全站仪测量的外壳3个点（单位：mm）
cv::Point3d Q1(1000.0, 2000.0, 1500.0);
cv::Point3d Q2(1200.0, 2000.0, 1500.0);
cv::Point3d Q3(1100.0, 2100.0, 1600.0);

// 一步得到相机位姿
cv::Mat R_world_cam, t_world_cam;
bool success = QuickLocateCamera(
    Q1, Q2, Q3,
    "T_shell_cam.yaml",
    R_world_cam, t_world_cam
);

if (success) {
    // 使用 R_world_cam 和 t_world_cam
    PrintTransform(R_world_cam, t_world_cam, "相机位姿");
}
```

---

## 五、标定流程（7步）

### 步骤1：在外壳上定义3个固定基准点
- 不共线，建议形成三角形
- 刚性固定，不能移动
- 容易被全站仪测量

### 步骤2：用全站仪测量外壳3个基准点
- 得到Q1、Q2、Q3的世界坐标
- 计算T_world_shell

### 步骤3：用全站仪测量四点光源
- 得到P1、P2、P3、P4的世界坐标

### 步骤4：相机拍摄四点光源
- 图像处理识别光点中心
- 得到p1、p2、p3、p4的图像坐标

### 步骤5：用PnP求解T_world_cam
- 输入：K、D、四点世界坐标、四点图像坐标
- 输出：T_world_cam

### 步骤6：反算T_shell_cam
- 公式：T_shell_cam = (T_world_shell)^(-1) × T_world_cam

### 步骤7：保存T_shell_cam
- 保存为YAML文件
- 作为出厂固定参数

---

## 六、验证标准

### 标定精度
- 位置误差 < 5mm
- 角度误差 < 0.5°

### 验证方法
1. 换一个位置，重新测量外壳3个点
2. 用T_shell_cam计算相机位姿
3. 用PnP独立求解相机位姿
4. 对比两个结果

---

## 七、硬件准备清单

### 必需硬件
- [x] 相机（K/D已标定）
- [ ] 相机外壳/支架（带3个固定基准点）
- [ ] 全站仪
- [ ] 四点光源刚体靶标
- [x] 电脑（OpenCV环境）

### 外壳基准点要求
- 3个反光贴片或小棱镜
- 刚性固定在外壳上
- 形成三角形布局

---

## 八、关键理解

### T_shell_cam的物理意义
- 相机相对外壳的固定关系
- 出厂标定一次，永久有效
- 只要外壳不变形，相机不松动，就一直准确

### 为什么要标定T_shell_cam？
- 现场快速部署：只需测3个点，不需要拍靶标
- 节省时间：30分钟 → 16分钟
- 操作简单：减少出错概率
- 适合产品化：批量使用

### 与T_world_cam的区别
- T_world_cam：相机在世界坐标系的位姿（每次现场都不同）
- T_shell_cam：相机相对外壳的固定关系（出厂标定，固定不变）
- 关系：T_world_cam = T_world_shell × T_shell_cam

---

## 九、注意事项

### 标定时
- K/D必须先完成
- 相机焦距、对焦不能改变
- 外壳基准点必须刚性固定
- 四点光源要在相机视野内

### 使用时
- 外壳基准点不能移动或变形
- 相机与外壳的固定关系不能改变
- 如果更换相机或调整安装，必须重新标定

---

## 十、文件位置总结

```
F:\工作\实际进展与验证\s视觉\Claude视觉\T_shell_cam\
├── T_shell_cam_functions.h              - 头文件
├── T_shell_cam_functions.cpp            - 实现文件
├── T_shell_cam_usage_example.cpp        - 使用示例
└── T_shell_cam标定流程.html             - 标定流程文档
```

---

## Why: 为什么保存这个记忆

这是全电脑模型视觉系统的核心功能之一，T_shell_cam标定是实现现场快速部署的关键。用户已经完成K/D标定，正在推进T_shell_cam标定，需要记录完整的实现方案和使用方法。

## How to apply: 如何使用

- 当用户提到"T_shell_cam"、"相机外壳标定"、"现场快速定位"时，调用这个记忆
- 帮助用户完成出厂标定流程
- 指导用户使用C++函数库
- 解答标定和使用过程中的问题
- 跟踪标定进展和验证结果
