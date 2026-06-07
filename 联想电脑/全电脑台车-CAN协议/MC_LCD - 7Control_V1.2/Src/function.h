/*****************************************************************************
 *   uart.c:  UART API file for NXP LPC17xx Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.27  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/

#include "config.h"	
#define  _Ramp_N		100		// TON Num	  
 
int Ramp( unsigned char vCh ,int IN, uchar ASCEND, uchar DESCEND,uint32 TIMEBASE, uchar Reset ) ;   
extern int Ramp_OUT	;
