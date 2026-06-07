/********************************************************************
*  
*  力矩有关的功能函数
*  功能：
*
********************************************************************/

#ifndef __APP_USR_H_
#define __APP_USR_H_


#include "config.h"
 /************************************ 函数 *************************************************/

void DIBitV_to_DIxx(void);
void DOxx_to_DOPort(void);
void DIx_to_DOx(void);
void DI_Byte_DO(void);
void Ramp_PWM(void);
void Zero_set_(void);	 
void adjust_PWM(void);	 	 
 void work_logic_auto(void);	
 void ASS_logic(void);	  // 辅助功能
void Remote_121_receive_data(void);
  
void index_com(void);
void work_logic(void);	  	 
   void data_bit_318_319_to_CAN2 (unsigned int vMS);	 

//void Can_Send_datas(void);

void MyLogic_1ms(void);
void MyLogic_10ms(void);
void CAN_Send_data(void ) ;
void CAN_receive_data(void);



#endif  //__APP_LIJU_H_
