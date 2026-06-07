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
#include "function.h"
typedef struct
{
	unsigned int  vOUT;
	unsigned int vOLD_IN;		 
} mRamp, *pmRamp;
	 

 
 mRamp gRamp[_Ramp_N];



 
 int Ramp( unsigned char vCh ,int IN, uchar ASCEND, uchar DESCEND,uint32 TIMEBASE, uchar Reset )
{
  	
	if (Reset)
	{
	   gRamp[vCh].vOLD_IN =0 ;
	   gRamp[vCh].vOUT = gRamp[vCh].vOLD_IN ;

	   return gRamp[vCh].vOUT ;		
	}	
	
	 TONx(vCh , !gTON[vCh].vQ  , TIMEBASE ) ; 
 	 if ( ( gRamp[vCh].vOUT  < gRamp[vCh].vOLD_IN ) &&	gTON[vCh].vQ==1 )	 
	 {
			gRamp[vCh].vOUT = gRamp[vCh].vOUT + ASCEND ;
			if ( gRamp[vCh].vOUT > gRamp[vCh].vOLD_IN )
			{
				gRamp[vCh].vOUT = gRamp[vCh].vOLD_IN ;	
			} 
	 }
	 else if (  (gRamp[vCh].vOUT > gRamp[vCh].vOLD_IN  ) &&	gTON[vCh].vQ==1 ) 
	 {
			if 	 ( gRamp[vCh].vOUT >= DESCEND )
			{
					gRamp[vCh].vOUT = gRamp[vCh].vOUT - DESCEND ;	
			} 
			else
			{
				gRamp[vCh].vOUT = gRamp[vCh].vOLD_IN ;
			}	 
	 }
	 
	gRamp[vCh].vOLD_IN = IN ;	 	  	  

 
	return gRamp[vCh].vOUT ;

}