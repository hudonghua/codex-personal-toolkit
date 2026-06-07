#include "ScanCycle.h"
#include "config.h"

unsigned char gScanTestEn = 0;        /* 扫描周期检测总开关；默认关闭 */
unsigned long gScanLastStartMs = 0;   /* 上一轮开始时刻 */
unsigned int  gScanPeriodMs = 0;      /* 当前周期 */
unsigned int  gScanPeriodMinMs = 0;   /* 最小周期 */
unsigned int  gScanPeriodMaxMs = 0;   /* 最大周期 */

unsigned long gScanStartMs = 0;       /* 本轮开始时刻 */
unsigned int  gScanRunMs = 0;         /* 本轮运行时间 */
unsigned int  gScanRunMinMs = 0;      /* 最小运行时间 */
unsigned int  gScanRunMaxMs = 0;      /* 最大运行时间 */

static unsigned char gScanStartOk = 0; /* 是否已经出现过第一轮开始 */

void ScanCycle_Init(void)
{
    gScanLastStartMs = 0;
    gScanPeriodMs = 0;
    gScanPeriodMinMs = 0;
    gScanPeriodMaxMs = 0;
    gScanStartMs = 0;
    gScanRunMs = 0;
    gScanRunMinMs = 0;
    gScanRunMaxMs = 0;
    gScanStartOk = 0;
}

void ScanCycle_Clear(void)
{
    ScanCycle_Init(); /* 当前清空就等同于重新初始化 */
}

void ScanCycle_Begin(void)
{
    unsigned long now_ms;
    unsigned long delta_ms;

    if(gScanTestEn == 0) return; /* 不使能时不统计 */

    now_ms = gSysMsTick;
    gScanStartMs = now_ms;

    if(gScanStartOk)
    {
        delta_ms = now_ms - gScanLastStartMs;
        gScanPeriodMs = (unsigned int)delta_ms;

        if((gScanPeriodMinMs == 0) || (gScanPeriodMs < gScanPeriodMinMs))
        {
            gScanPeriodMinMs = gScanPeriodMs;
        }
        if(gScanPeriodMs > gScanPeriodMaxMs)
        {
            gScanPeriodMaxMs = gScanPeriodMs;
        }
    }

    gScanLastStartMs = now_ms;
    gScanStartOk = 1;
}

void ScanCycle_End(void)
{
    unsigned long now_ms;
    unsigned long delta_ms;

    if(gScanTestEn == 0) return; /* 不使能时不统计 */
    if(gScanStartOk == 0) return; /* 没有开始标记时不统计 */

    now_ms = gSysMsTick;
    delta_ms = now_ms - gScanStartMs;
    gScanRunMs = (unsigned int)delta_ms;

    if((gScanRunMinMs == 0) || (gScanRunMs < gScanRunMinMs))
    {
        gScanRunMinMs = gScanRunMs;
    }
    if(gScanRunMs > gScanRunMaxMs)
    {
        gScanRunMaxMs = gScanRunMs;
    }
}
