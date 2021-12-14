#include "timer.h"
#include "can.h"
#include "usart.h"
#include "sys.h"

/***************�Զ�ʱ��timer2��Ϊ������������PWM******************************/
/*
 * TIM2 PWM���ֳ�ʼ��
 * PWM�����ʼ��
 * arr���Զ���װֵ
 * psc��ʱ��Ԥ��Ƶ��
 */


/*******************************
 *        ����Ħ������1����
 *   �����·PWM
 *********************************/

void TIM2_PWM_Init( u16 arr2, u16 psc2 )
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef	TIM_OCInitStructure;


	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE );  /* ʹ�ܶ�ʱ��2ʱ�� */

	/***************CH1-PA15,CH2-PB3,CH3-PB10,CH4-PB11**********************/
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE ); /* ʹ���õ��Ķ˿��� */
	
	/****************GPIOB������************/
	{
		GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_0 | GPIO_Pin_1;            /* TIM_CH3 ,    TIM-CH2 �˿�PB5 PB0 */
		GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;                      /*����������� */
		GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
		GPIO_Init( GPIOA, &GPIO_InitStructure );                                /* ��ʼ��GPIO */
	}

	/* ��ʼ��TIM2 */
	{
		TIM_TimeBaseStructure.TIM_Period	= arr2;                         /* ��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ */
		TIM_TimeBaseStructure.TIM_Prescaler	= psc2;                         /* ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ */
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;                            /* ����ʱ�ӷָ�:TDTS = Tck_tim */
		TIM_TimeBaseStructure.TIM_CounterMode	= TIM_CounterMode_Up;           /* TIM���ϼ���ģʽ */
		TIM_TimeBaseInit( TIM2, &TIM_TimeBaseStructure );                       /* ����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ */
	}

	/* ��ʼ��TIM3 Channel2 PWMģ */
	{
		TIM_OCInitStructure.TIM_OCMode		= TIM_OCMode_PWM2;              /*ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2 */
		TIM_OCInitStructure.TIM_OutputState	= TIM_OutputState_Enable;       /* �Ƚ����ʹ�� */
		TIM_OCInitStructure.TIM_OCPolarity	= TIM_OCPolarity_Low;           /* �������:TIM����Ƚϼ��Ը� */
		TIM_OC1Init( TIM2, &TIM_OCInitStructure );                              /* ����Tָ���Ĳ�����ʼ������TIM3 OC1ͨ��1 */
		TIM_OC2Init( TIM2, &TIM_OCInitStructure );                              /* ����Tָ���Ĳ�����ʼ������TIM3 OC2ͨ��2 */
		TIM_OC3Init( TIM2, &TIM_OCInitStructure );                              /* ����Tָ���Ĳ�����ʼ������TIM3 OC3ͨ��3 */
		TIM_OC4Init( TIM2, &TIM_OCInitStructure );                              /* ����Tָ���Ĳ�����ʼ������TIM3 OC4ͨ��4 */
	}
	/***************ʹ�ܸ�ͨ��*********************/

	{
		TIM_OC1PreloadConfig( TIM2, TIM_OCPreload_Enable );                     /* ʹ��TIM3��CCR1�ϵ�Ԥװ�ؼĴ��� */
		TIM_OC2PreloadConfig( TIM2, TIM_OCPreload_Enable );                     /* ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ��� */
		TIM_OC3PreloadConfig( TIM2, TIM_OCPreload_Enable );                     /* ʹ��TIM3��CCR3�ϵ�Ԥװ�ؼĴ��� */
		TIM_OC4PreloadConfig( TIM2, TIM_OCPreload_Enable );                     /* ʹ��TIM3��CCR4�ϵ�Ԥװ�ؼĴ��� */
	}

	TIM_Cmd( TIM2, ENABLE );                                                        /* ʹ��TIM2 */
}


/***************�Զ�ʱ��timer3��Ϊ������������PWM******************************/
/*
 * TIM3 PWM���ֳ�ʼ��
 * PWM�����ʼ��
 * arr���Զ���װֵ
 * psc��ʱ��Ԥ��Ƶ��
 * δӳ��
 */
/****CH1-PA6,CH2-PA7,CH3-PB0,CH4-PB1**************/


/*******************************************
 * ����Ħ������2�Ŀ���,�����ֵ���Ŀ���
 *            �����·PWM
 *
 ******************************************/
