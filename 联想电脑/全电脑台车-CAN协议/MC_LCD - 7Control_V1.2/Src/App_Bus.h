/********************************************************************
*  
*  APP_BUS  与 总线 有关的功能函数  IIC SPI CAN UART 等
*  功能：
*
********************************************************************/

#ifndef __APP_BUS_H_
#define __APP_BUS_H_


#include "config.h"



/****************************************************************************************/

///////////////////////////////     IIC 器件地址	 ////////////////////////////										  
#define AT24_ADDR			0x06
#define X1226_ADDR			0xde	//0xDE

#define AT24C_WRCMD  		0xA0
#define AT24C_RDCMD   		0xA1

///////////////////////////////     外部 EEPROM 空间分配	 ////////////////////////////										  

//#define  EX_STCNT_SITE  	50000
#define  EX_EEPROM_SIZE			0x8000		// 32 KB，平分为 2 块  243C256
#define  EX_SYSINFO_STADDR		0x10		// SysInfo --- 8 Byte
//#define  EX_SYSINFO_STADDR		0x00		// SysInfo --- 8 Byte
#define  EX_OVERTABLE_STADDR	0x20		// 超载信息表 OverInfoTable --- 8 Byte
#define  EX_OV_H_TABLE_STADDR	0x30		// 超载解除按钮操作表OvHandInfoTable --- 8 Byte
#define  EX_BDINFO_STADDR		0x80		// BDInfo1 ---- 64 Byte
#define  EX_BDINFO_STADDR2		(EX_BDINFO_STADDR+0x40)		// BDInfo2 ---- 64 Byte
#define  EX_WT_STADDR			(0x60)		// worktime ---- 12Byte

#define  EX_PWMBDINFO_STADDR		0x100		// PWM_BD ---- 256 Byte
#define  EX_OVER_INFO_STADDR	0x200		// 超载信息存储起始位置
#define  EX_OVER_INFO_SIZE		32			// 超载信息存储长度，100条记录
#define  EX_OV_H_INFO_STADDR	0x1200		// 超载解除操作存储起始位置
#define  EX_OV_H_INFO_SIZE		16//24		// 超载解除操作存储长度，100条记录

#define  OVER_INFO_NUM			60			// 记录的最大超载次数, < 128 

///////////////////////////////     UART 无线 帧头定义		/////////////////////////////
#define  UART_H1				0xFE
#define  UART_H2				0xFD

#define  UART_LEN				4			// 无线数据的长度 2+1+8+1=12

#define  SYSINFO_HEAD0			0x5a
#define  SYSINFO_HEAD1			0xa5



typedef struct
{
	unsigned char vHead[2];
	unsigned char vCANID;
	unsigned char vCANID2;
	unsigned char vBps;
	unsigned char vBps2;
	unsigned char vWLID[4];				// 无线通信，系统配对的ID
	unsigned char vCycle;				// 发送周期 n * 10 ms
	unsigned char vCycle2;				// 发送周期 n * 10 ms
	unsigned char vRsv;
	unsigned char vRsv2;

	unsigned char vChkSum;
}*pSysInfo,SysInfo;

////  系统时间
typedef struct 
{
	unsigned char  vSec;				// 秒
	unsigned char  vMin;				// 分
	unsigned char  vHour;				// 时
	unsigned char  vDay;				// 日
	unsigned char  vMon;				// 月
	unsigned char  vYear;				// 年
	unsigned char  vWeek;				// 年
	unsigned char  vY2K;				// 世纪，19/ 20 h
}*pSysTime, SysTime;

extern SysInfo gSysInfo[2];

extern unsigned char gID;
extern unsigned char gID2;
extern unsigned int  gCanSCycle;
extern SysTime  gSysTime;
extern SysTime  gSysTime0;
extern uchar x12026_st ;



/****************************************************************************************/


void Can_Prog_Rcv(unsigned int vFlag);

uchar I2C_Write_B(uchar device,uint address,uchar bytedata);//向slave写入1字节数据 
uchar I2C_Read_B(uchar device,uint address);

uchar ReadRTC_ST(void); 			   //读时钟状态 
uchar ReadRTC(void); 			   //读时钟 
uchar WriteRTC(uchar nian,uchar yue,uchar ri,uchar shi,uchar fen,uchar miao); //写时钟 
void x12027_chk(void);

unsigned char AT24_Read_Str(unsigned int byte_addr, unsigned char *buff, unsigned char num);
unsigned char AT24_Write_Page(unsigned int byte_addr, unsigned char *buff, unsigned char num);
//unsigned char  Sys_Read_Len(void);
//void Sys_Write_Len(unsigned char vPowerFlg);

void Sys_Param_Init(unsigned char vID, unsigned char vID2);
void BD_Patam_Init(void);
unsigned char  Sys_Read_BD(void);
void Sys_Write_BD(void);
void BD2_Patam_Init(void);
unsigned char  Sys_Read_BD2(void);
void Sys_Write_BD2(void);
unsigned char  Sys_Read_WT(void);
void Sys_Write_WT(void);
 
void Sys_Write_PwmBD(void);
unsigned char  Sys_Read_PwmBD(void);
unsigned char  Sys_Read_Info(void);
void Sys_Save_Info(void);



unsigned long AT24_Read_ULong(unsigned int byte_addr);
unsigned char AT24_Write_ULong(unsigned int byte_addr, unsigned long wData);




#endif  //__APP_BUS_H_
