/****************************************************************************
*
*	 用户计算 有关的函数
*
*
*****************************************************************************/

#include "APP_usr.h"
#include "LPC17xx.h"
#include "config.h"	
 #include "app_comm.h"

#include "Arm200A_Kine.h"
#include "Arm200A_Store.h"
#include "ScanCycle.h"
#include <math.h>
void Sensor_Logic_V (unsigned int vX ,unsigned int vMax  ,unsigned int vMin , unsigned int vRange 	) ;// 电压型传感器处理。
 
		  		  
unsigned char can_send_10ms = 0	 ;	 
unsigned char gYunx20ms = 0	 ;	 
unsigned int Prog_Run_2s = 0	 ;	 
unsigned int Prog_Run_1s = 0	 ;	   	   	 		    


static unsigned char gArm200AKineInitDone = 0;
/*
 * Arm200A CAN1 传感器缓存。
 * 三个推进梁油缸只用于孔深显示/上报，不参与 H 点正解。
 * 断线标志通过 CAN2 的 0x15A 发给电脑屏幕，D0~D3 每一位对应一个具体传感器。
 */
static unsigned int gArmLeftHoleDepthRaw = 0;
static unsigned int gArmMidHoleDepthRaw = 0;
static unsigned int gArmRightHoleDepthRaw = 0;
static float gArmLeftHoleDepthMm = 0.0f;
static float gArmMidHoleDepthMm = 0.0f;
static float gArmRightHoleDepthMm = 0.0f;
static unsigned char gArmCanOffLineByte0 = 0;
static unsigned char gArmCanOffLineByte1 = 0;
static unsigned char gArmCanOffLineByte2 = 0;
static unsigned char gArmCanOffLineByte3 = 0;
static unsigned int gArmCanOffLineSendCnt = 0;
extern uchar boom_type;
/*
 * 编码器传动比。默认 1.0 表示编码器轴转 1 度，实际关节也转 1 度。
 * 如果后续某个关节有齿轮/链条/连杆传动比，只改对应 Ratio 变量即可。
 */
static float gArmLeftRootRatio = 1.0f;
static float gArmLeftBaseRatio = 1.0f;
static float gArmLeftBoomRatio = 1.0f;
static float gArmLeftD1Ratio = 1.0f;
static float gArmLeftE1Ratio = 1.0f;
static float gArmLeftFRatio = 1.0f;
static float gArmMidBaseRatio = 1.0f;
static float gArmMidBoomRatio = 1.0f;
static float gArmMidD1Ratio = 1.0f;
static float gArmMidE1Ratio = 1.0f;
static float gArmMidFRatio = 1.0f;
static float gArmRightRootRatio = 1.0f;
static float gArmRightBaseRatio = 1.0f;
static float gArmRightBoomRatio = 1.0f;
static float gArmRightD1Ratio = 1.0f;
static float gArmRightE1Ratio = 1.0f;
static float gArmRightFRatio = 1.0f;

#define ARM200A_SELF_PARALLEL_PI 3.14159265358979323846

typedef struct
{
	float ArmLeftRightAngle_HMI;
	float ArmUpDownAngle_HMI;
	float ArmAlLeftRightAngle_HMI;
	float ArmAlUpDownAngle_HMI;
	float ArmAlNearAngle_HMI;
	float ArmLeftRightAngle_initial;
	float ArmUpDownAngle_initial;
	float ArmAlLeftRightAngle_initial;
	float ArmAlUpDownAngle_initial;
	float ArmAlNearAngle_initial;
	unsigned char init_flags;
} ARM200A_SELF_PARALLEL_IN;

typedef struct
{
	float target_ud_deg;
	float target_near_deg;
	float ud_dev_deg;
	float near_dev_deg;
	unsigned char up;
	unsigned char down;
	unsigned char left;
	unsigned char right;
	unsigned char ok;
} ARM200A_SELF_PARALLEL_OUT;

static ARM200A_SELF_PARALLEL_IN gArmLeftSelfParallelParam = {0};
static ARM200A_SELF_PARALLEL_IN gArmMidSelfParallelParam = {0};
static ARM200A_SELF_PARALLEL_IN gArmRightSelfParallelParam = {0};
static ARM200A_SELF_PARALLEL_OUT gArmLeftSelfParallelOut = {0};
static ARM200A_SELF_PARALLEL_OUT gArmMidSelfParallelOut = {0};
static ARM200A_SELF_PARALLEL_OUT gArmRightSelfParallelOut = {0};
static float gArmSelfParallelDeadbandDeg = 0.2f;
unsigned char Enable_ON = 0	 ;	 
unsigned char YJ_ON = 0	 ;	 
unsigned char Motor_ON = 0	 ;	 
unsigned char engine_ON_ykq = 0	 ;	 
unsigned char engine_OFF_ykq = 0	 ;
unsigned char Plat_Left_ykq = 0	 ;	 
unsigned char Plat_Right_ykq = 0	 ;	 

unsigned char ABCD_down_flags = 0	 ;	 
 
unsigned char ABCD_up_flags = 0	 ;	 
 
unsigned char byte_mode = 0	 ;	 
unsigned char Fast_Dwei = 0	 ;	 
 
unsigned char byte_mode_old = 0	 ;	 
unsigned char Main_motor_setup_DI = 0	 ;	 
unsigned char Main_motor_setup_DO = 0	 ;	 
unsigned char leg_DO = 0	 ;	 
unsigned char Horn_DO = 0	 ;	 
unsigned char ALARM_DO = 0	 ;	 
unsigned char Main_motor_setup_DI_buf = 0	 ;
unsigned char Main_motor_setup_DI_buf_old = 0	 ;	 
 		   
													  
unsigned char SC121_DI1 = 0	 ;
unsigned char SC121_DI2 = 0		 ;
unsigned char SC121_DI3 = 0	 ;
unsigned char SC121_DI4 = 0		 ;
unsigned char SC121_DI5 = 0		 ;
unsigned char SC121_DI6 = 0		 ;
unsigned char SC121_DI7 = 0		 ;
unsigned char SC121_DI8 = 0		 ;
	
unsigned char CEshi_1 = 0		 ;
unsigned char CE_NO  = 0		 ;
unsigned char SC121_DI9 = 0		 ;
unsigned char SC121_DI10 = 0		 ;
unsigned char SC121_DI11 = 0		 ;
unsigned char SC121_DI12 = 0		 ;
unsigned char SC121_DI13 = 0		 ;
unsigned char SC121_DI14 = 0		 ;
unsigned char SC121_DI15 = 0		 ;
unsigned char SC121_DI16 = 0		 ;

unsigned char SC121_DI17 =0		 ;
unsigned char SC121_DI18 =0		 ;
unsigned char SC121_DI19 = 0		 ;
unsigned int  MC121_AI1 = 0		 ;
unsigned int  MC121_AI2 = 0		 ;
unsigned int  MC121_AI3 = 0		 ;
unsigned int  MC121_AI4 = 0		 ;
unsigned int  MC121_AI5 = 0		 ;
unsigned int  MC121_AI6 = 0		 ;
unsigned int  SC121_AI1 = 0		 ;
unsigned int  SC121_AI2 = 0		 ;
unsigned int  SC121_AI3 = 0		 ;
unsigned int  SC121_AI4 = 0		 ;
unsigned int  SC121_AI5 = 0		 ;
unsigned int  SC121_AI6 = 0		 ;
																	  	
unsigned char MC121_DI1 = 0	 ;
unsigned char MC121_DI2 = 0		 ;
unsigned char MC121_DI3 = 0	 ;
unsigned char MC121_DI4 = 0		 ;
unsigned char MC121_DI5 = 0		 ;
unsigned char MC121_DI6 = 0		 ;
unsigned char MC121_DI7 = 0		 ;
unsigned char MC121_DI8 = 0		 ;

unsigned char MC121_DI9 = 0		 ;
unsigned char MC121_DI10 = 0		 ;
unsigned char MC121_DI11 = 0		 ;
unsigned char MC121_DI12 = 0		 ;
unsigned char MC121_DI13 = 0		 ;
unsigned char MC121_DI14 = 0		 ;
unsigned char MC121_DI15 = 0		 ;
unsigned char MC121_DI16 = 0		 ;

unsigned char MC121_DI17 =0		 ;
unsigned char MC121_DI18 =0		 ;
unsigned char MC121_DI19 = 0		 ;
 

unsigned int MC121_PWM1B = 0	 ;
unsigned int MC121_PWM1A = 0	 ; 
unsigned int MC121_PWM2B = 0	 ;
unsigned int MC121_PWM2A = 0	 ; 
unsigned int MC121_PWM3B = 0	 ;
unsigned int MC121_PWM3A = 0	 ; 
unsigned int MC121_PWM4B = 0	 ;
unsigned int MC121_PWM4A = 0	 ; 
unsigned int MC121_PWM5B = 0	 ;
unsigned int MC121_PWM5A = 0	 ; 
unsigned int MC121_PWM6B = 0	 ;
unsigned int MC121_PWM6A = 0	 ; 
unsigned int on_off_time_dly = 0	 ; 
unsigned char on_off_time_out = 0	 ;  
unsigned char   y_key_1 = 0;	
unsigned char   y_key_2 = 0;	
unsigned char   y_key_3 = 0;	
unsigned char   y_key_3_old = 0;	
unsigned char   y_key_4 = 0;	
unsigned char   y_key_5 = 0;		    
 unsigned  char   y_key_5_dly= 0;   
 unsigned  char   y_key_1_dly= 0;   
 unsigned  char   Tisho_S= 1;
 
unsigned char   y_key_6 = 0;	
unsigned char   y_key_7 = 0;	
unsigned char   y_key_8 = 0;	  	    
unsigned char   Offline_weak = 0;	  
unsigned char   maunal_mode_flags = 0;	  
unsigned char   Auto_mode_flags = 0;	  
unsigned char   E_stop_DI = 0;	  	
unsigned int   Boom_bar_AI = 0;	  				    	    

unsigned char   Fast_Dwei_local_DI = 0;	  	   	 										   											  

