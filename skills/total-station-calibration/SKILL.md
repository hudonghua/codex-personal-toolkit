---
name: total-station-calibration
description: Total station calibration and error-analysis workflow. Use for total-station instrument checks, collimation error, trunnion-axis error, vertical-index error, EDM additive/multiplicative constants, prism constant, atmospheric correction, two-face observations, resection checks, calibration records, and Excel-based calibration calculations.
---

# Total Station Calibration

Use this skill for total-station calibration, verification, and error diagnosis. It focuses on instrument behavior and measurement corrections, not only general survey data processing.

Trigger on Chinese or English requests involving 全站仪, 标定, 校准, 检校, 盘左盘右, 视准轴误差, 横轴误差, 竖盘指标差, 棱镜常数, 测距常数, 测角误差, EDM, prism constant, collimation error, trunnion-axis error, vertical-index error, or total-station calibration reports.

## First Questions

Ask or infer these before calculating:

- Instrument model and nominal accuracy, for example 1", 2", 5", 1 mm + 1 ppm.
- Observation unit: DMS, decimal degree, gon/grad, meter, millimeter.
- Whether vertical reading is elevation angle, vertical angle, or zenith angle.
- Whether observations include face-left and face-right values.
- Prism type and prism constant.
- EDM mode: prism, reflectorless, mini prism, 360 prism.
- Atmospheric data: temperature, pressure, humidity, or instrument PPM setting.
- Baseline/control-point coordinates and certified distances if available.
- Required standard or tolerance.

## Protect Raw Data

- Preserve raw observations exactly.
- Keep original DMS strings before converting.
- Record every assumption: angle convention, distance type, prism constant, atmospheric correction, and unit.
- If working in Excel, keep a raw-data sheet and put formulas in separate calculation sheets.

## Main Error Sources

### 视准轴误差 / Collimation Error

The line of sight is not perpendicular to the horizontal axis. It mainly affects horizontal angle observations and is reduced by observing in both faces.

Checks:

- Observe the same target in face left and face right.
- Compare transformed readings after applying the 180 degree relationship.
- Use multiple targets at different directions when possible.

Rule:

- Two-face mean reduces collimation-related effects.
- Large face-left/face-right disagreement indicates instrument check or setup problem.

### 横轴误差 / Trunnion-Axis Error

The horizontal axis is not perpendicular to the vertical axis. It is more visible at steep vertical angles.

Checks:

- Use targets with high and low vertical angles.
- Compare face-left and face-right observations.
- Avoid judging this error from near-horizontal targets only.

### 竖盘指标差 / Vertical-Index Error

The vertical circle zero is biased.

Checks:

- Observe the same target in both faces.
- For zenith-angle instruments, face-left and face-right zenith observations should satisfy the expected paired relationship.
- Compute index error only after confirming whether readings are zenith angles or elevation angles.

Common zenith-angle check:

```text
Z_L + Z_R should be close to 360 degrees
index_error ~= (Z_L + Z_R - 360 degrees) / 2
```

Do not use this formula blindly if the instrument exports elevation angle instead of zenith angle.

### EDM Additive Constant

The distance measurement has a fixed offset, often related to EDM/prism combination.

Use a calibrated baseline with multiple known distances. Compare measured slope/horizontal distances against certified distances after confirming prism constant and atmospheric correction.

### EDM Multiplicative Constant / Scale

Distance error proportional to length, often expressed in ppm.

Estimate using baseline observations at several lengths:

```text
error = measured_distance - known_distance
error ~= additive_constant + scale_ppm * known_distance / 1,000,000
```

Use linear regression when there are enough certified baseline points.

### Prism Constant

The prism constant may be applied by the instrument, by the prism mode, or by post-processing. Double-applying it is a common mistake.

Check:

- Instrument prism setting.
- Prism manufacturer constant.
- Export metadata.
- Whether raw distance already includes correction.

## Angle Handling

Be strict with DMS conversion:

```text
decimal_degree = degree + minute / 60 + second / 3600
```

For negative angles, keep the sign on the whole angle, not only the degree part.

Normalize horizontal angles to 0-360 degrees only after differences are computed correctly across wrap-around.

## Two-Face Observation Pattern

For each target:

1. Record face-left horizontal and vertical readings.
2. Record face-right horizontal and vertical readings.
3. Convert DMS to decimal degrees.
4. Normalize paired readings according to instrument convention.
5. Compute face difference and mean.
6. Flag targets exceeding tolerance.

Never average face-left and face-right values until the 180 degree relationship and vertical-angle convention are handled.

## Distance Calibration Workflow

1. Choose certified baseline or control points with known distances.
2. Observe each line multiple times.
3. Record temperature, pressure, humidity, prism constant, and EDM mode.
4. Reduce slope distance to horizontal distance only when required and when height difference is known.
5. Compare measured distance to known distance.
6. Fit additive constant and scale ppm when multiple distances are available.
7. Report residuals by line.

## Coordinate / Setup Diagnostics

When total-station output seems wrong:

- Check station coordinate and backsight coordinate.
- Check backsight azimuth.
- Check instrument height and prism height.
- Check whether north/east columns are swapped.
- Check whether vertical angle is zenith or elevation.
- Check distance is slope or horizontal.
- Check angle unit and DMS parsing.
- Check prism constant duplication.

## Excel Calculation Guidance

Use separate sheets:

- `raw_observations`
- `constants`
- `angle_conversion`
- `two_face_check`
- `distance_baseline`
- `adjustment_or_regression`
- `report`

Recommended columns:

- station_id
- target_id
- face
- horizontal_angle_raw
- vertical_angle_raw
- slope_distance
- instrument_height
- prism_height
- prism_constant
- temperature
- pressure
- ppm
- known_distance
- remarks

Keep raw DMS text columns and add converted decimal columns rather than replacing the original values.

## Report Checklist

- Instrument model and serial number.
- Prism model and constant.
- Date, operator, weather, temperature, pressure.
- Control points or baseline certificate reference.
- Observation method and number of rounds.
- Angle-unit convention and vertical-angle convention.
- Computed collimation, trunnion, vertical-index, additive, and scale results where data supports them.
- Residual table and tolerance comparison.
- Pass/fail or recommended service action.
- Assumptions and limitations.

## Red Flags

- Single-face observations used to claim instrument calibration.
- No prism constant recorded.
- No atmospheric data for precision distance calibration.
- DMS parsed as decimal degrees.
- Vertical angle formula used without confirming zenith/elevation convention.
- Calibration based on one short distance only.
- Control points not independently verified.
- Raw data overwritten by processed values.

## Response Style

When helping with calculations, state:

- What can be computed from the given data.
- What cannot be verified.
- Which assumptions were made.
- Which raw fields are missing.
- Whether the result is a field check, a calibration estimate, or a formal calibration report.
