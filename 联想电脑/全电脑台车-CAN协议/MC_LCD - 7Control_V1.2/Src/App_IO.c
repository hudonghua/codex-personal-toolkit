/********************************************************************
*
*  APP_IO 与 IO 口有关的函数，key, di, do 等
*  功能：
*
********************************************************************/


#include "app_io.h"
#include "LPC17xx.h"
#include "config.h"


unsigned int  gKeyValue = 0;
unsigned int  gKeyValueXY = 0;
unsigned int  gKeyValue0 = 0;

PINCON_TypeDef pGPIOCfg;

// DI  输入结果
unsigned char gDIScanFlg[_DI_MAXNUM];
// DI扫描计数( 去抖 )
unsigned char gDIScanCnt[_DI_MAXNUM];
// 按键扫描结果
unsigned char gKeyScanFlg[_KEY_MAXNUM];
unsigned char gKeyScanFlg0[_KEY_MAXNUM];
// 按键扫描计数( 去抖 )
unsigned char gKeyScanCnt[_KEY_MAXNUM];
// 按键触发标记( 按下为1，松开为0)
unsigned char gKeyTurnFlg[_KEY_MAXNUM];


unsigned int  gDIBitV = 0;			// 用位定义的DI
unsigned int  gPIVal = 0;				// PI
unsigned int  gPIValAdd = 0;				// PI TEMP
unsigned int  gPIVal1 = 0;	
unsigned int  gPIVal2 = 0;	
unsigned int  gPIVal3 = 0;	
unsigned int  gPIVal4 = 0;	
unsigned int  gPIValAdd1 = 0;
unsigned int  gPIValAdd2 = 0;
unsigned int  gPIValAdd3 = 0;
unsigned int  gPIValAdd4 = 0;

unsigned char  gDOBitV[8];			// 用位定义的Do输出记录，1bit -- 1路DO	
unsigned char  gDOPNum = 32;

unsigned char  gXNLeft = 0;
unsigned char  gXNRight = 0;

//// 按键扫描(低有效) 顺序---- 从左到右，
unsigned char gKeyScanTable[_KEY_MAXNUM][2] = {
	{0, _KEY1},  {0, _KEY2}, {0, _KEY3}, {0, _KEY4}, {0, _KEY5}, {2, _KEY6},  {2, _KEY7}, {0, _KEY8}, 
	{_EOT, _EOT}		  
};
//// 按键灯 顺序---- 从左到右，
unsigned char gKeyLedTable[_LED_MAXNUM][2] = {
	{4, _LED7}, {4, _LED5}, {0, _LED3}, {0, _LED1}, {0, _LED8}, {0, _LED6}, {2, _LED4},  {2, _LED2},   {1, _LED9_CPU}, 
//	{0, _LED1},  {2, _LED2}, {0, _LED3}, {2, _LED4}, {4, _LED5}, {0, _LED6},  {4, _LED7}, {0, _LED8},  {1, _LED9_CPU}, 
	{_EOT, _EOT}		  
};

//// DI扫描(1~3低有效，4~5高有效) 顺序---- 从左到右，
/*
unsigned char gDIScanTable[_DI_MAXNUM][2] = {
	{1, _DI_1}, {_CNT, _DI_2},  {0, _DI_2},  {3, _DI_3},  {1, _DI_4},  {1, _DI_5},  
	{1, _DI_6},  {3, _DI_7},  {1, _DI_8},  {1, _DI_9},  {1, _DI_10}, 
	{1, _DI_11}, {0, _DI_12}, {0, _DI_13}, 
	{_EOT, _EOT}		  
};
*/	
////  _DI_7, _DI_5 为低有效 
unsigned char gDIScanTable[_DI_MAXNUM][2] = {
	{1, _DI_1}, {_CNT, 1}, {3, _DI_7}, {_CNT, 3}, {3, _DI_3}, 
	{_CNT, 5}, {_CNT, 6}, {1, _DI_5}, {1, _DI_6}, {1, _DI_4}, {_CNT, 9}, 
	{0, _DI_12}, {0, _DI_13},  {1, _DI_8},  {1, _DI_9},   
	{1, _DI_10}, {1, _DI_11},  {0, _DI_2},  {0, _DI_SJ},  // P0.25 -- DI-SJ
	
	{_EOT, _EOT}		  
};