unsigned char 		Left_limit_Rotation_DI =  0 ;
unsigned char 		Right_limit_Rotation_DI =  0 ;

unsigned char 		YJ_ON_Local_DI = 0   ;
		 
	 	
unsigned char 		 engine_start_DI =  0 ;
unsigned char 		 engine_off_DI =  0 ;
unsigned char 		 Plat_Left_DI =  0 ;
unsigned char 		 Plat_Right_DI =  0 ; 	
unsigned char 		 Leg3_Limit_DI =  0 ; 	
unsigned char 		 Leg2_Limit_DI =  0 ; 	
unsigned char 		 Leg1_Limit_DI =  0 ; 	
unsigned char 		 Leg4_Limit_DI =  0 ; 	
		  
unsigned int	    PWM_4A_119_CAN2	= 0 ;	
unsigned int	    PWM_4B_119_CAN2	= 0 ;	


unsigned int	    PWM_5A_119_CAN2	= 0 ;	
unsigned int	    PWM_5B_119_CAN2	= 0 ;	


unsigned int	    PWM_6A_119_CAN2	= 0 ;	
unsigned int	    PWM_6B_119_CAN2	= 0 ;	


unsigned int	    PWM_3A_119_CAN2	= 0 ;	
unsigned int	    PWM_3B_119_CAN2	= 0 ;	    

		    
unsigned int		    PWM_2A_119_CAN2	= 0 ;	
unsigned int		    PWM_2B_119_CAN2	= 0 ;	


unsigned int		    PWM_1A_119_CAN2	= 0 ;	
unsigned int		    PWM_1B_119_CAN2	= 0 ;
unsigned int		    DO_0_PWM	= 0 ;	
unsigned int		    DO_1_PWM	= 0 ;
unsigned int		    DO_2_PWM	= 0 ;	
unsigned int		    DO_3_PWM	= 0 ;
unsigned int		    DO_4_PWM	= 0 ;	
unsigned int		    DO_5_PWM	= 0 ;
unsigned int		    in_D_DI_buf	= 0 ;
unsigned int		    in_C_DI_buf	= 0 ;
unsigned int		    in_B_DI_buf	= 0 ;
unsigned int		    in_A_DI_buf	= 0 ;
 
unsigned char Horn_DI =0 ;	
unsigned char R_Horn =0 ;	 
unsigned char R_stop =0 ;	   
unsigned char in_D_DI =0 ;	  
unsigned char in_C_DI =0 ;
unsigned char in_B_DI =0 ;
unsigned char in_A_DI =0 ;	
unsigned char in_A_DI_old =0 ;	
unsigned char in_B_DI_old =0 ;	
unsigned char in_C_DI_old =0 ;	
unsigned char in_D_DI_old =0 ;	

unsigned char OUT_T_dead =0 ;
unsigned char OUT_M_dead =0 ;	 
unsigned int SC121_PWM1B = 0	 ;
unsigned int SC121_PWM1A = 0	 ; 
unsigned int SC121_PWM2B = 0	 ;
unsigned int SC121_PWM2A = 0	 ; 
unsigned int SC121_PWM3B = 0	 ;
unsigned int SC121_PWM3A = 0	 ; 
unsigned int SC121_PWM4B = 0	 ;
unsigned int SC121_PWM4A = 0	 ; 
unsigned int SC121_PWM5B = 0	 ;
unsigned int SC121_PWM5A = 0	 ; 
unsigned int SC121_PWM6B = 0	 ;
unsigned int SC121_PWM6A = 0	 ;  
		 
  unsigned int save_time1  =0 ;
  unsigned int save_time2  =0 ;
  unsigned int work_time1  =0 ;
  unsigned int work_time2  =0 ;
  unsigned int Whole_pumptime1_Min =0 ;
  unsigned int Whole_pumptime2_Min  =0 ;
  unsigned int Whole_Pumptime1	 =0 ;
  unsigned int Whole_Pumptime2	 =0 ;
  	  
 			
						    													  
unsigned int  fsx_wSpeed_buf = 0; 
unsigned int  Can_wSpeed_send = 0; 	 
    
long OUT_M=1 ;
long OUT_T=1 ;


unsigned char  ABCD_up_flags_old = 0 ;		 
unsigned char  MC121_DO3 = 0 ;	
unsigned char  MC121_DO4 = 0 ;	
unsigned char  MC121_DO5 = 0 ;	
unsigned char  MC121_DO6 = 0 ;	
unsigned char  MC121_DO7 = 0 ;	
unsigned char  MC121_DO8 = 0 ;	
unsigned char  MC121_DO9 = 0 ;	
unsigned char  MC121_DO11 = 0 ;	
unsigned char  MC121_DO14 = 0 ;	
unsigned char  MC121_DO15 = 0 ;	
unsigned char  MC121_DO22 = 0 ;	
unsigned char  MC121_DO24 = 0 ;	
unsigned char  MC121_DO25 = 0 ;	
unsigned char  MC121_DO26 = 0 ;	    

unsigned char  SC121_DO3 = 0 ;	
unsigned char  SC121_DO4 = 0 ;	
unsigned char  SC121_DO5 = 0 ;	
unsigned char  SC121_DO6 = 0 ;	
unsigned char  SC121_DO7 = 0 ;	
unsigned char  SC121_DO8 = 0 ;	
unsigned char  SC121_DO9 = 0 ;	
unsigned char  SC121_DO11 = 0 ;	
unsigned char  SC121_DO14 = 0 ;	
unsigned char  SC121_DO15 = 0 ;	
unsigned char  SC121_DO16 = 0 ;
unsigned char  SC121_DO19 = 0 ;	
unsigned char  SC121_DO22 = 0 ;	
unsigned char  SC121_DO23 = 0 ;	
unsigned char  SC121_DO21 = 0 ;	
unsigned char  SC121_DO24 = 0 ;	
unsigned char  SC121_DO25 = 0 ;	
unsigned char  SC121_DO26 = 0 ;  
   
  
  void PWM_logic(unsigned int vX ,
				unsigned int vMaxA , 
				unsigned int vMaxB,
				unsigned int vMinA,
				unsigned int vMinB 		   
				   ) ; 										  	
 
 
  
//////////////////////////////KX119_ can1///////////////////////////////////		 
unsigned char  DI_L1  = 0;	
unsigned char  DI_L2  = 0;	
unsigned char  DI_L3  = 0;	
unsigned char  DI_L4  = 0;	
unsigned char  DI_L5  = 0;	
unsigned char  DI_L9  = 0;	
unsigned char  DI_L10  = 0;	
unsigned char  DI_L11  = 0;	
unsigned char  DI_L12  = 0;	
unsigned char  DI_L13  = 0;	
unsigned char  DI_L14  = 0;	
unsigned char  DI_L16  = 0;	
unsigned char  DI_8P1  = 0;	
unsigned char  DI_8P2  = 0;	
unsigned char  DI_8P3  = 0;	
unsigned char  DI_8P4  = 0;	
unsigned char  DI_8P5  = 0;	


unsigned char  DO_H2   = 0;	
unsigned char  DO_H3   = 0;	
unsigned char  DO_H4   = 0;	

unsigned char  DO_H5   = 0;	
unsigned char  DO_H6   = 0;	
unsigned char  DO_H7   = 0;	
unsigned char  DO_H8   = 0;	
unsigned char  Drill_Back_Switch_DI   = 0;	
unsigned char  Clear_Flags   = 0;	
unsigned char  E_STOP   = 0;	
unsigned char  Clear_Flags_dly   = 0;	
 		  
unsigned char  E_STOP_dly   = 0;	
 		  
    		 
unsigned char  DO_H15   = 0;	
unsigned char  DO_L6    = 0;	
unsigned char  DO_L7    = 0;	
unsigned char  DO_8P6     = 0;	
unsigned char  DO_8P7     = 0;	
unsigned char  DO_8P8     = 0;	
unsigned char  alarm_Local_option     = 0;	
 	
unsigned char  DO_H17     = 0;	
unsigned char  DO_H18     = 0;	
unsigned char  DO_H19     = 0;	
unsigned char  DO_H20     = 0;	
unsigned char  DO_H21     = 0;	
unsigned char  DO_H22     = 0;	

unsigned char  DO_L17     = 0;	
unsigned char  DO_L18     = 0;	
unsigned char  DO_L19     = 0;	
unsigned char  DO_L20     = 0;	
unsigned char  DO_L21     = 0;	
unsigned char  DO_L22     = 0;	

    
   unsigned char 	  Safe_Seat_DI =0  ;
 unsigned char  Back_walk_Led_DO =0  ;
 unsigned char  Brake_Led_DO=0  ;
	
unsigned char  V_pwm_save     = 0;	
	 

unsigned char Frame_Fresh   = 1;	

unsigned char key_Fresh	  = 0;	
 

unsigned int   Paramet_Set1_default =   450  ; 
unsigned int   Paramet_Set2_default =  280  ; 
unsigned int   Paramet_Set3_default =  45  ; 
unsigned int   Paramet_Set4_default =  70  ; 
unsigned int   Paramet_Set5_default =  450  ; 
unsigned int   Paramet_Set6_default =  280  ; 
unsigned int   Paramet_Set7_default =  45  ; 
unsigned int   Paramet_Set8_default =  70  ; 
unsigned int   Paramet_Set9_default =  450  ; 
unsigned int   Paramet_Set10_default =  280  ; 
unsigned int   Paramet_Set11_default =  45  ; 
unsigned int   Paramet_Set12_default =  70  ; 
unsigned int   Paramet_Set13_default =  480  ; 
unsigned int   Paramet_Set14_default =  490  ; 
unsigned int   Paramet_Set15_default =  75  ; 
unsigned int   Paramet_Set16_default =  75  ; 
 
unsigned char MC121_Offline_dly = 0;	
 
unsigned long int length_retain =0;	 			 															
 
		   
unsigned int	 PWMA_OUT   =0 ;
unsigned int	 PWMB_OUT   =0 ; 

unsigned char  A_flags =0 ;
unsigned char  B_flags=0 ;	  	   
	   
			 									 					 
		 
