/*****************************************************************************
 *   wdt.h:  Header file for NXP LPC214x Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.09.01  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __GPIO_H 
#define __GPIO_H




////	DI (2)
#define  _DJ_DI			15		// P1.15	电源电压检测脚
#define  GET_DJDI		(GPIO1->FIOPIN & (0x01 << _DJ_DI)) == (0x01 << _DJ_DI)? 1:0



//// 	IO_外围器件
#define  IO_WP			13		// p2.13 24C04 WP
#define  IO_WDI			4		// P1.4	 WDI
#define  IO_MD0			4		// P0.4	 as61_MD0
#define  IO_MD1			1		// P1.1	 as61_MD1
#define  IO_AUX			0		// P1.0	 as61_AUX

#define  IO_LC1			8		// P2.8	 AD Select Ctrl_1
#define  IO_LC2			7		// P2.7	 AD Select Ctrl_2

#define  IO_LC3			17		// P1.17	 uart Select Ctrl_1
#define  IO_LC4			16		// P1.16	 uart Select Ctrl_2

////	IO_Ctrl
#define  WP_H			GPIO2->FIOSET |= (0x01 << IO_WP);
#define  WP_L			GPIO2->FIOCLR |= (0x01 << IO_WP);
#define  RST_KYL_H		GPIO1->FIOSET |= (0x01 << IO_RST_KYL);
#define  RST_KYL_L		GPIO1->FIOCLR |= (0x01 << IO_RST_KYL);
#define  RST_430_H		GPIO0->FIOSET |= (0x01 << IO_RST_430);
#define  RST_430_L		GPIO0->FIOCLR |= (0x01 << IO_RST_430);
#define  LC1_H			GPIO2->FIOSET |= (0x01 << IO_LC1);
#define  LC1_L			GPIO2->FIOCLR |= (0x01 << IO_LC1);
#define  LC2_H			GPIO2->FIOSET |= (0x01 << IO_LC2);
#define  LC2_L			GPIO2->FIOCLR |= (0x01 << IO_LC2);

#define  LC3_H			GPIO1->FIOSET |= (0x01 << IO_LC3);
#define  LC3_L			GPIO1->FIOCLR |= (0x01 << IO_LC3);
#define  LC4_H			GPIO1->FIOSET |= (0x01 << IO_LC4);
#define  LC4_L			GPIO1->FIOCLR |= (0x01 << IO_LC4);


////	DO (12 + 5)
//  输出高有效
#define  _DO_BEEP		23		// p1.23	扬声器输出脚
////	SET DO
#define  PINSET_BEEP	GPIO1->FIOSET |= (0x01 << _DO_BEEP)  	// SET
#define  PINCLR_BEEP	GPIO1->FIOCLR |= (0x01 << _DO_BEEP)  	// SET
#define  PIN_TURN_BEEP	GPIO1->FIOPIN ^= (0x01 << _DO_BEEP)

#define  PIN_TURN_TST	GPIO0->FIOPIN ^= (0x01 << 27)



 
extern uint32_t GPIOInit( void );
extern uint32_t Get_DI(void);
extern void WDT_Init(void);
extern void WDT_Feed(void);

extern void TST_Feed(void);


#endif /* end __GPIO_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
