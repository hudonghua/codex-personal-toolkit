/*******************************************************************************
*   canopen.c
Platform: AT90CAN32
Project : 倾角传感器
Clock F : 内部1MHz，外部12MHz
Software: AVR STUDIO 4.18
Author  : LSG
Version : 12.06.25
Updata  : 
comments:
1、
2、
3、

CANOPEN 协议相关处理函数

*******************************************************************************/

#include "canopen.h"
#include "app_usr.h"
//#include "..\can\LPC1700can.h"


extern unsigned char gID;
extern unsigned short gCanCyc;


unsigned int gSysStatus = 0;
unsigned char gCanOpenFlag = 0;
unsigned char  gCanOutTst = 0;
unsigned int   gCanDOTstV = 0;

unsigned int gCanOvDly;


	 /*CAN数据缓存*/
	 unsigned char CAN1_RBuf[8]={0,0,0,0,0,0,0,0};	//MOb0缓存，1C1F3E02
	 unsigned char CAN2_RBuf[8]={0,0,0,0,0,0,0,0};	//MOb0缓存，1C1F3E02
	 unsigned char CAN_RBuf0[8]={0,0,0,0,0,0,0,0};	//MOb0缓存，1C1F3E02
	 unsigned char CAN_RBuf1[8]={0,0,0,0,0,0,0,0};	//MOb1缓存，1C1F3E03
	 unsigned char CAN_RBuf2[8]={0,0,0,0,0,0,0,0};	//MOb2缓存，1C1F3E23
	 unsigned char CAN_RBuf3[8]={0,0,0,0,0,0,0,0};	//MOb2缓存，1C1F3E23
	 unsigned char CAN_SBuf0[8]={0,0,0,0,0,0,0,0};	//MOb3缓存，1C1F3E0x
	 unsigned char CAN_SBuf1[8]={0,0,0,0,0,0,0,0};	//MOb4缓存，保存外呼按钮灯信息
	 unsigned char CAN_SBuf2[8]={0,0,0,0,0,0,0,0};	//MOb4缓存，保存外呼按钮灯信息
	 unsigned char CAN_SBuf3[8]={0,0,0,0,0,0,0,0};	//MOb4缓存，保存外呼按钮灯信息
	 unsigned char CAN_SBuf4[8]={0,0,0,0,0,0,0,0};	//MOb4缓存，保存外呼按钮灯信息	
	 unsigned char CAN1_RBuf_318[8]={0,0,0,0,0,0,0,0};	//MOb4缓存，保存外呼按钮灯信息	 
	 unsigned char CAN1_RBuf_319[8]={0,0,0,0,0,0,0,0};	//MOb4缓存，保存外呼按钮灯信息	 
	 unsigned char CAN2_RBuf_318[8]={0,0,0,0,0,0,0,0};	//MOb4缓存，保存外呼按钮灯信息	 
	 unsigned char CAN2_RBuf_319[8]={0,0,0,0,0,0,0,0};	//MOb4缓存，保存外呼按钮灯信息	 
	 unsigned char CAN1_RBuf_418[8]={0,0,0,0,0,0,0,0};	//MOb4缓存，保存外呼按钮灯信息	 
	 unsigned char CAN1_RBuf_419[8]={0,0,0,0,0,0,0,0};	//MOb4缓存，保存外呼按钮灯信息	 
	 unsigned char CAN_RBuf_byte[8]={0,0,0,0,0,0,0,0};	//MOb4缓存，保存外呼按钮灯信息 
	 unsigned char CAN_RBuf_bit[8]={0,0,0,0,0,0,0,0};	//MOb4缓存，保存外呼按钮灯信息	  	
unsigned long gCanRID0 = 0;		// RBUF0 对应的 ID 
unsigned long gCanRID1 = 0;		// RBUF1 对应的 ID 
unsigned long gCanRID2 = 0;		// RBUF2 对应的 ID 
unsigned long gCanRID3 = 0;		// RBUF3 对应的 ID 

