---
name: survey-data-processor
description: Survey data processing for total-station, GNSS, leveling, point clouds, DTM/TIN generation, coordinate transformation, traverse adjustment, level-loop adjustment, contour generation, and survey deliverables. Use when processing field survey observations, control points, adjusted coordinates, or measurement workbooks.
---

# Survey Data Processor

Use this skill for surveying and civil-engineering data workflows: total-station observations, GNSS observations, leveling loops, point-cloud data, DTM/TIN surfaces, contour generation, coordinate transformation, traverse adjustment, and measurement quality reports.

This local skill is based on the publicly indexed `survey-data-processor` agent skill by `a5c-ai`, expanded for Codex use.

## When To Use

- Processing total-station, GNSS, leveling, or point-cloud survey data.
- Cleaning field books, CSV exports, or Excel workbooks from instruments.
- Computing or checking traverse closure, coordinate transformation, or adjusted coordinates.
- Creating DTM/TIN surfaces, contours, survey maps, or transformation reports.
- Validating measurement quality before producing CAD/GIS deliverables.

## Inputs To Ask For

- Instrument type and exported data format.
- Coordinate system, datum, units, and project control points.
- Raw observations, not only processed coordinates.
- Field method: traverse, resection, stakeout, topographic pickup, leveling, GNSS, scan, or mixed.
- Required tolerance or specification.
- Whether results need Excel, CSV, CAD, GIS, PDF, or report output.

## Core Workflow

1. Preserve raw data.
2. Identify coordinate system, units, control network, and observation type.
3. Validate schema: station ID, target ID, horizontal angle, vertical angle or zenith angle, slope distance, prism height, instrument height, temperature, pressure, prism constant, and timestamp where applicable.
4. Check for impossible values: negative distance, duplicated point IDs, missing backsight, wrong angle units, swapped north/east, inconsistent height units.
5. Apply required corrections and transformations.
6. Compute closures, residuals, and quality checks.
7. Produce adjusted coordinates and a clear report of assumptions.

## Total Station Data Checks

- Confirm angle convention: horizontal angle, vertical angle, zenith angle, degrees-minutes-seconds, grads, or decimal degrees.
- Confirm distance type: slope distance, horizontal distance, or already reduced distance.
- Confirm prism constant and whether it was already applied by the instrument.
- Confirm atmospheric correction: temperature, pressure, humidity, and PPM.
- Confirm instrument height and prism height signs.
- Confirm face-left / face-right observations when available.
- Confirm station setup: known station plus backsight, resection, free station, or traverse.

## Traverse Adjustment

Check:

- Angular closure.
- Linear closure.
- Relative precision.
- Coordinate misclosure in north/east.
- Height closure if elevations are observed.

Adjustment methods:

- Compass/Bowditch rule for routine traverse adjustment.
- Transit rule when angular precision is stronger than distance precision and local practice supports it.
- Least-squares adjustment when redundancy and observation standard deviations are available.

Report:

- Raw closure.
- Allowed closure.
- Adjustment method.
- Corrections by station or leg.
- Final coordinates and residuals.

## Coordinate Transformation

For 2D work, consider:

- Translation only.
- 2D similarity transform: translation, rotation, scale.
- Affine transform when local distortion requires it, but avoid using it casually.

For 3D work, consider:

- 3D similarity / Helmert transformation.
- Rigid transform when scale must remain fixed.

Always report:

- Control points used.
- Residuals at control points.
- RMS error.
- Units.
- Whether scale was solved or fixed.

## Deliverables

- Cleaned survey table.
- Adjusted coordinate table.
- Closure and residual report.
- Transformation parameter report.
- DTM/TIN or contour outputs when data supports it.
- Notes on rejected points and why they were rejected.

## Safety

- Do not overwrite raw data.
- Do not silently assume units.
- Do not mix coordinate systems without explicit transformation.
- Do not average observations until instrument face, setup, and target identity are confirmed.
- Flag assumptions clearly when raw metadata is missing.
