/*****************************************************************************
 *   pwm.c:  PWM module file for NXP LPC17xx Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.26  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "lpc17xx.h"
#include "config.h"
#include "pwm.h"


 
uint32_t GADC_Databuf = 0;
mPWM PWM;
unsigned int  gPwmCycle;
volatile uint32 match_counter1;	 //match_counter0, 

 float _R_Valve[12]=       			// 阀芯电阻
{
// 右转、左转、主落幅、主起幅、主落钩、主起钩、主伸臂、主缩臂、副落钩、副起钩、副伸臂、副收臂
	22.4,22.4, 22.2,   22.9,   23.0,   23.0,   22.6,   22.4,   22.3,   22.3,   22.0,   22.0	
//	27.2,27.2, 27.2,   27.2,   27.2,   27.2,   27.2,   27.2,   27.2,   27.2,   27.2,   27.2	
//	29.0,29.0, 29.0,   29.0,   30.0,   30.0,   30.0,   30.0,   30.0,   30.0,   30.0,   30.0	
};


/******************************************************************************
** Function name:		PWM1_IRQHandler
**
** Descriptions:		PWM1 interrupt handler
**						For now, it only deals with PWM1 match 0
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void PWM1_IRQHandler (void) 
{
  uint32 regVal;
//  WDT_Feed();

  regVal =  PWM1->IR;
  if ( regVal & MR0_INT )
  {
//	match_counter1++;	
  }
   PWM1->IR |= regVal;		/* clear interrupt flag on match 0 */
  return;
}

/******************************************************************************
** Function name:		PWM_Init
**
** Descriptions:		PWM initialization, setup all GPIOs to PWM0~6,
**				reset counter, all latches are enabled, interrupt
**				on PWMMR0, install PWM interrupt to the VIC table.
**
** parameters:			ChannelNum, Duty cycle
** Returned value:		true or fase, if VIC table is full, return false
** 
******************************************************************************/
	uint32 pwm_pclkdiv, pwm_pclk = PWM_CLK;
uint32 PWM_Init( uint32 ChannelNum, uint32 cycle )
{
#if _PWM_EN
	pwm_pclkdiv = (SC->PCLKSEL0 >> 24) & 0x03;
	switch ( pwm_pclkdiv )
	{
		case 0x00:
		default:
		  pwm_pclk = SystemFrequency/4;
		break;
		case 0x01:
		  pwm_pclk = SystemFrequency;
		break; 
		case 0x02:
		  pwm_pclk = SystemFrequency/2;
		break; 
		case 0x03:
		  pwm_pclk = SystemFrequency/8;
		break;
	}
  {
	match_counter1 = 0;
	 SC->PCONP |= (1 << 6);
//	 PINCON->PINSEL4 &= ~0x00003fff;
//	 PINCON->PINSEL4 |= 0x00001555;	/* set GPIOs for all PWM pins on PWM0 */

	 PWM1->TCR = TCR_RESET;	/* Counter Reset */ 
	 //PWM1->CTCR = 0;
	 //PWM1->CCR = 0;
	 PWM1->PR = 0x0;		/* count frequency:Fpclk */
	 PWM1->MCR = PWMMR0I | PWMMR0R;  /*  ;	 interrupt on PWMMR0, reset on PWMMR0, reset 
								TC if PWM0 matches */		
		pwm_pclk = pwm_pclk/cycle;									
	 PWM1->MR0 = pwm_pclk;		/* set PWM cycle */
	 PWM1->MR1 = 0;
	 PWM1->MR2 = 0;
	 PWM1->MR3 = 0;
	 PWM1->MR4 = 0;
	 PWM1->MR5 = 0;
	 PWM1->MR6 = 0;	
/*	 PWM1->MR1 = cycle * 5/6;
	 PWM1->MR2 = cycle * 2/3;
	 PWM1->MR3 = cycle * 1/2;
	 PWM1->MR4 = cycle * 1/3;
	 PWM1->MR5 = cycle * 1/6;
	 PWM1->MR6 = 0;	
*/ 
	/* all PWM latch enabled    */
	 PWM1->LER = 3;// LER0_EN;
	  PWM1->LER |= LER0_EN | LER1_EN | LER2_EN | LER3_EN | LER4_EN | LER5_EN | LER6_EN;
  }

  NVIC_EnableIRQ(PWM1_IRQn);
  return (TRUE);
#else
  return (FALSE);
#endif
}