CAN_MSG MsgBuf_TX1[ID_SEND_NUM], MsgBuf_TX2[ID_SEND_NUM]; // TX and RX Buffers for CAN message
CAN_MSG MsgBuf_RX1[ID_RCV_NUM];
CAN_MSG MsgBuf_RX2[ID_RCV_NUM]; // TX and RX Buffers for CAN message

volatile uint32_t CAN1RxDone, CAN2RxDone;
unsigned int gCanRFlg = 0;


#if 1

unsigned long gRcvCanID[2][ID_RCV_NUM];
unsigned long gRcvCan2ID[2][ID_RCV_NUM];

/****************************************************************************
* 时间: 2008-04-04
* 名称：unsigned char RegisterID(unsigned int vIDParam, unsigned char vIDIndex, unsigned int vOvTime_ms)
* 功能：CAN通讯ID号注册函数
* 入口参数： vIDParam: 需要注册的ID号 
						 vIDIndex: 注册的顺序号  
						 vOvTime_ms：接收超时，单位 ms
* 出口参数：
****************************************************************************/
unsigned char RegisterID(unsigned int vIDParam, unsigned char vIDIndex, unsigned int vOvTime_ms)
{
	unsigned char i = 0;

	if(vIDIndex >= ID_RCV_NUM)
		return 0;
	for(i=0; i<ID_RCV_NUM; i++)
	{
		if(gRcvCanID[0][i] == 0)
		{
			gRcvCanID[0][i] = vIDParam;
			gRcvCanID[1][i] = vOvTime_ms;
			break;
		}
	}
	return 1;
}
unsigned char RegisterID2(unsigned int vIDParam, unsigned char vIDIndex, unsigned int vOvTime_ms)
{
	unsigned char i = 0;

	if(vIDIndex >= ID_RCV_NUM)
		return 0;
	for(i=0; i<ID_RCV_NUM; i++)
	{
		if(gRcvCan2ID[0][i] == 0)
		{
			gRcvCan2ID[0][i] = vIDParam;
			gRcvCan2ID[1][i] = vOvTime_ms;
			break;
		}
	}
	return 1;
}

/*
unsigned char Can_ID_Chk(unsigned char vCh, unsigned int vID)
{
	unsigned char i = 0;
	
	for(i=0; i<ID_RCV_NUM; i++)
	{
		if(gRcvCanID[vCh][i] == vID)
		{
			gCanRFlg = 1<<i;
			return i;
		}
	}
	return 0xff;
}
*/
unsigned char Can_ID_Chk(unsigned char vCh, unsigned int vID)
{
	unsigned char i = 0;
	
	for(i=0; i<ID_RCV_NUM; i++)
	{
		if(gRcvCanID[0][i] == vID)
		{
			gCanRFlg = 1<<i;
			return i;
		}
	}
	return 0xff;
}
unsigned char Can2_ID_Chk(unsigned char vCh, unsigned int vID)
{
	unsigned char i = 0;
	
	for(i=0; i<ID_RCV_NUM; i++)
	{
		if(gRcvCan2ID[0][i] == vID)
		{
			gCanRFlg = 1<<i;
			return i;
		}
	}
	return 0xff;
}

