/****************************************************************************
*
*	 用户计算 有关的函数
*
*
*****************************************************************************/

#include "APP_sys.h"
#include "LPC17xx.h"
#include "config.h"

extern void ADC_Bessel(uint16_t chn, uint16_t vADvalue);

mBDParam 	gBDParam[2];			// 本地存储的标定信息	
mBDParam2 gBDParam2[2];			// 本地存储的标定信息	

mWorkTime gWorkTime[2];
mWorkTime gWorkTimeRd[2];
mPWMx gPWMx[_PWM_N];
	
unsigned int gADVal_pwm[ADC_NUM];
unsigned int gADVal[ADC_NUM];
unsigned int gADV_mV[ADC_NUM];				// 每个通道的电压值 mV 
unsigned int gAIValx[ADC_NUM];				// 每个通道的 最终值（换算成电压、电流或电阻的值）   

unsigned int  gTimeFlg_1mS = 0;
unsigned int  gTimeFlg_10mS = 0;
unsigned int  gTimeFlg_1000mS = 0;

/****************************************************************************************/
uchar   gSaveFlg = 0;

unsigned char gADxChn[ADC_NUM] = {1, 9, 12, 5, 8, 0, 0, 0, 0, 0};
void ADC_BSL_Chk(void)
{
	unsigned char i = 0;
	
#if (ADC_BESSEL_EN )	
	ADC_Bessel(0, ADCRst[0]);
	ADC_Bessel(1, ADCRst[1]);
	ADC_Bessel(2, ADCRst[2]);
	gADVal[0] = ADCBsl[0];
	gADVal[1] = ADCBsl[1];
	gADVal[2] = ADCBsl[2];

#else	
	for(i=0; i<8; i++)
		gADVal[i] = ADCRst[gADxChn[i]];
	
#endif		
}

unsigned int  gMARcv = 0;									// CAN 接收到的电流值
unsigned int  gAOVal = 0;									// DAC 输出的值

void MA_to_DAC(void)
{
	float  fVal = 0;

	//// AO    AD694: 0~2.5v ---- 0~20mA, 0.5V----4mA;   AO----0~3V, 0.45V--153.5, 2.55V--870  -5~90?
	// gAOVal = fVal / (95 * xyMulti) * (870 - 153.5) + 153.5;	
/*	// 电流输出
	if(gMARcv > 20100)
		gMARcv = 20100;
	fVal = gMARcv;
	gAOVal = fVal / 20000 * 2.5 * 1025 / 3;
	*/

#if 0				//// 电流输出	
	// 电压输入值0~5V，输出电流0~20mA，再外部接250欧姆电阻转换为电压，   鑫通 的 CAN 转 电压
	if(gMARcv > 5000)
		gMARcv = 5000;
	fVal = gMARcv;
	gAOVal = fVal / 5000 * 2.5 * 1025 / 3;
//	gAOVal = gAOVal * 250/220;								// 外部用220欧电阻时，需提高输出电流

#else				//// 电压输出
	if(gMARcv > 5000)
		gMARcv = 5000;
	fVal = gMARcv;
//	gAOVal = fVal / 5000 * 1025 *5/3 + 100 + gMARcv/100;			// 外部有反向二极管，压降为0.3V，所以输出提高0.3V, 3V--1024, 则0.3V--102，取100的值
	gAOVal = fVal / 5000 * 1024 *5/3/2 ;			// 外部放大1倍，本地输出0~2.5V即可
	gAOVal *= 1.01;

	if(gAOVal > 5)
		gAOVal -= 3;
#endif	
}


void WTime_Cnt(void)
{
	static unsigned char  vWorkTimeCnt = 0;
	static unsigned char counter = 0;

	counter++;
	if((counter>60))	   // 分钟
	{
		counter = 0;

		gWorkTime[0].vWorkTime++;
		gWorkTime[1].vWorkTime = gWorkTime[0].vWorkTime;
		vWorkTimeCnt++;
		if(vWorkTimeCnt == 2)
		{
			vWorkTimeCnt = 0;
			Sys_Write_WT();
		}
	}

}
void Save_Info_Prog(void)
{
		// 保存标定信息
		if(gSaveFlg == 1)
		{
			Sys_Write_BD();
			gSaveFlg = 0;
		}
		// 保存标定信息	2
		if(gSaveFlg == 2)
		{
			gSaveFlg = 0;
			Sys_Write_BD();			  // 读标定信息
		}
		if(gSaveFlg == 4)
		{
			gSaveFlg = 0;
		}
}