void PWM_SetCyc(unsigned int vCyc)
{
		pwm_pclk = PWM_CLK/vCyc;									
	 PWM1->MR0 = pwm_pclk;		/* set PWM cycle */
	 PWM1->LER = 3;// LER0_EN;
	 PWM1->TCR = TCR_CNT_EN | TCR_PWM_EN;	/* counter enable, PWM enable */
}

/******************************************************************************
** Function name:		PWM_SetX
**
** Descriptions:		PWM1.x cycle setup
**
** parameters:			Channel number:1~6, PWM cycle 百分比
** Returned value:		None
** 
******************************************************************************/
void PWM_SetX(uint32 vCh, uint32 vPre)
{
#if _PWM_EN	
	switch(vCh)
	{
		case 0:
			PWM1->MR1 = pwm_pclk * vPre / 100;
			PWM1->LER |= LER1_EN;
			PWM1_EN;
			break;
		case 1:
			PWM1->MR2 = pwm_pclk * vPre / 100;
			PWM1->LER |= LER2_EN;
			PWM2_EN;
			break;
		case 2:
			PWM1->MR3 = pwm_pclk * vPre / 100;
			PWM1->LER |= LER3_EN;
			PWM3_EN;
			break;
		case 3:
			PWM1->MR4 = pwm_pclk * vPre / 100;
			PWM1->LER |= LER4_EN;
			PWM4_EN;
			break;
		case 4:
			PWM1->MR5 = pwm_pclk * vPre / 100;
			PWM1->LER |= LER5_EN;
			PWM5_EN;
			break;
		case 5:
			PWM1->MR6 = pwm_pclk * vPre / 100;
			PWM1->LER |= LER6_EN;
			PWM6_EN;
			break;
		default:
			break;
	}
#endif	
}

//// 1A/1B ~ 6A/6B, vCh=0~11
void PWM_SetXAB(uint32 vCh, uint32 vPre)
{
#if _PWM_EN	
	switch(vCh)
	{
		case 0:
			PWM1_A
			PWM1->MR1 = pwm_pclk * vPre / 1000;
			PWM1->LER |= LER1_EN;
			PWM1_EN;
			break;
		case 1:
			PWM1_B
			PWM1->MR1 = pwm_pclk * vPre / 1000;
			PWM1->LER |= LER1_EN;
			PWM1_EN;
			break;
		case 2:
			PWM2_A
			PWM1->MR2 = pwm_pclk * vPre / 1000;
			PWM1->LER |= LER2_EN;
			PWM2_EN;
			break;
		case 3:
			PWM2_B
			PWM1->MR2 = pwm_pclk * vPre / 1000;
			PWM1->LER |= LER2_EN;
			PWM2_EN;
			break;
		case 4:
			PWM3_A
			PWM1->MR3 = pwm_pclk * vPre / 1000;
			PWM1->LER |= LER3_EN;
			PWM3_EN;
			break;
		case 5:
			PWM3_B
			PWM1->MR3 = pwm_pclk * vPre / 1000;
			PWM1->LER |= LER3_EN;
			PWM3_EN;
			break;
		case 6:
			PWM4_A
			PWM1->MR4 = pwm_pclk * vPre / 1000;
			PWM1->LER |= LER4_EN;
			PWM4_EN;
			break;
		case 7:
			PWM4_B
			PWM1->MR4 = pwm_pclk * vPre / 1000;
			PWM1->LER |= LER4_EN;
			PWM4_EN;
			break;
		case 8:
			PWM5_A
			PWM1->MR5 = pwm_pclk * vPre / 1000;
			PWM1->LER |= LER5_EN;
			PWM5_EN;
			break;
		case 9:
			PWM5_B
			PWM1->MR5 = pwm_pclk * vPre / 1000;
			PWM1->LER |= LER5_EN;
			PWM5_EN;
			break;
		case 10:
			PWM6_A
			PWM1->MR6 = pwm_pclk * vPre / 1000;
			PWM1->LER |= LER6_EN;
			PWM6_EN;
			break;
		case 11:
			PWM6_B
			PWM1->MR6 = pwm_pclk * vPre / 1000;
			PWM1->LER |= LER6_EN;
			PWM6_EN;
			break;
		default:
			break;
	}
#endif	
}

