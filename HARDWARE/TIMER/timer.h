#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

#define MAX_up      4200//上限最大值
#define Mid_up      3700//中间值
#define MIN_up      3600 //下限最小值

#define Middle_value_206   500

#define MAX_dn   6000//左限最大值
#define Mid_dn   5000//中间值
#define MIN_dn   4500//右限最小值

#define Middle_value_205 500


void TIM2_PWM_Init(u16 arr2,u16 psc2);
void TIM3_PWM_Init(u16 arr3,u16 psc3);
void TIM4_Int_Init(u16 arr4,u16 psc4); 
void TIM5_Int_Init(u16 arr5,u16 psc5);
#endif
