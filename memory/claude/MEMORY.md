# Memory Index

**⚠️ 恢复记忆后必须首先读取：[编程手册与纪律.md](编程手册与纪律.md)**

## Setup / Layout
- [reference_shared_memory_layout.md](reference_shared_memory_layout.md) — Junction map: this dir + .cursor/memory → toolkit/memory/* on GitHub (hudonghua/codex-personal-toolkit); Stop hook auto-pushes; portable git/gh paths

## Project Memories
- [project_qdn_full_computer.md](project_qdn_full_computer.md) — 全电脑模型项目核心信息、文档位置、工作流程
- [project_qdn_calibration_strategy.md](project_qdn_calibration_strategy.md) — 现场标定策略：两层标定、视觉自学习修正、倾角补偿机制
- [project_qdn_work_scenario.md](project_qdn_work_scenario.md) — 实际工作场景：自动定位钻孔流程、炮孔图应用、操作界面设计
- [project_qdn_vision_architecture.md](project_qdn_vision_architecture.md) — 视觉系统架构：四点光源+solvePnP、实时性分析、第三层在电脑端实现
- [project_mc_lcd_framework.md](project_mc_lcd_framework.md) — MC_LCD - 7Control_V1.2项目框架：模块分工、正解计算、标定流程、HMI交互
- [project_qdn_vision_decisions.md](project_qdn_vision_decisions.md) — 视觉系统架构与决策（2026-04-12）：定位、靶标、相机、标定链
- [project_qdn_vision_calibration.md](project_qdn_vision_calibration.md) — 视觉内参标定方案（Codex版本）：自动筛帧、清晰度判定、样本管理
- [project_qdn_t_shell_cam_implementation.md](project_qdn_t_shell_cam_implementation.md) — T_shell_cam标定实现：C++函数库、标定流程、现场快速定位相机位姿
- [project_200a_screen_engineering.md](project_200a_screen_engineering.md) — 200A屏项目工程经验：MCU/屏端分工、代码风格、页面修改路径、首页画图规范
- [project_drillplan_visualization.md](project_drillplan_visualization.md) — 炮孔图生成：XML/Excel数据源、坐标系转换、多视角PNG/3D HTML、驾驶室操作手视角
- [project_csxjqr_lcd_display.md](project_csxjqr_lcd_display.md) — C除锈机器人发射机LCD界面：系统状态+6路压力监控+状态报警，BF变量控制刷新
- [project_tt_emulsion_charging_logic.md](project_tt_emulsion_charging_logic.md) — 天腾乳化装药车(唐山V1.3)：装药计量+收管控制；**已大改→4联动模式(无/孔间/同孔/管速PI自适应)、速度改米每秒、分区阈值13~37**、重量来源、流量丢失报警、两份HTML

## Reference Materials
- [reference_200a_interface_definition.md](reference_200a_interface_definition.md) — 200A屏接口定义：35芯/8芯插头针脚、DI/DO/PWM/CAN/模拟量分配
- [reference_mcgspro_analysis.md](reference_mcgspro_analysis.md) — McgsPro MCP文件分析案例：华矿中型机、CAN协议提取、28变量、脚本统计
- [reference_can_id_naming.md](reference_can_id_naming.md) — CAN ID命名规则：EA05→0x2EA.05，变量名与CAN帧的映射关系
- [reference_can_id_naming.md](reference_can_id_naming.md) — CAN ID命名规则：EA05→0x2EA.05，变量名与CAN帧的映射关系

## Feedback & Principles
- [feedback_backup_discipline.md](feedback_backup_discipline.md) — **文件修改备份纪律：仅代码改前必备份（带时间戳），每文件 3 份轮转，改坏不私自回退，退档需用户确认；文档/HTML不备份**
- [feedback_memory_sync_workflow.md](feedback_memory_sync_workflow.md) — **记忆同步流程：改 memory/claude/* 必须走联想电脑本地仓库 + sync-memory.ps1，不直接 gh api**
- [feedback_gbk_file_modification.md](feedback_gbk_file_modification.md) — **GBK编码文件修改方法：只能用Python保持原编码，禁用sed/Edit工具**
- [feedback_brace_problem_fix.md](feedback_brace_problem_fix.md) — **大括号问题修复方法：读程序分析逻辑，找真实配对关系，现场修复，绝不退档**
- [feedback_context_management.md](feedback_context_management.md) — 上下文管理约定：对话过长时主动提醒用户压缩，防止API 400错误
- [feedback_problem_solving_attitude.md](feedback_problem_solving_attitude.md) — 工作态度：遇到技术障碍时自己想办法，不要停止
- [feedback_deep_thinking.md](feedback_deep_thinking.md) — 深入思考原则：主动分析本质，从现象反推原因，提供完整解决方案
- [feedback_diagram_style.md](feedback_diagram_style.md) — 图表默认风格：所有图表使用清爽蓝色风格展示
- [feedback_mcp_file_analysis.md](feedback_mcp_file_analysis.md) — MCP文件分析方法：用Python提取Access数据库内容，不要直接放弃
- [feedback_protocol_generation.md](feedback_protocol_generation.md) — 协议文档生成规范：必须基于实际代码，不能凭想象，先读代码再验证bin文件
- [feedback_sincerity_principle.md](feedback_sincerity_principle.md) — 真诚工作原则：不玩套路，不确定就说不确定，建立真正的信任关系
- [feedback_cost_economy.md](feedback_cost_economy.md) — **成本节制：用户对token花费敏感(20元事件)，启动记忆仪式与探索要克制，先读索引按需回读**
- [feedback_hardware_project_rigor.md](feedback_hardware_project_rigor.md) — 硬件项目严谨性：不能有半点马虎，协议错了设备就不工作，只有对或错没有差不多
- [feedback_file_encoding_disaster.md](feedback_file_encoding_disaster.md) — 文件编码处理底线：修改前必须备份，绝对不能把程序搞没了
- [feedback_embedded_code_style.md](feedback_embedded_code_style.md) — 嵌入式代码风格：避免指针/数组/枚举/结构体，用简单变量方便Keil全局搜索追踪
- [feedback_user_coding_standards.md](feedback_user_coding_standards.md) — 用户编码规范：变量命名、函数命名、200A屏项目套路、已犯错误记录
- [feedback_edit_function_safety.md](feedback_edit_function_safety.md) — 编辑函数安全流程：先锁函数头，再锁结束}，只在函数内改，改完检查下一个函数
- [feedback_bf_variable_pattern.md](feedback_bf_variable_pattern.md) — BF变量双重数据源模式：正常运行从CAN读取，调试时手动修改，两者都需要
- [feedback_bf_wl_compatibility.md](feedback_bf_wl_compatibility.md) — BF和WL变量兼容模式：条件赋值实现双重数据源共存，只要有一个非0就全部赋值