////  模拟量转换，电流、电阻、电压 采集

////  吸尘车，AMP35.8/9/19/21/22/31 分别为 电流、电压、电压、电阻、电压、电阻 
void AI_Data_Cnv(void)			//// 吸尘车 
{
	unsigned char k = 0;			// 
	unsigned int  vR1 = 0;			// 电阻 阻值 
	
	k = 0;		// AMP35.8, 电流型 uA    
	gADV_mV[k] = gADVal[k] * 3000 / 4096 * (82 + 82) / 82;
	gAIValx[k] = gADV_mV[k] * 1000 / 220;
	
	k = 1;		// AMP35.9, 电压型      
	gADV_mV[k] = gADVal[k] * 3000 / 4096 * (82 + 33) / 33;
	gAIValx[k] = gADV_mV[k];
	
	k = 2;		// AMP35.19, 电压型       
	gADV_mV[k] = gADVal[k] * 3000 / 4096 * (82 + 33) / 33;
	gAIValx[k] = gADV_mV[k];
	
	k = 3;		// AMP35.21, 电阻型  阻值102      
	vR1 = 1000;
	gADV_mV[k] = gADVal[k] * 3000 / 4096 * (82 + 82) / 82;
	gAIValx[k] = gADV_mV[k] * vR1 / (4500 - gADV_mV[k]);
	
	k = 4;		// AMP35.22, 电压型       
	gADV_mV[k] = gADVal[k] * 3000 / 4096 * (82 + 33) / 33;
	gAIValx[k] = gADV_mV[k];
	
	k = 5;		// AMP35.31, 电阻型  阻值102      
	vR1 = 1000;
	gADV_mV[k] = gADVal[k] * 3000 / 4096 * (82 + 82) / 82;
	gAIValx[k] = gADV_mV[k] * vR1 / (4500 - gADV_mV[k]);

}

////  while 循环中执行的 系统后台 处理程序  需保留 
void Sys_Prog_While(void)
{
	unsigned char i = 0;
	
		if(gT0Flg )				// T0 , 1 ms
		{
			gTimeFlg_1mS += gT0Flg;
			WDT_Feed();					// 喂狗
			T0_mS_Prog();
			ADC_BSL_Chk();
			
			AI_Data_Cnv();
			
		//	MA_to_DAC();				
		//	DAC_SetV(gAOVal);		// AO 
 
			gT0Flg = 0;
		}
		if(gT010msFlg)				// 10 ms
		{
		
			
			gT010msFlg = 0;
			Can_Rcv_Dly();			// can 接收是否超时 的倒计时，用户不需处理，但需保留    LSG
			Key_Led_On();			// 按键灯  保留
		//	Output_Pwm(); 			// PWM 输出  保留

		}

		for(i=0; i<_TOD_N; i++)						// 	TODH, TODL
		{
			if(gTODH[i].vQ)
			{
				gTODH[i].vQ--;
			}
			if(gTODL[i].vQ)
			{
				gTODL[i].vQ--;
			}
		}

		if(gT0500mSFlg)
		{
			gT0500mSFlg = 0;
			_CPU_ON;
		}
		if(gT0SFlg)					// T0 , 1000 ms
		{
			gTimeFlg_1000mS++;
			gT0SFlg = 0;
			_CPU_OFF;
			Save_Info_Prog();
		}
		
		// 发送 CAN 数据，用户不需处理，但需保留    LSG 
		if(gCanSFlg)
		{
			Can_Send_Prog();
		}
		if( CAN1RxDone == TRUE )		 // 收到CAN数据
		{
			CAN1RxDone = FALSE;				 
			Can_Prog_Rcv(gCanRFlg);
		}
		if(	gUart0RcvFlg == 1)			 // 收到UART 数据， LCD 
		{
			gUart0RcvFlg = 0;
			gDGUSRtnFlg = Uart0_DGUS();

		}


		// 刷新显示 
		if(gLcdFresh || (gDataFresh>1))
		{
			gp_lcdtask();			  // 显示、按键 任务 
		}
		
}




