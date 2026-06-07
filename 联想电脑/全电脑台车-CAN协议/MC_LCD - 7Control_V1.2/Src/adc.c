/*****************************************************************************
 *   adc.c:  ADC module file for NXP LPC17xx Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.25  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "lpc17xx.h"
#include "config.h"
#include "adc.h"

volatile uint32_t ADC0Value[ADC_NUM];
volatile uint32_t ADC0IntDone = 0;
volatile unsigned  stAdChn = 0;


uint16_t   X000,X001,X002,X003,X004,X005,X006,X007;
uint16_t   X100,X101,X102,X103,X104,X105,X106,X107;
uint16_t   X200,X201,X202,X203,X204,X205,X206,X207;
uint16_t   Y100,Y101,Y102,Y103,Y104,Y105,Y106,Y107;	
uint16_t   Y200,Y201,Y202,Y203,Y204,Y205,Y206,Y207;
unsigned int   ADCRst[ADC_NUM];
unsigned int   ADCBsl[ADC_NUM];

unsigned char  gXYSel = 0;
unsigned int  gADConvFlg = 0;		// AD 采样完成的标记


#if BURST_MODE
volatile uint32_t channel_flag; 
#endif

#if ADC_INTERRUPT_FLAG
/******************************************************************************
** Function name:		ADC_IRQHandler
**
** Descriptions:		ADC interrupt handler
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
#if 0
void ADC_IRQHandler (void)  
{
	static int CnvNum = 0;
	static unsigned char stFlg = 1;
	uint32_t regVal;
	uint32_t vAd = 0;
	uint32_t vAd0 = 0;
	unsigned char i = 0;  

  //stAdChn = (ADC -> ADGDR>>24)&0x07;
	regVal = ADC -> ADSTAT;		/* Read ADC will clear the interrupt */
	if ( regVal & 0x0000FF00 )	/* check OVERRUN error first */
	{
		regVal = (regVal & 0x0000FF00) >> 0x08;
	/* if overrun, just read ADDR to clear */
	/* regVal variable has been reused. */
 #if 1
	switch ( regVal )
	{
		case 0x01:
			regVal = ADC -> ADDR0;
			break;
		case 0x02:
			regVal = ADC -> ADDR1;
			break;
		case 0x04:
			regVal = ADC -> ADDR2;
			break;
		case 0x08:
			regVal = ADC -> ADDR3;
			break;
		
		default:
			break;
	}
#else
	  regVal = ADC -> ADGDR;
#endif
	return;
  }
    
	if ( regVal & ADC_ADINT )
	{
		vAd = ( ADC -> ADGDR >> 4 ) & 0xFFF;
	#if 1
		switch ( regVal & 0xFF )	/* check DONE bit */
		{
			case 0x01:
				{
					vAd = ( ADC -> ADDR0 >> 4 ) & 0xfFF;
					if(vAd <= _ADC_CMP_MAX)
					{
						ADC0Value[0] += vAd;
					}
				}
				break;
			case 0x02:
				{
					vAd = ( ADC -> ADDR1 >> 4 ) & 0xfFF;
					if(vAd < _ADC_CMP_MAX)
					{
						ADC0Value[1] += vAd;
					}
				}
				break;
			case 0x04:
				{
					vAd = ( ADC -> ADDR2 >> 4 ) & 0xfFF;
					if(vAd < _ADC_CMP_MAX)
					{
						ADC0Value[2] += vAd;
					}
				}
				break;
			case 0x08:
				{
					vAd = ( ADC -> ADDR3 >> 4 ) & 0xfFF;
					if(vAd < _ADC_CMP_MAX)
					{
						ADC0Value[3] += vAd;
			CnvNum++;
					}
				}
					break;
			default:
			break;
		}
		#else
		   ADC0Value[stAdChn] += (ADC -> ADGDR>>4) & 0xFFF;
		   if(stAdChn == 3)
			CnvNum++;
		#endif
  	}
	if(CnvNum >= 128)	   			// 16次取平均
	{
		for(i=0; i<ADC_NUM; i++)
		{
			ADCRst[i] = (ADC0Value[i] /128);
			ADC0Value[i] = 0;
		}
		CnvNum = 0;
		gADConvFlg++;
	}
