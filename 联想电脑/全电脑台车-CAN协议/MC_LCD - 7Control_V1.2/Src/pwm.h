/*****************************************************************************
 *   pwm.h:  Header file for NXP LPC17xx Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.5.25  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __PWM_H 
#define __PWM_H
#include "config.h"

#define  _PWM_EN		1						// PWM 开关，为1，使用PWM功能，为0，关闭PWM功能

#define  _PWM_1A		1
#define  _PWM_1B		1
#define  _PWM_2A		1
#define  _PWM_2B		1
#define  _PWM_3A		1
#define  _PWM_3B		1
#define  _PWM_4A		1
#define  _PWM_4B		1
#define  _PWM_5A		1
#define  _PWM_5B		1
#define  _PWM_6A		0
#define  _PWM_6B		0


#define PWM_CLK			(4000000UL/4)		// PWM 周期，	IRC_OSC = 4000000UL
#define PWM_CYCLE		125					// 电磁阀的频率 150~180
#define PWM_CYCLE2		100					// 电磁阀的频率 150~180
#define PWM_OFFSET		200

#define PWM_time		25 //10 					//  200hz
#define MR0_INT			(1 << 0	)
#define MR1_INT			(1 << 1	)
#define MR2_INT			(1 << 2	)
#define MR3_INT			(1 << 3	)
#define MR4_INT			(1 << 8	)
#define MR5_INT			(1 << 9	)
#define MR6_INT			(1 << 10)

#define TCR_CNT_EN		0x00000001
#define TCR_RESET		0x00000002
#define TCR_PWM_EN		0x00000008

#define PWMMR0I			(1 << 0	)
#define PWMMR0R			(1 << 1	)
#define PWMMR0S			(1 << 2	)
#define PWMMR1I			(1 << 3	)
#define PWMMR1R			(1 << 4	)
#define PWMMR1S			(1 << 5	)
#define PWMMR2I			(1 << 6	)
#define PWMMR2R			(1 << 7	)
#define PWMMR2S			(1 << 8	)
#define PWMMR3I			(1 << 9	)
#define PWMMR3R			(1 << 10 )
#define PWMMR3S			(1 << 11 )
#define PWMMR4I			(1 << 12 )
#define PWMMR4R			(1 << 13 )
#define PWMMR4S			(1 << 14 )
#define PWMMR5I			(1 << 15 )
#define PWMMR5R			(1 << 16 )
#define PWMMR5S			(1 << 17 )
#define PWMMR6I			(1 << 18 )
#define PWMMR6R			(1 << 19 )
#define PWMMR6S			(1 << 20 )

#define PWMSEL2			(1 << 2	)
#define PWMSEL3			(1 << 3	)
#define PWMSEL4			(1 << 4	)
#define PWMSEL5			(1 << 5	)
#define PWMSEL6			(1 << 6	)

#define LER0_EN			(1 << 0	)
#if(_PWM_1A || _PWM_1B)
	#define PWMENA1			(1 << 9	)
	#define LER1_EN			(1 << 1	)
#else
	#define PWMENA1			(1 << 0	)
	#define LER1_EN			(1 << 0	)
#endif
#if(_PWM_2A || _PWM_2B)
	#define PWMENA2			(1 << 10 )
	#define LER2_EN			(1 << 2	)
#else
	#define PWMENA2			(1 << 0 )
	#define LER2_EN			(1 << 0	)
#endif
#if(_PWM_3A || _PWM_3B)
	#define PWMENA3			(1 << 11 )
	#define LER3_EN			(1 << 3	)
#else
	#define PWMENA3			(1 << 0 )
	#define LER3_EN			(1 << 0	)
#endif
#if(_PWM_4A || _PWM_4B)
	#define PWMENA4			(1 << 12 )
	#define LER4_EN			(1 << 4	)
#else
	#define PWMENA4			(1 << 0 )
	#define LER4_EN			(1 << 0	)
#endif
#if(_PWM_5A || _PWM_5B)
	#define PWMENA5			(1 << 13 )
	#define LER5_EN			(1 << 5	)
#else
	#define PWMENA5			(1 << 0 )
	#define LER5_EN			(1 << 0	)
#endif
#if(_PWM_6A || _PWM_6B)
	#define PWMENA6			(1 << 14 )
	#define LER6_EN			(1 << 6	)
#else
	#define PWMENA6			(1 << 0 )
	#define LER6_EN			(1 << 0	)
#endif


#if 1

//// PWMx Set cycle and Enable Output
#define PWM1_SET(cycle)	{ PWM1->MR1 = cycle; PWM1->LER |= LER1_EN; }
#define PWM2_SET(cycle)	{ PWM1->MR2 = cycle; PWM1->LER |= LER2_EN; }
#define PWM3_SET(cycle)	{ PWM1->MR3 = cycle; PWM1->LER |= LER3_EN; }
#define PWM4_SET(cycle)	{ PWM1->MR4 = cycle; PWM1->LER |= LER4_EN; }
#define PWM5_SET(cycle)	{ PWM1->MR5 = cycle; PWM1->LER |= LER5_EN; }
#define PWM6_SET(cycle)	{ PWM1->MR6 = cycle; PWM1->LER |= LER6_EN; }

#define PWM1_EN		PWM1->PCR |= PWMENA1;
#define PWM2_EN		PWM1->PCR |= PWMENA2;
#define PWM3_EN		PWM1->PCR |= PWMENA3;
#define PWM4_EN		PWM1->PCR |= PWMENA4;
#define PWM5_EN		PWM1->PCR |= PWMENA5;
#define PWM6_EN		PWM1->PCR |= PWMENA6;

#define PWM1_DIS		PWM1->PCR &= ~PWMENA1;
#define PWM2_DIS		PWM1->PCR &= ~PWMENA2;
#define PWM3_DIS		PWM1->PCR &= ~PWMENA3;
#define PWM4_DIS		PWM1->PCR &= ~PWMENA4;
#define PWM5_DIS		PWM1->PCR &= ~PWMENA5;
#define PWM6_DIS		PWM1->PCR &= ~PWMENA6;

//// PWMx Output disable, GPIO disable
#define PWM1_OFF	;//{ PWM1->PCR &= ~PWMENA1; PINCON->PINSEL4 &= ~(0x03<<0); PINCON->PINSEL3 &= ~(0x03<<4);  }
#define PWM2_OFF	;//{ PWM1->PCR &= ~PWMENA2; PINCON->PINSEL4 &= ~(0x03<<2); PINCON->PINSEL3 &= ~(0x03<<8);  }
#define PWM3_OFF	;//{ PWM1->PCR &= ~PWMENA3; PINCON->PINSEL4 &= ~(0x03<<4); PINCON->PINSEL3 &= ~(0x03<<10);  }
#define PWM4_OFF	;//{ PWM1->PCR &= ~PWMENA4; PINCON->PINSEL4 &= ~(0x03<<6); PINCON->PINSEL3 &= ~(0x03<<14);  }
#define PWM5_OFF	;//{ PWM1->PCR &= ~PWMENA5; PINCON->PINSEL4 &= ~(0x03<<8); PINCON->PINSEL3 &= ~(0x03<<16);  }
#define PWM6_OFF	;//{ PWM1->PCR &= ~PWMENA6; PINCON->PINSEL4 &= ~(0x03<<10); PINCON->PINSEL3 &= ~(0x03<<20); }
//// PWMx A/B Pin enable
//#define PWM1_A	{ PINCON->PINSEL3 &= ~(0x03<<4); GPIO1->FIODIR |= (1<<18); GPIO1->FIOCLR |= (1<<18); PINCON->PINSEL4 |= (0x01<<0); }	  // P2.0
//#define PWM1_B	{ PINCON->PINSEL4 &= ~(0x03<<0); GPIO2->FIODIR |= (1<<0); GPIO2->FIOCLR |= (1<<0); PINCON->PINSEL3 |= (0x02<<4); }	  // P1.18
//#define PWM2_A	{ PINCON->PINSEL3 &= ~(0x03<<8); GPIO1->FIODIR |= (1<<20); GPIO1->FIOCLR |= (1<<20);  PINCON->PINSEL4 |= (0x01<<2); }	  // P2.1
//#define PWM2_B	{ PINCON->PINSEL4 &= ~(0x03<<2); GPIO2->FIODIR |= (1<<1); GPIO2->FIOCLR |= (1<<1); PINCON->PINSEL3 |= (0x02<<8); }	  // P1.20
#if _PWM_1A
	#define PWM1_A	{ PINCON->PINSEL3 &= ~(0x03<<4);  PINCON->PINSEL4 |= (0x01<<0); }	  // P2.0
#else
	#define PWM1_A	;
#endif	
#if _PWM_1B
	#define PWM1_B	{ PINCON->PINSEL4 &= ~(0x03<<0);  PINCON->PINSEL3 |= (0x02<<4); }	  // P1.18
#else
	#define PWM1_B	;
#endif	
	
#if _PWM_2A
	#define PWM2_A	{ PINCON->PINSEL3 &= ~(0x03<<8);  PINCON->PINSEL4 |= (0x01<<2); }	  // P2.1
#else
	#define PWM2_A	;
#endif	
#if _PWM_2B
	#define PWM2_B	{ PINCON->PINSEL4 &= ~(0x03<<2);  PINCON->PINSEL3 |= (0x02<<8); }	  // P1.20
#else
	#define PWM2_B	;
#endif	

#if _PWM_3A
	#define PWM3_A	{ PINCON->PINSEL3 &= ~(0x03<<10);  PINCON->PINSEL4 |= (0x01<<4); }	  // P2.2
#else
	#define PWM3_A	;
#endif	
#if _PWM_3B
	#define PWM3_B	{ PINCON->PINSEL4 &= ~(0x03<<4);  PINCON->PINSEL3 |= (0x02<<10); }	  // P1.21
#else
	#define PWM3_B	;
#endif	

#if _PWM_4A
	#define PWM4_A	{ PINCON->PINSEL3 &= ~(0x03<<14);  PINCON->PINSEL4 |= (0x01<<6); }	  // P2.3
#else
	#define PWM4_A	;
#endif	
#if _PWM_4B
	#define PWM4_B	{ PINCON->PINSEL4 &= ~(0x03<<6);  PINCON->PINSEL3 |= (0x02<<14); }	  // P1.23
#else
	#define PWM4_B	;
#endif	

#if _PWM_5A
	#define PWM5_A	{ PINCON->PINSEL3 &= ~(0x03<<16);  PINCON->PINSEL4 |= (0x01<<8); }	  // P2.4
#else
	#define PWM5_A	;
#endif	
#if _PWM_5B
	#define PWM5_B	{ PINCON->PINSEL4 &= ~(0x03<<8);  PINCON->PINSEL3 |= (0x02<<16); }	  // P1.24
#else
	#define PWM5_B	;
#endif	

#if _PWM_6A
	#define PWM6_A	{ PINCON->PINSEL3 &= ~(0x03<<20);  PINCON->PINSEL4 |= (0x01<<10); }	  // P2.5
#else
	#define PWM6_A	;
#endif	
#if _PWM_6B
	#define PWM6_B	{ PINCON->PINSEL4 &= ~(0x03<<10); PINCON->PINSEL3 |= (0x02<<20); }	  // P1.26
#else
	#define PWM6_B	;
#endif	

#endif


#define PWM_ENA		PWM1->TCR = TCR_CNT_EN | TCR_PWM_EN;	/* counter enable, PWM enable */



