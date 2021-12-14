#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h" 

#define S1  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)//读取按键1
#define S2  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)//读取按键2

#define S1_PRES 	1	//KEY4按下
#define S2_PRES 	2	//KEY5按下


void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8);  	//按键扫描函数					    
#endif
