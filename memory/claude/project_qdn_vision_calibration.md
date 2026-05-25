---
name: 视觉内参标定方案（Codex版本）
description: 相机内参自动标定的完整工程方案，包括自动筛帧、清晰度判定、样本管理
type: project
originSessionId: bf2ea610-3b35-44d2-aa67-04383ecb3ffd
---
## 核心问题与解决方案

**原问题**：
- 检测到棋盘格就直接存图 → 连续视频里塞满重复图像
- 样本张数多但对标定帮助不大（姿态覆盖差）

**新方案**（2026-04-13）：
```
视频流 → 自动筛帧 → 只保留代表性图像 → 再标定
```

## 自动筛帧核心逻辑

**职责拆分**：
1. `DetectChessboardCornersOnly()` - 只检测角点，不入库
2. `ComputeSharpness()` - 拉普拉斯方差判断清晰度
3. `BuildAutoFrameFeature()` - 提取几何特征（中心、面积、透视倾斜）
4. `IsTooSimilar()` - 判断是否与已有样本重复
5. `TryAutoCollectSample()` - 核心函数，连续调用自动筛选

**筛选阈值**（工程初值，可调）：
- `kMinSharpness = 120.0` - 最低清晰度
- `kMinCenterGap = 0.08` - 中心位置差异阈值（归一化）
- `kMinAreaGap = 0.06` - 面积差异阈值（反映远近变化）
- `kMinTiltGap = 0.10` - 透视倾斜差异阈值（反映姿态变化）
- `kMinImagesForCalibration = 15` - 最低标定张数
- `kTargetImagesForCalibration = 25` - 建议目标张数

## 棋盘格参数

- 纸面图案：12×9 黑白格
- 内角点：11×8
- 每格边长：25 mm
- OpenCV PatternSize：(11, 8)

## 标定流程

1. **采样阶段**：连续读视频帧 → 自动筛帧 → 保留代表性样本
2. **标定阶段**：样本数≥15张时可开始，目标25张
3. **验证阶段**：计算每张图重投影误差 → 剔除最差样本 → 重新标定
4. **保存阶段**：K/D 保存为 YAML，后续直接加载使用

## 关键函数

- `PerformCalibration(image_size)` - 执行K/D标定，返回RMS
- `ComputePerImageReprojectionErrors(errors)` - 计算每张图误差
- `RemoveWorstFramesAndRecalibrate(image_size, remove_count)` - 剔除最差样本再标定
- `SaveCalibrationResults(yaml_path)` - 保存K/D到YAML
- `LoadCalibrationFromFile(yaml_path)` - 加载K/D
- `UndistortImage(source, dest)` - 去畸变验证

## 实现文件

- `camera_calibration_functions_codex.cpp` - 完整C++实现
- 位置：`F:\工作\实际进展与验证\s视觉\codex视觉\`

## 使用场景

- 相机连续出视频流
- 程序连续读帧
- 自动从视频里挑出典型20~30张
- 再做内参标定

这比"手动随便截很多张"更稳，也比"检测到就全部存下来"更符合工程使用场景。

**Why:** 工程现场需要稳定、自动化的标定流程，不能依赖手工操作

**How to apply:** 后续集成到上位机时，直接调用 `TryAutoCollectSample()` 和 `PerformCalibration()` 即可
