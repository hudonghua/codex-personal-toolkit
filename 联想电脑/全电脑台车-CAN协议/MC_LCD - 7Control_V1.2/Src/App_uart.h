/********************************************************************
*  
*  串口 的功能函数
*  功能：
*
********************************************************************/

#ifndef __APP_UART_H_
#define __APP_UART_H_


#include "config.h"



//extern unsigned long  gWLRcvOk ;
//extern unsigned char gWLDType ;
//extern unsigned int   gCommOvDly ;
//extern unsigned char  gWLSStr[80];
extern unsigned int  gUart0RcvOk ;		// 接收到无线数据，且数据验证通过

extern unsigned int  gWLKeyVal ;


void Uart1_WL_Send(void);
uchar Uart1_WL_Rcv(void);


#endif  //__APP_LIJU_H_