void CAN_SendX(unsigned long vID, unsigned char* vpData)
{
	unsigned long canDA = 0;
	unsigned long canDB = 0;
	unsigned char vCh = 0;
	
	canDA = *vpData + (*(vpData+1)<<8) + (*(vpData+2)<<16) + (*(vpData+3)<<24);
	canDB = *(vpData+4) + (*(vpData+5)<<8) + (*(vpData+6)<<16) + (*(vpData+7)<<24);
	for(vCh=0; vCh<ID_SEND_NUM; vCh++)
	{
		if(MsgBuf_TX1[vCh].vEmpty == 0)
		{
			MsgBuf_TX1[vCh].MsgID = vID;
			MsgBuf_TX1[vCh].Frame &= ~0x000f0000;		// bit16~19 -- DLC
			MsgBuf_TX1[vCh].Frame |= (0x08 << 16);
			if(vID > 0x7ff)
				MsgBuf_TX1[vCh].Frame |= 0X80000000;
			else	
				MsgBuf_TX1[vCh].Frame &= ~0X80000000;
			MsgBuf_TX1[vCh].DataA = canDA;
			MsgBuf_TX1[vCh].DataB = canDB;
			MsgBuf_TX1[vCh].vEmpty = 1;
			break;
		}
	}
}

void CAN_SendXLen(unsigned long vID, unsigned char* vpData, char vLen)
{
	unsigned long canDA = 0;
	unsigned long canDB = 0;
	unsigned char vCh = 0;
	
	if(vLen > 8)
	vLen = 8;
	canDA = *vpData + (*(vpData+1)<<8) + (*(vpData+2)<<16) + (*(vpData+3)<<24);
	canDB = *(vpData+4) + (*(vpData+5)<<8) + (*(vpData+6)<<16) + (*(vpData+7)<<24);
	for(vCh=0; vCh<ID_SEND_NUM; vCh++)
	{
		if(MsgBuf_TX1[vCh].vEmpty == 0)
		{
			MsgBuf_TX1[vCh].MsgID = vID;
			MsgBuf_TX1[vCh].Frame &= ~0x000f0000;		// bit16~19 -- DLC
			MsgBuf_TX1[vCh].Frame |= (vLen << 16);
			if(vID > 0x7ff)
				MsgBuf_TX1[vCh].Frame |= 0X80000000;
			else	
				MsgBuf_TX1[vCh].Frame &= ~0X80000000;
			MsgBuf_TX1[vCh].DataA = canDA;
			MsgBuf_TX1[vCh].DataB = canDB;
			MsgBuf_TX1[vCh].vEmpty = 1;
			break;
		}
	}
}

void CAN2_SendX(unsigned long vID, unsigned char* vpData)
{
	unsigned long canDA = 0;
	unsigned long canDB = 0;
	unsigned char vCh = 0;
	
	canDA = *vpData + (*(vpData+1)<<8) + (*(vpData+2)<<16) + (*(vpData+3)<<24);
	canDB = *(vpData+4) + (*(vpData+5)<<8) + (*(vpData+6)<<16) + (*(vpData+7)<<24);
	for(vCh=0; vCh<ID_SEND_NUM; vCh++)
	{
		if(MsgBuf_TX2[vCh].vEmpty == 0)
		{
			MsgBuf_TX2[vCh].MsgID = vID;
			MsgBuf_TX2[vCh].Frame &= ~0x000f0000;		// bit16~19 -- DLC
			MsgBuf_TX2[vCh].Frame |= (0x08 << 16);
			if(vID > 0x7ff)
				MsgBuf_TX2[vCh].Frame |= 0X80000000;
			else	
				MsgBuf_TX2[vCh].Frame &= ~0X80000000;
			MsgBuf_TX2[vCh].DataA = canDA;
			MsgBuf_TX2[vCh].DataB = canDB;
			MsgBuf_TX2[vCh].vEmpty = 1;
			break;
		}
	}
}