//// DO，输出高有效
unsigned char gDOSetTable[_DO_MAXNUM][2] = {
	{2, _DO_1}, {2, _DO_2}, {1, _DO_3}, {2, _DO_4}, {1, _DO_5}, {1, _DO_6}, 	
	{1, _DO_7}, {2, _DO_8}, {1, _DO_9},{2, _DO_10},{1, _DO_11},{2, _DO_12},
	{_EOT, _EOT} 	  
};
/*
unsigned char gDOSetTable[_DO_MAXNUM][2] = {
	{1, _DO_6}, {1, _DO_5}, {1, _DO_3}, {1, _DO_4}, {1, _DO_7}, {2, _DO_8}, {1, _DO_2},	
	{2, _DO_9}, {2, _DO_10},{2, _DO_11},{2, _DO_12},{2, _DO_13},{2, _DO_14},{1, _DO_1}, 
		{2, _DO_15},{2, _DO_16},
	{_EOT, _EOT} 	  
};
*/
mTON gTON[_TON_N];
mTOF gTOF[_TOF_N];
mTOD gTODH[_TOD_N];
mTOD gTODL[_TOD_N];
mTODXOR gTODXOR[_TOD_N];

//// TON
char TONx(unsigned char vCh, unsigned char in, unsigned int pt)
{
	if(vCh >= _TON_N)
		return 0;
	if(in)
	{
		if(gTON[vCh].vIn0 == 0)
		{
			gTON[vCh].vPT = pt;
			gTON[vCh].vIn = in;
			gTON[vCh].vIn0 = in;
		}
		if(	gTON[vCh].vPT == 0)
			gTON[vCh].vQ = 1;							
	}
	else
	{
			gTON[vCh].vPT = 0;
			gTON[vCh].vIn = 0;
			gTON[vCh].vIn0 = 0;
			gTON[vCh].vQ = 0;		
	}
	return 1;
}
//// TOF
char TOFx(unsigned char vCh, unsigned char in, unsigned int pt)
{
	if(vCh >= _TOF_N)
		return 0;
	if(in)
	{
		gTOF[vCh].vPT = pt;
		gTOF[vCh].vPT0 = pt;
		gTOF[vCh].vIn = in;
		gTOF[vCh].vQ = 1;
	}
	else
	{
		if(	gTOF[vCh].vPT == 0)
			gTOF[vCh].vQ = 0;
		gTOF[vCh].vPT0 = 0;
		gTOF[vCh].vIn = 0;
	}
	return 1;
}


////  TOD
char TODx_H(unsigned char vCh, unsigned char vDI, unsigned char vCyc)
{
	if(vCh >= _TOD_N)
		return 0;
	if((vDI > 0) && (gTODH[vCh].vDI0 == 0))
	{
		gTODH[vCh].vDI0 = vDI;
		gTODH[vCh].vQ = vCyc;
		gTODH[vCh].vInc++;
	}
	if(vDI == 0)
		gTODH[vCh].vDI0 = 0;
	return 1;
}



char TODx_L(unsigned char vCh, unsigned char vDI, unsigned char vCyc)
{
	if(vCh >= _TOD_N)
		return 0;
	if((vDI == 0) && (gTODL[vCh].vDI0 > 0))
	{
		gTODL[vCh].vDI0 = vDI;
		gTODL[vCh].vQ = vCyc;
		gTODL[vCh].vInc++;
	}
	if(vDI > 0)
		gTODL[vCh].vDI0 = 1;
	return 1;
}