//  ADC -> ADCR &= 0xF8FFFFFF;	/* stop ADC now */ 
//  ADC0IntDone = 1;
//		stAdChn = 0;
	ADC -> ADCR &= 0xFFFFFF00;
	ADC -> ADCR |= (1 << (stAdChn));		//(1 << 24)
	ADC -> ADCR |= 0x01000000 ;
	stAdChn++;
	if(stAdChn > 3)
		stAdChn = 0;
}
#else
void ADC_IRQHandler (void)  
{
	static int CnvNum = 0;
	static unsigned char stFlg = 1;
	uint32_t regVal;
	uint32_t vAd = 0;
	uint32_t vAd0 = 0;
	unsigned char i = 0;  

  //stAdChn = (ADC -> ADGDR>>24)&0x07;
	regVal = ADC -> ADSTAT;		/* Read ADC will clear the interrupt */
	if ( regVal & 0x0000FF00 )	/* check OVERRUN error first */
	{
		regVal = (regVal & 0x0000FF00) >> 0x08;
	/* if overrun, just read ADDR to clear */
	/* regVal variable has been reused. */
 #if 1
	switch ( regVal )
	{
		case 0x01:
			regVal = ADC -> ADDR0;
			break;
		case 0x02:
			regVal = ADC -> ADDR1;
			break;
		case 0x04:
			regVal = ADC -> ADDR2;
			break;
		case 0x08:
			regVal = ADC -> ADDR3;
			break;
		
		default:
			break;
	}
#else
	  regVal = ADC -> ADGDR;
#endif
	return;
  }
    
	if ( regVal & ADC_ADINT )
	{
	vAd = ( ADC -> ADGDR >> 4 ) & 0xFFF;
	if(vAd > 128)
		vAd = 0;
	#if 1
		switch ( regVal & 0xFF )	/* check DONE bit */
		{
			case 0x01:
				if(stFlg)
				{
					vAd = ( ADC -> ADDR0 >> 4 ) & 0xfFF;
					if(vAd < _ADC_CMP_MAX)
					{
						ADC0Value[gXYSel*4+0] += vAd;
						vAd0 = vAd;
					}
					else
						;//ADC0Value[gXYSel*4+0] += vAd0;
				}
				else
				{
					regVal = ADC -> ADDR0;
					//ADC0Value[gXYSel*4+0] += vAd0;
				}
				break;
			case 0x02:
				//if(stFlg)
				//ADC0Value[gXYSel*4+1] += ( ADC -> ADDR1 >> 4 ) & 0x7FF;
				//else
				//	regVal = ADC -> ADDR1;
				if(stFlg)
				{
					vAd = ( ADC -> ADDR1 >> 4 ) & 0xfFF;
					if(vAd < _ADC_CMP_MAX)
					{
						ADC0Value[gXYSel*4+1] += vAd;
						vAd0 = vAd;
					}
					else
						;//ADC0Value[gXYSel*4+1] += vAd0;

					gXYSel++;
					if(gXYSel >= 4)			// gXYSel = 0/1/2/3
					{
						CnvNum++;
						gXYSel = 0;
					}
					if(gXYSel == 0)
						HC4052_XY0
					if(gXYSel == 1)
						HC4052_XY1
					if(gXYSel == 2)
						HC4052_XY2
					if(gXYSel == 3)
						HC4052_XY3
					stFlg = 0;
					stFlg = 0;
					stFlg = 0;
					stFlg = 0;
				}
				else
				{
					regVal = ADC -> ADDR1;
					//ADC0Value[gXYSel*4+1] += vAd0;
					stFlg = 1;
				}

				break;
			case 0x04:
				if(stFlg)
				{
					vAd = ( ADC -> ADDR2 >> 4 ) & 0xfFF;
					if(vAd < _ADC_CMP_MAX)
					{
						ADC0Value[gXYSel*4+2] += vAd;
						vAd0 = vAd;
					}
					else
						;//ADC0Value[gXYSel*4+2] += vAd0;
				}
				else
				{
					regVal = ADC -> ADDR2;
					//ADC0Value[gXYSel*4+2] += vAd0;
				}
				break;
			case 0x08:
				if(stFlg)
				{
					//ADC0Value[gXYSel*4+3] += ( ADC -> ADDR3 >> 4 ) & 0x7FF;
					vAd = ( ADC -> ADDR3 >> 4 ) & 0xfFF;
					if(vAd < _ADC_CMP_MAX)
					{
						ADC0Value[gXYSel*4+3] += vAd;
						vAd0 = vAd;
					}
					else
						;//ADC0Value[gXYSel*4+3] += vAd0;

					gXYSel++;
					if(gXYSel >= 3)			// gXYSel = 0/1/2
					{
						CnvNum++;
						gXYSel = 0;
					}
					if(gXYSel == 0)
						HC4052_XY0
					if(gXYSel == 1)
						HC4052_XY1
					if(gXYSel == 2)
						HC4052_XY2
					stFlg = 0;
					stFlg = 0;
					stFlg = 0;
					stFlg = 0;
				}
				else
				{
					regVal = ADC -> ADDR3;
					//ADC0Value[gXYSel*4+3] += vAd0;
					stFlg = 1;
				}
				break;
			default:
			break;
		}
		#else
		   ADC0Value[stAdChn] += (ADC -> ADGDR>>4) & 0xFFF;
		   if(stAdChn == 3)
			CnvNum++;
		#endif
  	}
	if(CnvNum >= 32)	   			// 16次取平均
	{
		for(i=0; i<ADC_NUM; i++)
		{
			ADCRst[i] = (ADC0Value[i] >> 5);
			ADC0Value[i] = 0;
		}
		CnvNum = 0;
		gADConvFlg++;
//		WDT_Feed();
	}
//  ADC -> ADCR &= 0xF8FFFFFF;	/* stop ADC now */ 
//  ADC0IntDone = 1;
	ADC -> ADCR &= 0xFFFFFF00;
	ADC -> ADCR |= (1 << (stAdChn));		//(1 << 24)
	ADC -> ADCR |= 0x01000000 ;
	stAdChn++;
	stAdChn &= 0x01;
}
#endif
#endif




