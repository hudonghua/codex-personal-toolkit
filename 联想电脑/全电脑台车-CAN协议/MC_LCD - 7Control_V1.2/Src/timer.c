/*****************************************************************************
 *   timer.c:  Timer C file for NXP LPC17xx Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.26  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include <lpc17xx.h>
#include "config.h"
#include "timer.h"
#include "app_comm.h"

volatile uint32_t timer0_counter = 0;
volatile uint32_t timer0_counter2 = 0;
volatile uint32_t timer1_counter = 0;
volatile uint32_t gSysMsTick = 0;         // 系统累计毫秒计数；用于扫描周期检测

unsigned int   gT0CanCnt = 0;
unsigned char  gCanSFlg = 0;
unsigned char  gT0Flg = 0;				// T0 上升沿标记(100HZ)
unsigned char  gT010msFlg = 0;				// T0 上升沿标记(100HZ)
unsigned short gCanCyc = 0;
unsigned short gCan2Cyc = 0;
unsigned short gT0SCnt = 0;
unsigned char  gT0SFlg = 0;	   				// Timer0 秒
unsigned char  gT0500mSFlg = 0;	   				// Timer0 0.5 秒

unsigned short gLCDCnt = 0;
unsigned char  gLCDFlg = 0;

/*****************************************************************************
** Function name:		delayMs
**
** Descriptions:		Start the timer delay in milo seconds
**						until elapsed
**
** parameters:			timer number, Delay value in milo second			 
** 						
** Returned value:		None
** 
*****************************************************************************/
void delayMs(uint8_t timer_num, uint32_t delayInMs)
{
  if ( timer_num == 0 )
  {
	/*
	* setup timer #0 for delay
	*/
	TIM0 -> TCR = 0x02;		/* reset timer */
	TIM0 -> PR  = 0x00;		/* set prescaler to zero */
	TIM0 -> MR0 = delayInMs * (9000000 / 1000-1);
	TIM0 -> IR  = 0xff;		/* reset all interrrupts */
	TIM0 -> MCR = 0x04;		/* stop timer on match */
	TIM0 -> TCR = 0x01;		/* start timer */
  
	/* wait until delay time has elapsed */
	while (TIM0 -> TCR & 0x01);
  }
  else if ( timer_num == 1 )
  {
	/*
	* setup timer #1 for delay
	*/
	TIM1 -> TCR = 0x02;		/* reset timer */
	TIM1 -> PR  = 0x00;		/* set prescaler to zero */
	TIM1 -> MR0 = delayInMs * (9000000 / 1000-1);
	TIM1 -> IR  = 0xff;		/* reset all interrrupts */
	TIM1 -> MCR = 0x04;		/* stop timer on match */
	TIM1 -> TCR = 0x01;		/* start timer */
  
	/* wait until delay time has elapsed */
	while (TIM1 -> TCR & 0x01);
  }
  return;
}

/******************************************************************************
** Function name:		disable_timer
**
** Descriptions:		Disable timer
**
** parameters:			timer number: 0 or 1
** Returned value:		None
** 
******************************************************************************/
void disable_timer( uint8_t timer_num )
{
  if ( timer_num == 0 )
  {
	TIM0 -> TCR = 0;
  }
  else
  {
	TIM1 -> TCR = 0;
  }
  return;
}

