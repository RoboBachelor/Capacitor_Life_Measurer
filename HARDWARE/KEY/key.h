#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h" 

#define S1  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)//��ȡ����1
#define S2  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)//��ȡ����2

#define S1_PRES 	1	//KEY4����
#define S2_PRES 	2	//KEY5����


void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(u8);  	//����ɨ�躯��					    
#endif
