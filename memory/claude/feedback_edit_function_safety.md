---
name: 编辑函数的安全操作流程
description: 修改C代码函数时的三步锁定法，防止函数边界被破坏
type: feedback
originSessionId: d657bbda-4e6e-426a-8b1e-727dcc2e5b3f
---
## 编辑函数的安全操作流程

**规则：修改C代码中的函数时，必须按以下三步操作**

### 操作步骤

1. **先锁死函数头**
   - 在 old_string 中包含完整的函数签名
   - 例如：`void work_logic(void)`

2. **再锁死对应结束 }**
   - 在 old_string 中包含函数的结束大括号
   - 确保匹配到正确的函数结束位置

3. **只在函数范围里改**
   - old_string 和 new_string 都必须包含函数头和结束}
   - 只修改函数内部的代码
   - 不要跨函数边界修改

4. **改完马上检查下一个函数有没有被吃进去**
   - 修改完成后立即检查
   - 确认下一个函数的函数头还在
   - 如果被吃掉，立即回滚重新修改

## Why（为什么需要这个规则）

**问题：** 使用 Edit 工具时，如果 old_string 匹配不精确，可能会：
- 把函数结束}误删
- 把下一个函数的开头吃进去
- 导致代码结构被破坏

**解决：** 通过锁定函数头和结束}，确保修改范围严格限定在单个函数内部。

## How to apply（如何应用）

### 正确示例

```c
// 要修改 work_logic() 函数内部的某段代码
old_string:
void work_logic(void)
{
    // ... 原有代码 ...
}

new_string:
void work_logic(void)
{
    // ... 修改后的代码 ...
}
```

### 错误示例（会导致问题）

```c
// ❌ 错误：只匹配函数内部片段，没有锁定边界
old_string:
    AppAuxiliaryFunctionLogic();
    
new_string:
    AppAuxiliaryFunctionLogic();
    NewFunction();
```

这种方式可能会误匹配到其他位置，或者破坏函数结构。

## 检查清单

修改函数后必须检查：
- [ ] 函数头是否完整
- [ ] 函数结束}是否完整
- [ ] 下一个函数的函数头是否还在
- [ ] 代码缩进是否正确
- [ ] 没有多余的}或缺失的}

## 适用场景

- 修改C/C++代码中的函数
- 修改嵌入式代码（如本项目的App_usr.c）
- 任何需要精确控制修改范围的代码编辑

## 相关记忆

- feedback_file_encoding_disaster.md - 修改前必须备份
- feedback_embedded_code_style.md - 嵌入式代码风格规范
