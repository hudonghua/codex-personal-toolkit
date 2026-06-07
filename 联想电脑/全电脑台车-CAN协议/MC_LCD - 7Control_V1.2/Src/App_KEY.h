/********************************************************************
*
*  与 IO 口有关的函数，key, di, do 等
*  功能：
*
********************************************************************/

#ifndef ___APP_KEY_H_
#define ___APP_KEY_H_


#include "config.h"
#include "gpio.h"


#define  WP_H			GPIO2->FIOSET |= (0x01 << IO_WP);
#define  WP_L			GPIO2->FIOCLR |= (0x01 << IO_WP);
	
////  按键： F1~F4
#define  _F1			0x01
#define  _F2			0x02
#define  _F3			0x04
#define  _F4			0x08
#define  _F5			0x10
#define  _F6			0x20
#define  _F7			0x40
#define  _F8			0x80

////  各页面的按键定义
#define  CMD_SEL_NUM	8


void Key_Press(void);




#endif  //___APP_IO_H_
