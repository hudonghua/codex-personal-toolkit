---
name: robot-perception
description: Comprehensive best practices for robot perception systems covering cameras, LiDARs, depth sensors, IMUs, multi-sensor setups, OpenCV, Open3D, PCL, RealSense, ZED, OAK-D, AprilTags, ArUco, stereo vision, RGBD, point clouds, calibration, synchronization, and perception pipeline optimization.
---

# Robot Perception Skill

Use this skill when working with RGB image processing, depth maps, point clouds, sensor calibration, object detection, segmentation, tracking, 3D reconstruction, visual servoing, or perception pipelines in a robotics context.

## When to Use

- Setting up and configuring cameras, LiDAR, depth sensors, IMUs, or multi-sensor rigs.
- Building RGB, depth, RGBD, or point-cloud processing pipelines.
- Calibrating cameras: intrinsic, extrinsic, stereo, camera-LiDAR, and hand-eye.
- Implementing object detection, semantic segmentation, instance segmentation, or tracking.
- Working with OpenCV, Open3D, PCL, RealSense, ZED, OAK-D, AprilTags, ArUco, stereo vision, RGBD, ICP, coordinate transforms, image undistortion, or frame synchronization.
- Debugging perception failures such as latency, dropped frames, misalignment, bad depth, exposure flicker, calibration drift, or unstable detections.

## Core Rules

- Calibrate before deployment. Do not use guessed camera intrinsics.
- Undistort images before geometric computation, pose estimation, stereo matching, or visual servoing.
- Timestamp at capture time, not processing time.
- Decouple capture from processing with bounded queues so slow inference does not block fresh frames.
- Validate depth values before back-projecting pixels into 3D.
- Track detections across frames; avoid acting on a single flickering detection.
- Keep coordinate frames explicit: camera, base, world, tool, and sensor frames must be named and transformed deliberately.
- Benchmark latency by stage: capture, transfer, undistort, detection, depth filtering, point cloud generation, tracking, and pose estimation.

## Sensor Landscape

| Sensor | Output | Typical Use |
| --- | --- | --- |
| RGB camera | H x W x 3 uint8 | Detection, tracking, visual servoing |
| Stereo camera | RGB pair and disparity/depth | Passive depth, outdoor robotics |
| Structured light depth | Depth plus RGB | Indoor manipulation |
| ToF depth | Depth plus RGB | Indoor medium range |
| LiDAR | 3D point cloud | Mapping, navigation, obstacle detection |
| IMU | Acceleration/gyro/orientation | Motion estimation and synchronization |
| Event camera | Async events | High-speed and HDR scenes |

## Calibration Checklist

- Intrinsic calibration RMS should generally be below 0.5 px, preferably below 0.3 px.
- Use at least 20 calibration images with the target covering center, corners, and edges.
- Tilt the calibration board at multiple angles; avoid only fronto-parallel views.
- Keep lens focus and zoom fixed after calibration.
- Verify extrinsic and hand-eye calibration with independent measurements.
- Recalibrate after the camera is bumped, remounted, refocused, rezoomed, or exposed to major temperature changes.

## Streaming Pattern

Use a capture thread and a bounded buffer. Keep the latest frame when inference is slower than camera FPS.

```python
from collections import deque
import threading
import time

class CameraStream:
    def __init__(self, camera, maxlen=2):
        self.camera = camera
        self.frames = deque(maxlen=maxlen)
        self.lock = threading.Lock()
        self.running = False

    def start(self):
        self.running = True
        threading.Thread(target=self._loop, daemon=True).start()

    def stop(self):
        self.running = False

    def _loop(self):
        while self.running:
            frame = self.camera.capture()
            stamp = time.monotonic()
            with self.lock:
                self.frames.append((stamp, frame))

    def latest(self):
        with self.lock:
            return self.frames[-1] if self.frames else None
```

## Depth Back-Projection

Always validate depth and use calibrated intrinsics.

```python
def backproject(u, v, depth, K, depth_scale=0.001, max_depth=5.0):
    z = float(depth[v, u]) * depth_scale
    if z <= 0.0 or z > max_depth:
        return None
    fx, fy = K[0, 0], K[1, 1]
    cx, cy = K[0, 2], K[1, 2]
    x = (u - cx) * z / fx
    y = (v - cy) * z / fy
    return x, y, z
```

## Latency Budget

For 10 Hz perception, total perception latency should stay below about 100 ms. For 30 Hz, target about 33 ms. If perception is slower than control, pipeline it: process frame N while control uses the latest stable result from frame N-1 or earlier.

Typical stages to measure:

- Sensor capture and transfer
- Undistortion or rectification
- Detection or segmentation inference
- Depth filtering
- Point cloud generation
- Tracking update
- 3D pose estimation
- ROS2 publish/subscribe or IPC overhead

## Anti-Patterns

- Processing every camera frame with a heavy model when decimation is enough.
- Using unbounded queues between capture and inference.
- Sampling one depth pixel instead of a small valid neighborhood.
- Treating raw sensor timestamps and processing timestamps as interchangeable.
- Assuming factory intrinsics are good enough for precision work.
- Mixing camera-frame and robot-base-frame coordinates without explicit transforms.
- Letting auto-exposure flicker during detection without testing stability.

## Production Checklist

1. Calibrate intrinsics, extrinsics, and hand-eye transforms.
2. Validate calibration with independent measurements.
3. Lock or manage exposure to avoid detection flicker.
4. Undistort before geometric computation.
5. Filter depth and remove invalid/flying pixels.
6. Timestamp at capture.
7. Track objects across frames.
8. Handle missing frames, overexposure, bad depth, and sensor disconnects.
9. Log perception output: boxes, masks, confidence, 3D positions, frame IDs, and timestamps.
10. Benchmark latency per pipeline stage.
11. Test edge cases: empty scenes, clutter, reflective objects, sunlight, motion blur, and partial occlusion.
12. Pin model versions and calibration files in deployment.

## Source Note

This local fallback is based on the publicly indexed `robot-perception` agent skill listing for `majiayu000/claude-skill-registry`. The original GitHub path returned 404 during installation, so this preserves the usable guidance locally.