void MyLogic_1ms(void);
void walk_logic(void);	
void Hand_Shank_logic(void);	
void SaveRetain_(void);	

void MyLogic_10ms(void);


 short gABPulseCount_CAN2 =0  ;
 short gABPulseCount_CAN2_buf =0  ;	   
unsigned int gABPulseCount_CAN1_buf =0  ;
unsigned int GAB_CAN2_ZERO =0;
unsigned int gABPulseCount_CAN2_buf_old =0  ;

 short gABPulseCount_CAN1_Temp =0  ;	
 short gABPulse_Count_First =0  ;	 
 
unsigned char 	Left_Dirl_Stop_DI = 0 ;	 
unsigned char 	Right_Dirl_Stop_DI = 0 ;	

unsigned char  gABPulseCount_CAN1_init = 0;
unsigned char  DI_Pin07 = 0;
unsigned char  DI_Pin10 = 0;
unsigned char  DI_Pin20 = 0;
unsigned char  DI_Pin23 = 0;
unsigned char  DI_Pin24 = 0;
unsigned char  DI_Pin30 = 0;
unsigned char  DI_Pin32 = 0;
unsigned char  DI_Pin33 = 0;
unsigned char  DI_8P01 = 0;	
unsigned char  DI_8P02 = 0;	
unsigned char  DI_8P03 = 0;	
unsigned char  DI_8P04 = 0;
unsigned char  DO_Pin02 = 0; 
unsigned char  DO_Pin03 = 0;		 
unsigned char  Horn_out_CC =0 ;	 
unsigned char  Horn_out =0 ;	  
unsigned int  Horn_out_count =0 ;   
 				 	   											    	  
	  
unsigned char  DO_Pin04 =0 ;   

unsigned char  DO_Pin11=0 ;

unsigned char  DO_Pin12=0 ;
unsigned char  DO_Pin14=0 ;
unsigned char  DO_Pin15=0 ;
unsigned char  DO_Pin16=0 ;
unsigned char  DO_Pin25=0 ;
unsigned char  DO_Pin26=0 ;
unsigned   DO_Pin34=0 ;							 
unsigned char  DO_Pin35	=0 ;				  
unsigned int  AI_Pin08 =0 ;

unsigned int  AI_Pin09 =0 ;
unsigned int  AI_Pin10 =0 ;

unsigned int  AI_Pin19 =0 ;
unsigned int  AI_Pin21 =0 ;
unsigned int  AI_Pin22 =0 ;
unsigned int  AI_Pin31 =0 ;
unsigned int  gECUDly =0 ; 				  

unsigned int  wSpeed =0 ; 			 	 	  
unsigned int  Min_Pwm =280 ; 	
unsigned int  Max_Pwm =580 ; 	
											   
unsigned int   Paramet_Set1 = 0  ; 
unsigned int   Paramet_Set2 = 0  ; 
unsigned int   Paramet_Set3 = 0  ; 
unsigned int   Paramet_Set4 = 0  ; 
unsigned int   Paramet_Set5 = 0  ; 
unsigned int   Paramet_Set6 = 0  ; 
unsigned int   Paramet_Set7 = 0  ; 
unsigned int   Paramet_Set8 = 0  ; 
unsigned int   Paramet_Set9 = 0  ; 
unsigned int   Paramet_Set10 = 0  ; 
unsigned int   Paramet_Set11 = 0  ; 
unsigned int   Paramet_Set12 = 0  ; 
unsigned int   Paramet_Set13 = 0  ; 
unsigned int   Paramet_Set14 = 0  ; 
unsigned int   Paramet_Set15 = 0  ; 
unsigned int   Paramet_Set16 = 0  ; 		    
      
 unsigned int   Min_Hand_buf =0 ;

 unsigned int   Max_Hand_buf =0 ;
 unsigned int   _ARM_MIDV =127 ;
 unsigned int   _PWM_MAXV =6 ;	 // 认为PWM最大为600 ；
 unsigned char   JOY_127_OutPut =0 ;
 unsigned char   Main_left_LCD =0 ;
 
  		 
unsigned int   Paramet_Set_Frame11[20] ;
unsigned int   Paramet_Set_Frame6[12] ;	 

unsigned int   Paramet_Set_Frame11_default[20]=
{				   
	500 ,500,40,100,60,120,140,45,5,450 ,
	500 ,500,40,100,60,120,140,45,5,450   
} ;

unsigned int   Paramet_Set_Frame6_default[20]=
{				   
	210 ,340,430,230,360,450,
	210 ,340,430,230,360,450 
} ;  						 
	   											  		  
 
unsigned char DI_Buf[22]={0,0,0,0,0,0,0,0};	//MOb2缓存，hudh 
float Senso_Logic_out ;
void Sensor_Logic_V (unsigned int vX ,
unsigned int vMax  ,    
unsigned int vMin ,
unsigned int vRange 	)// 电压型传感器处理。
{			  		
	if( (vMax > vMin) &&   (vX >vMin ) ) 
	{								   
		Senso_Logic_out =(float) ((vX-vMin) * vRange ) / (float) ( vMax - vMin) ;		 
	}
	else
	{
		Senso_Logic_out =0  ;
	}
  
	 	  		 
}
 
int gABPulseCount = 0;
unsigned char   Aff_CAN2 = 0;
unsigned char   gDIBitV_AB = 0;
unsigned char   Aff = 0;
			   
void AB_Pulse(unsigned int PulseCounting)
{
	static unsigned char AB_C = 0;
	static unsigned char AB_L = 0;
	static unsigned char bFirstRun = 1;
	unsigned char A = 0;
	unsigned char B = 0;

	A = _BitV(gDIBitV_AB, 0);
	B = _BitV(gDIBitV_AB, 1);
//	A = _BitV(gDIBitV_M, 9);
//	B = _BitV(gDIBitV_M, 10);
						   

	AB_C = A | (B<<1);


	if( ((AB_C==2 && AB_L==0) ||
		(AB_C==3 &&	AB_L==2) ||
		(AB_C==1 &&	AB_L==3) ||
		(AB_C==0 &&	AB_L==1))
	)
	{
		gABPulseCount = gABPulseCount +1;
	 	Aff = 1;
	}
	if( ((AB_C==1 && AB_L==0) ||
		(AB_C==3 &&	AB_L==1) ||
		(AB_C==2 &&	AB_L==3) ||
		(AB_C==0 &&	AB_L==2))
	)
	 {
		gABPulseCount = gABPulseCount-1;
		Aff = 2;
	  }
	AB_L=AB_C;

	if	(bFirstRun) 
	{
		bFirstRun=0;
		gABPulseCount = 0;
	}

} 
 
 
void clear_Stop (void) //清除动力源
{		

		 ;	    
	 
}	 
  

void clear_zy (void) //清除臂架阀以及中间变量
{			 
	   unsigned int i =0  ; 
		

 									   			
    	 	for(i=0; i<10;i++)
	  	{	   Ramp(1+i ,   0 ,	 0,	0, 50,1)  ;	  
	    }		  
	   
}
/*

	Right_Front_Cycle_Hz_s
Left_Back_Cycle_Hz_s
Left_Front_Cycle_Hz_s
Right_Back_Cycle_Hz_s
*/	 
 
void DI_Byte_DO (void)	 
{
	unsigned char i = 0; 
 	for(i=0; i<20; i++)
	{
		DI_Buf[i]= _BitV(gDIBitV, i);
	
	}
	
	 DI_Pin07 =  DI_Buf[0] ;
	 DI_Pin10 =  DI_Buf[2] ;
	 DI_Pin20 =  DI_Buf[4] ;
	 DI_Pin23 =  DI_Buf[7] ;
	 DI_Pin24 =  DI_Buf[8] ;
	 DI_Pin30 =  DI_Buf[9] ;	
	 
	 DI_Pin32 = DI_Buf[11] ;
	 DI_Pin33 = DI_Buf[12] ;
	 DI_8P01 =  DI_Buf[13] ;
	 DI_8P02 =  DI_Buf[14] ;
	 DI_8P03 =  DI_Buf[15] ;
	 DI_8P04 =  DI_Buf[16] ;	
							  
	 AI_Pin08 = gADV_mV[0] ;		// 单位 mV
	 AI_Pin09 = gADV_mV[1] ;
	 AI_Pin19 = gADV_mV[2] ;
	 AI_Pin21 = gADV_mV[3] ;
	 AI_Pin22 = gADV_mV[4] ;
	 AI_Pin31 = gADV_mV[5] ;

//	AI_Pin08   20mA 对应的是 4372mV  4372/220 = 19.87mA     4-20mA输入接口
//	AI_Pin09   高有效. AI_Pin09 >5000 说明有信号进来.
//	AI_Pin19   高有效. 24v 进来 AI_Pin19 >5000    低有效 0V进来 AI_Pin19 <100 ; 该点既可以高电平，又可以低电平。
//	AI_Pin21   默认为0-1k 电阻输入。如果做DI的话：高有效. 24v 进来 AI_Pin21 >5000    低有效 0V进来 AI_Pin21 <100 ; 该点既可以高电平，又可以低电平。
//	AI_Pin22   	24v 进来 AI_Pin22 >5000   如果需要做DI,只能做高电平
//	AI_Pin31   默认为0-1k 电阻输入 888 	0v 进来 AI_Pin31 <100 
  
	 if (DO_Pin02) 
	 {
	 	DOx_Set(0, 1) ;
	 }
	 else
	 {
	 	DOx_Set(0, 0) ;
	 
	 }	 
	 if (DO_Pin03) 
	 {
	 	DOx_Set(1, 1) ;
	 }
	 else
	 {
	 	DOx_Set(1, 0) ;
	 
	 }	 
 
	 if (DO_Pin04) 
	 {
	 	DOx_Set(2, 1) ;
	 }									   	

	 else
	 {
	 	DOx_Set(2, 0) ;
	 
	 }	 
	 if (DO_Pin11) 
	 {
	 	DOx_Set(3, 1) ;
	 }
	 else
	 {
	 	DOx_Set(3, 0) ;
	 
	 }	 
 
	 if (DO_Pin12) 
	 {
	 	DOx_Set(4, 1) ;
	 }
	 else
	 {
	 	DOx_Set(4, 0) ;
	 
	 }	 
 	 if (DO_Pin14) 
	 {
	 	DOx_Set(5, 1) ;
	 }
	 else
	 {
	 	DOx_Set(5, 0) ;
	 
	 }	 
  	 if (DO_Pin15) 
	 {
	 	DOx_Set(6, 1) ;
	 }
	 else
	 {
	 	DOx_Set(6, 0) ;
	 
	 }
 	 
	 if (DO_Pin16) 		 //模拟24V  16脚
	 {
	 	DOx_Set(7, 0) ;//	PWM_SetV(2,7,100 ) ;	 // DO8
	 }
	 else
	 {
	 	DOx_Set(7, 0) ;//	PWM_SetV(2,7, 0 ) ;	 // DO8
	 
	 }	
 	  
	 if (DO_Pin25) 
	 {
	 	DOx_Set(8, 1) ;
	 }
	 else
	 {
	 	DOx_Set(8, 0) ;
	 
	 }	
	 if (DO_Pin26) 
	 {
	 	DOx_Set(9, 1) ;
	 }
	 else
	 {
	 	DOx_Set(9, 0) ;
	 
	 }	
 
	 if (DO_Pin34) 
	 {
	 	DOx_Set(10, 1) ; //	PWM_SetV(0,10,100 ) ; //	DOx_Set(10, 1) ;
	 }
	 else
	 {
		DOx_Set(10, 0) ; // PWM_SetV(0,10,0 ) ; //DOx_Set(10, 0) ;
	 
	 }	
	 if (DO_Pin35) 
	 {
	 	DOx_Set(11, 1) ;//	PWM_SetV(0,11,100 ) ; ///DOx_Set(11, 1) ;
	 }
	 else
	 {
	 	DOx_Set(11, 0) ;//	PWM_SetV(0,11,0 ) ;
	 
	 }	
	 /* 0	1A
		1	1B
		2	2A
		3	2B
		4	3A
		5	3B
		6	4A
		7	4B
		8	5A
		9	5B
		10	6A
		11	6B
   */
 				   

}
 