void CAN2_SendXLen(unsigned long vID, unsigned char* vpData, char vLen)
{
	unsigned long canDA = 0;
	unsigned long canDB = 0;
	unsigned char vCh = 0;
	
	vLen = vLen & 7;
	canDA = *vpData + (*(vpData+1)<<8) + (*(vpData+2)<<16) + (*(vpData+3)<<24);
	canDB = *(vpData+4) + (*(vpData+5)<<8) + (*(vpData+6)<<16) + (*(vpData+7)<<24);
	for(vCh=0; vCh<ID_SEND_NUM; vCh++)
	{
		if(MsgBuf_TX2[vCh].vEmpty == 0)
		{
			MsgBuf_TX2[vCh].MsgID = vID;
			MsgBuf_TX2[vCh].Frame &= ~0x000f0000;		// bit16~19 -- DLC
			MsgBuf_TX2[vCh].Frame |= (vLen << 16);
			if(vID > 0x7ff)
				MsgBuf_TX2[vCh].Frame |= 0X80000000;
			else	
				MsgBuf_TX2[vCh].Frame &= ~0X80000000;
			MsgBuf_TX2[vCh].DataA = canDA;
			MsgBuf_TX2[vCh].DataB = canDB;
			MsgBuf_TX2[vCh].vEmpty = 1;
			break;
		}
	}
}

 #endif

/*=======================================================================================
=========================================================================================
*
* 时间: 2015-05-26
* 名称：CAN 应用函数，配置ID，接收，发送等函数
* 
=========================================================================================
=======================================================================================*/

/******************************************************************
* 时间: 2015-05-26
* 名称：unsigned char Can1_RcvID_Cfg(void)
******************************************************************/
unsigned char Can1_RcvID_Cfg(void)
{
	unsigned char vIDNum = 0;
	unsigned char i=0;
	/* CAN1 Arm200A sensors. PDO1 = 0x180 + nodeID. */
	vIDNum += RegisterID(ID_RCV_M1 , i++, 2000);
	vIDNum += RegisterID(0x181 , i++, 1000);
	vIDNum += RegisterID(0x182 , i++, 1000);
	vIDNum += RegisterID(0x183 , i++, 1000);
	vIDNum += RegisterID(0x184 , i++, 1000);
	vIDNum += RegisterID(0x185 , i++, 1000);
	vIDNum += RegisterID(0x186 , i++, 1000);
	vIDNum += RegisterID(0x187 , i++, 1000);
	vIDNum += RegisterID(0x188 , i++, 1000);
	vIDNum += RegisterID(0x189 , i++, 1000);
	vIDNum += RegisterID(0x192 , i++, 1000);
	vIDNum += RegisterID(0x193 , i++, 1000);
	vIDNum += RegisterID(0x194 , i++, 1000);
	vIDNum += RegisterID(0x195 , i++, 1000);
	vIDNum += RegisterID(0x196 , i++, 1000);
	vIDNum += RegisterID(0x197 , i++, 1000);
	vIDNum += RegisterID(0x198 , i++, 1000);
	vIDNum += RegisterID(0x199 , i++, 1000);
	vIDNum += RegisterID(0x1A1 , i++, 1000);
	vIDNum += RegisterID(0x1A2 , i++, 1000);
	vIDNum += RegisterID(0x1A3 , i++, 1000);
	vIDNum += RegisterID(0x1A4 , i++, 1000);
	vIDNum += RegisterID(0x1A5 , i++, 1000);
	vIDNum += RegisterID(0x1A6 , i++, 1000);
	vIDNum += RegisterID(0x1A7 , i++, 1000);
	vIDNum += RegisterID(0x1A8 , i++, 1000);
	vIDNum += RegisterID(0x1A9 , i++, 1000);
	vIDNum += RegisterID(0x28F , i++, 1000);

	CAN_SBuf0[0] = gSysStatus;
	CAN_SBuf0[1] = gID;
	CAN_SBuf0[2] = vIDNum;
	CAN_SBuf0[3] = 1;

	return vIDNum;
}
unsigned char Can2_RcvID_Cfg(void)
{
	unsigned char vIDNum = 0;
	unsigned char i=0;

	vIDNum += RegisterID2(ID_RCV_M1 , i++, 2000);									// 0x7e5， 2秒超时	 0	    
	vIDNum += RegisterID2(0x318 , i++, 1000);											//    1	 	    
	vIDNum += RegisterID2(0x319 , i++, 1000);											//    2	 	     
	vIDNum += RegisterID2(0x50 , i++, 1000);											// NET_TO_CAN zero cmd
	vIDNum += RegisterID2(0x17E , i++, 1000);										// NET_TO_CAN self-parallel init cmd

	 	    					     
			 	      	   		   
	CAN_SBuf1[0] = 0;
	CAN_SBuf1[1] = gID2;
	CAN_SBuf1[2] = vIDNum;
	CAN_SBuf1[3] = 2;
	CAN2_SendXLen(ID_SEND_READY+gID2, CAN_SBuf1,4);		// AI, 0x700

	return vIDNum;
}