uint32_t ADC0ReadAverageU40( uint8_t channelNum1 )
{
	uint8_t i;
	uint32_t ADC_Temp[8]={0,0,0,0,0,0,0,0};
	uint32_t ADC_Databuf = 0;
	for(i=0;i<4;i++)
	{
	  ADC_Temp[i] = ADC0Read(channelNum1);
		ADC_Databuf += ADC_Temp[i];
	}
	ADC_Databuf =  ADC_Databuf>>2;
	
	return ( ADC_Databuf );
}



/*****************************************************************************
** Function name:		ADCInit
**
** Descriptions:		initialize ADC channel
**
** parameters:			ADC clock rate
** Returned value:		true or false
** 
*****************************************************************************/
uint32_t ADCInit( uint32_t vADC_Clk )
{
	uint32_t pclkdiv, pclk;

	stAdChn = 0;
	
	/* Enable CLOCK into ADC controller */
	SC->PCONP |= (1 << 12);

	/*  the related pins are set to ADC inputs, AD0.0~3--p0.23/24/25/26 */

	PINCON->PINSEL1 &= ~0x0003C000;	/* P0.23~26, A0.0~1, function 01 */
	PINCON->PINSEL1 |=  0x00014000;
//	HC4052_XY0;

	/* By default, the PCLKSELx value is zero, thus, the PCLK for
	all the peripherals is 1/4 of the SystemFrequency. */
	/* Bit 24~25 is for UART0 */
	pclkdiv = (SC->PCLKSEL0 >> 24) & 0x03;
	switch ( pclkdiv )
	{
		case 0x00:
		default:
		  pclk = SystemFrequency/4;
		break;
		case 0x01:
		  pclk = SystemFrequency;
		break; 
		case 0x02:
		  pclk = SystemFrequency/2;
		break; 
		case 0x03:
		  pclk = SystemFrequency/8;
		break;
	}

	ADC->ADCR = ( 0x001 << 0 ) | 	/* SEL=0xff,select channel 0~7 on ADC0~7 */
		( ( pclk  / vADC_Clk - 1 ) << 8 ) |  /* CLKDIV = Fpclk / ADC_Clk - 1 */ 
		( 0 << 16 ) | 		/* BURST = 1, BURST, hardware controlled */
		( 0 << 17 ) |  		/* CLKS = 0, 11 clocks/10 bits */
		( 1 << 21 ) |  		/* PDN = 1, normal operation */
		( 0 << 24 );  		/* START = 0 A/D conversion stops */
	//	( 0 << 27 );		/* EDGE = 0 (CAP/MAT singal falling,trigger A/D conversion) */ 

  /* If POLLING, no need to do the following */
#if ADC_INTERRUPT_FLAG
	ADC->ADINTEN = 0x103;		/* Enable all interrupts */
	NVIC_EnableIRQ(ADC_IRQn);
#endif

  //ADC->ADCR |=  (1<<24);                    

  return (TRUE);
}