/*
 * CANopen PDO 低字节在前：Byte0 是低字节，Byte1 是高字节。
 * 编码器当前只取单圈 0~65535，先换成 0~360 度，再乘对应 Ratio。
 * CD 拉绳先按 12m 量程，GG1 油缸先按 3m 量程，后续现场量程变化只改换算函数。
 */
static unsigned int Arm200A_CanRaw16(void)
{
	return ((unsigned int)CAN1_RBuf[0]) + (((unsigned int)CAN1_RBuf[1]) << 8);
}

static float Arm200A_Raw16ToDeg(unsigned int raw)
{
	return ((float)raw) * 360.0f / 65535.0f;
}

static float Arm200A_Raw16ToCdMm(unsigned int raw)
{
	return ((float)raw) * 12000.0f / 65535.0f;
}

static float Arm200A_Raw16ToGg1Mm(unsigned int raw)
{
	return ((float)raw) * 3000.0f / 65535.0f;
}


static long Arm200A_FloatToLongX100(float value)
{
	if(value >= 0.0f)
	{
		return (long)(value * 100.0f + 0.5f);
	}
	return (long)(value * 100.0f - 0.5f);
}

static int Arm200A_FloatToS16X100(float value)
{
	long pack_value;

	pack_value = Arm200A_FloatToLongX100(value);
	if(pack_value > 32767L)
	{
		pack_value = 32767L;
	}
	if(pack_value < -32768L)
	{
		pack_value = -32768L;
	}
	return (int)pack_value;
}


static void Arm200A_PackU16(unsigned char byte_index, unsigned int value)
{
	CAN_SBuf2[byte_index] = (unsigned char)(value & 0xff);
	CAN_SBuf2[byte_index + 1] = (unsigned char)((value >> 8) & 0xff);
}

static void Arm200A_PackS16(unsigned char byte_index, int value)
{
	unsigned int pack_value;

	pack_value = (unsigned int)value;
	CAN_SBuf2[byte_index] = (unsigned char)(pack_value & 0xff);
	CAN_SBuf2[byte_index + 1] = (unsigned char)((pack_value >> 8) & 0xff);
}

static void Arm200A_PackS32(unsigned char byte_index, long value)
{
	unsigned long pack_value;

	pack_value = (unsigned long)value;
	CAN_SBuf2[byte_index] = (unsigned char)(pack_value & 0xff);
	CAN_SBuf2[byte_index + 1] = (unsigned char)((pack_value >> 8) & 0xff);
	CAN_SBuf2[byte_index + 2] = (unsigned char)((pack_value >> 16) & 0xff);
	CAN_SBuf2[byte_index + 3] = (unsigned char)((pack_value >> 24) & 0xff);
}
static void Arm200A_ReadCan1Sensor(unsigned int can_id, float *dst, unsigned char is_cd, unsigned char is_gg1, float ratio)
{
	unsigned char rdCan;
	unsigned int raw;

	rdCan = CAN1_Get_Data(can_id);
	if((rdCan == 0xFF) || (rdCan == 0xEE))
	{
		return;
	}

	raw = Arm200A_CanRaw16();
	if(is_cd)
	{
		*dst = Arm200A_Raw16ToCdMm(raw);
	}
	else if(is_gg1)
	{
		*dst = Arm200A_Raw16ToGg1Mm(raw);
	}
	else
	{
		*dst = Arm200A_Raw16ToDeg(raw) * ratio;
	}
}

/*
 * 推进梁油缸协议参考 MH-Series CANopen PDF：PDO 为 0x180+NodeID，D0~D3 是位置值，D4~D5 是速度。
 * 目前先沿用 16 位 raw 和 3m 量程做孔深临时换算；后续确认实际量程/单位后，再改为 32 位位置值。
 */
static void Arm200A_ReadCan1HoleDepth(unsigned int can_id, unsigned int *raw_dst, float *mm_dst)
{
	unsigned char rdCan;
	unsigned int raw;

	rdCan = CAN1_Get_Data(can_id);
	if((rdCan == 0xFF) || (rdCan == 0xEE))
	{
		return;
	}

	raw = Arm200A_CanRaw16();
	*raw_dst = raw;
	*mm_dst = Arm200A_Raw16ToGg1Mm(raw);
}

static unsigned int Arm200A_HoleDepthSendValue(unsigned int raw, float zero_raw)
{
	long value;

	value = (long)raw - (long)(zero_raw + 0.5f);
	if(value < 0)
	{
		value = 0;
	}
	if(value > 65535L)
	{
		value = 65535L;
	}
	return (unsigned int)value;
}

static void Arm200A_ReadCan1Biaxial(void)
{
	unsigned char rdCan;
	unsigned int roll_raw;
	unsigned int pitch_raw;

	rdCan = CAN1_Get_Data(0x28F);
	if((rdCan == 0xFF) || (rdCan == 0xEE))
	{
		return;
	}

	roll_raw = ((unsigned int)CAN1_RBuf[0]) + (((unsigned int)CAN1_RBuf[1]) << 8);
	pitch_raw = ((unsigned int)CAN1_RBuf[2]) + (((unsigned int)CAN1_RBuf[3]) << 8);
	gArmLeftRaw.body_roll_raw = Arm200A_Raw16ToDeg(roll_raw);
	gArmMidRaw.body_roll_raw = gArmLeftRaw.body_roll_raw;
	gArmRightRaw.body_roll_raw = gArmLeftRaw.body_roll_raw;
	gArmLeftRaw.body_pitch_raw = Arm200A_Raw16ToDeg(pitch_raw);
	gArmMidRaw.body_pitch_raw = gArmLeftRaw.body_pitch_raw;
	gArmRightRaw.body_pitch_raw = gArmLeftRaw.body_pitch_raw;
}

static unsigned char Arm200A_Can1IsOffline(unsigned int can_id)
{
	unsigned char i;

	for(i = 0; i < ID_RCV_NUM; i++)
	{
		if(gRcvCanID[0][i] == can_id)
		{
			if(MsgBuf_RX1[i].vEmpty == 0)
			{
				return 1;
			}
			return 0;
		}
	}
	return 1;
}

/*
 * 精确断线位图：1 表示对应传感器超时/断线。
 * D0: 左臂 0x181~0x188。
 * D1: 中臂 0x192~0x198 + 右臂 root 0x1A1。
 * D2: 右臂 0x1A2~0x1A8 + 左臂推进梁 0x189。
 * D3: 中臂推进梁 0x199、右臂推进梁 0x1A9、双轴 0x28F。
 */
static void Arm200A_UpdateCanOfflineFlag(void)
{
	unsigned char b0;
	unsigned char b1;
	unsigned char b2;
	unsigned char b3;

	b0 = 0;
	b1 = 0;
	b2 = 0;
	b3 = 0;
	if(Arm200A_Can1IsOffline(0x181)) b0 |= 0x01;
	if(Arm200A_Can1IsOffline(0x182)) b0 |= 0x02;
	if(Arm200A_Can1IsOffline(0x183)) b0 |= 0x04;
	if(Arm200A_Can1IsOffline(0x184)) b0 |= 0x08;
	if(Arm200A_Can1IsOffline(0x185)) b0 |= 0x10;
	if(Arm200A_Can1IsOffline(0x186)) b0 |= 0x20;
	if(Arm200A_Can1IsOffline(0x187)) b0 |= 0x40;
	if(Arm200A_Can1IsOffline(0x188)) b0 |= 0x80;

	if(Arm200A_Can1IsOffline(0x189)) b1 |= 0x01;
	if(Arm200A_Can1IsOffline(0x192)) b1 |= 0x02;
	if(Arm200A_Can1IsOffline(0x193)) b1 |= 0x04;
	if(Arm200A_Can1IsOffline(0x194)) b1 |= 0x08;
	if(Arm200A_Can1IsOffline(0x195)) b1 |= 0x10;
	if(Arm200A_Can1IsOffline(0x196)) b1 |= 0x20;
	if(Arm200A_Can1IsOffline(0x197)) b1 |= 0x40;
	if(Arm200A_Can1IsOffline(0x198)) b1 |= 0x80;

	if(Arm200A_Can1IsOffline(0x199)) b2 |= 0x01;
	if(Arm200A_Can1IsOffline(0x1A1)) b2 |= 0x02;
	if(Arm200A_Can1IsOffline(0x1A2)) b2 |= 0x04;
	if(Arm200A_Can1IsOffline(0x1A3)) b2 |= 0x08;
	if(Arm200A_Can1IsOffline(0x1A4)) b2 |= 0x10;
	if(Arm200A_Can1IsOffline(0x1A5)) b2 |= 0x20;
	if(Arm200A_Can1IsOffline(0x1A6)) b2 |= 0x40;
	if(Arm200A_Can1IsOffline(0x1A7)) b2 |= 0x80;

	if(Arm200A_Can1IsOffline(0x1A8)) b3 |= 0x01;
	if(Arm200A_Can1IsOffline(0x1A9)) b3 |= 0x02;
	if(Arm200A_Can1IsOffline(0x28F)) b3 |= 0x08;

	gArmCanOffLineByte0 = b0;
	gArmCanOffLineByte1 = b1;
	gArmCanOffLineByte2 = b2;
	gArmCanOffLineByte3 = b3;
}

