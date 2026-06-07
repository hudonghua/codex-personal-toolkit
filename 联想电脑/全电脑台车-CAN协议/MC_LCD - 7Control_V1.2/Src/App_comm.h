/********************************************************************
*
*  与 IO 口有关的函数，key, di, do 等
*  功能：
*
********************************************************************/

#ifndef ___APP_COMM_H_
#define ___APP_COMM_H_


#include "config.h"
#include "gpio.h"



extern unsigned int  gWLRcvs ;
extern unsigned int  gWLRcvOkN ;
extern unsigned char gWLDType;
extern unsigned int   gArmFX ;				// 摇杆的方向位
 extern unsigned char gWLRcvOk ;		// 接收到无线数据，且数据验证通过
extern unsigned int  gWLAckCnt;		// 当前无线反馈次数，收到CAN（有线）数据时清0

extern unsigned int  gCommOvDly ;
extern unsigned int  gUartSendDly;

extern unsigned char gWLDType ;


//////
typedef struct 
{
	// 遥控器发过来的数据	0x05
	unsigned char vArm[12];		// 臂架 + 模拟量
	unsigned char vYKCmd;			// 遥控器的指令，如：标定角度、重量、风速等
	unsigned char vYKDI1[2];	// 遥控器的按钮信息，两档 *16
	unsigned char vYKDI2[2];	// 遥控器的按钮信息，三档 * 8
	unsigned char vYKDI3[2];		// 遥控器的旋钮信息，16档 * 2
	unsigned char vYKJiTing;	// 遥控器的急停信息
	unsigned char vJKJiTing;	// 遥控器的急停信息
	unsigned char vLaBaFlg;	
	
	unsigned char vMCmd;			// 主控板状态、命令字   b7--急停，b6--电机启动，b5--回转销
	unsigned char vDICAN;
	unsigned char vRcvs[64];			// 从总线接收的数据，发送给遥控器
	
	unsigned char  vDJStart;		// 电机是否启动

	unsigned int   vDOCAN;		// CAN 发送过来的 DO 指令，1bit----1DO
}*pRunInfo, mRunInfo;

 
 extern mRunInfo gRunInfo;
uchar Uart0_WL_Rcv(void);
void  Uart0_WL_Send(void);

 
#define  _WL_SLEEP	{ GPIO0->FIOSET |= IO_MD0;  GPIO1->FIOSET |= IO_MD1;  }				// 无线休眠
#define  _WL_WAKEUP	{ GPIO0->FIOCLR |= IO_MD0;  GPIO1->FIOCLR |= IO_MD1;  }				// 无线唤醒

#define  _LCD_OFF_DLY (1 * 30)				// (3 * 60) 秒  持续无操作时，关LCD电源

extern unsigned char  gWLInfo[8];							// 无线参数（全部），用来读写
extern unsigned char  gWLCmd ;							// 无线命令
extern unsigned short  gWLParam ;						// 无线参数，新
extern unsigned char  gWLRWFlg ;

extern	 unsigned char stBKClose ;															    
extern unsigned char g500msFlg ;			


uint32_t wl_rd( void );
uint32_t wl_wr( void );
void wl_rd_rcv(void);
void WL_Set(void);
void wl_set_param(void);

void T0S_Prog(void);
void wl_lcd_power(void);
void AI_DI_NiuZi(void);

void wl_reset(void);



#endif  //___APP_COMM_H_
