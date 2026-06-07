#ifndef __SCAN_CYCLE_H__
#define __SCAN_CYCLE_H__

/*
 * 控制器扫描周期检测模块
 *
 * 作用：
 * 1. 统计 MyLogic_10ms() 两次开始之间的实际周期
 * 2. 统计 MyLogic_10ms() 本身跑完用了多少时间
 *
 * 当前单位统一用 ms。
 */

extern unsigned char  gScanTestEn;        /* 扫描周期检测使能；1=开启，0=关闭 */

extern unsigned long  gScanLastStartMs;   /* 上一轮 MyLogic_10ms 开始时刻 */
extern unsigned int   gScanPeriodMs;      /* 当前两轮之间的周期 */
extern unsigned int   gScanPeriodMinMs;   /* 当前观察到的最小周期 */
extern unsigned int   gScanPeriodMaxMs;   /* 当前观察到的最大周期 */

extern unsigned long  gScanStartMs;       /* 本轮 MyLogic_10ms 开始时刻 */
extern unsigned int   gScanRunMs;         /* 本轮 MyLogic_10ms 运行时间 */
extern unsigned int   gScanRunMinMs;      /* 当前观察到的最小运行时间 */
extern unsigned int   gScanRunMaxMs;      /* 当前观察到的最大运行时间 */

void ScanCycle_Init(void);                /* 初始化统计量 */
void ScanCycle_Clear(void);               /* 清空历史统计结果 */
void ScanCycle_Begin(void);               /* 在 MyLogic_10ms 入口调用 */
void ScanCycle_End(void);                 /* 在 MyLogic_10ms 结束前调用 */

#endif