/*
 * 每 100ms 左右把 D0~D3 断线位图发到 CAN2，ID=0x15A，供电脑屏幕精确显示故障传感器。
 */
static void Arm200A_SendCanOfflineFlag(void)
{
	gArmCanOffLineSendCnt++;
	if(gArmCanOffLineSendCnt < 50)
	{
		return;
	}
	gArmCanOffLineSendCnt = 0;

	CAN_SBuf2[0] = gArmCanOffLineByte0;
	CAN_SBuf2[1] = gArmCanOffLineByte1;
	CAN_SBuf2[2] = gArmCanOffLineByte2;
	CAN_SBuf2[3] = gArmCanOffLineByte3;
	CAN_SBuf2[4] = 0;
	CAN_SBuf2[5] = 0;
	CAN_SBuf2[6] = 0;
	CAN_SBuf2[7] = 0;
	CAN2_SendX(0x15A, CAN_SBuf2);
}

/*
 * CAN1 绑定顺序：先读取 26 个三臂传感器和双轴，再更新断线位图，最后发送到 CAN2。
 * gArmUseZeroComp 先强制为 0，表示 CAN 解析结果已经是工程量，直接进入正解。
 */
void Arm200A_CAN1_Sensor_Binding(void)
{
	gArmUseZeroComp = 0;
	Arm200A_ReadCan1Sensor(0x181, &gArmLeftRaw.root_raw, 0, 0, gArmLeftRootRatio);
	Arm200A_ReadCan1Sensor(0x182, &gArmLeftRaw.base_raw, 0, 0, gArmLeftBaseRatio);
	Arm200A_ReadCan1Sensor(0x183, &gArmLeftRaw.boom_raw, 0, 0, gArmLeftBoomRatio);
	Arm200A_ReadCan1Sensor(0x184, &gArmLeftRaw.d1_raw, 0, 0, gArmLeftD1Ratio);
	Arm200A_ReadCan1Sensor(0x185, &gArmLeftRaw.e1_raw, 0, 0, gArmLeftE1Ratio);
	Arm200A_ReadCan1Sensor(0x186, &gArmLeftRaw.f_raw, 0, 0, gArmLeftFRatio);
	Arm200A_ReadCan1Sensor(0x187, &gArmLeftRaw.cd_raw, 1, 0, 1.0f);
	Arm200A_ReadCan1Sensor(0x188, &gArmLeftRaw.gg1_raw, 0, 1, 1.0f);
	Arm200A_ReadCan1HoleDepth(0x189, &gArmLeftHoleDepthRaw, &gArmLeftHoleDepthMm);
	gArmLeftRaw.hole_depth_raw = (float)gArmLeftHoleDepthRaw;

	gArmMidRaw.root_raw = 0.0f;
	Arm200A_ReadCan1Sensor(0x192, &gArmMidRaw.base_raw, 0, 0, gArmMidBaseRatio);
	Arm200A_ReadCan1Sensor(0x193, &gArmMidRaw.boom_raw, 0, 0, gArmMidBoomRatio);
	Arm200A_ReadCan1Sensor(0x194, &gArmMidRaw.d1_raw, 0, 0, gArmMidD1Ratio);
	Arm200A_ReadCan1Sensor(0x195, &gArmMidRaw.e1_raw, 0, 0, gArmMidE1Ratio);
	Arm200A_ReadCan1Sensor(0x196, &gArmMidRaw.f_raw, 0, 0, gArmMidFRatio);
	Arm200A_ReadCan1Sensor(0x197, &gArmMidRaw.cd_raw, 1, 0, 1.0f);
	Arm200A_ReadCan1Sensor(0x198, &gArmMidRaw.gg1_raw, 0, 1, 1.0f);
	Arm200A_ReadCan1HoleDepth(0x199, &gArmMidHoleDepthRaw, &gArmMidHoleDepthMm);
	gArmMidRaw.hole_depth_raw = (float)gArmMidHoleDepthRaw;

	Arm200A_ReadCan1Sensor(0x1A1, &gArmRightRaw.root_raw, 0, 0, gArmRightRootRatio);
	Arm200A_ReadCan1Sensor(0x1A2, &gArmRightRaw.base_raw, 0, 0, gArmRightBaseRatio);
	Arm200A_ReadCan1Sensor(0x1A3, &gArmRightRaw.boom_raw, 0, 0, gArmRightBoomRatio);
	Arm200A_ReadCan1Sensor(0x1A4, &gArmRightRaw.d1_raw, 0, 0, gArmRightD1Ratio);
	Arm200A_ReadCan1Sensor(0x1A5, &gArmRightRaw.e1_raw, 0, 0, gArmRightE1Ratio);
	Arm200A_ReadCan1Sensor(0x1A6, &gArmRightRaw.f_raw, 0, 0, gArmRightFRatio);
	Arm200A_ReadCan1Sensor(0x1A7, &gArmRightRaw.cd_raw, 1, 0, 1.0f);
	Arm200A_ReadCan1Sensor(0x1A8, &gArmRightRaw.gg1_raw, 0, 1, 1.0f);
	Arm200A_ReadCan1HoleDepth(0x1A9, &gArmRightHoleDepthRaw, &gArmRightHoleDepthMm);
	gArmRightRaw.hole_depth_raw = (float)gArmRightHoleDepthRaw;

	Arm200A_ReadCan1Biaxial();
	Arm200A_UpdateCanOfflineFlag();
	Arm200A_SendCanOfflineFlag();
}

void CAN_receive_data(void)
{
	static unsigned char last_b0 = 0;
	static unsigned char last_b1 = 0;
	static unsigned char last_b2 = 0;
	static unsigned char last_b3 = 0;
	static unsigned char last_17e_b7 = 0;
	unsigned char rdCan;
	unsigned char buf[8];
	unsigned char rise_b0;
	unsigned char rise_b1;
	unsigned char rise_b2;
	unsigned char rise_b3;
	unsigned char rise_17e_b7;
	unsigned int left_mask;
	unsigned int mid_mask;
	unsigned int right_mask;
	unsigned int body_mask;

	rdCan = CAN2_Get_Data_Buf(0x17E, buf);
	if((rdCan != 0xFF) && (rdCan != 0xEE))
	{
		rise_17e_b7 = buf[7] & (unsigned char)(~last_17e_b7);
		last_17e_b7 = buf[7];
		if(rise_17e_b7 & 0x01) gArmLeftSelfParallelParam.init_flags = 1;
		if(rise_17e_b7 & 0x02) gArmMidSelfParallelParam.init_flags = 1;
		if(rise_17e_b7 & 0x04) gArmRightSelfParallelParam.init_flags = 1;
	}
	rdCan = CAN2_Get_Data_Buf(0x50, buf);
	if((rdCan == 0xFF) || (rdCan == 0xEE))
	{
		return;
	}

	rise_b0 = buf[0] & (unsigned char)(~last_b0);
	rise_b1 = buf[1] & (unsigned char)(~last_b1);
	rise_b2 = buf[2] & (unsigned char)(~last_b2);
	rise_b3 = buf[3] & (unsigned char)(~last_b3);
	last_b0 = buf[0];
	last_b1 = buf[1];
	last_b2 = buf[2];
	last_b3 = buf[3];

	if((rise_b0 == 0) && (rise_b1 == 0) && (rise_b2 == 0) && (rise_b3 == 0))
	{
		return;
	}

	left_mask = 0;
	mid_mask = 0;
	right_mask = 0;
	body_mask = 0;

	if(rise_b0 & 0x01) left_mask |= ARM200A_ZERO_MASK_ROOT;
	if(rise_b0 & 0x02) left_mask |= ARM200A_ZERO_MASK_BASE;
	if(rise_b0 & 0x04) left_mask |= ARM200A_ZERO_MASK_BOOM;
	if(rise_b0 & 0x08) left_mask |= ARM200A_ZERO_MASK_D1;
	if(rise_b0 & 0x10) left_mask |= ARM200A_ZERO_MASK_E1;
	if(rise_b0 & 0x20) left_mask |= ARM200A_ZERO_MASK_F;
	if(rise_b0 & 0x40) left_mask |= ARM200A_ZERO_MASK_CD;
	if(rise_b0 & 0x80) left_mask |= ARM200A_ZERO_MASK_GG1;
	if(rise_b1 & 0x01) left_mask |= ARM200A_ZERO_MASK_HOLE_DEPTH;

	if(rise_b1 & 0x02) mid_mask |= ARM200A_ZERO_MASK_BASE;
	if(rise_b1 & 0x04) mid_mask |= ARM200A_ZERO_MASK_BOOM;
	if(rise_b1 & 0x08) mid_mask |= ARM200A_ZERO_MASK_D1;
	if(rise_b1 & 0x10) mid_mask |= ARM200A_ZERO_MASK_E1;
	if(rise_b1 & 0x20) mid_mask |= ARM200A_ZERO_MASK_F;
	if(rise_b1 & 0x40) mid_mask |= ARM200A_ZERO_MASK_CD;
	if(rise_b1 & 0x80) mid_mask |= ARM200A_ZERO_MASK_GG1;
	if(rise_b2 & 0x01) mid_mask |= ARM200A_ZERO_MASK_HOLE_DEPTH;

	if(rise_b2 & 0x02) right_mask |= ARM200A_ZERO_MASK_ROOT;
	if(rise_b2 & 0x04) right_mask |= ARM200A_ZERO_MASK_BASE;
	if(rise_b2 & 0x08) right_mask |= ARM200A_ZERO_MASK_BOOM;
	if(rise_b2 & 0x10) right_mask |= ARM200A_ZERO_MASK_D1;
	if(rise_b2 & 0x20) right_mask |= ARM200A_ZERO_MASK_E1;
	if(rise_b2 & 0x40) right_mask |= ARM200A_ZERO_MASK_F;
	if(rise_b2 & 0x80) right_mask |= ARM200A_ZERO_MASK_CD;
	if(rise_b3 & 0x01) right_mask |= ARM200A_ZERO_MASK_GG1;
	if(rise_b3 & 0x02) body_mask |= ARM200A_ZERO_MASK_BODY_ROLL;
	if(rise_b3 & 0x04) body_mask |= ARM200A_ZERO_MASK_BODY_PITCH;

	left_mask |= body_mask;
	mid_mask |= body_mask;
	right_mask |= body_mask;
	gArmZeroMaskLeft |= left_mask;
	gArmZeroMaskMid |= mid_mask;
	gArmZeroMaskRight |= right_mask;
}


