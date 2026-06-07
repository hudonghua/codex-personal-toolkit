# Arm200A 正解与标定验证使用说明

## 1. 目的

这份说明只服务实验室验证。

当前目标是：
- 验证第一类误差，也就是 Creo 基线误差
- 手工输入传感器物理量
- 计算三条臂各关键点坐标
- 用 20 个已知点去拟合固定参数修正量
- 把修正量保存到 EEPROM

当前不处理：
- 机械加工误差
- 销轴装配误差
- 现场摆车误差

## 2. 已经落到工程里的文件

### 正解主模块

- [Arm200A_Kine.h](F:\工作\实际进展与验证\模型确定\MC_LCD - 7Control_V1.2\MC_LCD - 7Control_V1.2\Src\Arm200A_Kine.h)
- [Arm200A_Kine.c](F:\工作\实际进展与验证\模型确定\MC_LCD - 7Control_V1.2\MC_LCD - 7Control_V1.2\Src\Arm200A_Kine.c)

### 保存模块

- [Arm200A_Store.h](F:\工作\实际进展与验证\模型确定\MC_LCD - 7Control_V1.2\MC_LCD - 7Control_V1.2\Src\Arm200A_Store.h)
- [Arm200A_Store.c](F:\工作\实际进展与验证\模型确定\MC_LCD - 7Control_V1.2\MC_LCD - 7Control_V1.2\Src\Arm200A_Store.c)

### 已挂接入口

- [App_usr.c](F:\工作\实际进展与验证\模型确定\MC_LCD - 7Control_V1.2\MC_LCD - 7Control_V1.2\Src\App_usr.c)
- [main.c](F:\工作\实际进展与验证\模型确定\MC_LCD - 7Control_V1.2\MC_LCD - 7Control_V1.2\Src\main.c)

## 3. 三臂模型口径

### 左臂

`O0 -> O -> B -> A -> C -> D -> D1 -> E -> E1 -> F -> G -> G1 -> H`

共 8 自由度。

### 中臂

`O -> B -> A -> C -> D -> D1 -> E -> E1 -> F -> G -> G1 -> H`

共 7 自由度。

### 右臂

`O0 -> O -> B -> A -> C -> D -> D1 -> E -> E1 -> F -> G -> G1 -> H`

共 8 自由度。

## 4. 输入接口

你后面直接给物理量，不给 ADC 原始值。

### 左臂输入

`gArmLeftIn`

### 中臂输入

`gArmMidIn`

### 右臂输入

`gArmRightIn`

### 每条臂需要填写的量

- `root_deg`
- `base_deg`
- `boom_deg`
- `cd_mm`
- `d1_deg`
- `e1_deg`
- `f_deg`
- `gg1_mm`

说明：
- 角度单位：度
- 长度单位：mm
- 中臂没有 `O0` 根部层，因此中臂的 `root_deg` 直接给 `0`

## 5. 结果输出

每条臂的计算结果都已经展开成单独点位，便于逐点验证。

### 左臂结果

`gArmLeftOut`

可直接看：
- `gArmLeftOut.o0`
- `gArmLeftOut.o`
- `gArmLeftOut.b`
- `gArmLeftOut.a`
- `gArmLeftOut.c`
- `gArmLeftOut.d`
- `gArmLeftOut.d1`
- `gArmLeftOut.e`
- `gArmLeftOut.e1`
- `gArmLeftOut.f`
- `gArmLeftOut.g`
- `gArmLeftOut.g1`
- `gArmLeftOut.h`

### 中臂结果

`gArmMidOut`

### 右臂结果

`gArmRightOut`

`ok = 1` 表示本轮点位已完成计算。

## 6. 自动运行逻辑

### 上电时

在 `MyLogic_10ms()` 第一次进入时会自动执行：
- `Arm200A_Init()`
- `Arm200A_StoreLoad()`

也就是说：
- 默认几何参数会先装载
- EEPROM 中已有的修正量会自动读回

### 运行时

每个 10ms 周期会自动执行：
- `Arm200A_Task10ms()`

