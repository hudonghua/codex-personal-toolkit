/****************************************************************************
*
*	 串口  有关的函数
*
*
*****************************************************************************/

#include "APP_uart.h"
#include "LPC17xx.h"
#include "config.h"

#include "app_lcd.h"

// UART0 -- DGUS LCD
unsigned int gDGUSSndCmd = 0;	// 发送给LCD的指令
unsigned int gDGUSSndAddr = 0;	// 发送给LCD的指定地址
unsigned int gDGUSSndLen = 0;	// 发送给LCD的数据长度
unsigned int gDGUSRtnCmd = 0;	// LCD的返回数据的指令
unsigned int gDGUSRtnAddr = 0;	// LCD的返回数据的指定地址
unsigned int gDGUSRtnLen = 0;	// LCD的返回数据的数据长度
unsigned long gDGUSRtnData = 0;	// LCD的返回数据
unsigned char gDGUSRtnFlg = 0;	// DGUS屏反馈标志
 
/****************************************************************************************
****************************************************************************************
* 时间: 2014-11-24
* 名称：
* 功能：			UART 接收处理函数
* DGUS返回帧结构： 5A A5 len cmd Addr Data
* 入口参数：
* 出口参数：
****************************************************************************************
****************************************************************************************/
uchar DGUS_GetData(unsigned short vCmd, unsigned int vAddr, unsigned int vData)
{
	switch(vAddr)
	{
		case 0x200:
			;//gBDParam[1].armLenIn[0] = vData;
			break;
		case 0x201:
			;//gBDParam[1].armLenIn[1] = vData;
			break;
	}
	return 0;
}

uchar Uart0_DGUS(void)
{
	uchar i=0;
	uchar vULen = 0;		// UART 接收字符数
	uchar vRLen = 0;		// DGUS 屏返回数据字段长度
	uchar vFlg = 0xff;
	uchar vUStr[128];

	//gUart0RcvFlg = 0;
	vULen = UART0Count;
	for(i=0; i<vULen; i++)
	{
		if(UART0Buffer[i] == DGUS_H1)
		{
			if(UART0Buffer[i+1] == DGUS_H2)		 // 找到帧头
			{
				vFlg = i;
				break;
			}
		}	 
	}
	if(vFlg < vULen)							 // 检查长度，拷贝数据
	{
		for(i=0; i<vULen; i++)
			vUStr[i] = UART0Buffer[vFlg+i];
	}
	else
		return 0;
	i=2;
	vRLen = vUStr[i++];
	if((vFlg + vRLen + 2) > vULen)		// 接收数据的长度不够
		return 0;

	UART0Count = 0;
	gDGUSRtnCmd = vUStr[i++];
	if(	gDGUSRtnCmd == 0x81)			// 读寄存器的DGUS屏应答， ADR--1B，长度为字节（1Byte）
	{
		gDGUSRtnAddr = vUStr[i++];
		gDGUSRtnLen = vUStr[i++];
		if(gDGUSRtnLen == 1)			// 读一个字节
			gDGUSRtnData = vUStr[i++];
		else if(gDGUSRtnLen == 2)		// 读两个字节
		{
			gDGUSRtnData = vUStr[i++];
			gDGUSRtnData = (gDGUSRtnData<<8) + vUStr[i++];
		}
	}
	if(	gDGUSRtnCmd == 0x83)			// 读变量存储区的DGUS屏应答， ADR--2B，长度为字（2Byte）
	{
		gDGUSRtnAddr = vUStr[i++];
		gDGUSRtnAddr = (gDGUSRtnAddr<<8) + vUStr[i++];
		gDGUSRtnLen = vUStr[i++];
		if(gDGUSRtnLen == 1)			// 读数据的长度，单位：字，2Byte
		{
			gDGUSRtnData = vUStr[i++];
			gDGUSRtnData = (gDGUSRtnData<<8) + vUStr[i++];
		}
		else if(gDGUSRtnLen == 2)		// 读取长度为 2 Word
		{
			gDGUSRtnData = vUStr[i++];
			gDGUSRtnData = (gDGUSRtnData<<8) + vUStr[i++];
			gDGUSRtnData = (gDGUSRtnData<<8) + vUStr[i++];
			gDGUSRtnData = (gDGUSRtnData<<8) + vUStr[i++];
		}
	}

	DGUS_GetData(gDGUSRtnCmd, gDGUSRtnAddr, gDGUSRtnData);

	return gDGUSRtnCmd;
}

// UART0Buffer[UART0Count]