static double Arm200A_ClampDouble(double v, double min_v, double max_v)
{
	if(v < min_v)
	{
		return min_v;
	}
	if(v > max_v)
	{
		return max_v;
	}
	return v;
}

static unsigned char Arm200A_SelfParallelCalcOne(ARM200A_SELF_PARALLEL_IN *in, float *ArmAlUpDownAngle_DEV, float *ArmAlNearAngle_DEV)
{
	double a1;
	double a2;
	double a3;
	double a6;
	double a7;
	double a8;
	double a9;
	double a10;
	double a4;
	double a5;
	double a4_num;
	double a5_arg;
	double c5;

	if((in == 0) || (ArmAlUpDownAngle_DEV == 0) || (ArmAlNearAngle_DEV == 0))
	{
		return 0;
	}

	if(in->init_flags)
	{
		in->ArmLeftRightAngle_initial = in->ArmLeftRightAngle_HMI;
		in->ArmUpDownAngle_initial = in->ArmUpDownAngle_HMI;
		in->ArmAlLeftRightAngle_initial = in->ArmAlLeftRightAngle_HMI;
		in->ArmAlUpDownAngle_initial = in->ArmAlUpDownAngle_HMI;
		in->ArmAlNearAngle_initial = in->ArmAlNearAngle_HMI;
		in->init_flags = 0;
	}

	a1 = (double)in->ArmLeftRightAngle_HMI * ARM200A_SELF_PARALLEL_PI / 180.0;
	a2 = (double)in->ArmUpDownAngle_HMI * ARM200A_SELF_PARALLEL_PI / 180.0;
	a3 = (double)in->ArmAlLeftRightAngle_HMI * ARM200A_SELF_PARALLEL_PI / 180.0;
	a6 = (double)in->ArmLeftRightAngle_initial * ARM200A_SELF_PARALLEL_PI / 180.0;
	a7 = (double)in->ArmUpDownAngle_initial * ARM200A_SELF_PARALLEL_PI / 180.0;
	a8 = (double)in->ArmAlLeftRightAngle_initial * ARM200A_SELF_PARALLEL_PI / 180.0;
	a9 = (double)in->ArmAlUpDownAngle_initial * ARM200A_SELF_PARALLEL_PI / 180.0;
	a10 = (double)in->ArmAlNearAngle_initial * ARM200A_SELF_PARALLEL_PI / 180.0;

	a5_arg = cos(a1)*cos(a3)*cos(a6)*cos(a8)*sin(a10) - cos(a2)*cos(a9)*cos(a10)*sin(a3)*sin(a7) + cos(a3)*cos(a8)*sin(a1)*sin(a6)*sin(a10) + cos(a2)*cos(a7)*sin(a3)*sin(a8)*sin(a10) + cos(a1)*cos(a3)*cos(a7)*cos(a9)*cos(a10)*sin(a6) - cos(a3)*cos(a6)*cos(a7)*cos(a9)*cos(a10)*sin(a1) + cos(a1)*cos(a3)*cos(a6)*cos(a10)*sin(a8)*sin(a9) - cos(a2)*cos(a7)*cos(a8)*cos(a10)*sin(a3)*sin(a9) - cos(a1)*cos(a8)*sin(a2)*sin(a3)*sin(a6)*sin(a10) + cos(a6)*cos(a8)*sin(a1)*sin(a2)*sin(a3)*sin(a10) + cos(a1)*cos(a3)*sin(a6)*sin(a7)*sin(a8)*sin(a10) - cos(a3)*cos(a6)*sin(a1)*sin(a7)*sin(a8)*sin(a10) + cos(a3)*cos(a10)*sin(a1)*sin(a6)*sin(a8)*sin(a9) + cos(a1)*cos(a6)*sin(a2)*sin(a3)*sin(a7)*sin(a8)*sin(a10) - cos(a1)*cos(a10)*sin(a2)*sin(a3)*sin(a6)*sin(a8)*sin(a9) + cos(a6)*cos(a10)*sin(a1)*sin(a2)*sin(a3)*sin(a8)*sin(a9) + sin(a1)*sin(a2)*sin(a3)*sin(a6)*sin(a7)*sin(a8)*sin(a10) + cos(a1)*cos(a6)*cos(a7)*cos(a9)*cos(a10)*sin(a2)*sin(a3) - cos(a1)*cos(a3)*cos(a8)*cos(a10)*sin(a6)*sin(a7)*sin(a9) + cos(a3)*cos(a6)*cos(a8)*cos(a10)*sin(a1)*sin(a7)*sin(a9) + cos(a7)*cos(a9)*cos(a10)*sin(a1)*sin(a2)*sin(a3)*sin(a6) - cos(a1)*cos(a6)*cos(a8)*cos(a10)*sin(a2)*sin(a3)*sin(a7)*sin(a9) - cos(a8)*cos(a10)*sin(a1)*sin(a2)*sin(a3)*sin(a6)*sin(a7)*sin(a9);
	a5 = asin(Arm200A_ClampDouble(a5_arg, -1.0, 1.0));
	c5 = cos(a5);
	if(c5 < 0.000001 && c5 > -0.000001)
	{
		return 0;
	}

	a4_num = cos(a2)*cos(a3)*cos(a9)*cos(a10)*sin(a7) + cos(a1)*cos(a6)*cos(a8)*sin(a3)*sin(a10) - cos(a2)*cos(a3)*cos(a7)*sin(a8)*sin(a10) + cos(a8)*sin(a1)*sin(a3)*sin(a6)*sin(a10) + cos(a2)*cos(a3)*cos(a7)*cos(a8)*cos(a10)*sin(a9) + cos(a1)*cos(a7)*cos(a9)*cos(a10)*sin(a3)*sin(a6) - cos(a6)*cos(a7)*cos(a9)*cos(a10)*sin(a1)*sin(a3) + cos(a1)*cos(a3)*cos(a8)*sin(a2)*sin(a6)*sin(a10) - cos(a3)*cos(a6)*cos(a8)*sin(a1)*sin(a2)*sin(a10) + cos(a1)*cos(a6)*cos(a10)*sin(a3)*sin(a8)*sin(a9) + cos(a1)*sin(a3)*sin(a6)*sin(a7)*sin(a8)*sin(a10) - cos(a6)*sin(a1)*sin(a3)*sin(a7)*sin(a8)*sin(a10) + cos(a10)*sin(a1)*sin(a3)*sin(a6)*sin(a8)*sin(a9) - cos(a3)*sin(a1)*sin(a2)*sin(a6)*sin(a7)*sin(a8)*sin(a10) - cos(a1)*cos(a3)*cos(a6)*cos(a7)*cos(a9)*cos(a10)*sin(a2) - cos(a3)*cos(a7)*cos(a9)*cos(a10)*sin(a1)*sin(a2)*sin(a6) - cos(a1)*cos(a3)*cos(a6)*sin(a2)*sin(a7)*sin(a8)*sin(a10) + cos(a1)*cos(a3)*cos(a10)*sin(a2)*sin(a6)*sin(a8)*sin(a9) - cos(a3)*cos(a6)*cos(a10)*sin(a1)*sin(a2)*sin(a8)*sin(a9) - cos(a1)*cos(a8)*cos(a10)*sin(a3)*sin(a6)*sin(a7)*sin(a9) + cos(a6)*cos(a8)*cos(a10)*sin(a1)*sin(a3)*sin(a7)*sin(a9) + cos(a1)*cos(a3)*cos(a6)*cos(a8)*cos(a10)*sin(a2)*sin(a7)*sin(a9) + cos(a3)*cos(a8)*cos(a10)*sin(a1)*sin(a2)*sin(a6)*sin(a7)*sin(a9);
	a4 = asin(Arm200A_ClampDouble(a4_num / c5, -1.0, 1.0));

	*ArmAlUpDownAngle_DEV = in->ArmAlUpDownAngle_HMI - (float)(a4 * 180.0 / ARM200A_SELF_PARALLEL_PI);
	*ArmAlNearAngle_DEV = in->ArmAlNearAngle_HMI - (float)(a5 * 180.0 / ARM200A_SELF_PARALLEL_PI);
	return 1;
}

