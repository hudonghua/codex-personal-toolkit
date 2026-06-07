/*****************************************************************************
 *   wdt.c:  Watchdog C file for NXP LPC17xx Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.27  ver 1.00    Prelimnary version, first Release
 *
*****************************************************************************/
#include "LPC17xx.h"
#include "config.h"


unsigned long  gDIValue = 0;

/*****************************************************************************
** Function name:		GPIO_Init
**
** Descriptions:		GPIO_Init, Init DI, DO
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/

uint32_t GPIOInit( void )
{
	GPIO_TypeDef * pGPIO = NULL;

	/* P0.0~8 -> TXD RXD AI7 AI8, DI1~5; P0.23~26 -> AI1~4  */
	//PINCON->PINSEL0 = 0x0 | (0x02<<0) | (0x02<<2)  | (0x02<<4)  | (0x02<<6);
	//PINCON->PINMODE0 = 0x0;				/* 00 Pull_Up, 11 Pull_Down	*/
	//PINCON->PINSEL1 = 0x0 | (0x01<<14) | (0x01<<16) | (0x01<<18) | (0x01<<20);
	//PINCON->PINMODE1 = 0x0;				/* 00 Pull_Up, 11 Pull_Down	*/
	GPIO0->FIODIR &= ~0xe0000000;		/* P0.29/30 defined as KEY1~2 */

	/* P1.4,8,9,10,14,15 -> DI7~12; P1.19,20,22,28,29,30,31 -> NK1,NK2,WDI,NK3,NK4,AI5,AI6; P1.26 -> PWM1  */
	//PINCON->PINSEL2 = 0x0 | (0x02<<0) | (0x02<<2)  | (0x02<<4)  | (0x02<<6);
	//PINCON->PINMODE2 = 0x0;				/* 00 Pull_Up, 11 Pull_Down	*/
	//PINCON->PINSEL3 = 0x0 | (0x01<<14) | (0x01<<16) | (0x01<<18) | (0x01<<20);
	//PINCON->PINMODE3 = 0x0;				/* 00 Pull_Up, 11 Pull_Down	*/

	//// WP
	pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*2)); 		// P2.13
	pGPIO->FIODIR |= (0x01 << IO_WP);
	WP_L;
		
	//// WDI 
	pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*1)); 		// P1.4
	pGPIO->FIODIR |= (0x01 << IO_WDI);

	//// IO_MD1 
	pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*1)); 		// P1.1
	pGPIO->FIODIR |= (0x01 << IO_MD1);
	pGPIO->FIOCLR |= (0x01 << IO_MD1);

	//// IO_MD0 
	pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*0)); 		// P0.4
	pGPIO->FIODIR |= (0x01 << IO_MD0);
	pGPIO->FIOCLR |= (0x01 << IO_MD0);

	//// IO_LC1 
	pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*2)); 		// P2.8
	pGPIO->FIODIR |= (0x01 << IO_LC1);

	//// IO_LC2 
	pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*2)); 		// P2.7
	pGPIO->FIODIR |= (0x01 << IO_LC2);

	//// IO_LC3 
	pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*1)); 		// P1.17
	pGPIO->FIODIR |= (0x01 << IO_LC3);

	//// IO_LC4 
	pGPIO = (GPIO_TypeDef *)((GPIO_BASE + 0x00020*1)); 		// P1.16
	pGPIO->FIODIR |= (0x01 << IO_LC4);

	return( TRUE );
}


/*****************************************************************************
** Function name:		WDI_Init
**
** Descriptions:		GPIO_Init, Init WDI OutPin
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void WDT_Init(void)
{
	 GPIO1->FIODIR |= (0x01 << IO_WDI);	   		// P1.22, WDI OutPin
	 GPIO1->FIOCLR |= (0x01 << IO_WDI);
}


/*****************************************************************************
** Function name:		WDI_Feed
**
** Descriptions:		Feed WDT 
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void WDT_Feed(void)
{
	static unsigned char stWdi = 0;

	if(stWdi == 0)
	{
		stWdi = 1;
		GPIO1->FIOCLR |= (0x01 << IO_WDI);
	//	WP_L;
	}
	else
	{
		stWdi = 0;
		GPIO1->FIOSET |= (0x01 << IO_WDI);
//		WP_H;
	}
}


/*****************************************************************************
** Function name:		WDI_Feed
**
** Descriptions:		Feed WDT 
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void TST_Feed(void)
{
	if(GPIO0->FIOPIN & (0x01 << 27))
		GPIO0->FIOCLR |= (0x01 << 27);
	else
		GPIO0->FIOSET |= (0x01 << 27);
}

/******************************************************************************
**                            End Of File
******************************************************************************/