void ADC_StartCnv (void) {
  ADC->ADCR &= ~(7<<24);                    
  ADC->ADCR |=  (1<<24);                    
}
void ADC_StopCnv (void) {
  ADC->ADCR &= ~(7<<24);     
  }
/*****************************************************************************
** Function name:		ADC0Read
**
** Descriptions:		Read ADC0 channel
**
** parameters:			Channel number
** Returned value:		Value read, if interrupt driven, return channel #
** 
*****************************************************************************/
uint32_t ADC0Read( uint8_t channelNum )
{	 unsigned char i = 0;
#if !ADC_INTERRUPT_FLAG
  uint32_t regVal, ADC_Data;
#endif

  /* channel number is 0 through 7 */
  if ( channelNum >= ADC_NUM )
  {
	channelNum = 0;		/* reset channel number to 0 */
  }
  ADC->ADCR &= 0xFFFFFF00;
  ADC->ADCR |= (1 << 24) | (1 << channelNum);	
				/* switch channel,start A/D convert */
#if !ADC_INTERRUPT_FLAG
  while ( 1 )			/* wait until end of A/D convert */
  {
	regVal = *(volatile unsigned long *)(ADC_BASE 
			+ ADC_OFFSET + ADC_INDEX * channelNum);
	/* read result of A/D conversion */
	i++ ;
	if ( ( regVal & ADC_DONE ) || (i>250) )
	{
	  break;
	}
  }	
        
  ADC->ADCR &= 0xF8FFFFFF;	/* stop ADC now */    
  if ( regVal & ADC_OVERRUN )	/* save data when it's not overrun, otherwise, return zero */
  {
	return ( 0 );
  }
  ADC_Data = ( regVal >> 4 ) & 0xFFF;
  return ( ADC_Data );	/* return A/D conversion value */
#else
  return ( channelNum );	/* if it's interrupt driven, the ADC reading is 
							done inside the handler. so, return channel number */
#endif
}

/*****************************************************************************
** Function name:		ADC0BurstRead
**
** Descriptions:		Use burst mode to convert multiple channels once.
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void ADCBurstRead( void )
{
  if ( ADC->ADCR & (0x7<<24) )
  {
	ADC->ADCR &= ~(0x7<<24);
  }
  /* Test channel 5,6,7 using burst mode because they are not shared
  with the JTAG pins. */
  ADC->ADCR &= ~0xFF;
  /* Read all channels, 0 through 7. */
  ADC->ADCR |= (0xFF);
  ADC->ADCR |= (0x1<<16);	/* Set burst mode and start A/D convert */
  return;						/* the ADC reading is done inside the 
								handler, return 0. */
}