/****************************************************************************
*
*  字符串 与 数据 格式转换函数
*
*****************************************************************************/
unsigned long DStrToInt(unsigned char* vInBuf)
{
	unsigned long vLout = 0;
	unsigned char i = 0;
	unsigned char ch = 0;

	for(i=0; i<16; i++)
	{
		ch = *vInBuf++;
		if(ch == 0)
			return vLout;
		vLout = vLout * 10;
		if((ch>='0') && (ch<='9'))
		{
			vLout += ch-'0';
		}		
	}
	return vLout;
}
unsigned long HStrToInt(unsigned char* vInBuf)
{
	unsigned long vLout = 0;
	unsigned char i = 0;
	unsigned char ch = 0;

	for(i=0; i<16; i++)
	{
		ch = *vInBuf++;
		if(ch == 0)
			return vLout;
		vLout <<= 4;
		if((ch>='0') && (ch<='9'))
		{
			ch = ch-'0';
		}		
		if((ch>='A') && (ch<='F'))
		{
			ch = ch - 'A' + 10;
		}		
		if((ch>='a') && (ch<='f'))
		{
			ch = ch - 'a' + 10;
		}	
		vLout += ch;
	}
	return vLout;
}
//// 格式转换，如: "1F342C556d4f3341" --> str[]={0x1F,0x34,0x2C,0x55,0x6D,0x4F,0x33,0x41}
void HBCDtoHStr(unsigned char* vInBuf, unsigned char* vOutBuf)
{
	unsigned char i = 0;
	unsigned char j = 0;
	unsigned char ch = 0;
	unsigned char chOut = 0;

	for(i=0; i<32; )
	{
		i++;
		ch = *vInBuf++;
		if(ch == 0)
			return ;
		if((ch>='0') && (ch<='9'))
		{
			ch = ch-'0';
		}		
		if((ch>='A') && (ch<='F'))
		{
			ch = ch - 'A' + 10;
		}		
		if((ch>='a') && (ch<='f'))
		{
			ch = ch - 'a' + 10;
		}	
		chOut = (chOut<< 4) + ch;
		if((i & 0x01)== 0)		// 偶数个字节
		{
			vOutBuf[j++] = chOut;
			chOut = 0;
		}
	}
}


void IO_Init(void)
{
	unsigned char i = 0;
//	unsigned char v = 0;
	GPIO_TypeDef * pGPIO = NULL;

	GPIOInit();	

	// KEY Init	
	for(i=0; i<_KEY_MAXNUM; i++)
	{
		if(gKeyScanTable[i][0] == _EOT)
		{
			break;
		}
		pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*gKeyScanTable[i][0]));
		pGPIO->FIODIR &= ~(0x01 << gKeyScanTable[i][1]);
	}
	// DI Init	
	for(i=0; i<_DI_MAXNUM; i++)
	{
		if(gDIScanTable[i][0] == _EOT)
		{
			break;
		}
		pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*gDIScanTable[i][0]));
		pGPIO->FIODIR &= ~(0x01 << gDIScanTable[i][1]);
	}
	// LED
	for(i=0; i<_LED_MAXNUM; i++)
	{
		if(gKeyLedTable[i][0] == _EOT)
		{
			break;
		}
		pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*gKeyLedTable[i][0]));
		pGPIO->FIODIR |= (0x01 << gKeyLedTable[i][1]);
		pGPIO->FIOCLR |= (0x01 << gKeyLedTable[i][1]);
	}
	// DO
	for(i=0; i<_DO_MAXNUM; i++)
	{
		if(gDOSetTable[i][0] == _EOT)
		{
			gDOPNum = i;
			break;
		}
		pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*gDOSetTable[i][0]));
		pGPIO->FIODIR |= (0x01 << gDOSetTable[i][1]);
		pGPIO->FIOCLR |= (0x01 << gDOSetTable[i][1]);
	}
} 