// can 接收倒计时
void Can_Rcv_Dly(void)
{
	unsigned char vCanCh = 0;
	
	for(vCanCh=0; vCanCh<ID_RCV_NUM; vCanCh++)				
	{
		if(MsgBuf_RX1[vCanCh].vEmpty >= 10)
			MsgBuf_RX1[vCanCh].vEmpty -= 10;
		else
			MsgBuf_RX1[vCanCh].vEmpty = 0;
		if(MsgBuf_RX2[vCanCh].vEmpty >= 10)
			MsgBuf_RX2[vCanCh].vEmpty -= 10;
		else
			MsgBuf_RX2[vCanCh].vEmpty = 0;
	}
}
/******************************************************************
* 时间: 2015-05-26
* 名称：unsigned char CanOpen_Prog(void)
* 功能：CAN 配置函数，可配置ID/BPS/CYCLE/角度标定等
* 入口参数：
*			
* 出口参数：
******************************************************************/

unsigned char CanOpen_Prog(unsigned char vFlag)
{
	unsigned char i = 0;
	unsigned char vSBuf[8];
//	unsigned int  vIndex = 0;

	for(i=0; i<8; i++)
		vSBuf[i] = 0;

	if(0 == gCanOpenFlag)
	{
		vSBuf[0] = 0;
		vSBuf[1] = 0;
		CAN_SendXLen(ID_SEND_READY + gID, vSBuf,1);	
		gCanOpenFlag = CANOPEN_MOD_INIT;
	}

	if((0x01 == (vFlag & 0x01)) && (CAN_RBuf0[1] == gID))				// Mob0, ID_RCV_M1 0x7e5
	{
		if(CAN_RBuf0[0] == 0x11)				// Cfg NID
		{
			if((CAN_RBuf0[2] > 0) && (CAN_RBuf0[2] < 128))
			{
				if(CAN_RBuf0[3] == 0)
					gSysInfo[0].vCANID = CAN_RBuf0[2];
				if(CAN_RBuf0[3] == 1)
					gSysInfo[0].vCANID2 = CAN_RBuf0[2];
				//gSysInfo.v1018_1_ID = gSysInfo.vID;
				//gID = gSysInfo.vID;
				gCanOpenFlag = CANOPEN_MOD_SETID;
			}
			vSBuf[0] = 0x11;
			vSBuf[1] = 0;
			CAN_SendX( ID_ACK_M1, vSBuf);	
			gCanOpenFlag = CANOPEN_MOD_SETID_OK;
		}
		if(CAN_RBuf0[0] == 0x12)				// Cfg CYCLE
		{
			//if((CAN_RBuf0[1] == 0))
			{
				if(CAN_RBuf0[3] == 0)
					gSysInfo[0].vCycle = CAN_RBuf0[2];
				if(CAN_RBuf0[3] == 1)
					gSysInfo[0].vCycle2 = CAN_RBuf0[2];
				gCanCyc = gSysInfo[0].vCycle * 10;
				gCanOpenFlag = CANOPEN_MOD_SETCYCL;
			}
			vSBuf[0] = 0x12;
			vSBuf[1] = 0;
			CAN_SendX( ID_ACK_M1, vSBuf);	
			gCanOpenFlag = CANOPEN_MOD_SETCYCL_OK;
		}
		if(CAN_RBuf0[0] == 0x13)				// Cfg BPS
		{
			//if((CAN_RBuf0[1] == 0))
			{
				if(CAN_RBuf0[3] == 0)
				{
					gSysInfo[0].vBps = CAN_RBuf0[2];
					if(gSysInfo[0].vBps > 2)
						gSysInfo[0].vBps = 1;
				}
				if(CAN_RBuf0[3] == 1)
				{
					gSysInfo[0].vBps2 = CAN_RBuf0[2];
					if(gSysInfo[0].vBps2 > 2)
						gSysInfo[0].vBps2 = 1;
				}
				gCanOpenFlag = CANOPEN_MOD_SETBPS;
			}
			vSBuf[0] = 0x13;
			vSBuf[1] = 0;
			CAN_SendX( ID_ACK_M1, vSBuf);	
			gCanOpenFlag = CANOPEN_MOD_SETBPS_OK;
		}
		if(CAN_RBuf0[0] == 0x17)				// Save
		{
			gCanOpenFlag = CANOPEN_MOD_SAVE;
			Sys_Save_Info();
		//	Sys_Read_Info();
			gID = gSysInfo[0].vCANID;
			gID2 = gSysInfo[0].vCANID2;
			gSysStatus |= 0x80;
			vSBuf[0] = 0x17;
			vSBuf[1] = 0;
			CAN_SendX( ID_ACK_M1, vSBuf);	
			gCanOpenFlag = CANOPEN_MOD_SAVE_OK;
		}
	}
	return gCanOpenFlag;
}