#define  _V_VCC			24000		//12000			// mV, 系统电压
#define  _R_DCF			22			//4.72			// Ω,  电磁阀阻值
#define  _R_ADC			0.05				// Ω,  采样电阻，串入
#define  _R_MULTI		20					// 采样电路的放大倍数
#define  _FB_U_I		(_R_ADC * _R_MULTI)	// 反馈电路上，电压与电流的比例关系，0.05Ω*20=1
#define  _I_VCCx(aCh)	(_V_VCC/(_R_Valve[aCh]+_R_ADC))     //(_V_VCC/(_R_DCF+_R_ADC))

typedef struct 
{
	// 定义PWM数据
	unsigned int  PwmCyc;      		//接收到的PWM频率  100-1000HZ	
#if 0
	unsigned char PWM_Adj;     		//调整区间 1 - 3 mA	
	unsigned int  PWM_I[12];    		//AD采集回来的电流
	unsigned int  PWM_RcvI[12];    	//接收到的电流  0-1000ma
	unsigned char PWM_Duty[12];		//调整的占空比 0 - 100
	unsigned int  PWM_OutI[12];      //调整后输出电流
	unsigned int  PWM_TI[12];    	//计算出的总电流，每个电磁阀不一样，电流也不一样
	unsigned int  PWM_TI_Change[12]; //计算出变化后的总电流，每个电磁阀不一样，电流也不一样
	unsigned char PWM_Res[12];		//电阻阻值
	unsigned char PWM_Res_Change[12];//变化后电阻阻值
#else
	float  PWM_Adj;     		//调整区间 1 - 3 mA
	float  PWM_AdjStep;     	//调整步长
	float  PWM_I[12];    		//AD采集回来的电流
	float  PWM_RcvI[12];    	//接收到的电流  0-1000ma
	float  PWM_OutI[12];      	//调整后输出电流
	float  PWM_TI[12];    		//计算出的总电流，每个电磁阀不一样，电流也不一样
	float  PWM_TI_Change[12]; 	//计算出变化后的总电流，每个电磁阀不一样，电流也不一样
	float PWM_Res[12];			//电阻阻值
	float PWM_Res_Change[12];	//变化后电阻阻值	
	float PWM_Res_Change1[12];	//变化后电阻阻值
#endif
    float Adj_Err;                  //误差调整系数	
}*pPWM, mPWM;

extern mPWM PWM;

	
extern uint32 PWM_Init( uint32 channelNum, uint32 cycle );
//extern void PWM_Set( uint32 channelNum, uint32 cycle, uint32 offset );
extern void PWM_Set(uint32 cycle); /* PWM cycle setup  */
extern void PWM_Start( uint32 channelNum );  /* All single edge, all enable */
extern void PWM_Stop( uint32 channelNum );   /* Stop all PWM channels		  */
extern void PWM_SetX(uint32 vCh, uint32 vPre);
extern void PWM_SetX_mV(uint32 vCh, uint32 vmV, uint32 vmVMax);
extern void PWM_SetXAB(uint32 vCh, uint32 vPre);

void Output_Pwm(void);
void Pwm_Out_FB(void);


extern void FeedBack_I_PWM(unsigned char vCh);
extern void Init_PwmData(void);

extern void PwmX_SetV(unsigned char vCh, unsigned short vmAV);
extern void PWM_SetCyc(unsigned int vCyc);



extern unsigned int  gPwmCycle;
 void Get_PWM_Adc(void);


#endif /* end __PWM_H */
/****************************************************************************
**                            End Of File
****************************************************************************/
