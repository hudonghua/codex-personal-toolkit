/********************************************************************
*
*  与 IO 口有关的函数，key, di, do 等
*  功能：
*
********************************************************************/

#ifndef ___APP_IO_H_
#define ___APP_IO_H_


#include "config.h"
#include "gpio.h"


#define  _BitV(	v, b)		((v & (1<<b)) == (1<<b)? 1:0)
							 

#define  _DI_MAXNUM			24		// DI  输入的最大个数
#define  _DO_MAXNUM			20		// DO输出最大个数
#define  _KEY_MAXNUM		12		
#define  _LED_MAXNUM		12		

#define  _EOT						0xaa	// End of Table
#define  _CNT						0x55	// Continue 
#define  _CAN_T					0x20	// 输出到CAN 

#define  _KEY_SCAN_CNT		10
#define  _KEY_SCAN_CNT2		5
#define  _DI_SCAN_CNT		10		// 去抖的计数次数

////	KEY (8)
#define  _KEY1					17		//P0.
#define  _KEY2					21		//P0.
#define  _KEY3					18		//P0.
#define  _KEY4					22		//P0.
#define  _KEY5					19		//P0.
#define  _KEY6					11		//P2.
#define  _KEY7					12		//P2.			
#define  _KEY8					20		//P0.

////   LED 8
#define  _LED1					7		//P0.
#define  _LED2					9		//P2.
#define  _LED3					6		//P0.
#define  _LED4					6		//P2.
#define  _LED5					28		//P4.
#define  _LED6					9		//P0.
#define  _LED7					29		//P4.
#define  _LED8					8		//P0.

#define  _LED9_CPU				22		// P1.
#define  _CPU_ON				GPIO1->FIOCLR |= (0x01 << _LED9_CPU);
#define  _CPU_OFF				GPIO1->FIOSET |= (0x01 << _LED9_CPU);

////	DI (6, 1~3H 4~6L)
//#define  _DI_1					11		//P0
#define  _DI_0					0		//不扫描的端口 
#define  _DI_1					19		//P1.
#define  _DI_2					10		//P0.
#define  _DI_3					25		//P3.
#define  _DI_4					29		//P1
#define  _DI_5					28		//P1
#define  _DI_6					27		//P1
#define  _DI_7					26		//P3.
#define  _DI_8					8		//P1.
#define  _DI_9					9		//P1.
#define  _DI_10					15		//P1.
#define  _DI_11					14		//P1.
#define  _DI_12					29		//P0.
#define  _DI_13					30		//P0.

#define  _DI_SJ					25		// P0.


////	DO/PWM  (4)
#define  _DO_1			2			// P2.   PWM3A 
#define  _DO_2			3			// P2.   PWM4A 
#define  _DO_3			23			// P1.   PWM4B 
#define  _DO_4			4			// P2.   PWM5A 
#define  _DO_5			24			// P1.   PWM5B 
#define  _DO_6			21			// P1.   PWM3B 

#define  _DO_7			18			// P1.   PWM1B 
#define  _DO_8			0			// P2.   PWM1A 
#define  _DO_9			20			// P1.   PWM2B 
#define  _DO_10			1			// P2.   PWM2A 
#define  _DO_11			26			// P1.   PWM6B 
#define  _DO_12			5			// P2.   PWM6A 
/*
#define  _DO_13			5			// P2.5
#define  _DO_14			6			// P2.6
#define  _DO_15			7			// P2.7
#define  _DO_16			8			// P2.8
*/

#define  _DO_AA_SEL		30			// P1.30 电流选择
#define  _DO_AA_H		GPIO1->FIOSET |= (0x01 << _DO_AA_SEL);
#define  _DO_AA_L		GPIO1->FIOCLR |= (0x01 << _DO_AA_SEL);


///////////////////////////////     DGUS LCD 帧头定义		/////////////////////////////
#define  DGUS_H1				0x5A
#define  DGUS_H2				0xA5


#define  _TON_N		100		// TON Num
#define  _TOF_N		10		// TOF Num
#define  _TOD_N		25		// TOD Num
////  TON(in, pt)
typedef struct
{
	unsigned int  vPT;
	unsigned char vIn;
	unsigned char vIn0;
	unsigned char vQ;
} mTON, *pmTON;
////  TOF(in, pt)
typedef struct
{
	unsigned int  vPT;
	unsigned int  vPT0;
	unsigned char vIn;
	unsigned char vQ;
} mTOF, *pmTOF;
////  TOD(in, DI)
typedef struct
{
	unsigned char  vDI;
	unsigned char  vDI0;
	unsigned int  vInc;
	unsigned char vQ;
} mTOD, *pmTOD;

////  TOD_xor(in, DI)
typedef struct
{
	unsigned char  vDI;
	unsigned char  vDI0;
	unsigned int  vInc;
	unsigned char vQ;
} mTODXOR, *pmTODXOR;


extern mTON gTON[_TON_N];
extern mTOF gTOF[_TOF_N];
extern mTOD gTODH[_TOD_N];
extern mTOD gTODL[_TOD_N];
extern mTODXOR gTODXOR[_TOD_N];

extern unsigned char gDIScanFlg[_DI_MAXNUM];
extern unsigned char gDIScanTable[_DI_MAXNUM][2];
extern unsigned int  gDIBitV ;			// 用位定义的DI

extern  unsigned int  gKeyValue;
extern  unsigned int  gKeyValue0;

extern unsigned char  gDOBitV[8] ;			// 用位定义的DO输出记录，1bit -- 1路DO
extern unsigned char  gDOPNum ;
extern unsigned int  gPIVal ;				// PI
extern unsigned int  gPIValAdd ;				// PI TEMP
 
extern unsigned  int  gPIVal1  ;	
extern unsigned  int  gPIVal2  ;	
extern unsigned  int  gPIVal3  ;	
extern unsigned  int  gPIVal4  ;	
extern unsigned  int  gPIValAdd1  ;
extern unsigned  int  gPIValAdd2  ;
extern unsigned  int  gPIValAdd3  ;
extern unsigned  int  gPIValAdd4  ;
extern unsigned char gDGUSRtnFlg;	// DGUS屏反馈标志


extern unsigned char  gXNLeft ;
extern unsigned char  gXNRight ;
extern unsigned int  gKeyValueXY;



unsigned long DStrToInt(unsigned char* vInBuf);
unsigned long HStrToInt(unsigned char* vInBuf);
void HBCDtoHStr(unsigned char* vInBuf, unsigned char* vOutBuf); 

char TODx_H(unsigned char vCh, unsigned char vDI, unsigned char vCyc);
char TODx_L(unsigned char vCh, unsigned char vDI, unsigned char vCyc);
char TOFx(unsigned char vCh, unsigned char in, unsigned int pt);
char TONx(unsigned char vCh, unsigned char in, unsigned int pt);

void IO_Init(void);
void Key_Scan(void);
void DI_Scan(void) ;
void DO_OutPut(void);
uchar Uart0_WL_Rcv(void);
void  Uart0_WL_Send(void);
void DOx_Set(unsigned char vX, unsigned char vH_L);
void DOx_IntSet(unsigned char vX, unsigned char vH_L);
void DOxx_Set_All(unsigned int vXX);
void Key_Led_On(void);

uchar Uart0_DGUS(void);


#endif  //___APP_IO_H_
