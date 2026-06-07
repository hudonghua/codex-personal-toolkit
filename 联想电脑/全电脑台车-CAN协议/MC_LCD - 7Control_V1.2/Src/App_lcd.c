/****************************************************************************
*
*	 APP_LCD 与 LCD 界面显示有关的函数
*
*
*****************************************************************************/

#include "app_key.h"
#include "app_lcd.h"
#include "LPC17xx.h"
#include "config.h"	    


mLCDPage gLCDPage;
void Curve_N(unsigned char vCHN, float fXin, unsigned char Reset, unsigned char MapNo);
void mainFrame(void);
void IOFrame_IN1(void);
void IOFrame_IN2(void);
void IOFrame_IN4(void);
void password_6(void);
void password_200(void);
void IOFrame_YJ5(void);	  	  
 void parmchang_Boom(unsigned char index ,unsigned char dir,unsigned char vX   ) ;		    			 
void IOFrame_3(void);    			 
void IOFrame_4(void);		 
void IOFrame_6(void);	  
void IOFrame_13(void);
void IOFrame_14(void);	   
void IOFrame_16(void);	 
void IOFrame_17(void);	  
void IOFrame_18(void);	   
void IOFrame_19(void);	    
			  
	  	
 float TEST_buf 	=0.0 ;
unsigned int set_dly 	=0 ;
unsigned int _Value_buf 	=0 ;
unsigned int _fast_Max_pwm 	=600 ;
unsigned int _slow_Max_pwm 	=420 ;
unsigned int _Min_pwm 	=260 ;
unsigned int _Accel_step 	=25 ;
unsigned int _Dccel_step 	=60 ;	 	
unsigned int Addr_flags 	=60 ;	 	
unsigned int ggg_lcd 	= 0 ;	 		  
									   
  
void keyset();
void keyoldset();
void messageshow();


typedef struct
{
	char  Row;				// 实时电流
	char  Column;			// 最终电流
	char  OK;			// 起始电流
	unsigned char  value[9][3];	    
} *pBdata_input_st, data_input_st;

data_input_st data_input_TT ; 


 void clearicon(ushort addr ,uchar len)
 {
	 uchar i=0 ;
	 for(;i<len;i++)LCD_WR_Data2B(addr+i,0);	 
 } 				
const uchar Warrn_together[21][34]=
{
	"         电机启动中........       ",  //0                               
	"		  电机启动成功......       ",  //1
	"	 相序错误，欠压或者过压！！    ",  //2
	"            急停！！！            ",  //3
	" 电压低，接触器跳开，电机停止运行 " ,   //4 
	"    液压油少，请立即加注液压油    ",  //5
	"		  油温高，请开启散热       ",  //6
	"         吸油滤油报警！！  	   ",  //7
	"		  回油滤油报警！           ",  //8                                      
	"       温度低，确保速凝剂畅通     ",  //9
	"       CAN总线中断，线路故障      ",  //10
	"		  高压滤油报警！！！       ",  //11
	"电压低，强起电机，接触器寿命会缩短" ,   //12
	"       液压油少，导致设备停机     ",  //13
	"                                  " ,  //14
	"        设备本地二级锁机          " ,   //15
	"       油温低，设备需要预热       " ,    //16  
	"       电机过载,导致停机!!!       "     //17  
} ;

uchar  TIAOSHI_ROW1=0;	  
uchar  TIAOSHI_ROW2=0;	  
uchar  TIAOSHI_ROW3=0;	  
uchar  TIAOSHI_ROW4=0;	  
uchar  TIAOSHI_ROW5=0;	  
uchar  TIAOSHI_ROW6=0;	 
uchar  TIAOSHI_ROW7=0;	  
uchar  TIAOSHI_ROW8=0;	  	    
uchar  TIAOSHI_Column0=0;	  
uchar  TIAOSHI_Column1=0;	  
uchar  TIAOSHI_Column2=0;	  
uchar  TIAOSHI_Column3=0;	  
uchar  TIAOSHI_Column4=0;	  
uchar  ICO_NUM=0;
	 
uchar  key_6_dly=0;	 
uchar  index_p=1;	 
uchar  key4_set=0;	 	 
uchar  redline_flags=0;	  
uchar  PhaseAlarm_cance =0;	 
uchar  vSave_dly =0;
uchar  PhaseAlarm_cance_display =0;	 
uchar  Motor_error_cance_display =0;
uchar  set_D_display =0;
uchar  with_yes =0;
uchar  Fan_Z_do =0;

uchar  set_D =0;		  
uint  snj_dead_zone =0;		  
uint  snj_kof_set =0;		  
uint  snj_time_dly =0;		  
uint  gCommOvDly_dly =0;	 
uint  y_key_count =0;		  
   
  								  		 	 		    									   
											 
unsigned char parameter_save_flag	=0 ;
unsigned char Prog_Run_3s_BOOL	=0 ;
 
   
int fCurve_State = 0;
float fOutput_Y = 0.0;
float ArrayX[_CURVE_N][_CURVE_X];
float ArrayY[_CURVE_N][_CURVE_X];
unsigned char  key[8];
unsigned char  keyold[8];
unsigned int  dead_zone =0 ;	
unsigned int  dead_zone_A =0 ;	
unsigned int  dead_zone_B =0 ;			  
void ( * gp_lcdtask)(void);
unsigned short gLcdFresh = 0;
unsigned short gDataFresh = 0;
	uchar   gPasswordIn[8];
 