void TIM3_PWM_Init( u16 arr3, u16 psc3 )
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef	TIM_OCInitStructure;


	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3, ENABLE );                          /* ʹ�ܶ�ʱ��3ʱ�� */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE );  /* ʹ���õ��Ķ˿��� */

	/****************GPIOB������************/

	{
		GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_0 | GPIO_Pin_1;              /* TIM_CH3 ,    TIM-CH2 �˿�PB5 PB0 */
		GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;                      /*����������� */
		GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
		GPIO_Init( GPIOB, &GPIO_InitStructure );                                /* ��ʼ��GPIO */
	}

	/****************GPIOA������************/


	{
		GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_6 | GPIO_Pin_7;              /* TIM_CH3 ,    TIM-CH2 �˿�PB5 PB0 */
		GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;                      /*����������� */
		GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
		GPIO_Init( GPIOA, &GPIO_InitStructure );                                /* ��ʼ��GPIO */
	}


	/* ��ʼ��TIM3 */
	{
		TIM_TimeBaseStructure.TIM_Period	= arr3;                         /* ��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ */
		TIM_TimeBaseStructure.TIM_Prescaler	= psc3;                         /* ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ */
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;                            /* ����ʱ�ӷָ�:TDTS = Tck_tim */
		TIM_TimeBaseStructure.TIM_CounterMode	= TIM_CounterMode_Up;           /* TIM���ϼ���ģʽ */
		TIM_TimeBaseInit( TIM3, &TIM_TimeBaseStructure );                       /* ����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ */
	}

	/* ��ʼ��TIM3 Channel2 PWMģ */
	{
		TIM_OCInitStructure.TIM_OCMode		= TIM_OCMode_PWM2;              /*ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2 */
		TIM_OCInitStructure.TIM_OutputState	= TIM_OutputState_Enable;       /* �Ƚ����ʹ�� */
		TIM_OCInitStructure.TIM_OCPolarity	= TIM_OCPolarity_Low;           /* �������:TIM����Ƚϼ��Ը� */
		TIM_OC1Init( TIM3, &TIM_OCInitStructure );                              /* ����Tָ���Ĳ�����ʼ������TIM3 OC1ͨ��1 */
		TIM_OC2Init( TIM3, &TIM_OCInitStructure );                              /* ����Tָ���Ĳ�����ʼ������TIM3 OC2ͨ��2 */
		TIM_OC3Init( TIM3, &TIM_OCInitStructure );                              /* ����Tָ���Ĳ�����ʼ������TIM3 OC3ͨ��3 */
		TIM_OC4Init( TIM3, &TIM_OCInitStructure );                              /* ����Tָ���Ĳ�����ʼ������TIM3 OC4ͨ��4 */
	}
	/***************ʹ�ܸ�ͨ��*********************/

	{
		TIM_OC1PreloadConfig( TIM3, TIM_OCPreload_Enable );                     /* ʹ��TIM3��CCR1�ϵ�Ԥװ�ؼĴ��� */
		TIM_OC2PreloadConfig( TIM3, TIM_OCPreload_Enable );                     /* ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ��� */
		TIM_OC3PreloadConfig( TIM3, TIM_OCPreload_Enable );                     /* ʹ��TIM3��CCR3�ϵ�Ԥװ�ؼĴ��� */
		TIM_OC4PreloadConfig( TIM3, TIM_OCPreload_Enable );                     /* ʹ��TIM3��CCR4�ϵ�Ԥװ�ؼĴ��� */
	}
	
	 

  //TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE �����ʹ��	
	
	TIM_ARRPreloadConfig(TIM1, ENABLE); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���

	TIM_Cmd( TIM3, ENABLE );                                                        /* ʹ��TIM3 */
}


void TIM3_IRQHandler( void )                                    /* TIM3�ж� */
{
	if ( TIM_GetITStatus( TIM3, TIM_IT_Update ) != RESET )  /* ���TIM3�����жϷ������ */
	{
		TIM_ClearITPendingBit( TIM3, TIM_IT_Update );   /* ���TIMx�����жϱ�־ */
	}
}

 

void TIM4_Int_Init( u16 arr4, u16 psc4 )
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;

	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM4, ENABLE );                  /* ʱ��ʹ�� */

	/* ��ʱ��TIM3��ʼ�� */
	TIM_TimeBaseStructure.TIM_Period	= arr4;                         /* ��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ */
	TIM_TimeBaseStructure.TIM_Prescaler	= psc4;                         /* ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;                            /* ����ʱ�ӷָ�:TDTS = Tck_tim */
	TIM_TimeBaseStructure.TIM_CounterMode	= TIM_CounterMode_Up;           /* TIM���ϼ���ģʽ */
	TIM_TimeBaseInit( TIM4, &TIM_TimeBaseStructure );                       /* ����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ */

	TIM_ITConfig( TIM4, TIM_IT_Update, ENABLE );                            /* ʹ��ָ����TIM3�ж�,��������ж� */

	/* �ж����ȼ�NVIC���� */
	NVIC_InitStructure.NVIC_IRQChannel			= TIM4_IRQn;    /* TIM3�ж� */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 0;            /* ��ռ���ȼ�0�� */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority		= 0;            /* �����ȼ�3�� */
	NVIC_InitStructure.NVIC_IRQChannelCmd			= ENABLE;       /* IRQͨ����ʹ�� */
	NVIC_Init( &NVIC_InitStructure );                                       /* ��ʼ��NVIC�Ĵ��� */


	TIM_Cmd( TIM4, ENABLE );                                                /* ʹ��TIMx */
}



