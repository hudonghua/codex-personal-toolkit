# Work State: Algorithm PCB CAN Protocol

This archive records the 2026-06-06 Algorithm PCB CAN protocol/code task.

## What Changed

- Updated firmware CAN sending in `App_usr.c` so periodic multi-frame groups are time-sliced. `CAN_Send_data()` now sends at most one CAN2 frame per call.
- Updated `CAN2_IO_pinding()` so the 119 output/debug frames are also time-sliced if that function is enabled later.
- Kept the self-parallel function changes: `0x17E B7.b0/b1/b2` trigger left/mid/right `init_flags` on rising edge.
- Updated `算法PCB通信说明.html` to match the current program, especially CAN1/CAN2 direction:
  - CAN1 receives encoder/body attitude IDs.
  - CAN2 receives NET_TO_CAN/HMI commands `0x50` and `0x17E`.
  - CAN2 sends algorithm output/status frames.

## Included Files

- `changed-files/App_usr.c`
- `changed-files/CanOpen.c`
- `changed-files/算法PCB通信说明.html`
- `protocol-files/`: full snapshot of `F:\工作\实际进展与验证\模型确定\通讯协议`.
- `logs/codex_rebuild_all_can_timeslice.log`
- `firmware-src.zip`: snapshot of the firmware `Src` directory.

## Verification

Keil rebuild result from `codex_rebuild_all_can_timeslice.log`:

```text
".\FLASH\MC_LCD - 7Control_V.axf" - 0 Error(s), 52 Warning(s).
```

## Source Paths

- Firmware project: `F:\工作\实际进展与验证\模型确定\MC_LCD - 7Control_V1.2\MC_LCD - 7Control_V1.2`
- Protocol document: `F:\工作\实际进展与验证\模型确定\通讯协议\算法PCB通信说明.html`

## Notes

The current local Codex raw session file for 2026-06-06 was not found under `C:\Users\DELL\.codex\sessions`, so this archive contains a task summary and code/document snapshots rather than a raw transcript.
