/********************************************************************
*
*  APP_IO 与 IO 口有关的函数，key, di, do 等
*  功能：
*
********************************************************************/


#include "app_comm.h"
#include "LPC17xx.h"
#include "config.h"



#define  _AMAXV				255				// 摇杆的最大值 ，0~255,  耿力版为 0~62/63/64~126


mRunInfo gRunInfo;
mRunInfo gRunInfoS;				// 发送给副控制板的信息

unsigned int  gWLRcvs = 0;
unsigned int  gWLRcvOkN = 0;
unsigned int  gCommOvDly = 0;
unsigned int  gUartSendDly = 0;

// UART0 -- 无线
unsigned int gUartDType = 0;		// 无线（遥控器）过来的数据类型：1~4
unsigned char gWLSStr[32];			// 无线发送的数据缓冲
 unsigned char gWLRcvOk = 0;			// 接收到无线数据，且数据验证通过
unsigned int  gWLAckCnt = 0;		// 当前无线反馈次数，收到遥控器的CAN（有线）数据时清0

unsigned char gWL_RCV_T[16] = "ZC7:3.1H    ";					// 接收机型号 


/****************************************************************************************
****************************************************************************************
* 时间: 2014-11-24
* 名称：
* 功能：			UART 接收处理函数，无线通信
* 无线帧结构： 5A A5 Type Data0~8 XOR
* 入口参数：
* 出口参数：
****************************************************************************************
****************************************************************************************/
unsigned char gWLDType = 0;
uchar Uart1_WL_Rcv(void)
{
	uchar i=0;
	uchar k=0;
	uchar ch = 0;
	uchar vULen = 0;		// UART 接收字符数
	uchar vXOR = 0;		// 
	ushort vChk = 0;		// 
	uchar vFlg = 0xff;
	uchar vUStr[128];
	static unsigned char vJiTing1 = 0;
	static unsigned char vJiTing2 = 0;

	if(gWLRWFlg >= 0x5A)				// 读取 无线模块的当前参数，等待模块返回数据   lsg_wl
	{  	dead_zone_A++ ;
		wl_rd_rcv();
		return 0;
	}		
		dead_zone_B++ ;
	vULen = UART1Count;
	for(i=0; i<vULen; i++)
	{
		if(UART1Buffer[i] == UART_H1)
		{
			if(UART1Buffer[i+1] == UART_H2)		 // 找到帧头
			{
				vFlg = i;
				break;
			}
		}	 
	}
	
	if((vFlg + UART_LEN) > vULen)				// 接收数据的长度不够
		return 0;

	for(i=0; i<vULen; i++)					 	// 检查长度，拷贝数据
	{
		vUStr[i] = UART1Buffer[vFlg+i];
		UART1Buffer[i] = 0;
	}

	UART1Count = 0;
	i=2;
	vULen = vUStr[i];
	vXOR = 0;
	for(i=2; i<vULen; i++)					 	// 检查长度，拷贝数据
	{
		vXOR ^= vUStr[i];
		vChk += vUStr[i];
	}
	
	if(vXOR != vUStr[i])						// 校验错
		return 0;
	if((uchar)vChk != vUStr[i+1])						// 校验错
		return 0;
		 
			CAN_SendXLen(0x680+gID+1, (unsigned char*)(vUStr), 8);

	
	gWLRcvOk ++;		// Rcv is OK
	i=3;		  // vUStr[3] == Data0
	gWLDType = vUStr[i++];

	if(gWLDType == 1)
	{
		gRunInfo.vArm[0] = (unsigned char)(((unsigned int)vUStr[i++])*_AMAXV/255);
		gRunInfo.vArm[1] = (unsigned char)(((unsigned int)vUStr[i++])*_AMAXV/255);
		gRunInfo.vArm[2] = (unsigned char)(((unsigned int)(vUStr[i++]))*_AMAXV/255);				// x
		gRunInfo.vArm[3] = (unsigned char)(((unsigned int)(vUStr[i++]))*_AMAXV/255);				// y
		gRunInfo.vArm[4] = (unsigned char)(((unsigned int)vUStr[i++])*_AMAXV/255);
		gRunInfo.vArm[5] = (unsigned char)(((unsigned int)vUStr[i++])*_AMAXV/255);
		gRunInfo.vArm[6] = (unsigned char)(((unsigned int)vUStr[i++])*_AMAXV/255);
		gRunInfo.vArm[7] = (unsigned char)(((unsigned int)vUStr[i++])*_AMAXV/255);
		gRunInfo.vArm[8] = (unsigned char)(((unsigned int)vUStr[i++])*255/250);
		gRunInfo.vArm[9] = (unsigned char)(((unsigned int)vUStr[i++])*255/250);
		gRunInfo.vArm[10] = (unsigned char)(((unsigned int)vUStr[i++])*255/250);
		gRunInfo.vYKDI1[0] = vUStr[i++];
		gRunInfo.vYKDI1[1] = vUStr[i++];
		gRunInfo.vYKDI2[0] = vUStr[i++];
		gRunInfo.vYKDI2[1] = vUStr[i++];
		gRunInfo.vYKDI3[0] = vUStr[i++];
		gRunInfo.vYKDI3[1] = vUStr[i++];
		gRunInfo.vYKCmd = vUStr[i++];					// 按键F1~F4	
		if(_BitV(gRunInfo.vYKCmd, 5))
			gRunInfo.vLaBaFlg = 1;
		else
			gRunInfo.vLaBaFlg = 0;
		gRunInfo.vYKJiTing = vUStr[i++];
			gCommOvDly = 0;

	 
	} 
	
	return 1;
}