/******************************************************************************
** Function name:		PWM_SetX_mV
**
** Descriptions:		PWM1.x cycle setup
**
** parameters:			Channel number:1~6, PWM mV mA
** Returned value:		None
** 
******************************************************************************/
void PWM_SetX_mV(uint32 vCh, uint32 vmV, uint32 vmVMax)
{
#if _PWM_EN	
	
	switch(vCh)
	{
		case 0:
			PWM1->MR1 = pwm_pclk * vmV / vmVMax;
			PWM1->LER |= LER1_EN;
			PWM1_EN;
			break;
		case 1:
			PWM1->MR2 = pwm_pclk * vmV / vmVMax;
			PWM1->LER |= LER2_EN;
			PWM2_EN;
			break;
		case 2:
			PWM1->MR3 = pwm_pclk * vmV / vmVMax;
			PWM1->LER |= LER3_EN;
			PWM3_EN;
			break;
		case 3:
			PWM1->MR4 = pwm_pclk * vmV / vmVMax;
			PWM1->LER |= LER4_EN;
			PWM4_EN;
			break;
		case 4:
			PWM1->MR5 = pwm_pclk * vmV / vmVMax;
			PWM1->LER |= LER5_EN;
			PWM5_EN;
			break;
		case 5:
			PWM1->MR6 = pwm_pclk * vmV / vmVMax;
			PWM1->LER |= LER6_EN;
			PWM6_EN;
			break;
		default:
			break;
	}
#endif	
}

/******************************************************************************
** Function name:		PWM_Set
**
** Descriptions:		PWM cycle setup
**
** parameters:			Channel number, PWM cycle, and offset
** Returned value:		None
** 
******************************************************************************/
void PWM_Set(uint32 cycle)
{
#if _PWM_EN	

//unsigned int offset = 100;			
  //if ( ChannelNum == 1 )
  {
	// PWM1->MR0 = cycle;		/* set PWM cycle */
	 PWM1->MR1 = pwm_pclk>>1;
	 PWM1->MR2 = pwm_pclk>>2;
	 PWM1->MR3 = pwm_pclk*2/3;
	 PWM1->MR4 = pwm_pclk*5/6;
	 PWM1->MR5 = pwm_pclk*1/3;
	 PWM1->MR6 = pwm_pclk*2/5;
/**/
	/* The LER will be cleared when the Match 0 takes place, in order to
	load and execute the new value of match registers, all the PWMLERs need to
	reloaded. all PWM latch enabled     */
	 PWM1->LER = LER0_EN | LER1_EN | LER2_EN | LER3_EN | LER4_EN | LER5_EN | LER6_EN;
  }
#endif	
  return;
}

/******************************************************************************
** Function name:		PWM_Start
**
** Descriptions:		Enable PWM by setting the PCR, PTCR registers
**
** parameters:			channel number
** Returned value:		None
** 
******************************************************************************/
void PWM_Start( uint32 channelNum )
{
#if _PWM_EN	
  if ( channelNum == 1 )
  {
	/* All single edge, all enable      */
	 PWM1->PCR = PWMENA1 | PWMENA2 | PWMENA3 | PWMENA4 | PWMENA5 | PWMENA6;
	 PWM1->TCR = TCR_CNT_EN | TCR_PWM_EN;	/* counter enable, PWM enable */
  }
				PWM1_A;
				PWM2_A;
				PWM3_A;
				PWM4_A;
				PWM5_A;
				PWM6_A;
#endif	
  return;
}

