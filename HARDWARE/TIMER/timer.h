#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

#define MAX_up      4200//�������ֵ
#define Mid_up      3700//�м�ֵ
#define MIN_up      3600 //������Сֵ

#define Middle_value_206   500

#define MAX_dn   6000//�������ֵ
#define Mid_dn   5000//�м�ֵ
#define MIN_dn   4500//������Сֵ

#define Middle_value_205 500


void TIM2_PWM_Init(u16 arr2,u16 psc2);
void TIM3_PWM_Init(u16 arr3,u16 psc3);
void TIM4_Int_Init(u16 arr4,u16 psc4); 
void TIM5_Int_Init(u16 arr5,u16 psc5);
#endif
