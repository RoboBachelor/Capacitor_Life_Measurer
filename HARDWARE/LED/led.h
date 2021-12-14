#ifndef __LED_H
#define __LED_H	 
#include "sys.h" 

#define  LEDG_ON()      GPIO_ResetBits(GPIOA, GPIO_Pin_5)
#define  LEDG_OFF()     GPIO_SetBits(GPIOA, GPIO_Pin_5)

#define  LEDR_ON()        GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define  LEDR_OFF()       GPIO_SetBits(GPIOA, GPIO_Pin_4)

#define  Trigger_ON()        GPIO_SetBits(GPIOA, GPIO_Pin_3)
#define  Trigger_OFF()       GPIO_ResetBits(GPIOA, GPIO_Pin_3)

#define  EnableOut_ON()        GPIO_SetBits(GPIOA, GPIO_Pin_8)
#define  EnableOut_OFF()       GPIO_ResetBits(GPIOA, GPIO_Pin_8)

#define  SW1_ON()        GPIO_SetBits(GPIOA, GPIO_Pin_11)
#define  SW1_OFF()       GPIO_ResetBits(GPIOA, GPIO_Pin_11)

#define  SW2_ON()        GPIO_SetBits(GPIOA, GPIO_Pin_12)
#define  SW2_OFF()       GPIO_ResetBits(GPIOA, GPIO_Pin_12)

void Led_Configuration(void);
void Sw_Configuration(void);
		 				    
#endif