ushort gBKColor = 0x1f;			 // 背景色
ushort gFTColor = 0xffff;			 // 前景色
	ushort gLCDX=0;				 // X
	ushort gLCDY=0;				 // Y
	ushort gLCDX1=0;				 // X1
	ushort gLCDY1=0;				 // Y1
	ushort gLCDW=0;				 // 宽
	ushort gLCDH=0;				 // 高
 
 void keyset()
 {
 	unsigned char i;
	for(i=0;i<8;i++)
	{
		if(gKeyValue==1<<i){key[i]=1;}else{key[i]=0;}
	} 
	 
 }
 
 void keyoldset()
 {
 	unsigned char i;
	for(i=0;i<8;i++)
	{
		keyold[i]=key[i];
	}
}
  
  
unsigned char	Page_No = 0 ;	 
unsigned char	pageindex = 1 ;	
uchar  pageold=10;
/****************************************************************************
*
*  LCD 显示界面：启动界面
*
*****************************************************************************/
void Disp_Start(void)
{
	if(gLcdFresh > 2)
	{
		gLcdFresh--;
		return;
	}
	if(gLcdFresh == 2)
	{
		gLcdFresh = 1;
		LCD_GO_Page(0);
	}											//  moment_percentage
//	Key_Start();
//	gDataFresh = 0;
}


 

/****************************************************************************
*
*  LCD 显示界面：主界面
*
*****************************************************************************/
 
void Disp_main(void)
{

 	if(pageindex!=pageold)
	{ 
		LCD_GO_Page(pageindex);
		
		Page_No=pageindex;keyset();keyoldset();
	 
		pageold=pageindex;
	}
	   
		keyset(); 	
			 
	switch(pageindex)
	{					 
		case 1: mainFrame() ;break;	//主界面 
		
		//case 3: Frame_23(1) ;break;	//主界面 
		//case 4: IOFrame_4() ;break;	//主界面 
		//case 2: Frame_23(0) ;break;	//主界面  password_200 
		//case 5: password_200() ;break;	//主界面   
	 //	case 6: IOFrame_6() ;break;	//主界面   	 		   
	 				  	  
	//	 case 11 : qiangzhi_work_Frame();	   break ; // 密码登录	 	  
	}

	keyoldset();


}
 								  
void mainFrame(void)
{					    
 
 								    					  
     	if(key[6]==1&&keyold[6]==0 ){ pageindex= 2 ;  }     
     	if(key[4]==1&&keyold[4]==0 ){ pageindex= 2 ;   }    
     	if(key[7]==1&&keyold[7]==0 ){ pageindex= 4 ;   }     	   					  	 	      
		
 
}

 
 
void Curve_N(unsigned char vCHN, float fXin, unsigned char Reset, unsigned char MapNo)
{
	unsigned char i = 0;
	unsigned char j = 0;
  //    Init : BOOL;
	float Temp =0.0;
	float a[_CURVE_X];
	float b[_CURVE_X];
	float OrderX[_CURVE_X];
	float OrderY[_CURVE_X];
	int  Temp2 =0 ;

	if(Reset)
	{
		fCurve_State = 0 ;
		for( i= 0; i< 5; i++)
			{
			OrderX[i] = 0.0;
			OrderY[i] = 0.0;
			a[i] = 0.0;
			b[i] = 0.0;

			fOutput_Y = 0.0 ;
		}
	}
	else
	{
		if( MapNo > _CURVE_X) 
			fCurve_State = -1 ;
		
		for (i=0; i<= (MapNo -1);i++) 
		{
			OrderX[i] = ( ArrayX[vCHN][i] );
			OrderY[i] =( ArrayY[vCHN][i] );
		}
		
		for (i=0; i<= (MapNo -2); i++)
		{
			for (j = i +1; j<= (MapNo -1);j++)
			{
				if (OrderX[i] > OrderX[j])
				{
					Temp 	  = OrderX[j] ;
					OrderX[j] = OrderX[i] ;
					OrderX[i] = Temp;		//OrderX[j] ;(

					Temp      = OrderY[j] ;
					OrderY[j] = OrderY[i] ;
					OrderY[i] = Temp;		//OrderY[j] ;
				}
				else if( OrderX[i] == OrderX[j] )
				{
					if (OrderY[i] != OrderY[j])
					{
						fCurve_State = -1 ;
						return ;
					}
				}
			}
		}
		
		if (fCurve_State == -1)
			return;
		
		for (i=0 ; i<=(MapNo -2); i++)
		{
			j = i +1 ;
			if (OrderX[j] == OrderX[i] )
			{
				a[i] = 0.0 ;
				b[i] = 0.0 ;
			}
			else
			{
				a[i] = (OrderY[j] - OrderY[i]) *10000.0  / (OrderX[j] - OrderX[i]) ;
				b[i] = OrderY[i] - a[i] * OrderX[i] /10000.0 ;
			}
		}

		Temp2 = MapNo -1 ;
		if ((fXin) < OrderX[0] )
			fOutput_Y =  OrderY[0] ;
		else if( (fXin) > OrderX[Temp2] )
			fOutput_Y =  OrderY[Temp2]  ;
		else
		{
			for (i= 0; i<= (MapNo -2); i++)
			{
				j = i +1;
				if (((fXin) >= OrderX[i]) && ((fXin) <= OrderX[j])) 
					fOutput_Y =  a[i] * (fXin) /10000.0  + b[i] ;
			}
		}
	}
	fCurve_State = 1 ;
}

		