static void Arm200A_SelfParallelCalc(ARM200A_SELF_PARALLEL_IN *in, ARM200A_SELF_PARALLEL_OUT *out)
{
	float ArmAlUpDownAngle_DEV;
	float ArmAlNearAngle_DEV;
	float deadband;

	memset(out, 0, sizeof(*out));
	if(Arm200A_SelfParallelCalcOne(in, &ArmAlUpDownAngle_DEV, &ArmAlNearAngle_DEV) == 0)
	{
		return;
	}

	out->ud_dev_deg = ArmAlUpDownAngle_DEV;
	out->near_dev_deg = ArmAlNearAngle_DEV;
	out->ok = 1;

	deadband = gArmSelfParallelDeadbandDeg;
	if(out->ud_dev_deg >= deadband)
	{
		out->up = 1;
	}
	else if(out->ud_dev_deg <= -deadband)
	{
		out->down = 1;
	}

	if(out->near_dev_deg >= deadband)
	{
		out->right = 1;
	}
	else if(out->near_dev_deg <= -deadband)
	{
		out->left = 1;
	}
}
static void Arm200A_UpdateSelfParallelOne(ARM200A_SELF_PARALLEL_IN *in, ARM200A_SELF_PARALLEL_OUT *out)
{
	Arm200A_SelfParallelCalc(in, out);
}

static void Arm200A_SelfParallelSetHmi(ARM200A_SELF_PARALLEL_IN *param, ARM200A_RAW_INPUT *raw)
{
	param->ArmLeftRightAngle_HMI = raw->base_raw;
	param->ArmUpDownAngle_HMI = raw->boom_raw;
	param->ArmAlLeftRightAngle_HMI = raw->d1_raw;
	param->ArmAlUpDownAngle_HMI = raw->e1_raw;
	param->ArmAlNearAngle_HMI = raw->f_raw;
}

static void Arm200A_UpdateSelfParallel(void)
{
	Arm200A_SelfParallelSetHmi(&gArmLeftSelfParallelParam, &gArmLeftRaw);
	Arm200A_SelfParallelSetHmi(&gArmMidSelfParallelParam, &gArmMidRaw);
	Arm200A_SelfParallelSetHmi(&gArmRightSelfParallelParam, &gArmRightRaw);
	Arm200A_UpdateSelfParallelOne(&gArmLeftSelfParallelParam, &gArmLeftSelfParallelOut);
	Arm200A_UpdateSelfParallelOne(&gArmMidSelfParallelParam, &gArmMidSelfParallelOut);
	Arm200A_UpdateSelfParallelOne(&gArmRightSelfParallelParam, &gArmRightSelfParallelOut);
}

static unsigned char Arm200A_SelfParallelPackOne(ARM200A_SELF_PARALLEL_OUT *out)
{
	if(out->ok == 0)
	{
		return 0;
	}
	return (unsigned char)((out->up ? 0x01 : 0) | (out->down ? 0x02 : 0) | (out->left ? 0x04 : 0) | (out->right ? 0x08 : 0));
}

void CAN_Send_data(void )
{
	static unsigned char send50msCnt = 0;
	static unsigned char send80msCnt = 0;
	static unsigned char send150msCnt = 0;
	static unsigned char send50msStep = 0;
	static unsigned char send150msStep = 0;
	static unsigned char send50msActive = 0;
	static unsigned char send80msActive = 0;
	static unsigned char send150msActive = 0;
	ARM200A_H1_INFO *sendH1;

	sendH1 = &gArmLeftH1Info;
	if(boom_type == ARM200A_MID)
	{
		sendH1 = &gArmMidH1Info;
	}
	else if(boom_type == ARM200A_RIGHT)
	{
		sendH1 = &gArmRightH1Info;
	}

	if(send50msActive == 0)
	{
		send50msCnt++;
		if(send50msCnt >= 5)
		{
			send50msCnt = 0;
			send50msActive = 1;
			send50msStep = 0;
		}
	}

	if(send80msActive == 0)
	{
		send80msCnt++;
		if(send80msCnt >= 8)
		{
			send80msCnt = 0;
			send80msActive = 1;
		}
	}

	if(send150msActive == 0)
	{
		send150msCnt++;
		if(send150msCnt >= 15)
		{
			send150msCnt = 0;
			send150msActive = 1;
			send150msStep = 0;
		}
	}

	if(send50msActive)
	{
		switch(send50msStep)
		{
			case 0:
				Arm200A_PackS32(0, Arm200A_FloatToLongX100(gArmLeftOut.h.x));
				Arm200A_PackS32(4, Arm200A_FloatToLongX100(gArmLeftOut.h.y));
				CAN2_SendX(0x150, CAN_SBuf2);
				break;

			case 1:
				Arm200A_PackS32(0, Arm200A_FloatToLongX100(gArmLeftOut.h.z));
				CAN_SBuf2[4] = 0;
				CAN_SBuf2[5] = 0;
				CAN_SBuf2[6] = 0;
				CAN_SBuf2[7] = 0;
				CAN2_SendX(0x151, CAN_SBuf2);
				break;

			case 2:
				Arm200A_PackS32(0, Arm200A_FloatToLongX100(gArmMidOut.h.x));
				Arm200A_PackS32(4, Arm200A_FloatToLongX100(gArmMidOut.h.y));
				CAN2_SendX(0x1D0, CAN_SBuf2);
				break;

			case 3:
				Arm200A_PackS32(0, Arm200A_FloatToLongX100(gArmMidOut.h.z));
				CAN_SBuf2[4] = 0;
				CAN_SBuf2[5] = 0;
				CAN_SBuf2[6] = 0;
				CAN_SBuf2[7] = 0;
				CAN2_SendX(0x1D1, CAN_SBuf2);
				break;

			case 4:
				Arm200A_PackS32(0, Arm200A_FloatToLongX100(gArmRightOut.h.x));
				Arm200A_PackS32(4, Arm200A_FloatToLongX100(gArmRightOut.h.y));
				CAN2_SendX(0x1E0, CAN_SBuf2);
				break;

			case 5:
				Arm200A_PackS32(0, Arm200A_FloatToLongX100(gArmRightOut.h.z));
				CAN_SBuf2[4] = 0;
				CAN_SBuf2[5] = 0;
				CAN_SBuf2[6] = 0;
				CAN_SBuf2[7] = 0;
				CAN2_SendX(0x1E1, CAN_SBuf2);
				break;

			case 6:
				Arm200A_UpdateSelfParallel();
				Arm200A_PackU16(0, Arm200A_HoleDepthSendValue(gArmLeftHoleDepthRaw, gArmLeftZero.hole_depth_zero_raw));
				Arm200A_PackU16(2, Arm200A_HoleDepthSendValue(gArmMidHoleDepthRaw, gArmMidZero.hole_depth_zero_raw));
				Arm200A_PackU16(4, Arm200A_HoleDepthSendValue(gArmRightHoleDepthRaw, gArmRightZero.hole_depth_zero_raw));
				CAN_SBuf2[6] = Arm200A_SelfParallelPackOne(&gArmLeftSelfParallelOut) | (Arm200A_SelfParallelPackOne(&gArmMidSelfParallelOut) << 4);
				CAN_SBuf2[7] = Arm200A_SelfParallelPackOne(&gArmRightSelfParallelOut);
				CAN2_SendX(0x153, CAN_SBuf2);
				break;

			default:
				Arm200A_PackS16(0, Arm200A_FloatToS16X100(gArmRightRaw.f_raw));
				Arm200A_PackS16(2, Arm200A_FloatToS16X100(gArmRightRaw.cd_raw));
				Arm200A_PackS16(4, Arm200A_FloatToS16X100(gArmRightRaw.gg1_raw));
				CAN_SBuf2[6] = 0;
				CAN_SBuf2[7] = 0;
				CAN2_SendX(0x159, CAN_SBuf2);
				break;
		}

		send50msStep++;
		if(send50msStep >= 8)
		{
			send50msStep = 0;
			send50msActive = 0;
		}
		return;
	}

	if(send80msActive)
	{
		Arm200A_PackS32(0, Arm200A_FloatToLongX100(sendH1->angle_xz_plane_deg));
		Arm200A_PackS32(4, Arm200A_FloatToLongX100(sendH1->angle_xy_plane_deg));
		CAN2_SendX(0x152, CAN_SBuf2);
		send80msActive = 0;
		return;
	}

	if(send150msActive)
	{
		switch(send150msStep)
		{
			case 0:
				Arm200A_PackS16(0, Arm200A_FloatToS16X100(gArmLeftRaw.root_raw));
				Arm200A_PackS16(2, Arm200A_FloatToS16X100(gArmLeftRaw.base_raw));
				Arm200A_PackS16(4, Arm200A_FloatToS16X100(gArmLeftRaw.boom_raw));
				Arm200A_PackS16(6, Arm200A_FloatToS16X100(gArmLeftRaw.d1_raw));
				CAN2_SendX(0x154, CAN_SBuf2);
				break;

			case 1:
				Arm200A_PackS16(0, Arm200A_FloatToS16X100(gArmLeftRaw.e1_raw));
				Arm200A_PackS16(2, Arm200A_FloatToS16X100(gArmLeftRaw.f_raw));
				Arm200A_PackS16(4, Arm200A_FloatToS16X100(gArmLeftRaw.cd_raw));
				Arm200A_PackS16(6, Arm200A_FloatToS16X100(gArmLeftRaw.gg1_raw));
				CAN2_SendX(0x155, CAN_SBuf2);
				break;

			case 2:
				Arm200A_PackS16(0, Arm200A_FloatToS16X100(gArmMidRaw.base_raw));
				Arm200A_PackS16(2, Arm200A_FloatToS16X100(gArmMidRaw.boom_raw));
				Arm200A_PackS16(4, Arm200A_FloatToS16X100(gArmMidRaw.d1_raw));
				Arm200A_PackS16(6, Arm200A_FloatToS16X100(gArmMidRaw.e1_raw));
				CAN2_SendX(0x156, CAN_SBuf2);
				break;

			case 3:
				Arm200A_PackS16(0, Arm200A_FloatToS16X100(gArmMidRaw.f_raw));
				Arm200A_PackS16(2, Arm200A_FloatToS16X100(gArmMidRaw.cd_raw));
				Arm200A_PackS16(4, Arm200A_FloatToS16X100(gArmMidRaw.gg1_raw));
				Arm200A_PackS16(6, Arm200A_FloatToS16X100(gArmRightRaw.root_raw));
				CAN2_SendX(0x157, CAN_SBuf2);
				break;

			default:
				Arm200A_PackS16(0, Arm200A_FloatToS16X100(gArmRightRaw.base_raw));
				Arm200A_PackS16(2, Arm200A_FloatToS16X100(gArmRightRaw.boom_raw));
				Arm200A_PackS16(4, Arm200A_FloatToS16X100(gArmRightRaw.d1_raw));
				Arm200A_PackS16(6, Arm200A_FloatToS16X100(gArmRightRaw.e1_raw));
				CAN2_SendX(0x158, CAN_SBuf2);
				break;
		}

		send150msStep++;
		if(send150msStep >= 5)
		{
			send150msStep = 0;
			send150msActive = 0;
		}
		return;
	}
}
void PIN_Binding_121(void) // 绑定121的IO点	 
{	
   /*

	Sensor_Logic_V	( MC121_AI1 , 0xf98, 0 ,4000) ; //  	 
 	Left_Front_Cycle_Hz = Senso_Logic_out  ;	 //  	 	    

	Sensor_Logic_V	( MC121_AI2 , 0xf98, 0 ,4000) ; //  	 
 	Left_Back_Cycle_Hz = Senso_Logic_out  ;	 //   			   

	Sensor_Logic_V	( MC121_AI3 , 0xf98, 0 ,4000) ; //  	 
 	IN_speed_Hz = Senso_Logic_out  ;	 //   			 

	Sensor_Logic_V	( MC121_AI4 ,  0x1385, 0x3dd ,100) ; //  	 
 	Temperature_sensor_Oil  = Senso_Logic_out  ;	 //   						 

  	
	Sensor_Logic_V	( MC121_AI5 , 0x1385, 0x3dd ,600) ; //  	 
 	Walk_press_bar = Senso_Logic_out  ;	 //  	
  	
	Sensor_Logic_V	( MC121_AI6 , 0x1385, 0x3dd ,500) ; //  	 
 	Hydraulic_oil_level = Senso_Logic_out  ;	 //  				

	
	Sensor_Logic_V	( SC121_AI1 , 0xf98, 0 ,4000) ; //  	 
 	Right_Front_Cycle_Hz = Senso_Logic_out  ;	 // 			    

	 
	Sensor_Logic_V	( SC121_AI2 , 0xf98, 0 ,4000) ; //  	 
 	Right_Back_Cycle_Hz = Senso_Logic_out  ;	 //  			   
	
	Sensor_Logic_V	( SC121_AI3 , 0xf98, 0 ,4000) ; //  	 
 	GT_Speed_Hz = Senso_Logic_out  ;	 //  			   
  */
  
}
 