#if 1
/*****************************************************************************
** Function name:		ADC_Bessel
**
** Descriptions:		对ADC通道的数据进行BESSEL滤波.
**
** parameters:			AD CHN
** Returned value:		None
** 
*****************************************************************************/
void ADC_Bessel(uint16_t chn, uint16_t vADvalue)
{
	float fV = 0.0;

	switch (chn & 0xff)
	{
		case 0x01:
			X200 = X100;
			X100 = X000;
			X000 = vADvalue;
			fV = X000 * _A0;
			fV += X100 * _A1;
			fV += X200 * _A2;
			fV += Y100 * _B1;
			fV += Y200 * _B2;
			ADCBsl[0] = (uint16_t)(fV);
			Y200 = Y100;
			Y100 = ADCBsl[0];
			break;
		case 0x02:
			X201 = X101;
			X101 = X001;
			X001 = vADvalue;
			fV = X001 * _A0;
			fV += X101 * _A1;
			fV += X201 * _A2;
			fV += Y101 * _B1;
			fV += Y201 * _B2;
			ADCBsl[1] = (uint16_t)(fV);
			Y201 = Y101;
			Y101 = ADCBsl[1];
			break;
		case 0x04:
			X202 = X102;
			X102 = X002;
			X002 = vADvalue;
			fV = X002 * _A0;
			fV += X102 * _A1;
			fV += X202 * _A2;
			fV += Y102 * _B1;
			fV += Y202 * _B2;
			ADCBsl[2] = (uint16_t)(fV);
			Y202 = Y102;
			Y102 = ADCBsl[2];
			break;
		case 0x08:
			X203 = X103;
			X103 = X003;
			X003 = vADvalue;
			fV = X003 * _A0;
			fV += X103 * _A1;
			fV += X203 * _A2;
			fV += Y103 * _B1;
			fV += Y203 * _B2;
			ADCBsl[3] = (uint16_t)(fV);
			Y203 = Y103;
			Y103 = ADCBsl[3];
			break;
	#if 1
		case 0x10:
			X204 = X104;
			X104 = X004;
			X004 = vADvalue;
			fV = X004 * _A0;
			fV += X104 * _A1;
			fV += X204 * _A2;
			fV += Y104 * _B1;
			fV += Y204 * _B2;
			ADCBsl[4] = (uint16_t)(fV);
			Y204 = Y104;
			Y104 = ADCBsl[4];
			break;
		case 0x20:
			X205 = X105;
			X105 = X005;
			X005 = vADvalue;
			fV = X005 * _A0;
			fV += X105 * _A1;
			fV += X205 * _A2;
			fV += Y105 * _B1;
			fV += Y205 * _B2;
			ADCBsl[5] = (uint16_t)(fV);
			Y205 = Y105;
			Y105 = ADCBsl[5];
			break;
		case 0x40:
			X206 = X106;
			X106 = X006;
			X006 = vADvalue;
			fV = X006 * _A0;
			fV += X106 * _A1;
			fV += X206 * _A2;
			fV += Y106 * _B1;
			fV += Y206 * _B2;
			ADCBsl[6] = (uint16_t)(fV);
			Y206 = Y106;
			Y106 = ADCBsl[6];
			break;
		case 0x80:
			X207 = X107;
			X107 = X007;
			X007 = vADvalue;
			fV = X007 * _A0;
			fV += X107 * _A1;
			fV += X207 * _A2;
			fV += Y107 * _B1;
			fV += Y207 * _B2;
			ADCBsl[7] = (uint16_t)(fV);
			Y207 = Y107;
			Y107 = ADCBsl[7];
			break;
	#endif
		default:
			break;
	}
}
#endif




/******************************************************************************
**                            DAC
******************************************************************************/
void DAC_Init(void)				 
{
	PINCON->PINSEL1 &= ~0x00300000;	/* P0.26, function 02 */
	PINCON->PINSEL1 |=  0x00200000;
	PINCON->PINMODE1 &= ~0x00300000;	/* P0.26, function 00--PullUp, 11--PullDown */
	//PINCON->PINMODE1 |=  0x00000000;
	DAC->DACR |= 0x10000;			   // DACR.16
}
void DAC_SetV(unsigned short vSet)
{
	if(vSet > 1023)
		vSet = 1023;
	DAC->DACR = 0x10000 | (vSet<<6);			   // DACR.6~15

}


/*********************************************************************************
**                            End Of File
*********************************************************************************/
