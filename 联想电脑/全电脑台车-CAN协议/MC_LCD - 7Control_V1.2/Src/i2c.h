/*****************************************************************************
 *   i2c.h:  Header file for NXP LPC17xx Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.26  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __I2C_H 
#define __I2C_H

#define IICBUFSIZE			0x80
#define MAX_TIMEOUT		0x000FFFFF

#define I2CMASTER		0x01
#define I2CSLAVE		0x02

/* For more info, read Philips's LM95 datasheet 
#define LM75_ADDR		0x90
#define LM75_TEMP		0x00
#define LM75_CONFIG		0x01
#define LM75_THYST		0x02
#define LM75_TOS		0x03
											  */


#define RD_BIT			0x01

#define I2C_IDLE			0
#define I2C_STARTED			1
#define I2C_RESTARTED		2
#define I2C_REPEATED_START	3
#define DATA_ACK			4
#define DATA_NACK			5

#define I2CONSET_I2EN		0x00000040  /* I2C Control Set Register */
#define I2CONSET_AA			0x00000004
#define I2CONSET_SI			0x00000008
#define I2CONSET_STO		0x00000010
#define I2CONSET_STA		0x00000020

#define I2CONCLR_AAC		0x00000004  /* I2C Control clear Register */
#define I2CONCLR_SIC		0x00000008
#define I2CONCLR_STAC		0x00000020
#define I2CONCLR_I2ENC		0x00000040

#define I2DAT_I2C			0x00000000  /* I2C Data Reg */
#define I2ADR_I2C			0x00000000  /* I2C Slave Address Reg */
#define I2SCLH_SCLH			0x00000078  /* I2C SCL Duty Cycle High Reg   0x00000080 */
#define I2SCLL_SCLL			0x00000078  /* I2C SCL Duty Cycle Low Reg   0x00000080*/



extern unsigned char   gIICStat[32];
extern unsigned short  gIICSLen;

extern unsigned long  gIICErrS;		// Start Err
extern unsigned long  gIICErrE;		// Stop Err


extern volatile uint32_t I2CMasterState;
extern volatile uint32_t I2CSlaveState;

extern volatile uint32_t I2CCmd;
extern volatile uint32_t I2CMode;

extern volatile uint8_t I2CMasterBuffer[IICBUFSIZE];
extern volatile uint8_t I2CSlaveBuffer[IICBUFSIZE];
extern volatile uint32_t I2CCount ;
extern volatile uint32_t I2CReadLength;
extern volatile uint32_t I2CWriteLength;

extern volatile uint32_t RdIndex ;
extern volatile uint32_t WrIndex ;


extern void I2C0_IRQHandler( void );
extern uint32_t I2CInit( uint32_t I2cMode );
extern uint32_t I2CStart( void );
extern uint32_t I2CStop( void );
extern uint32_t I2CEngine( void );

#endif /* end __I2C_H */
/****************************************************************************
**                            End Of File
*****************************************************************************/