/******************************************************************************
** Function name:		PWM_Stop
**
** Descriptions:		Stop all PWM channels
**
** parameters:			channel number
** Returned value:		None
** 
******************************************************************************/
void PWM_Stop( uint32 channelNum )
{
#if _PWM_EN	
	if ( channelNum == 1 )
	{
		//	 PWM1->PCR = 0;
		//	 PWM1->TCR = 0x00;		/* Stop all PWMs */
		
		 PWM1->MR0 = gPwmCycle;		/* set PWM cycle */
		 PWM1->MR1 = gPwmCycle;
		 PWM1->MR2 = gPwmCycle;
		 PWM1->MR3 = gPwmCycle;
		 PWM1->MR4 = gPwmCycle;
		 PWM1->MR5 = gPwmCycle;
		 PWM1->MR6 = gPwmCycle;
	
	}
#endif	
	return;
}

//		PWM_ENA;

/****************************************************************************************
*
* 时间:  2015-05-24
* 名称：	Arm -- PWM -- AD Feedback
* 功能：	摇杆输出 转换为 PWM 输出，并根据AD反馈调整PWM输出
* 说明： 	摇杆输出：上--1~125, 中--0, 下--126~250
*
****************************************************************************************/
////  摇杆输出值 转换为 PWM 输出值
////////////////////////////////////////////////////  PWM 输出  ////////////////////////////////////////////////////////
void Pwm_EN_Chk(void)
{
#if (_PWM_1A == 0)	
	gPWMx[0].vmA = 0;
#endif
#if (_PWM_1B == 0)	
	gPWMx[1].vmA = 0;
#endif
	
#if (_PWM_2A == 0)	
	gPWMx[2].vmA = 0;
#endif
#if (_PWM_2B == 0)	
	gPWMx[3].vmA = 0;
#endif
	
#if (_PWM_3A == 0)	
	gPWMx[4].vmA = 0;
#endif
#if (_PWM_3B == 0)	
	gPWMx[5].vmA = 0;
#endif
	
#if (_PWM_4A == 0)	
	gPWMx[6].vmA = 0;
#endif
#if (_PWM_4B == 0)	
	gPWMx[7].vmA = 0;
#endif
	
#if (_PWM_5A == 0)	
	gPWMx[8].vmA = 0;
#endif
#if (_PWM_5B == 0)	
	gPWMx[9].vmA = 0;
#endif
	
#if (_PWM_6A == 0)	
	gPWMx[10].vmA = 0;
#endif
#if (_PWM_6B == 0)	
	gPWMx[11].vmA = 0;
#endif
	
	
}
void Output_Pwm(void)
{
	unsigned char vCh = 0;
	
#if _PWM_EN	
	
	Pwm_EN_Chk();
	
	vCh = 0;									////  PWM-1A
	if(gPWMx[vCh].vmA > 0)
	{
		PWM1_A;
		PWM_SetX_mV(vCh>>1, gPWMx[vCh].vmA, _I_VCC);
	}
	else
	{
	vCh = 1;									////  PWM-1A
		if(gPWMx[vCh].vmA > 0)
		{
			if(gPWMx[vCh].vmA != PWM.PWM_RcvI[vCh])      // 控制输入电流改变时
			{
				PWM.PWM_OutI[vCh] = gPWMx[vCh].vmA;
				PWM.PWM_RcvI[vCh] = gPWMx[vCh].vmA;
			}
			PWM1_B;
			PWM_SetX_mV(vCh>>1, gPWMx[vCh].vmA, _I_VCC);
		//	FeedBack_I_PWM(vCh);
			PWM_SetX_mV(vCh>>1, ((uint32)PWM.PWM_OutI[vCh]), _I_VCC);	
		}
		else
		{
			PWM_SetX_mV(vCh>>1, 0, _I_VCC);
			PWM1_OFF;
		}
	}
	vCh = 2;									////  PWM-2A
	if(gPWMx[vCh].vmA > 0)
	{
		PWM2_A;
		PWM_SetX_mV(vCh>>1, gPWMx[vCh].vmA, _I_VCC);
	}
	else
	{
		vCh = 3;								////  PWM-2B
		if(gPWMx[vCh].vmA > 0)
		{
			if(gPWMx[vCh].vmA != PWM.PWM_RcvI[vCh])      // 控制输入电流改变时
			{
				PWM.PWM_OutI[vCh] = gPWMx[vCh].vmA;
				PWM.PWM_RcvI[vCh] = gPWMx[vCh].vmA;
			}
			PWM2_B;
			PWM_SetX_mV(vCh>>1, gPWMx[vCh].vmA, _I_VCC);
//					FeedBack_I_PWM(vCh);
			PWM_SetX_mV(vCh>>1, ((uint32)PWM.PWM_OutI[vCh]), _I_VCC);	
		}
		else
		{
			PWM_SetX_mV(vCh>>1, 0, _I_VCC);
			PWM1_OFF;
		}
	}
	
	vCh = 4;									////  PWM-3A
	if(gPWMx[vCh].vmA > 0)
	{
		PWM3_A;
		PWM_SetX_mV(vCh>>1, gPWMx[vCh].vmA, _I_VCC);
	}
	else
	{
		vCh = 5;								////  PWM-3B
		if(gPWMx[vCh].vmA > 0)
		{
			PWM3_B;
			PWM_SetX_mV(vCh>>1, gPWMx[vCh].vmA, _I_VCC);
		}
		else
		{
			PWM_SetX_mV(vCh>>1, 0, _I_VCC);
			PWM3_OFF;
		}
	}
	vCh = 6;									////  PWM-4A
	if(gPWMx[vCh].vmA > 0)
	{
		PWM4_A;
		PWM_SetX_mV(vCh>>1, gPWMx[vCh].vmA, _I_VCC);
	}
	else
	{
		vCh = 7;								////  PWM-4B
		if(gPWMx[vCh].vmA > 0)
		{
			PWM4_B;
			PWM_SetX_mV(vCh>>1, gPWMx[vCh].vmA, _I_VCC);
		}
		else
		{
			PWM_SetX_mV(vCh>>1, 0, _I_VCC);
			PWM4_OFF;
		}
	}

	vCh = 8;									////  PWM-5A
	if(gPWMx[vCh].vmA > 0)
	{
		PWM5_A;
		PWM_SetX_mV(vCh>>1, gPWMx[vCh].vmA, _I_VCC);
	}
	else
	{
		vCh = 9;								////  PWM-5B
		if(gPWMx[vCh].vmA > 0)
		{
			PWM5_B;
			PWM_SetX_mV(vCh>>1, gPWMx[vCh].vmA, _I_VCC);
		}
		else
		{
			PWM_SetX_mV(vCh>>1, 0, _I_VCC);
			PWM5_OFF;
		}
	}
	vCh = 10;									////  PWM-6A
	if(gPWMx[vCh].vmA > 0)
	{
		PWM6_A;
		PWM_SetX_mV(vCh>>1, gPWMx[vCh].vmA, _I_VCC);
	}
	else
	{
		vCh = 11;								////  PWM-6B
		if(gPWMx[vCh].vmA > 0)
		{
			PWM6_B;
			PWM_SetX_mV(vCh>>1, gPWMx[vCh].vmA, _I_VCC);
		}
		else
		{
			PWM_SetX_mV(vCh>>1, 0, _I_VCC);
			PWM6_OFF;
		}
	}
#endif	
}



