/*****************************************************************************
 *   main.c:  main file for Panel(SanHe) with NXP LPC17xx Family Microprocessors
 *
 *   Copyright(C) 2012, HuNan XiangRui Intelligence
 *   All rights reserved.
 *
 *   History
 *   2013.08.17  ver 1.00    Prelimnary version, first Release
 *
 *    开关面板(SanHe)，Key/Led/DI/DO/CAN
 *
******************************************************************************/
#include <lpc17xx.h>
#include "config.h"
#include "can.h"
#include "timer.h"

#include "app_comm.h"
#include "Arm200A_Store.h"

	
	
void delay_ms(unsigned int vDel)
{
	unsigned int i;
	while(vDel--)
	{
		for(i=0; i<19200; i++);
	}
}


void ValInit(void)			//初始化全局变量
{
	unsigned char i=0;
	
	for(i=0; i<8; i++)
	{
		gPWMx[i].vmA1 = 0;
		gPWMx[i].vmA0 = 0;
		gPWMx[i].vmA = 0;
	}
	
 	gPwmCycle = PWM_CYCLE; 
	gCanCyc = gSysInfo[0].vCycle * 10;
	//取废数据
//	AD_Val_Init();
}

/*****************************************************************************
** Function name:		main
**
** Descriptions:		main routine for CAN module test
**
** parameters:			None
** Returned value:		int
** 
*****************************************************************************/
int main( void )
{
	unsigned char i=0;
	static unsigned char stX10ms = 0;
	
	SystemInit();

	delay_ms(10);
	Sys_Param_Init(ID_DEFAULT, ID2_DEFAULT);
	IO_Init();
			WDT_Feed();					// 喂狗
  
	BD_Patam_Init();
	PWM_Init(1, 100);
	PWM_Start(1);
	
			WDT_Feed();					// 喂狗
	I2CInit(I2CMASTER);
	Sys_Read_Info();		  // 读系统信息
	ValInit();
	CAN_Init( gSysInfo[0].vBps);
	CAN2_Init( gSysInfo[0].vBps2);
	Sys_Read_BD();

//	 	Sys_Read_PwmBD();
	init_timer( ); // 10ms	   k
	UARTInit(0, 115200);					// 	串口0 初始化  LCD 
	UARTInit(1, 115200);					// 	串口1 初始化  无线模块 
	I2CInit(I2CMASTER);
#if 1
//	CAN_Init( gSysInfo[0].vBps);
	Can1_RcvID_Cfg();										//// 配置接收的CAN1 ID，需用户修改   LSG 
	Can2_RcvID_Cfg();										//// 配置接收的CAN2 ID，需用户修改   LSG 
	delay_ms(20);
			WDT_Feed();					// 喂狗
#endif
	ADCInit( ADC_CLK );
	ADC_StartCnv();
	//DAC_Init();
	enable_timer(0);
	delay_ms(20);
	gID = ID_DEFAULT;

	LCD_GO_Page(0);
	for(i=0; i<100; i++)
	{	
		delay_ms(30);
			WDT_Feed();					// 喂狗
	}

	gp_lcdtask =  Disp_main;		//;  Disp_main  页面显示函数  
	gLcdFresh = 3;
	   ReadRetain_() ;
	ValInit();
	/*******************************************  MC_LCD 模板说明  ********************************************
	DI,DO 的定义，从灰色、蓝色、黑色 三个 AMP23 头子上顺序排列， DI0~21， DO0~34
	DI: 输入值在 gDIBitV 变量中，1bit 对应 1DI，用 _BitV(gDIBitV, x) 取对应的DI值，其中，x为0~31，对应gDIBitV的b0~b31
	DO: 用 DOx_Set(x, H_L) 函数输出高或低，其中，x为0~34, 对应DOx，H_L为 1或0，1为输出高，0为输出低，输出值在gDOBitV[8]中
	CAN：接收，用户需在 Canx_RcvID_Cfg()函数中配置接收的ID，包括接收超时的时间(ms)，接收缓存 ID_RCV_NUM = 32 个帧
				在需要某帧 CAN 数据的时候调用 CANx_Get_Data(ID) 函数，数据在 CANx_Buf 中，函数返回值0xFF表示超时，0xEE表示ID不存在
				CANx_Get_Data_Buf(unsigned int vID, unsigned char* vBuf)
		发送：用户只需调整好发送周期，调用 CAN2_SendX(ID, SBuf) 函数即可，发送缓存 ID_SEND_NUM = 24 个帧
	PWM：在 PWM.h 中有个使能开关的宏定义 _PWM_EN, 为 1 使用PWM，为 0 关闭PWM
		 gPWMx[vCh].vmA 为PWM输出的电流值（mA），vCh=0,2,4,6,8,10 对应PWM1/2/3/4/5/6A, vCh=1,3,5,7,9,11 对应PWM1/2/3/4/5/6B

	ADC: gADV_mV[6] 里为6个模拟量端口的采样值转换成的 电压值，单位 mV
		 gAIValx [6] 里为6个模拟量端口的采样值转换成的 电压/电流/电阻值，单位 mV/uA/欧姆

	PI:  DI1可做PI输入  gPIVal 每秒的脉冲数
	AO:  电压输出，变量为 gMARcv， 0~5000 mV 
	TON, TOF, TODH, TODL 已加入
	定时：主循环中，提供了 1ms, 10ms 1000ms 三个时间标记，用户可根据需要把不同的处理函数放在不同的时间段中，比如CAN发送
	***************************N******************************************************************************/
	
	//gMARcv = 2000;
	while ( 1 )
	{
		
 
		if(gTimeFlg_10mS)				// 10 mS
		{
		
		 
			MyLogic_10ms() ;
		//	 PWMx_OutPut() ;

			gTimeFlg_10mS = 0;
		}
		if(gTimeFlg_1000mS)				// 1000 mS
		{
			gTimeFlg_1000mS = 0;
		//	Uart1_WL_Send();			// 无线发送 测试 
		}
		
		if(	gUart1RcvFlg == 1)			 // 收到UART 数据， 遥控器的无线数据，需保留 ****
		{
			gUart1RcvFlg = 0;
			//_LED_WRX_ON
			if(gCanOvDly == 0)				// CAN 优先
			{
				Uart1_WL_Rcv();
				if(gWLRcvOk >= 1)				 // 只有收到遥控器的正确数据，才回发数据
				{
				gWLRcvs++;
				//	_LED_WTX_ON;
					gWLRcvOk = 0;
					Uart1_WL_Send();				// 发送数据给 遥控器
				//	_LED_WTX_OFF;
				}	
			}
			else
				delay_ms(1);						// 20180509  
		//	_LED_WRX_OFF;
		}
		
		// 保存标定信息
		if(gLCDPage.vSave == 1 )
		{		    
				 WDT_Feed();
				 SaveRetain_();
				 WDT_Feed();
				 Arm200A_StoreSave();	
				 WDT_Feed();
				Sys_Write_BD();	 
				 WDT_Feed();
					gLCDPage.vSave = 0;
				  
				 
		}
 
		
		////////////////////////////////////////  保留  ///////////////////////////////////////////////
		Can_ask_rx();					// CAN1 异常诊断
		Sys_Prog_While();				// 系统后台处理的函数，需保留 
	}
}



