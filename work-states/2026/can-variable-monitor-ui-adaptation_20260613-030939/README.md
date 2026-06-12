# CanVariableMonitor V1.20 UI Adaptation Handoff

This archive contains the current CanVariableMonitor source and release after the DPI/layout adaptation work on 2026-06-13.

## What changed

- Reverted the earlier superficial toolbar text/style change; the toolbar button is back to `演示` / `退出演示`.
- Kept the normal .NET WinForms high-DPI initialization path via `ApplicationConfiguration.Initialize()`.
- Added runtime-DPI based sizing helpers for fixed UI elements in `MainForm.cs`.
- Added `UiDpi` to `MonitorProfile` so saved splitter/column widths can be interpreted across machines.
- Prevented startup/layout initialization from immediately writing bad splitter widths back into `monitor_profile.json`.
- Clamped the right split so `2 看数值` is not restored to a useless narrow width when a bad profile contains values like `MonitorPanelWidth=450` at DPI 144.
- Default main window now starts maximized for this three-pane workbench.

## Verification

- `dotnet build CanVariableMonitor.csproj -v:minimal`: 0 errors, 65 existing nullable warnings.
- `powershell -ExecutionPolicy Bypass -File .\PublishUnified.ps1`: succeeded.
- Runtime check on this machine: DPI 144, `IsZoomed=True`, final profile layout `LeftPanelWidth=483`, `MonitorPanelWidth=780`, `UiDpi=144`.
- Final screenshot: `ui-v120-final-maximized.png`.

## Important V1.20 facts preserved

- `CanFirmwareDownloader.cs`: `EnableFastModeProbe=false`, so blind 0x82 fast-mode probing stays disabled for old bootloaders.
- `0x16 00` remains recognized as boot control command failure ACK.
- `0x16` frames are not treated as generic pending download responses.
- SYS hot-unplug BSOD is a legacy kernel driver problem, not a normal C# exception.

## Restore

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\install.ps1 -IncludeData
Expand-Archive -Path .\work-states\2026\can-variable-monitor-ui-adaptation_20260613-030939\workspace.zip -DestinationPath "$env:USERPROFILE\Documents\CanVariableMonitor-restored" -Force
```

Open `CanVariableMonitor\CanVariableMonitor.csproj` from the extracted folder.

## Included files

- `workspace.zip`: current source snapshot, excluding `bin`, `obj`, `dist`, and old release artifacts.
- `release\上位机监控_V1.20_20260613_030555.zip`: final published customer zip.
- `V1.20_UI_ADAPTATION.md`: implementation notes.
- `transcript.md` and `raw-session.jsonl`: current Codex session context.