//unsigned char gWLDType = 0;
uchar Uart0_WL_Rcv(void)
{

			;
}


/****************************************************************************************
****************************************************************************************
* 时间: 2014-11-24
* 名称：
* 功能：			UART 发送函数，无线通信，收到无线数据才回发数据
* 无线帧结构： 5A A5 Type Data0~8 XOR
* 入口参数：
* 出口参数：
****************************************************************************************
****************************************************************************************/
void Uart1_WL_Send(void)
{
	static unsigned char vCh = 1;
	static unsigned char stBuf = 0;
	//unsigned char vStr[21];
	unsigned char i = 0;
	unsigned char j = 0;
	unsigned short vChk = 0;
	unsigned char vXor = 0;
	unsigned char vAck = 0;
	unsigned char vTmp = 0;

	gUartSendDly = 50;	
	gWLSStr[i++] = UART_H1;
	gWLSStr[i++] = UART_H2;
	gWLSStr[i++] = 0;

	{
		if(gWLDType >= 0x80)
		{
			gWLSStr[i++] = gWLDType;
			gWLSStr[i++] = gWL_RCV_T[0];
			gWLSStr[i++] = gWL_RCV_T[1];
			gWLSStr[i++] = gWL_RCV_T[2];
			gWLSStr[i++] = gWL_RCV_T[3];
			gWLSStr[i++] = gWL_RCV_T[4];
			gWLSStr[i++] = gWL_RCV_T[5];
			gWLSStr[i++] = gWL_RCV_T[6];
			gWLSStr[i++] = gWL_RCV_T[7];					//  D----DO, F----DFS,  H----HAW  
		}
		else
		{
				gWLSStr[i++] = vCh ;		  // 
			//gWLSStr[i++] = gBDCmd + (gLCDPage.page << 4);
			j = 0;
			switch(vCh)
			{
				case 0:				// 0x180+gID 的数据						// 20180509  
					j=0;
					gWLSStr[i++] = 1 ;//gRunInfo.vRcvs[j++];
					gWLSStr[i++] = 1 ;//gRunInfo.vRcvs[j++];
					gWLSStr[i++] = 1 ;//gRunInfo.vRcvs[j++];
					gWLSStr[i++] = 1 ;//gRunInfo.vRcvs[j++];
					gWLSStr[i++] = 1 ;//gRunInfo.vRcvs[j++];
					gWLSStr[i++] = 1 ;//gRunInfo.vRcvs[j++];
					gWLSStr[i++] =  1;//gRunInfo.vRcvs[j++];
					gWLSStr[i++] = 1 ;//gRunInfo.vRcvs[j++];
					break;
				case 1:				// 0x280+gID 的数据
					j=8;
					gWLSStr[i++] = gRunInfo.vRcvs[j++];
					gWLSStr[i++] = gRunInfo.vRcvs[j++];
					gWLSStr[i++] = gRunInfo.vRcvs[j++];
					gWLSStr[i++] = gRunInfo.vRcvs[j++];
					gWLSStr[i++] = gRunInfo.vRcvs[j++];
					gWLSStr[i++] = gRunInfo.vRcvs[j++];
					gWLSStr[i++] = gRunInfo.vRcvs[j++];
					gWLSStr[i++] = gRunInfo.vRcvs[j++];
					break;
				case 2:				// 0x380+gID 的数据	 电磁阀电流 或 工作斗称重传感器数据
					if(stBuf==0)
					{
						j=16;
						gWLSStr[i++] = gRunInfo.vRcvs[j++];	   // AD1
						gWLSStr[i++] = gRunInfo.vRcvs[j++];
						gWLSStr[i++] = gRunInfo.vRcvs[j++];	   // AD2
						gWLSStr[i++] = gRunInfo.vRcvs[j++];	   
						gWLSStr[i++] = gRunInfo.vRcvs[j++];	   // AD3
						gWLSStr[i++] = gRunInfo.vRcvs[j++];
						gWLSStr[i++] = gRunInfo.vRcvs[j++];	   // AD4
						gWLSStr[i++] = gRunInfo.vRcvs[j++];
					}

					break;
				case 3:				// 0x480+gID 的数据	 电磁阀电流 
					if(stBuf==0)
					{
						j=24;
						gWLSStr[i++] = gRunInfo.vRcvs[j++];	   // AD1
						gWLSStr[i++] = gRunInfo.vRcvs[j++];
						gWLSStr[i++] = gRunInfo.vRcvs[j++];	   // AD2
						gWLSStr[i++] = gRunInfo.vRcvs[j++];	   
						gWLSStr[i++] = gRunInfo.vRcvs[j++];	   // AD3
						gWLSStr[i++] = gRunInfo.vRcvs[j++];
						gWLSStr[i++] = gRunInfo.vRcvs[j++];	   // AD4
						gWLSStr[i++] = gRunInfo.vRcvs[j++];
					}
					break;
		
			}
			vCh++;
			gWLAckCnt++;
			if(vCh >= 3)							// 20180509  
				vCh = 0;
			if((gRunInfo.vYKCmd & 0xf0) == 0x50)	  // 工作斗状态，发送0x180,0x280,0x580
			{
				gWLAckCnt = 0;
			//	if(vCh > 2)
			//		vCh = 0;
			}
			else if((gRunInfo.vYKCmd & 0xf0) == 0x40) // 电磁阀电流，发送0x180,0x280,0x380,0x480
			{
				gWLAckCnt = 0;
			//	if(vCh > 3)
			//		vCh = 0;
			}										 
			else if(gWLAckCnt < 100)				  // 其他情况，发送0x180,0x280
			{
			//	if(vCh > 3)
			//		vCh = 0;
			}
			else
			{
				gWLAckCnt = 102;
			//	vCh &= 1;
			}
		}
	}
	gWLSStr[2] = i;
	for(j=2; j<i; j++)					 	// 检查长度，拷贝数据
	{
		vXor ^= gWLSStr[j];
		vChk += gWLSStr[j];
	}
	gWLSStr[i++] = (unsigned char)vXor;
	gWLSStr[i++] = (unsigned char)vChk;
			//CAN_Send(0, 0x680+gID, (unsigned char*)(&gWLSStr[2]));
			CAN_SendXLen(0x680+gID, (unsigned char*)(&gWLSStr[2]), 8);

	UARTSend(1, gWLSStr, i);
}




