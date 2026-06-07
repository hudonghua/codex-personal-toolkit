/*******************************************************************************
*   app.h
Platform: AT90CAN32
Project : 倾角传感器
Clock F : 内部1MHz，外部12MHz
Software: AVR STUDIO 4.18
Author  : LSG
Version : 12.06.25
Updata  : 
comments:


*******************************************************************************/
#ifndef  __appopen_h__
#define  __appopen_h__

#include "config.h"
//#include "can.h"



#define  ID_RCV_NUM				32			// 接收ID 的最大数
#define  ID_SEND_NUM			24			// 接收ID 的最大数

#define  ID_SYS_DBG				0x7f0		// 调试用ID, 0X7F0~0X7FF

#define  ID_DEFAULT				0x6  		// 控制器的 CAN ID
#define  ID2_DEFAULT			0x18  		// 控制器的 CAN ID


//#define  ID_DEFAULT				0x3f		// id
#define  ID_SEND_D1				0x180
#define  ID_SEND_D2				0x280
#define  ID_SEND_D3				0x380
#define  ID_SEND_D4				0x480
#define  ID_RCV_D1				0x180
#define  ID_RCV_D2				0x280
#define  ID_RCV_D3				0x380
#define  ID_RCV_D4				0x480

#define  ID_RCV_M1				0x7e8		// 接收主站发过来的ID，配置功能
#define  ID_ACK_M1				0x7e6
#define  ID_RCV_M2				0x600
#define  ID_ACK_M2				0x580
#define  ID_RCV_BC 				0x700		// 
#define  ID_SEND_READY			0x700		// 本机准备好


#define  CANOPEN_MOD_INIT		0x01		// 初始化完成
//#define  CANOPEN_MOD_
#define  CANOPEN_MOD_CFG		0x10		// 进入配置模式
#define  CANOPEN_MOD_REBOOT		0x11		// 进入操作模式
#define  CANOPEN_MOD_SETID		0x10		// 配置ID
#define  CANOPEN_MOD_SETID_OK	0x11		// 配置ID
#define  CANOPEN_MOD_SETBPS		0x20		// 配置BPS
#define  CANOPEN_MOD_SETBPS_OK	0x21		// 配置BPS
#define  CANOPEN_MOD_SETCYCL	0x30		// 配置CYCLE
#define  CANOPEN_MOD_SETCYCL_OK	0x31		// 配置CYCLE
#define  CANOPEN_MOD_SETSERI	0x40		// 配置serial
#define  CANOPEN_MOD_SETSERI_OK	0x41		// 配置serial
#define  CANOPEN_MOD_SETXY		0x40		// X Y  轴校准
#define  CANOPEN_MOD_SETXY_OK	0x41		// X Y  轴校准
#define  CANOPEN_MOD_SETTEMP	0x40		// 温度
#define  CANOPEN_MOD_SETTEMP_OK	0x41		// 温度
#define  CANOPEN_MOD_SAVE		0xA0		// 保存配置
#define  CANOPEN_MOD_SAVE_OK	0xA1		// 保存配置


extern unsigned char gCanOpenFlag;
extern unsigned int gSysStatus;
extern unsigned int gCanOvDly;


extern unsigned char CAN1_RBuf[8];  //MOb0缓存，1C1F3E02
extern unsigned char CAN2_RBuf[8];  //MOb0缓存，1C1F3E02

extern unsigned char CAN_RBuf0[8];  //MOb0缓存，1C1F3E02
extern unsigned char CAN_RBuf1[8];  //MOb1缓存，1C1F3E03
extern unsigned char CAN_RBuf2[8];  //MOb2缓存，1C1F3E23
extern unsigned char CAN_RBuf3[8];  //MOb2缓存，1C1F3E23
extern unsigned char CAN_SBuf0[8];  //MOb3缓存，1C1F3E0x
extern unsigned char CAN_SBuf1[8];  //MOb4缓存，保存外呼按钮灯信息
extern unsigned char CAN_SBuf2[8];  //MOb4缓存，保存外呼按钮灯信息
extern unsigned char CAN_SBuf3[8];	//MOb4缓存，保存外呼按钮灯信息
extern unsigned char CAN_SBuf4[8];	//MOb4缓存，保存外呼按钮灯信息
extern unsigned char CAN1_RBuf_318[8];	//MOb4缓存，保存外呼按钮灯信息
extern unsigned char CAN1_RBuf_319[8];	//MOb4缓存，保存外呼按钮灯信息
extern unsigned char CAN2_RBuf_318[8];	//MOb4缓存，保存外呼按钮灯信息
extern unsigned char CAN2_RBuf_319[8];	//MOb4缓存，保存外呼按钮灯信息	  
extern unsigned char CAN1_RBuf_418[8];	//MOb4缓存，保存外呼按钮灯信息
extern unsigned char CAN1_RBuf_419[8];	//MOb4缓存，保存外呼按钮灯信息
extern unsigned char CAN_RBuf_byte[8];	//MOb4缓存，保存外呼按钮灯信息
extern unsigned char CAN_RBuf_bit[8];	//MOb4缓存，保存外呼按钮灯信息
																				   
extern unsigned long gRcvCanID[2][ID_RCV_NUM];
extern unsigned long gRcvCan2ID[2][ID_RCV_NUM];
extern CAN_MSG MsgBuf_RX1[ID_RCV_NUM], MsgBuf_RX2[ID_RCV_NUM];
extern volatile uint32_t CAN1RxDone, CAN2RxDone;

extern unsigned char  gCanOutTst ;
extern unsigned int   gCanDOTstV ;
extern unsigned int  gCanRFlg;


//void Sys_Save_Info(unsigned char vSite);
//void Sys_Read_Info(void);
void Sys_Read_ID(void);
unsigned char CanOpen_Prog(unsigned char vFlag);
unsigned char CanOpen_Init(unsigned char vID);
unsigned char Can1_RcvID_Cfg(void);
unsigned char Can2_RcvID_Cfg(void);

void CAN_Send(unsigned char vChn, unsigned long vID, unsigned char* vpData);
void CAN_SendLen(unsigned char vChn, unsigned long vID, unsigned char* vpData, char vLen);
void CAN_SendX(unsigned long vID, unsigned char* vpData);
void CAN_SendXLen(unsigned long vID, unsigned char* vpData, char vLen);
void CAN2_SendX(unsigned long vID, unsigned char* vpData);
void CAN2_SendXLen(unsigned long vID, unsigned char* vpData, char vLen);
unsigned char Can_ID_Chk(unsigned char vCh, unsigned int vID);
unsigned char Can2_ID_Chk(unsigned char vCh, unsigned int vID);
unsigned char RegisterID(unsigned int vIDParam, unsigned char vIDIndex, unsigned int vOvTime_ms);
unsigned char RegisterID2(unsigned int vIDParam, unsigned char vIDIndex, unsigned int vOvTime_ms);
void Can_Rcv_Dly(void);
unsigned char CAN1_Get_Data(unsigned int vID);
unsigned char CAN2_Get_Data(unsigned int vID);

unsigned char CAN1_Get_Data_Buf(unsigned int vID, unsigned char* vBuf);
unsigned char CAN2_Get_Data_Buf(unsigned int vID, unsigned char* vBuf);

void Can_Send_Prog(void);



#endif