/****************************************************************************************
****************************************************************************************
* 时间: 2019
* 名称：
* 功能：			把管脚号定义成程序语言变量。
*  
* 入口参数：
* 出口参数：
****************************************************************************************
****************************************************************************************/

void PIN_Binding(void)	 
{
 	 												   ;


		 		   
}
					    

 
void PWM_logic(unsigned int vX ,
				unsigned int vMaxA , 
				unsigned int vMaxB,
				unsigned int vMinA,
				unsigned int vMinB 		   
				   ) // 
/* 1: 手柄输入 
   2 :手柄上的最大电流
   3：手柄下的最大电流
   4：手柄上的起步电流
   5：手柄下的起步电流	   
*/
{

			if( (vMaxA < vMinA ) || (vMaxB < vMinB ) )
			{
				  return ;
			}

				if ( vX  <= 125 )
				 {	
				 	 PWMA_OUT = (float)( 1.0-vX / 125.0 ) * (vMaxA -vMinA )   ;
					 PWMB_OUT = 0 ; 
					 A_flags = 1 ;
					 B_flags = 0 ;
					 				 
				 }
				 else if (vX >= 129)	 
				 {	
				 	 PWMB_OUT = (float)( (vX-129.0) / 125.0 ) * (vMaxB -vMinB )     ;
					 PWMA_OUT = 0 ;	 
					 
					 A_flags = 0 ;
					 B_flags = 1 ;						 								 
				 }
				 else
				 {
				 	PWMB_OUT =0 ;
					PWMA_OUT =0 ; 
					
					 A_flags = 0 ;
					 B_flags = 0 ;	
				 
				 }
					

}
													  
 



 						 
 void SaveRetain_(void)
{	 						 
    						 	 		     ;
																			  		    							    
}
  
void ReadRetain_(void)
{	
 																		  
 						     ;
																   		   
 						 											    				 														 
}  
void on_off_time (unsigned int vMS)  //  通断。
{	
	on_off_time_dly ++ ;
	if (on_off_time_dly >vMS && on_off_time_dly <2*vMS)
	{
			on_off_time_out = 0 ;	
	
	}
	if (on_off_time_dly <=vMS)
	{
			on_off_time_out = 1 ;		
	}

	if (on_off_time_dly >=2*vMS)
	{
		  on_off_time_dly =0  ;
	}
	

}

 
 
void work_time  (void)
{	
 
	if ( 1   )	  //泵送计时
	{
		work_time1++;		
		if ( work_time1 >=5500 )
		{
			Whole_pumptime1_Min  ++; 	  
				save_time1 ++ ;
				if 	( save_time1 >5 )
				{
				 	gLCDPage.vSave =1 ;
					save_time1 =0 ;	
					
				}				
			if (Whole_pumptime1_Min  >= 60 )
			{
				Whole_pumptime1_Min  = 0 ;
				Whole_Pumptime1 = Whole_Pumptime1++ ; 
			
			}			    
			work_time1 =0 ;
		}	
	}
	
   if (1   )	  //泵送计时
	{
		work_time2++;		
		if ( work_time2 >=5500 )
		{
			Whole_pumptime2_Min  ++; 	  
				save_time2 ++ ;
				if 	( save_time2 >5 )
				{
				 	gLCDPage.vSave =1 ;
					save_time2 =0 ;	
					
				}				
			if (Whole_pumptime2_Min  >= 60 )
			{
				Whole_pumptime2_Min  = 0 ;
				Whole_Pumptime2 = Whole_Pumptime2++ ; 
			
			}			    
			work_time2 =0 ;
		}	
	}	
 

}
     
 
void work_logic(void)
{	 
   				     
	  ;
  
}  
void index_com(void)
{	   
    //Walk_F1_Fast_Pwm = gPwm_BD[0].vPwm_Param[0][0] ;	  
   		;
}

					  	
void TiaoSH_data(void)
{					 
 
	   	y_key_3_old =  	y_key_3 ;	  

}
 

void user_init()
{


  gArmLeftSample[0].use = 1;
  gArmLeftSample[0].in.root_deg = 10.0f;
  gArmLeftSample[0].in.base_deg = 5.0f;
  gArmLeftSample[0].in.boom_deg = -3.0f;
  gArmLeftSample[0].in.cd_mm = 3580.0f;
  gArmLeftSample[0].in.d1_deg = 2.0f;
  gArmLeftSample[0].in.e1_deg = -8.0f;
  gArmLeftSample[0].in.f_deg = 6.0f;
  gArmLeftSample[0].in.gg1_mm = 2500.0f;

  gArmLeftSample[0].h_ref.x = 1234.0f;
  gArmLeftSample[0].h_ref.y = 567.0f;
  gArmLeftSample[0].h_ref.z = 890.0f;
	
	
  gArmLeftSample[1].use = 1;
  gArmLeftSample[1].in.root_deg = 10.0f;
  gArmLeftSample[1].in.base_deg = 5.0f;
  gArmLeftSample[1].in.boom_deg = -3.0f;
  gArmLeftSample[1].in.cd_mm = 3580.0f;
  gArmLeftSample[1].in.d1_deg = 2.0f;
  gArmLeftSample[1].in.e1_deg = -8.0f;
  gArmLeftSample[1].in.f_deg = 6.0f;
  gArmLeftSample[1].in.gg1_mm = 2500.0f;

  gArmLeftSample[1].h_ref.x = 1234.0f;
  gArmLeftSample[1].h_ref.y = 567.0f;
  gArmLeftSample[1].h_ref.z = 890.0f;
	
	
	
  gArmLeftSample[2].use = 1;
  gArmLeftSample[2].in.root_deg = 10.0f;
  gArmLeftSample[2].in.base_deg = 5.0f;
  gArmLeftSample[2].in.boom_deg = -3.0f;
  gArmLeftSample[2].in.cd_mm = 3580.0f;
  gArmLeftSample[2].in.d1_deg = 2.0f;
  gArmLeftSample[2].in.e1_deg = -8.0f;
  gArmLeftSample[2].in.f_deg = 6.0f;
  gArmLeftSample[2].in.gg1_mm = 2500.0f;

  gArmLeftSample[2].h_ref.x = 1234.0f;
  gArmLeftSample[2].h_ref.y = 567.0f;
  gArmLeftSample[2].h_ref.z = 890.0f;
	
	

}
uchar boom_type =0 ;
void MyLogic_10ms(void)
{
	ScanCycle_Begin();
	if(gArm200AKineInitDone == 0)
	{
		/* Load one default geometry set and default sensor values once. */
		Arm200A_Init();
		Arm200A_StoreLoad();
		ScanCycle_Init();
		gArm200AKineInitDone = 1;
	 
		//user_init() ;
	}			  				    					 
	
 
   										  
	  can_send_10ms++ ;
	 index_com() ;  
    DI_Byte_DO() ;
	PIN_Binding() ;
	Arm200A_CAN1_Sensor_Binding();
	CAN_receive_data();
	Arm200A_Task10ms();
	CAN_Send_data();
 
//	CAN2_IO_pinding(8) ;
	  
			 gYunx20ms++  ;		  
		 
			 Prog_Run_2s++   ;
			 		  							  
	 					    
 		if( gECUDly	 ) gECUDly-- ;  
		
		
  if(0)	TiaoSH_data() ;
  else
  {  	
  	work_logic();				  	   		  		 
   }
	//	work_time();	   	   		  		 
	ScanCycle_End();
}



 	 
