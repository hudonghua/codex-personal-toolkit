/********************************************************************
*  
*  力矩有关的功能函数
*  功能：
*
********************************************************************/

#ifndef __APP_SYS_H_
#define __APP_SYS_H_


#include "config.h"




#define  _TX_OV_DLY		1500		// ms 通信中断
#define  _TXC_OV_DLY		1000		// ms CAN 通信中断


#define  _I_VCC					1000

////  ADC参数定义
#define  _V_ADC					3000			// mV, ADC采样电压
#define  _ADC_MAX				4095			// ADC 最大采样值
#define  _ADC_CMP_MAX  	_ADC_MAX+1




typedef struct
{
	short  vmA;				// 实时电流
	short  vmA1;			// 最终电流
	short  vmA0;			// 起始电流
	unsigned short  vmS;			// 爬坡时间
	unsigned char vQ;
} mPWMx, *pmPWMx;

#define  _PWM_N		12		// PWM Num


typedef struct 
{
	uchar   vHead[2];
	// 密码
	uchar   password[2][7];
	// 时间设置
	uchar   DTime[8];				// yy-mm-dd  hh-mm-ss  Week
	uchar  DTimeFlg;
	//
	ushort  vChkSum;					// 校验和
}*pBDParam2, mBDParam2;

typedef struct 
{
	uchar   vHead[2];
	ushort  vRsv;
	uint    vWorkTime;					// 累计工作时间，分钟
	ushort   vWorkTime2;					// 累计工作时间，分钟

	ushort  vChkSum;					// 校验和
}*pWorkTime, mWorkTime;


////  存储信息
typedef struct
{
	unsigned char  vChkSum;					// 校验
}*pSaveInfo, SaveInfo;


////  标定的参数
typedef struct 
{
	uchar   vHead[2];
	// 定义需要存储的参数
	ushort  armLenIn[3]; 
																    
 
	   					    
	 ushort  vRsv;						// 
	 ushort  vChkSum;						// 校验和
}*pBDParam, mBDParam;
extern mBDParam gBDParam[2];



/************************************ 变量 *************************************************/

extern mBDParam2 gBDParam2[2];			// 本地存储的标定信息	
extern mPWMx gPWMx[_PWM_N];

extern mWorkTime gWorkTime[2];
extern mWorkTime gWorkTimeRd[2];


extern unsigned int  gTimeFlg_1mS ;
extern unsigned int  gTimeFlg_10mS ;
extern unsigned int  gTimeFlg_1000mS ;

extern uchar   gSaveFlg ;
extern unsigned int gADVal[ADC_NUM];
extern unsigned int gADV_mV[ADC_NUM];


/************************************ 函数 *************************************************/
void ADC_BSL_Chk(void);

void MA_to_DAC(void);

void WTime_Cnt(void);
void Save_Info_Prog(void);
void Sys_Prog_While(void);

void AI_Data_Cnv(void);			//// 吸尘车 


#endif  //__APP_LIJU_H_