/******************************************************************************
** Function name:		reset_timer
**
** Descriptions:		Reset timer
**
** parameters:			timer number: 0 or 1
** Returned value:		None
** 
******************************************************************************/
void reset_timer( uint8_t timer_num )
{
  uint32_t regVal;

  if ( timer_num == 0 )
  {
	regVal = TIM0 -> TCR;
	regVal |= 0x02;
	TIM0 -> TCR = regVal;
  }
  else
  {
	regVal = TIM1 -> TCR;
	regVal |= 0x02;
	TIM1 -> TCR = regVal;
  }
  return;
}
//  定时中断
void Timer0Cfg(void)
{
	TIM0->TCR = 0x02;
	TIM0->MR0 = TIME_INTERVAL;
	TIM0->MCR = 3;
	NVIC_EnableIRQ(TIMER0_IRQn);
	
}
void Timer1Cfg(void)
{
	TIM1->TCR = 0x02;
	TIM1->MR0 = TIME1_INTERVAL;
	TIM1->MCR = 3;
	NVIC_EnableIRQ(TIMER1_IRQn);
	
}
/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void TIMER0_IRQHandler (void) 
{

	TIM0 -> IR = 1;			/* clear interrupt flag */
	gT0Flg ++;
	gSysMsTick++;           // 每进一次1ms中断就累加一次系统毫秒计数
	  gCommOvDly++	 ;
	timer0_counter++;
	if(timer0_counter == 500)
		gT0500mSFlg = 1;
	if(timer0_counter >= 1000)
	{
		gT0SFlg = 1;
		timer0_counter = 0;
	}

	timer0_counter2++;
	if((timer0_counter2 >= 10))
	{
		gT010msFlg = 1;
		gTimeFlg_10mS++;
		timer0_counter2 = 0;
	}
	
	gT0CanCnt++;
	if((gT0CanCnt>>1) == 0)							// 2 ms
		gCanSFlg |= 1;
	if(gT0CanCnt >= gCanCyc)
	{
		gT0CanCnt = 0;
	}

	if(UART0RcvDly)
	{
		UART0RcvDly--;
		if(UART0RcvDly == 1)
			gUart0RcvFlg = 1;
	}
	if(UART1RcvDly)
	{
		UART1RcvDly--;
		if(UART1RcvDly == 1)
			gUart1RcvFlg = 1;
	}
	gLCDCnt++;
	if(gLCDCnt >= 100)
	{
		gLCDCnt = 0;
		gLcdFresh = 1;
	}
	DI_Scan();
	Key_Scan();
    
}

void T0_mS_Prog(void)
{
	unsigned char i=0;
	


	//// TON  TOF
	for(i=0; i<_TON_N; i++)
	{
		if(gTON[i].vPT > 0)
			gTON[i].vPT--;
	}
	for(i=0; i<_TOF_N; i++)
	{
		if(gTOF[i].vPT > 0)
			gTOF[i].vPT--;
		if(gTOF[i].vIn)
		{
			gTOF[i].vPT = gTOF[i].vPT0;
		}
	}	
}

unsigned int gT1Cnt = 0;
unsigned int gT1PWM = 0;
unsigned char gPWMXPre[6] ;
unsigned char gPWMXChn[6];
unsigned char gPWMXEn[6];			// 使能为 模拟 PWM 

void PWM_SetV(unsigned char vCh, unsigned char vDOX, unsigned char vPWM)
{
	if(vCh < 6)
	{
		gPWMXChn[vCh] = vDOX;
		gPWMXEn[vCh] = 1;
		gPWMXPre[vCh] = vPWM;
	}
}

