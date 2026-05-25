---
name: backup-and-edit
description: 修改文件前自动备份，滚动保留最近 3 份（超过 3 份时把最旧的删除）。适用于所有需要修改的文件，特别是 C/H 源码文件。
---

# 备份并修改文件

修改文件前自动备份，滚动保留最近 3 份。

## 核心原则

**修改文件前必须备份，滚动保留最近 3 份；超过 3 份时把最旧的备份删除。**

为什么 3 份不是 1 份：
- 1 份只能回到上一次修改前，连续两次改坏就无路可退。
- 3 份能覆盖"本次 / 上次 / 上上次"三档历史，足够追溯最近的有效状态。
- 更多份会导致备份堆积、混乱。

## 备份策略

### 备份文件命名规则
```
原文件名.bak_YYYYMMDD_HHMMSS
```

例如：
- `App_lcd.c.bak_20260526_140530`
- `Arm200A_Kine.c.bak_20260526_141205`

### 备份流程（滚动保留 3 份）

#### 1. 删除最旧的备份（仅当 ≥ 3 份现有备份时）

保留最新 2 份现有备份，加上即将新建的这次，总数刚好 3 份。

```bash
# 列出现有备份，按修改时间倒序（最新在前）
# 跳过前 2 份（保留它们），删除第 3 份及以后（最旧的）
ls -1t 原文件.bak_* 2>/dev/null | tail -n +3 | while IFS= read -r f; do rm -f "$f"; done
```

#### 2. 创建新备份
```bash
cp 原文件 "原文件.bak_$(date +%Y%m%d_%H%M%S)"
```

#### 3. 修改文件
```python
# 对于 GBK 编码的 C 文件（必须用 Python，禁用 Edit 工具）
with open('原文件', 'rb') as f:
    content = f.read().decode('gbk', errors='ignore')

# 执行修改
# ...

# 写回
with open('原文件', 'wb') as f:
    f.write(content.encode('gbk'))
```

UTF-8 文件可以直接用 Edit 工具。

## 完整示例

### 修改 GBK 编码的 C 文件

```bash
# 1. 滚动删除最旧备份（保留最新 2 份，加新建的共 3 份）
ls -1t App_lcd.c.bak_* 2>/dev/null | tail -n +3 | while IFS= read -r f; do rm -f "$f"; done

# 2. 创建新备份
cp App_lcd.c "App_lcd.c.bak_$(date +%Y%m%d_%H%M%S)"

# 3. 用 Python 修改
python3 << 'EOF'
with open('App_lcd.c', 'rb') as f:
    content = f.read().decode('gbk', errors='ignore')

content = content.replace('old_text', 'new_text')

with open('App_lcd.c', 'wb') as f:
    f.write(content.encode('gbk'))
EOF
```

### 修改 UTF-8 编码的文件

```bash
# 1. 滚动删除最旧备份
ls -1t config.json.bak_* 2>/dev/null | tail -n +3 | while IFS= read -r f; do rm -f "$f"; done

# 2. 创建新备份
cp config.json "config.json.bak_$(date +%Y%m%d_%H%M%S)"

# 3. 用 Edit 工具修改文件
# ...
```

## 报告格式

```
📋 现有备份：N 份
🗑️ 已删除最旧备份：App_lcd.c.bak_20260503_120000（仅当原本 ≥ 3 份时）
✅ 已创建新备份：App_lcd.c.bak_20260526_140530
📦 当前备份链：3 份（最新→最旧）
   - App_lcd.c.bak_20260526_140530
   - App_lcd.c.bak_20260525_181200
   - App_lcd.c.bak_20260524_103045
✅ 已修改文件：App_lcd.c
✅ 已验证：[显示修改后的关键内容]
```

## 特殊情况

### 第一次修改（没有旧备份）

```bash
ls -1t App_lcd.c.bak_* 2>/dev/null | tail -n +3 | while IFS= read -r f; do rm -f "$f"; done
# 没有备份，上面命令什么也不做
cp App_lcd.c "App_lcd.c.bak_$(date +%Y%m%d_%H%M%S)"
```

报告：
```
ℹ️ 没有旧备份
✅ 已创建新备份：App_lcd.c.bak_20260526_140530
📦 当前备份链：1 份
```

### 已有 1 或 2 份备份时

不删除任何旧备份，直接新建。备份链变为 2 或 3 份。

### 已有 3 份及以上时

删除"第 3 份及以后"的备份（也就是除最新 2 份外的所有旧备份），再新建。新建后正好 3 份。

### 需要保留更多备份时

如果用户明确要求保留 N 份（N > 3）或永久保留某个备份，询问：

```
❓ 检测到已有备份：
- App_lcd.c.bak_20260503_120000
- App_lcd.c.bak_20260503_130000
- App_lcd.c.bak_20260503_140000

按默认规则会删除最旧的 1 份并新建 1 份（保持 3 份）。
是否需要：
  A. 默认（滚动保留 3 份）
  B. 临时保留更多（指定份数）
  C. 把某份重命名为关键节点（不参与滚动）
```

### 关键节点备份

如果某份备份是"重要里程碑"（如重大重构前、协议升级前），可以重命名脱离 `.bak_` 命名族，避免被滚动删除：

```bash
mv App_lcd.c.bak_20260526_140530 App_lcd.c.milestone_v2_proto_change
```

这样 `rm -f *.bak_*` 不会动它。

## 禁止的操作

❌ **不要：**
- 不备份就修改文件
- 备份份数超过 3 份不清理（会堆积）
- 用 `rm -f *.bak_*` 全删（会把最近 2 份也删掉，违反 3 份保留）
- 删除备份时误删原文件
- 修改 GBK 文件时用 Edit 工具（必须用 Python）

## 安全检查

修改前确认：
- [ ] 删除的是 `.bak_*` 文件，不是原文件
- [ ] 删除使用 `ls -1t ... | tail -n +3 | xargs rm -f` 模式（只删第 3 份及以后）
- [ ] 新备份创建成功后才修改原文件
- [ ] GBK 文件用 Python；UTF-8 文件才允许 Edit

修改后确认：
- [ ] 备份链最多 3 份
- [ ] 编码没变（GBK 仍是 GBK，UTF-8 仍是 UTF-8）
- [ ] 原文件功能正确（让用户验证，不擅自说"完成"）

## 与"退档"配合

退档时备份链最多 3 档可选：

```
📦 可退档备份（3 份）：
  1) xxx.bak_20260526_140530（最近一次修改前）
  2) xxx.bak_20260525_181200（上次修改前）
  3) xxx.bak_20260524_103045（上上次修改前）

请确认退到哪一档？
```

退档前会把"坏版本"另存为 `xxx.corrupted_<时间戳>`（不计入 `.bak_*` 命名族，不会被滚动删除），方便事后排查。

## 记住

**修改前先滚动整理备份链：最多 3 份，超过就删最旧的。命名格式固定为 `.bak_YYYYMMDD_HHMMSS`。**
