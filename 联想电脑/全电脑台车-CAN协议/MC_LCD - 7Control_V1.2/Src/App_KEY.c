/********************************************************************
*
*  APP_IO 与 IO 口有关的函数，key, di, do 等
*  功能：
*
********************************************************************/


#include "app_usr.h"
#include "app_key.h"
#include "LPC17xx.h"
#include "config.h"



unsigned char  gDOFlag = 0;
unsigned char  gT0SFlag = 0;
volatile unsigned long   gT0Key2Cnt = 0;  	// KEY2 5MIN 计时

//// 
/****************************************************************************
* 时间: 2014-11-15
* 名称：void Key_Press(void)
* 功能：按键响应函数
* 入口参数：
* 出口参数：
****************************************************************************/
void Key_Press(void)
{ 
	switch(gKeyValue)
	{
		case _F1:
			gKeyValue &= 0xfffe;
			break;
		case _F2:
			gKeyValue &= 0xfffd;

			break;
		case _F3:
			gKeyValue &= 0xfffb;
			break;
		case _F4:
			gKeyValue &= 0xfff7;
			break;
		case _F5:
			gKeyValue &= 0xffef;
			break;
		case _F6:
			gKeyValue &= 0xffDf;
			break;
		case _F7:
			gKeyValue &= 0xffBf;
			break;
		case _F8:
			gKeyValue &= 0xff7f;
			break;
		default:
			gKeyValue = 0;
			break;
	}
}