/******************************************************************
* 时间: 2015-05-26
* 名称：unsigned char Can_Send_Prog(void)
* 功能：CAN 发送函数，发送 缓冲区里的CAN数据
* 入口参数：
*			
* 出口参数：
******************************************************************/
void Can_Send_Prog(void)
{
	static unsigned char stCh = 0;
	static unsigned char stCh2 = 0;
	
	for( ; stCh<ID_SEND_NUM; stCh++)
	{
		if(MsgBuf_TX1[stCh].vEmpty)
		{
			MsgBuf_TX1[stCh].vEmpty = 0;
			if ( CAN1_SendMessage( &MsgBuf_TX1[stCh] ) == FALSE )
			{
			}
				break;
		}
		if(stCh >= ID_SEND_NUM)
			stCh = 0;
	}
		if(stCh >= ID_SEND_NUM)
			stCh = 0;

	for( ; stCh2<ID_SEND_NUM; stCh2++)
	{
		if(MsgBuf_TX2[stCh2].vEmpty)
		{
			MsgBuf_TX2[stCh2].vEmpty = 0;
			if ( CAN2_SendMessage( &MsgBuf_TX2[stCh2] ) == FALSE )
			{
			}
				break;
		}
		if(stCh2 >= ID_SEND_NUM)
			stCh2 = 0;
	}
		if(stCh2 >= ID_SEND_NUM)
			stCh2 = 0;
}