void TIMER1_IRQHandler (void) 
{						// 0.1mS, 模拟PWM 
	GPIO_TypeDef * pGPIO = NULL;
	unsigned char v = 0;
	unsigned char i = 0;
	static unsigned char stPI = 0; 
	unsigned char v1 = 0;
	unsigned char v2 = 0;
	unsigned char v3 = 0;
	unsigned char v4 = 0;
	static unsigned char stPI1 = 0;
	static unsigned char stPI2 = 0;
	static unsigned char stPI3 = 0;
	static unsigned char stPI4 = 0;

	TIM1 -> IR = 1;			/* clear interrupt flag */
	gT1Cnt++;
	gT1PWM++;
	//speed_k ++ ;
	pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*1));
	v = ((pGPIO->FIOPIN & (0x01 << _DI_1)) == (0x01 << _DI_1)? 1:0);
	if(v != stPI)
	{
		 gPIValAdd++;
		stPI = v;
	}

	
	pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*1));
	v1 = ((pGPIO->FIOPIN & (0x01 << _DI_8)) == (0x01 << _DI_8)? 1:0);
	if(v1 != stPI1)
	{
		 gPIValAdd1++;
		stPI1 = v1;
	}
	
	pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*1));
	v2 = ((pGPIO->FIOPIN & (0x01 << _DI_9)) == (0x01 << _DI_9)? 1:0);
	if(v2 != stPI2)
	{
		 gPIValAdd2++;
		stPI2 = v2;
	}
	
	pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*1));
	v3 = ((pGPIO->FIOPIN & (0x01 << _DI_10)) == (0x01 << _DI_10)? 1:0);
	if(v3 != stPI3)
	{
		 gPIValAdd3++;
		stPI3 = v3;
	}
	
	pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*1));
	v4 = ((pGPIO->FIOPIN & (0x01 << _DI_11)) == (0x01 << _DI_11)? 1:0);
	if(v4 != stPI4)
	{
		 gPIValAdd4++;
		stPI4 = v4;
	}
	
	
	if(gT1Cnt > 10000)
	{

		gT1Cnt = 0;
		gPIVal = gPIValAdd >> 1;
		gPIVal1 = gPIValAdd1 >> 1;
		gPIVal2 = gPIValAdd2 >> 1;
		gPIVal3 = gPIValAdd3 >> 1;
		gPIVal4 = gPIValAdd4 >> 1;
		gPIValAdd = 0;	gPIValAdd1 =0 ;  gPIValAdd2 =0 ;	  gPIValAdd3 =0 ;  gPIValAdd4 =0 ;
	}
  /*
	for(i=0; i<6; i++)
	{
		if(gPWMXEn[i])
		{
			if(gT1PWM < T1_P / 100 * gPWMXPre[i] / PWM_P)
			{
				DOx_IntSet(gPWMXChn[i],1);
			}
			else
			{
				DOx_IntSet(gPWMXChn[i],0);
			}
		}
	}
	if(gT1PWM >= T1_P / PWM_P)
		gT1PWM = 0;
	*/
/*
	if(gT1Cnt < T1_P / 100 * gPWMXPre[0] / PWM_P)
	{
		//GPIO3->FIOSET |= _DO_10;
		DOx_IntSet(gPWMXChn[0],1);
	}
	else
	{
		//GPIO3->FIOCLR |= _DO_10;
		DOx_IntSet(gPWMXChn[0],0);
	}
	if(gT1Cnt < T1_P / 100 * gPWMXPre[1] / PWM_P)
	{
		//GPIO3->FIOSET |= _DO_10;
		DOx_IntSet(gPWMXChn[1],1);
	}
	else
	{
		//GPIO3->FIOCLR |= _DO_10;
		DOx_IntSet(gPWMXChn[1],0);
	}
	if(gT1Cnt > 10000)
	{
		gT1Cnt = 0;
		gPIVal = gPIValAdd >> 1;
		gPIValAdd = 0;
	}
	*/
/*		
	i = 15;
		pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*gDIScanTable[i][0]));
	v = ((pGPIO->FIOPIN & (0x01 << gDIScanTable[i][1])) == (0x01 << gDIScanTable[i][1])? 1:0);
	if(v == 0)
	{
		gDIBitV_AB |= 1;
	}
	else
		gDIBitV_AB &= ~1;
	i = 16;
		pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*gDIScanTable[i][0]));
	v = ((pGPIO->FIOPIN & (0x01 << gDIScanTable[i][1])) == (0x01 << gDIScanTable[i][1])? 1:0);
	if(v == 0)
	{
		gDIBitV_AB |= 2;
	}
	else
		gDIBitV_AB &= ~2;
	AB_Pulse(0);
	*/
}

/******************************************************************************
** Function name:		init_timer
**
** Descriptions:		Initialize timer, set timer interval, reset timer,
**						install timer interrupt handler
**
** parameters:			timer number and timer interval
** Returned value:		true or false, if the interrupt handler can't be
**						installed, return false.
** 
******************************************************************************/
uint32_t init_timer ( void ) 
{

	Timer0Cfg();
	Timer1Cfg();
	//Timer2Cfg();
	//Timer3Cfg();

	return (TRUE);
}

/******************************************************************************
** Function name:		enable_timer
**
** Descriptions:		Enable timer
**
** parameters:			timer number: 0 or 1
** Returned value:		None
** 
******************************************************************************/
void enable_timer( uint8_t timer_num )
{
	TIM0 -> TCR = 1;
	TIM1 -> TCR = 1;
//	TIM2 -> TCR = 1;
//	TIM3 -> TCR = 1;

	return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