所以只要你更新了 `gArmLeftIn / gArmMidIn / gArmRightIn`，下一轮就会自动刷新各点坐标。

## 7. 20 点样本固定模板槽位

每条臂都有 20 个固定模板槽位，客户直接手工填写即可。

### 左臂样本

`gArmLeftSample[20]`

### 中臂样本

`gArmMidSample[20]`

### 右臂样本

`gArmRightSample[20]`

### 每个样本要填什么

- `use`
- `in`
- `h_ref`

说明：
- `use = 1` 表示该样本有效
- `in` 是该样本下的传感器物理量
- `h_ref` 是该样本下 Creo 已知的 H 点坐标

程序会自动计算：
- `h_now`
- `h_err`

如需全部清空，可调用：
- `Arm200A_ClearLeftSample()`
- `Arm200A_ClearMidSample()`
- `Arm200A_ClearRightSample()`
- `Arm200A_ClearAllSample()`

## 8. 拟合入口

### 左臂

调用：

`Arm200A_FitLeft();`

### 中臂

调用：

`Arm200A_FitMid();`

### 右臂

调用：

`Arm200A_FitRight();`

### 拟合结果

分别看：
- `gArmLeftFit`
- `gArmMidFit`
- `gArmRightFit`

里面有：
- `count`
- `rms_mm`
- `max_mm`
- `last_step_mm`
- `ok`

当前规则：
- 有效样本数至少要 `>= 6`
- 当前实验室门槛先按 `rms_mm <= 3.0f` 认为 `ok = 1`

## 9. 固定修正量

每条臂都有一套固定修正量。

### 左臂

`gArmLeftFix`

### 中臂

`gArmMidFix`

### 右臂

`gArmRightFix`

这套修正量当前服务的是第一类误差。

当前已经做成 19 个标量映射，主要覆盖：
- `O0 -> O`
- `O -> B`
- `B -> A`
- `A -> C`
- `D1 -> E`
- `E -> E1`
- `E1 -> F`
- `F -> G`
- `G1 -> H`
- `D -> D1`
- `E1 -> E2`

## 10. 保存与读取

### 读取

上电自动读一次：

`Arm200A_StoreLoad();`

### 保存

当前已经挂到原模板的保存流程里。

当：

`gLCDPage.vSave = 1`

时，会跟原来的 `SaveRetain_()` 一起执行：

`Arm200A_StoreSave();`

### 当前保存方式

- 单份保存
- 单独 EEPROM 地址区
- 不占原 16 个参数保存槽

## 11. 推荐验证顺序

### 第一步

先只做正解，不做拟合。

做法：
- 手工写一条臂的输入
- 看 `o / b / a / c / d / d1 / e / e1 / f / g / g1 / h`
- 和 Creo 对点核对

### 第二步

逐点验证。

推荐先盯：
- `B`
- `A`
- `D1`
- `E1`
- `F`
- `H`

### 第三步

再上 20 点样本。

做法：
- 填样本输入 `in`
- 填已知 `h_ref`
- 调 `Arm200A_FitLeft()` 或中/右对应函数
- 看 `rms_mm` 和 `max_mm`

### 第四步

拟合后再回头逐点看各铰接点是否一起变准。

不能只看 `H` 点。

## 12. 当前限制

- 这版还没有做编译验证
- 这版先按实验室阶段使用
- 这版默认你给的是已经换算好的物理角度和物理长度
- 这版只针对第一类误差，不处理机械装配和现场摆放误差

## 13. 一句最短用法

1. 给 `gArmLeftIn / gArmMidIn / gArmRightIn` 写传感器物理量  
2. 让 `Arm200A_Task10ms()` 自动计算点位  
3. 看 `gArmLeftOut / gArmMidOut / gArmRightOut` 的各点坐标  
4. 填 20 个样本  
5. 调 `Arm200A_FitLeft / Mid / Right` 做第一类误差拟合  
6. 看 `rms_mm / max_mm`  
7. 触发 `gLCDPage.vSave = 1` 保存修正量
