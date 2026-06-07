/*****************************************************************************
 *   timer.h:  Header file for NXP LPC17xx Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.26  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __TIMER_H 
#define __TIMER_H

#include "config.h"


#define Cclk			(Fextal * 8)
#define Fcclk 			( Cclk / 4)

#define T1_P				10000

#define PWM_P				100

#define TIME_INTERVAL	(Fcclk/1000 - 1)
#define TIME1_INTERVAL	(Fcclk/10000 - 1)

#define TIME_PR_V		4			// 分频



extern volatile uint32_t timer0_counter;
extern volatile uint32_t gSysMsTick;      // 系统累计毫秒计数

extern unsigned char  gT010msFlg ;				// T0 10ms 标记
extern unsigned char  gT0Flg;
extern unsigned char  gSysPower;
extern unsigned short  gPowerDly;
extern unsigned char  gCanSFlg;
extern unsigned char  gT0SFlg ;
extern unsigned char  gT0500mSFlg ;	   				// Timer0 0.5 秒

extern unsigned short gLCDCnt ;
extern unsigned char  gLCDFlg ;
extern unsigned short gCanCyc;
extern unsigned short gCan2Cyc;

extern unsigned char gPWMXPre[6];

extern void delayMs(uint8_t timer_num, uint32_t delayInMs);
extern uint32_t init_timer( void );
extern void enable_timer( uint8_t timer_num );
extern void disable_timer( uint8_t timer_num );
extern void reset_timer( uint8_t timer_num );
extern void TIMER0_IRQHandler (void);
extern void TIMER1_IRQHandler (void);
void T0_mS_Prog(void);


void PWM_SetV(unsigned char vCh, unsigned char vDOX, unsigned char vPWM);




#endif /* end __TIMER_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/

