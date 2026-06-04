---
name: 天腾乳化装药车装药计量逻辑
description: 唐山V1.3 装药计量+收管控制：4模式/PI自适应/米每秒口径/重量来源/流量报警/两份HTML
type: project
---
# 天腾乳化装药车 装药计量与收管控制逻辑（唐山 MC_LCD-7Control_V1.3 -20250405）

源码（仅读未改，GBK）：`E:\AI_划时代\T天腾\C采矿装药车\贵阳后续版本\唐山\MC_LCD - 7Control_V1.3 -20250405\Src\`。
注意同名另一份在 `乳化装药车\唐山\` 下且不一致；以 `贵阳后续版本\唐山` 为准（活跃版本，2026-06 仍在改，大量 .bak）。

## ⚠️ 程序有过"大改"（约 2026-06 初）
- App_usr.c 从 ~3900 行涨到 **8186 行**；`work_logic` 单函数 ~1488 行。
- 从"单一查表"升级为 **4 种联动模式 + 速度自适应闭环**。
- 速度口径从 **米/分 改为 米/秒**（cube_speed_logic 末尾加了 `/60.0`，L4085）。
- 分区阈值改为 **13/17/22/27/32/37**，索引改为 `tube_Set_Speed(m/s)×100`（AI_logic_study，work_logic L6312）。
- 早先那份 HTML `装药量-管速-电流-修正-调参.html` 描述的是**大改前**版本：面积/药量/密度/泵流量公式仍成立，但速度单位、分区阈值、修正机制已过时。

## 计算链（基础，未变）
1. 截面积 `PaoKong_area=π(孔径Paramet_Set1 mm/2/1000)²`
2. 单孔理论药量 `logic_kg_no=截面积×(孔深−余高H_set/10)×密度`，密度=`Paramet_Set7/100×1000`kg/m³
3. 泵流量 `Aomo_min_kg`（泵次数×系数 或 流量计 gPIVal×6）
4. 理论收管速度 `tube_Set_Speed=(Aomo_min_kg/密度)/截面积/60` → **米/秒**
5. 分区 `AI_logic_study((int)(tube_Set_Speed×100))`→PWM_ZONE 1~7（阈值13/17/22/27/32/37）
6. 查表 `Tube_close_PWM=Paramet_Set8~14`（7区阀开度，限幅10~99）
7. 输出 0xCFE3606(节点134) B0=Tube_close_PWM×125/100 → 丹佛斯比例阀

## 4 种联动模式（屏"联动模式"页 +/− 切换，Link_Mode_Apply）
`Link_Mode_Set`：0=无联动 / 1=孔间关联 / 2=同孔关联 / 3=管速关联。掉电记忆。每模式在 EEPROM 各存一套 7 区阀开度表（gModeMap.map[mode][0..6]），切模式即换表（Mode_Map_Save_Current/Load）。屏名："无联动/孔间关联/同孔关联/管速关联"。
- **无联动**：只查表+人工加减键，不自动修。
- **孔间关联**(Hole_Link_Enable)：每孔结束 `Auto_Hole_Between_Learn_Check(整孔理论, 实际)`，按 actual/theory 比例缩放该区阀开度。
- **同孔关联**(Same_Hole_Link_Enable)：装药满10s后每 Auto_Adjust_Time 秒 `Auto_Weight_Learn_Check(阶段理论, 实际)`，差超 Auto_Adjust_Threshold(默认10kg)就±Auto_Adjust_Step。
- **管速关联**：`Tube_Speed_Adaptive_Control()` PI闭环，err=tube_Set_Speed−Drug_Speed_m(每秒采)，adjust=Kp·err+积分(每步±3)；自整定(同号10次Kp×1.1,震荡5次Kp×0.8并削积分)；稳定6拍把开度0.9/0.1混合回灌区表并存Kp/Ki。
- 通用方向：实际>理论(过装)→阀大→收快→减药；实际<理论(欠装)→阀小→收慢→增药。

## 工作主循环 MyLogic_10ms（每10ms，main.c gTimeFlg_10mS 门控）
顺序：CAN_Send_data→CAN_receive_data→Remote_PWM_receive_data→DI_Byte_DO→PIN_Binding→Boom_work_→index_com→App_BengS_times→App_FangL_(空)→work_logic→cube_speed_logic→CAN2_IO_pinding。
注意调用顺序：work_logic 在 cube_speed_logic 之前、CAN_Send_data 在最前→发送的阀命令/所用理论速度滞后一个10ms周期（非bug）。

## 报警保护（work_logic 后半）
6类传感器阈值=Paramet_Set3(主压)/Set2(温度)/Set5(敏化压)/Set6(催化压)/Set4(送管压,触发即禁送管)，约0.8s防抖。
**新增流量信号丢失报警**：自动送药10s后连续3s流量计无脉冲→Drug_flux_no_signal_alarm_flags。
严重报警→work_stop_dly=500停泵+退自动+喇叭；急停 Emergency_alarm_ALL→停泵阀全关。

## 重量来源 Weight_Source_Set（全项目+显示屏一起切）
0=泵送次数(Pump_KG_Aomo)；1=流量计(Flow_KG_Aomo=脉冲/10，**10脉冲=1kg**；Aomo_min_kg=gPIVal×6=60/10)。屏显实际kg(0x56/0x10+i)随之变，理论kg(0x55/0x20+i)不变。

## 产出 HTML（同工程目录）
- `装药车工作逻辑-矿工版.html`（**当前/大改后**，白话+泳道，给矿工/操作工）
- `装药量-管速-电流-修正-调参.html`（大改前计算链，调参向，速度单位等已过时）
关联 [[feedback_protocol_generation]] [[feedback_embedded_code_style]] [[feedback_gbk_file_modification]] [[feedback_cost_economy]]