void Can_Prog_Rcv(unsigned int vFlag)
{ 
	unsigned char vCh = 0;
		
		vCh = 0;
	if((1<<vCh) == (vFlag & (1<<vCh)))	  //  CFG
	{
		vFlag &= ~(1<<vCh);
		CAN_RBuf0[0] = (MsgBuf_RX1[vCh].DataA >> 0) & 0xff;
		CAN_RBuf0[1] = (MsgBuf_RX1[vCh].DataA >> 8) & 0xff;
		CAN_RBuf0[2] = (MsgBuf_RX1[vCh].DataA >> 16) & 0xff;
		CAN_RBuf0[3] = (MsgBuf_RX1[vCh].DataA >> 24) & 0xff;
		CAN_RBuf0[4] = (MsgBuf_RX1[vCh].DataB >> 0) & 0xff;
		CAN_RBuf0[5] = (MsgBuf_RX1[vCh].DataB >> 8) & 0xff;
		CAN_RBuf0[6] = (MsgBuf_RX1[vCh].DataB >> 16) & 0xff;
		CAN_RBuf0[7] = (MsgBuf_RX1[vCh].DataB >> 24) & 0xff;

		if((CAN_RBuf0[0]==0xf1)&&(CAN_RBuf0[1]==0xf2)&&(CAN_RBuf0[2]==0xf3)&&(CAN_RBuf0[3]==0xf4))
		{
			if((CAN_RBuf0[4]==0xf5)&&(CAN_RBuf0[5]==0xf6)&&(CAN_RBuf0[6]==gID))
				gCanOutTst = CAN_RBuf0[7];
			if((CAN_RBuf0[4]==0xff))
			{
				gCanOutTst = CAN_RBuf0[5];
				gCanDOTstV = CAN_RBuf0[6] + (CAN_RBuf0[7]<<8);
			}
		}
		else
			CanOpen_Prog(1);
	}
}

////  从接收缓冲区中读取指定ID的数据（CANx_RBuf），并返回状态：0xFF：超时, 0xEE：没配置接收该ID，其他值：接收序号
unsigned char CAN1_Get_Data(unsigned int vID)
{
	unsigned char i = 0;
	unsigned char vCh = 0;
	
	for(i=0; i<8; i++)
		CAN1_RBuf[i] = 0;
	for(i=0; i<ID_RCV_NUM; i++)
	{
		if(gRcvCanID[0][i] == vID)
		{
			vCh = i;
			CAN1_RBuf[0] = (MsgBuf_RX1[vCh].DataA >> 0) & 0xff;
			CAN1_RBuf[1] = (MsgBuf_RX1[vCh].DataA >> 8) & 0xff;
			CAN1_RBuf[2] = (MsgBuf_RX1[vCh].DataA >> 16) & 0xff;
			CAN1_RBuf[3] = (MsgBuf_RX1[vCh].DataA >> 24) & 0xff;
			CAN1_RBuf[4] = (MsgBuf_RX1[vCh].DataB >> 0) & 0xff;
			CAN1_RBuf[5] = (MsgBuf_RX1[vCh].DataB >> 8) & 0xff;
			CAN1_RBuf[6] = (MsgBuf_RX1[vCh].DataB >> 16) & 0xff;
			CAN1_RBuf[7] = (MsgBuf_RX1[vCh].DataB >> 24) & 0xff;
			if(MsgBuf_RX1[vCh].vEmpty == 0)
				return 0xFF;
			else
				return i;
		}
	}
	return 0xEE;	
}

unsigned char CAN1_Get_Data_Buf(unsigned int vID, unsigned char* vBuf)
{
	unsigned char i = 0;
	unsigned char vCh = 0;
	
	for(i=0; i<ID_RCV_NUM; i++)
	{
		if(gRcvCanID[0][i] == vID)
		{
			vCh = i;
			CAN1_RBuf[0] = (MsgBuf_RX1[vCh].DataA >> 0) & 0xff;
			CAN1_RBuf[1] = (MsgBuf_RX1[vCh].DataA >> 8) & 0xff;
			CAN1_RBuf[2] = (MsgBuf_RX1[vCh].DataA >> 16) & 0xff;
			CAN1_RBuf[3] = (MsgBuf_RX1[vCh].DataA >> 24) & 0xff;
			CAN1_RBuf[4] = (MsgBuf_RX1[vCh].DataB >> 0) & 0xff;
			CAN1_RBuf[5] = (MsgBuf_RX1[vCh].DataB >> 8) & 0xff;
			CAN1_RBuf[6] = (MsgBuf_RX1[vCh].DataB >> 16) & 0xff;
			CAN1_RBuf[7] = (MsgBuf_RX1[vCh].DataB >> 24) & 0xff;
			
			vBuf[0] = CAN1_RBuf[0];
			vBuf[1] = CAN1_RBuf[1];
			vBuf[2] = CAN1_RBuf[2];
			vBuf[3] = CAN1_RBuf[3];
			vBuf[4] = CAN1_RBuf[4];
			vBuf[5] = CAN1_RBuf[5];
			vBuf[6] = CAN1_RBuf[6];
			vBuf[7] = CAN1_RBuf[7];
			if(MsgBuf_RX1[vCh].vEmpty == 0)
				return 0xFF;
			else
				return i;
		}
	}
	return 0xEE;	
}

