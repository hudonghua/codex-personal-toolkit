/********************************************************************
*  
*  APP_LCD  与 LCD 有关的功能函数
*  功能：
*
********************************************************************/

#ifndef __APP_LCD_H_
#define __APP_LCD_H_


#include "config.h"


#define COLOR_RED_  		0xf800		  // 红
#define COLOR_GREEN_  	0x07C0		  // 绿
#define COLOR_BLUE_  		0x001f		  // 蓝
#define COLOR_YELLW_  	0xf7e0		  // 黄
#define COLOR_ORANGE_  	0xfC00		  // 橙色

#define PASSWORD_LEN	5			  // 密码长度


////  显示页面及切换的函数
typedef struct
{
	uchar  page;			// 页面
	ushort pic;				// 背景图片
	uchar  key;				// 按键
	uchar  cmdsel;			// 命令选择
	uchar  TypeSel;			// 车型选择
	uchar  BDRect;		// 长度标定页面的输入框
	uchar  BDRectSite;	// 输入框内的输入位置
	uchar  sonPage;			// 子页面
	uchar  OvPage;			// 超载查询时，页面数
	uchar  vOverRd;			// 超载信息详细记录位置
	uchar  vSave;			// 保存标定参数标记

}*pLCDPage, mLCDPage;

extern void ( * gp_lcdtask)(void);




extern uchar   gPasswordIn[8];
extern unsigned short gLcdFresh ;
extern unsigned short gDataFresh;
extern mLCDPage gLCDPage;
extern ushort gBKColor ;			 // 背景色
extern ushort gFTColor ;			 // 前景色

//// LCD
extern	ushort gLCDX;				 // X
extern	ushort gLCDY;				 // Y
extern	ushort gLCDX1;				 // X1
extern	ushort gLCDY1;				 // Y1
extern	ushort gLCDW;				 // 宽
extern	ushort gLCDH;				 // 高


void Disp_Start(void);
void Disp_main(void);
 extern void messageshow();
void Disp_Input_Password(void);
 						   
void Disp_Cmd7_DatetimeSet(void);

void Curve_N(unsigned char vCHN, float fXin, unsigned char Reset, unsigned char MapNo);

#define  _CURVE_N		8			// Curve功能块中数组的个数
#define  _CURVE_X		30		// Curve功能块中数组的最大数

extern	unsigned char	pageindex   ;

extern	unsigned char 	parameter_save_flag  ;	

extern	unsigned char	Page_No   ;	
extern	unsigned int  dead_zone ;	
extern	unsigned int  dead_zone_A   ;	
extern	unsigned int  dead_zone_B   ;	
extern	unsigned int  force_pump_time_D ;	
extern	unsigned int  Walk_Left_PWM_Set ;	
extern	unsigned int  Walk_Right_PWM_Set ;	 

extern	unsigned int ggg_lcd 	  ;	 		
extern	uchar  TIAOSHI_ROW9 ; 	 
extern	uchar  TIAOSHI_Column0 ;	  
extern	uchar  TIAOSHI_Column1 ;	  
extern	uchar  TIAOSHI_Column2 ;	  
extern	uchar  TIAOSHI_Column3 ;	  
extern	uchar  TIAOSHI_Column4 ;	

extern	uchar   Data_Input_Row   ;	
extern	uchar   Data_Input_Column   ;
extern	uchar   Data_Input_OK  ;

extern	uchar    PhaseAlarm_cance  ;	 
extern	uchar    vSave_dly  ;	 
extern	uchar    MotorError_cance  ;	

extern	uchar   PhaseAlarm_cance_display  ;	 
extern	uchar   Motor_error_cance_display  ; 
extern	uchar   set_D_display  ; 
extern	uchar   set_D  ;

 	 
extern  int   fCurve_State ;
extern	float  fOutput_Y  ;		  
extern	float  ArrayX[_CURVE_N][_CURVE_X];   		  
extern	float   ArrayY[_CURVE_N][_CURVE_X];		  
		    
 		  						   
#endif  //__APP_LCD_H_