/****************************************************************************
* 时间: 2014-11-15
* 名称：void Key_Scan(void)
* 功能：按键扫描函数
* 入口参数：
* 出口参数：
****************************************************************************/
void Key_Scan(void)
{
	unsigned char i = 0;
	unsigned char v = 0;
//	unsigned char vtmp = 0;

	GPIO_TypeDef * pGPIO = NULL;
	for(i=0; i<_KEY_MAXNUM; i++)
	{
		if(gKeyScanTable[i][0] == _EOT)
		{
			break;
		}
		pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*gKeyScanTable[i][0]));
		// 低有效

		v = ((pGPIO-> FIOPIN & (0x01 << gKeyScanTable[i][1])) == (0x01 << gKeyScanTable[i][1])? 1:0);
		if(v == 0)
		{
			{
				if(gKeyScanCnt[i] < _KEY_SCAN_CNT+20)
					gKeyScanCnt[i]++;
				if(gKeyScanCnt[i] == _KEY_SCAN_CNT)
				{
					gKeyScanFlg[i] = 1;
					gKeyValue |= (1<<i);
					gKeyValueXY |= (1<<i);
				//	gKeyValue0 = gKeyValue;
				}
			}

		}
		else
		{
			if(gKeyScanCnt[i] > 0)
				gKeyScanCnt[i]>>=1;
			if(gKeyScanCnt[i] == 0)
			{
				gKeyScanFlg[i] = 0;
				gKeyValue &= ~(1<<i);
				gKeyValueXY &= ~(1<<i);
			}
		}
	}
					gKeyValue0 = gKeyValue;
}

void Key_Led_On(void)
{
	unsigned char i = 0;
	GPIO_TypeDef * pGPIO = NULL;

	for(i=0; i<_KEY_MAXNUM; i++)
	{
		if(gKeyScanTable[i][0] == _EOT)
		{
			break;
		}
		pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*gKeyLedTable[i][0]));
		if(_BitV(gKeyValueXY, i))
		{
			pGPIO->FIOSET |= (0x01 << gKeyLedTable[i][1]);
		}
		else
		{
			pGPIO->FIOCLR |= (0x01 << gKeyLedTable[i][1]);
		}
	}
	
}

/****************************************************************************
* 时间: 2014-11-15
* 名称：void DI_Scan(void)
* 功能：DI扫描函数
* 入口参数：
* 出口参数：
****************************************************************************/
void DI_Scan(void)
{
	unsigned char i = 0;
	unsigned char k = 0;
	unsigned char v = 0;
	unsigned short vDICnt = _DI_SCAN_CNT;				// 按键保持时间
//	unsigned char vtmp = 0;

	GPIO_TypeDef * pGPIO = NULL;
	for(i=0; i<_DI_MAXNUM; i++)
	{
		if(gDIScanTable[i][0] == _EOT)
		{
			break;
		}
		if(gDIScanTable[i][0] == _CNT)
		{
			continue;
		}
		pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*gDIScanTable[i][0]));
		// 5~12 外部高有效
		if( (i != 2) && (i != 7))	
		{
			vDICnt = _DI_SCAN_CNT;	
			v = ((pGPIO->FIOPIN & (0x01 << gDIScanTable[i][1])) == (0x01 << gDIScanTable[i][1])? 1:0);
			if(v == 0)
			{
				if(gDIScanCnt[i] < vDICnt+2)
					gDIScanCnt[i]++;
				if(gDIScanCnt[i] == vDICnt)
				{
					gDIScanFlg[i] = 1;
					gDIBitV |= (1<<i);
				}
			}
			else
			{
				if(gDIScanCnt[i] > 0)
					gDIScanCnt[i]--;
				if(gDIScanCnt[i] == 0)
				{
					gDIScanFlg[i] = 0;
					gDIBitV &= ~(1<<i);
				}
			}
		}
		// 0~4 低有效
		else
		{
			v = ((pGPIO->FIOPIN & (0x01 << gDIScanTable[i][1])) == (0x01 << gDIScanTable[i][1])? 1:0);
			if(v == 1)
			{
				if(gDIScanCnt[i] < vDICnt+2)
					gDIScanCnt[i]++;
				if(gDIScanCnt[i] == vDICnt)		// 按键已按下
				{
					gDIScanFlg[i] = 1;
					gDIBitV |= (1<<i);
				}
			}
			else
			{
				if(gDIScanCnt[i] > 0)
					gDIScanCnt[i]--;
				if(gDIScanCnt[i] == 0)		// 按键已松开
				{
					gDIScanFlg[i] = 0;
					gDIBitV &= ~(1<<i);
				}
			}
		}
	}
	////  模拟量端口 的 DI 转换
	k = 1;
	if(gADV_mV[k] > 2000)		// AMP35.9  
		gDIBitV |= (1<<1);
	else if(gADV_mV[k] < 1200)		   
		gDIBitV &= ~(1<<1);
	
	k = 2;
	if(gADV_mV[k] > 2000)		// AMP35.19  
		gDIBitV |= (1<<3);
	else if(gADV_mV[k] < 1200)		   
		gDIBitV &= ~(1<<3);
	
	k = 3;
	if(gADV_mV[k] > 2000)		// AMP35.21  
		gDIBitV |= (1<<5);
	else if(gADV_mV[k] < 1200)		   
		gDIBitV &= ~(1<<5);
	
	k = 4;
	if(gADV_mV[k] > 2000)		// AMP35.22  
		gDIBitV |= (1<<6);
	else if(gADV_mV[k] < 1200)		   
		gDIBitV &= ~(1<<6);
	
	k = 5;
	if(gADV_mV[k] > 2000)		// AMP35.31  
		gDIBitV |= (1<<10);
	else if(gADV_mV[k] < 1200)		   
		gDIBitV &= ~(1<<10);
	
}