unsigned char CAN2_Get_Data(unsigned int vID)
{
	unsigned char i = 0;
	unsigned char vCh = 0;
	
	for(i=0; i<8; i++)
		CAN2_RBuf[i] = 0;
	for(i=0; i<ID_RCV_NUM; i++)
	{
		if(gRcvCan2ID[0][i] == vID)
		{
			vCh = i;
			CAN2_RBuf[0] = (MsgBuf_RX2[vCh].DataA >> 0) & 0xff;
			CAN2_RBuf[1] = (MsgBuf_RX2[vCh].DataA >> 8) & 0xff;
			CAN2_RBuf[2] = (MsgBuf_RX2[vCh].DataA >> 16) & 0xff;
			CAN2_RBuf[3] = (MsgBuf_RX2[vCh].DataA >> 24) & 0xff;
			CAN2_RBuf[4] = (MsgBuf_RX2[vCh].DataB >> 0) & 0xff;
			CAN2_RBuf[5] = (MsgBuf_RX2[vCh].DataB >> 8) & 0xff;
			CAN2_RBuf[6] = (MsgBuf_RX2[vCh].DataB >> 16) & 0xff;
			CAN2_RBuf[7] = (MsgBuf_RX2[vCh].DataB >> 24) & 0xff;
			if(MsgBuf_RX2[vCh].vEmpty == 0)
				return 0xFF;
			else
				return i;
		}
	}
	return 0xEE;	
}

unsigned char CAN2_Get_Data_Buf(unsigned int vID, unsigned char* vBuf)
{
	unsigned char i = 0;
	unsigned char vCh = 0;
	
	for(i=0; i<8; i++)
		CAN2_RBuf[i] = 0;
	for(i=0; i<ID_RCV_NUM; i++)
	{
		if(gRcvCan2ID[0][i] == vID)
		{
			vCh = i;
			CAN2_RBuf[0] = (MsgBuf_RX2[vCh].DataA >> 0) & 0xff;
			CAN2_RBuf[1] = (MsgBuf_RX2[vCh].DataA >> 8) & 0xff;
			CAN2_RBuf[2] = (MsgBuf_RX2[vCh].DataA >> 16) & 0xff;
			CAN2_RBuf[3] = (MsgBuf_RX2[vCh].DataA >> 24) & 0xff;
			CAN2_RBuf[4] = (MsgBuf_RX2[vCh].DataB >> 0) & 0xff;
			CAN2_RBuf[5] = (MsgBuf_RX2[vCh].DataB >> 8) & 0xff;
			CAN2_RBuf[6] = (MsgBuf_RX2[vCh].DataB >> 16) & 0xff;
			CAN2_RBuf[7] = (MsgBuf_RX2[vCh].DataB >> 24) & 0xff;

			vBuf[0] = CAN2_RBuf[0];
			vBuf[1] = CAN2_RBuf[1];
			vBuf[2] = CAN2_RBuf[2];
			vBuf[3] = CAN2_RBuf[3];
			vBuf[4] = CAN2_RBuf[4];
			vBuf[5] = CAN2_RBuf[5];
			vBuf[6] = CAN2_RBuf[6];
			vBuf[7] = CAN2_RBuf[7];
			if(MsgBuf_RX2[vCh].vEmpty == 0)
				return 0xFF;
			else
				return i;
		}
	}
	return 0xEE;	
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