/*****************************************************************************
' Function      : PWM输出  电流反馈补偿
' Parameter     : 
' ReturnValue   :  
' Date          : 20200108
' Author        : LPQ
' Description   : 电流调整范围：0mA ~ PWM.PWM_TI[vCh]   mA 	
******************************************************************************/
void FeedBack_I_PWM(unsigned char v_Ch)
{
	unsigned char i;
	unsigned char Ch_I;
	unsigned short V_ad;
	float fv_ad;
	float vf1;
	float vf2;
	float vRx = 0;				// 计算出来的电阻 
	
	PWM.Adj_Err = 1.00;   		 // 误差调整系数
	PWM.PWM_Adj = 5;      		 // 调整区间 1 - 3 mA
	PWM.PWM_AdjStep = 4;     	 //调整步长

	
#if 1       // 调整电阻法
	
	
#else	  // 调整电流法	
	//delay_ms(5);	 
	V_ad = Read_AD(v_Ch);
	PWM.PWM_I[v_Ch] = (((((float)V_ad)*3*1000)/4096)*(1000/20/50))*PWM.Adj_Err;	  // 20倍   50毫欧	
	fv_ad = PWM.PWM_RcvI[v_Ch] -	PWM.PWM_I[v_Ch];
	if((PWM.PWM_I[v_Ch] > (PWM.PWM_RcvI[v_Ch] - PWM.PWM_Adj))&&(PWM.PWM_I[v_Ch] < (PWM.PWM_RcvI[v_Ch] + PWM.PWM_Adj)))
	{
		//PWM.PWM_Res_Change[v_Ch] = PWM.PWM_TI[v_Ch]*(PWM.PWM_Res[v_Ch] + _R_ADC)/PWM.PWM_I[v_Ch] - _R_ADC;   // 根据电压相等 I0*R0 = I1*R1   
		//PWM.PWM_TI[v_Ch] = (_V_VCC/(PWM.PWM_Res_Change[v_Ch]+_R_ADC));	
		vRx = _V_VCC / PWM.PWM_I[v_Ch] - _R_ADC;
		if(vRx > (_R_Valve[v_Ch]+0.3))
			_R_Valve[v_Ch] += 0.05;
		if(vRx < (_R_Valve[v_Ch]-0.5))
			_R_Valve[v_Ch] -= 0.05;
	}	
	else if(PWM.PWM_I[v_Ch] <= (PWM.PWM_RcvI[v_Ch] - PWM.PWM_Adj))
	{
		if(PWM.PWM_OutI[v_Ch] < (_I_VCCx(v_Ch) - PWM.PWM_Adj))    
		{
			if(fv_ad > 40)
				PWM.PWM_OutI[v_Ch] += fv_ad*0.1;
			else
			PWM.PWM_OutI[v_Ch] += PWM.PWM_AdjStep; 
		}

		//delay_ms(2);
	}
	else if(PWM.PWM_I[v_Ch] >= (PWM.PWM_RcvI[v_Ch] + PWM.PWM_Adj))
	{
		if(PWM.PWM_OutI[v_Ch] > (0 + PWM.PWM_Adj))    
		{
			if(fv_ad < -40)
				PWM.PWM_OutI[v_Ch] += fv_ad*0.1;
			else
			PWM.PWM_OutI[v_Ch] -= PWM.PWM_AdjStep; 
		}
		//delay_ms(2);	
	}

#endif		
}
/*****************************************************************************
' Function      : PWM数据初始化
' Parameter     : 
' ReturnValue   :  
' Date          : 20200110
' Author        : LPQ
' Description   : 	
******************************************************************************/
void Init_PwmData(void)
{
	unsigned char i;
	for(i=0;i<12;i++)
	{
		gPWMx[i].vmA = 10;
		PWM.PWM_RcvI[i] = 0;
		PWM.PWM_OutI[i] = 0; 
		PWM.PWM_Res[i] = _R_Valve[i];
		PWM.PWM_TI[i] = _V_VCC/(PWM.PWM_Res[i] + _R_ADC);
		PWM.PWM_TI_Change[i] = PWM.PWM_TI[i];
	}
}


void Pwm_Out_FB(void)
{
//	gPWMx[1].vmA = 600;
//	gPWMx[3].vmA = 300;
	Output_Pwm();
}

void PwmX_SetV(unsigned char vCh, unsigned short vmAV)
{
	if(vCh >= 12)
		return;
	if(vmAV > 2000)
		vmAV = 2000;
	gPWMx[vCh].vmA = vmAV;
}



void Get_PWM_Adc(void)
{
	static unsigned char j=0,k=0;
	
	GADC_Databuf = GADC_Databuf + ADC0ReadAverageU40(0);  //2é?ˉAI1-PWM1~AI1-PWM8μ??1￡? gADVal[0]~gADVa[7]±￡′?AI1-PWM1~AI1-PWM8μ??μ AI4-cpu
	k++;
	if(k>=PWM_time)  //2é?ˉ′?êy
	{
		k = 0;
		gADVal_pwm[j] = GADC_Databuf/PWM_time;
		GADC_Databuf = 0;
		
		if(j<7)
			j++;  //0-7
		else
			j = 0;
		
	//	DOx_CanADU40(j);
	}
}

/******************************************************************************
**                            End Of File
******************************************************************************/