/****************************************************************************
* 时间: 2015-5-25
* 名称：void DOx_Set(void)
* 功能：DO 输出控制  设置DOx输出高或低
* 入口参数：
* 出口参数： gRunInfo.vDJStart
****************************************************************************/
unsigned char gPWM_DO_Tab[6][2] = { {0, 7}, {1, 9}, {2, 0}, {3, 1}, {4, 3}, {5, 11}};
void PWMx_Set(unsigned char vX, unsigned char vPre)
{
	GPIO_TypeDef * pGPIO = NULL;
	unsigned char i = 0;
	unsigned char vPreV = 0;

	if(vX >= gDOPNum )			// 最后3个DO用作 模拟 PWM
		return;
	
	vPreV = vPre;
	if(vPreV > 100)
		vPreV = 100;
	for(i=0; i<6; i++)
	{
		if(gPWM_DO_Tab[i][1] == vX)
			PWM_SetX(gPWM_DO_Tab[i][0], vPreV);
	}
}
void DOx_Set(unsigned char vX, unsigned char vH_L)
{
	GPIO_TypeDef * pGPIO = NULL;

	if(vX >= gDOPNum)
		return;
	pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*gDOSetTable[vX][0]));
	if(vH_L)
	{
		pGPIO->FIOSET |= (0x01 << gDOSetTable[vX][1]);
		gDOBitV[vX/8] |= (1<<(vX & 0x07));
	}
	else
	{
		pGPIO->FIOCLR |= (0x01 << gDOSetTable[vX][1]);
		gDOBitV[vX/8] &= ~(1<<(vX & 0x07));
	}
}
void DOx_IntSet(unsigned char vX, unsigned char vH_L)
{
	GPIO_TypeDef * pGPIO = NULL;

	if(vX >= gDOPNum)
		return;
	pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*gDOSetTable[vX][0]));
	if(vH_L)
	{
		pGPIO->FIOSET |= (0x01 << gDOSetTable[vX][1]);
		gDOBitV[vX/8] |= (1<<(vX & 0x07));
	}
	else
	{
		pGPIO->FIOCLR |= (0x01 << gDOSetTable[vX][1]);
		gDOBitV[vX/8] &= ~(1<<(vX & 0x07));
	}
}

void DOxx_Set_All(unsigned int vXX)
{
	unsigned char i = 0;
	unsigned char vH_L = 0;
	
	GPIO_TypeDef * pGPIO = NULL;
	
	for(i=0; i<_DO_MAXNUM; i++)
	{
		if(i >= gDOPNum)
			return;
		vH_L = _BitV(vXX, i);
		pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*gDOSetTable[i][0]));
		if(vH_L)
		{
			pGPIO->FIOSET |= (0x01 << gDOSetTable[i][1]);
			gDOBitV[i/8] |= (1<<(i & 0x07));
		}
		else
		{
			pGPIO->FIOCLR |= (0x01 << gDOSetTable[i][1]);
			gDOBitV[i/8] &= ~(1<<(i & 0x07));
		}
	}
}