void Uart0_WL_Send(void)
{ 
			;
}

// UART0Buffer[UART0Count]



////  XRZNWR  0x: 58 52 5A 4E 57 52  
unsigned char  gWLInfo[8];							// 无线参数（全部），用来读写
unsigned char  gWLCmd = 0;							// 无线命令
unsigned short  gWLParam = 0;						// 无线参数，新
unsigned char  gWLRWFlg = 0;

	 unsigned char stBKClose = 0;
unsigned char  gPower24V = 1;						// 是否 24V 供电，24V供电时，不考虑电池，不关无线

/*****************************************************************************
** Function name:		无线配置
**
** Descriptions:		需要在 CANOPEN的接收、UART的接收、主循环中调用不同的函数，备注 lsg_wl
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/

void wl_reset(void)
{
	GPIO_TypeDef * pGPIO = NULL;
	//// IO_MD0 
	GPIO0->FIODIR |= (0x01 << IO_MD0); 		// P0.4
	GPIO0->FIOSET |= (0x01 << IO_MD0);
	//// IO_MD1 
	GPIO1->FIODIR |= (0x01 << IO_MD1);		// P1.1
	GPIO1->FIOSET |= (0x01 << IO_MD1);

	UARTInit(0, 115200);
//	UARTInit(1, 115200);
		delay_ms(10);

	//// IO_MD0 
	GPIO0->FIOCLR |= (0x01 << IO_MD0);
	//// IO_MD1 
	GPIO1->FIOCLR |= (0x01 << IO_MD1);	
}


uint32_t wl_rd( void )
{
	unsigned char i = 0;
	unsigned char vStr[10];
	GPIO_TypeDef * pGPIO = NULL;


	//// IO_MD0 
	pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*0)); 		// P0.4
	pGPIO->FIODIR |= (0x01 << IO_MD0);
	pGPIO->FIOSET |= (0x01 << IO_MD0);
	//// IO_MD1 
	pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*1)); 		// P1.1
	pGPIO->FIODIR |= (0x01 << IO_MD1);
	pGPIO->FIOSET |= (0x01 << IO_MD1);
		delay_ms(10);

	if(gWLRWFlg == 0x5A)				// 读取
	{
		i = 0;
		UARTInit(1, 9600);
		delay_ms(20);
		vStr[i++] = 0xc1;
		vStr[i++] = 0xc1;
		vStr[i++] = 0xc1;
			UARTSend(1, vStr, i);
		gWLRWFlg = 0x5E;
	}

	return 0;
}
uint32_t wl_wr( void )
{
	unsigned char i = 0;
	unsigned char vStr[10];
	GPIO_TypeDef * pGPIO = NULL;


	//// IO_MD0 
	pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*0)); 		// P0.4
	pGPIO->FIODIR |= (0x01 << IO_MD0);
	pGPIO->FIOSET |= (0x01 << IO_MD0);
	//// IO_MD1 
	pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*1)); 		// P1.1
	pGPIO->FIODIR |= (0x01 << IO_MD1);
	pGPIO->FIOSET |= (0x01 << IO_MD1);
		delay_ms(10);

	if(gWLRWFlg == 0x55)				// 设置
	{
		i = 0;
		UARTInit(1, 9600);
		delay_ms(20);

		if(gWLInfo[0] == 0xc0)
		{
			if(gWLCmd == 0x81)
			{
				gWLInfo[3] &= 0xf8;
				gWLInfo[3] |= gWLParam & 7;
			}
			UARTSend(1, gWLInfo, 6);
		}
		delay_ms(20);
		gWLRWFlg = 0x5A;
	}
	return 0;
}

void WL_Set(void)
{
	if((CAN_RBuf0[4] == 'W'))						// 	设置无线参数，模块地址，通道，空速，串口速率等
	{
		gWLCmd = CAN_RBuf0[5];															// 配置的命令字， 0x81--空速
		gWLParam = (CAN_RBuf0[6]) | (CAN_RBuf0[7]<<8);			// 参数
		gWLRWFlg = 0x55;
		//wl_wr();
	}
	if((CAN_RBuf0[4] == 'R'))						// 	读取无线参数，频率，跳频信道数，跳频序列
	{
		gWLRWFlg = 0x5A;
		//wl_rd();
	}
}

void wl_set_param(void)
{
	if(gWLRWFlg == 0x55)				// 设置
		wl_wr();
	if(gWLRWFlg == 0x5A)				// 读取
		wl_rd();
}

void wl_rd_rcv(void)
{
	unsigned char i=0;
	unsigned char k=0;
	unsigned char l=0;
	unsigned char vULen = 0;		// UART 接收字符数 
	unsigned char vCStr[10];

	if(gWLRWFlg >= 0x5A)				// 读取 无线模块的当前参数，等待模块返回数据
	{
		vULen = UART1Count;
		if(vULen >= 6)
		{
			for(i=0; i<vULen; i++)
			{
				if(UART1Buffer[i] == 0xC0)
				{
					k = 0;
					l = i;
					vCStr[k++] = 'X';
					vCStr[k++] = 'R';
					vCStr[k++] = UART1Buffer[i++];
					vCStr[k++] = UART1Buffer[i++];
					vCStr[k++] = UART1Buffer[i++];
					vCStr[k++] = UART1Buffer[i++];
					vCStr[k++] = UART1Buffer[i++];
					vCStr[k++] = UART1Buffer[i++];
					//CAN_Send(0, ID_ACK_M1, vCStr);
					CAN_SendXLen(ID_ACK_M1, vCStr, 8);
					k=0;
					i=l;
					gWLInfo[k++] = UART1Buffer[i++];
					gWLInfo[k++] = UART1Buffer[i++];
					gWLInfo[k++] = UART1Buffer[i++];
					gWLInfo[k++] = UART1Buffer[i++];
					gWLInfo[k++] = UART1Buffer[i++];
					gWLInfo[k++] = UART1Buffer[i++];
					gWLInfo[k++] = 0;
					gWLRWFlg = 0x5f;
					UART1Count = 0;
					for(i=0; i<vULen; i++)					 	// 检查长度，拷贝数据
					{
						UART1Buffer[i] = 0;
					}
					break;
				}
			}
		}
	}
}



