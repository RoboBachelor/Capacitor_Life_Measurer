#include "timer.h"
#include "can.h"
#include "usart.h"
#include "sys.h"

/***************以定时器timer2作为发生器产生的PWM******************************/
/*
 * TIM2 PWM部分初始化
 * PWM输出初始化
 * arr：自动重装值
 * psc：时钟预分频数
 */


/*******************************
 *        用于摩擦轮组1控制
 *   输出两路PWM
 *********************************/

void TIM2_PWM_Init( u16 arr2, u16 psc2 )
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef	TIM_OCInitStructure;


	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE );  /* 使能定时器2时钟 */

	/***************CH1-PA15,CH2-PB3,CH3-PB10,CH4-PB11**********************/
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE ); /* 使能用到的端口组 */
	
	/****************GPIOB的配置************/
	{
		GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_0 | GPIO_Pin_1;            /* TIM_CH3 ,    TIM-CH2 端口PB5 PB0 */
		GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;                      /*复用推挽输出 */
		GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
		GPIO_Init( GPIOA, &GPIO_InitStructure );                                /* 初始化GPIO */
	}

	/* 初始化TIM2 */
	{
		TIM_TimeBaseStructure.TIM_Period	= arr2;                         /* 设置在下一个更新事件装入活动的自动重装载寄存器周期的值 */
		TIM_TimeBaseStructure.TIM_Prescaler	= psc2;                         /* 设置用来作为TIMx时钟频率除数的预分频值 */
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;                            /* 设置时钟分割:TDTS = Tck_tim */
		TIM_TimeBaseStructure.TIM_CounterMode	= TIM_CounterMode_Up;           /* TIM向上计数模式 */
		TIM_TimeBaseInit( TIM2, &TIM_TimeBaseStructure );                       /* 根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位 */
	}

	/* 初始化TIM3 Channel2 PWM模 */
	{
		TIM_OCInitStructure.TIM_OCMode		= TIM_OCMode_PWM2;              /*选择定时器模式:TIM脉冲宽度调制模式2 */
		TIM_OCInitStructure.TIM_OutputState	= TIM_OutputState_Enable;       /* 比较输出使能 */
		TIM_OCInitStructure.TIM_OCPolarity	= TIM_OCPolarity_Low;           /* 输出极性:TIM输出比较极性高 */
		TIM_OC1Init( TIM2, &TIM_OCInitStructure );                              /* 根据T指定的参数初始化外设TIM3 OC1通道1 */
		TIM_OC2Init( TIM2, &TIM_OCInitStructure );                              /* 根据T指定的参数初始化外设TIM3 OC2通道2 */
		TIM_OC3Init( TIM2, &TIM_OCInitStructure );                              /* 根据T指定的参数初始化外设TIM3 OC3通道3 */
		TIM_OC4Init( TIM2, &TIM_OCInitStructure );                              /* 根据T指定的参数初始化外设TIM3 OC4通道4 */
	}
	/***************使能各通道*********************/

	{
		TIM_OC1PreloadConfig( TIM2, TIM_OCPreload_Enable );                     /* 使能TIM3在CCR1上的预装载寄存器 */
		TIM_OC2PreloadConfig( TIM2, TIM_OCPreload_Enable );                     /* 使能TIM3在CCR2上的预装载寄存器 */
		TIM_OC3PreloadConfig( TIM2, TIM_OCPreload_Enable );                     /* 使能TIM3在CCR3上的预装载寄存器 */
		TIM_OC4PreloadConfig( TIM2, TIM_OCPreload_Enable );                     /* 使能TIM3在CCR4上的预装载寄存器 */
	}

	TIM_Cmd( TIM2, ENABLE );                                                        /* 使能TIM2 */
}


/***************以定时器timer3作为发生器产生的PWM******************************/
/*
 * TIM3 PWM部分初始化
 * PWM输出初始化
 * arr：自动重装值
 * psc：时钟预分频数
 * 未映射
 */
/****CH1-PA6,CH2-PA7,CH3-PB0,CH4-PB1**************/


/*******************************************
 * 用于摩擦轮组2的控制,及拨轮电机的控制
 *            输出四路PWM
 *
 ******************************************/
void TIM3_PWM_Init( u16 arr3, u16 psc3 )
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef	TIM_OCInitStructure;


	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3, ENABLE );                          /* 使能定时器3时钟 */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE );  /* 使能用到的端口组 */

	/****************GPIOB的配置************/

	{
		GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_0 | GPIO_Pin_1;              /* TIM_CH3 ,    TIM-CH2 端口PB5 PB0 */
		GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;                      /*复用推挽输出 */
		GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
		GPIO_Init( GPIOB, &GPIO_InitStructure );                                /* 初始化GPIO */
	}

	/****************GPIOA的配置************/


	{
		GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_6 | GPIO_Pin_7;              /* TIM_CH3 ,    TIM-CH2 端口PB5 PB0 */
		GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;                      /*复用推挽输出 */
		GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
		GPIO_Init( GPIOA, &GPIO_InitStructure );                                /* 初始化GPIO */
	}


	/* 初始化TIM3 */
	{
		TIM_TimeBaseStructure.TIM_Period	= arr3;                         /* 设置在下一个更新事件装入活动的自动重装载寄存器周期的值 */
		TIM_TimeBaseStructure.TIM_Prescaler	= psc3;                         /* 设置用来作为TIMx时钟频率除数的预分频值 */
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;                            /* 设置时钟分割:TDTS = Tck_tim */
		TIM_TimeBaseStructure.TIM_CounterMode	= TIM_CounterMode_Up;           /* TIM向上计数模式 */
		TIM_TimeBaseInit( TIM3, &TIM_TimeBaseStructure );                       /* 根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位 */
	}

	/* 初始化TIM3 Channel2 PWM模 */
	{
		TIM_OCInitStructure.TIM_OCMode		= TIM_OCMode_PWM2;              /*选择定时器模式:TIM脉冲宽度调制模式2 */
		TIM_OCInitStructure.TIM_OutputState	= TIM_OutputState_Enable;       /* 比较输出使能 */
		TIM_OCInitStructure.TIM_OCPolarity	= TIM_OCPolarity_Low;           /* 输出极性:TIM输出比较极性高 */
		TIM_OC1Init( TIM3, &TIM_OCInitStructure );                              /* 根据T指定的参数初始化外设TIM3 OC1通道1 */
		TIM_OC2Init( TIM3, &TIM_OCInitStructure );                              /* 根据T指定的参数初始化外设TIM3 OC2通道2 */
		TIM_OC3Init( TIM3, &TIM_OCInitStructure );                              /* 根据T指定的参数初始化外设TIM3 OC3通道3 */
		TIM_OC4Init( TIM3, &TIM_OCInitStructure );                              /* 根据T指定的参数初始化外设TIM3 OC4通道4 */
	}
	/***************使能各通道*********************/

	{
		TIM_OC1PreloadConfig( TIM3, TIM_OCPreload_Enable );                     /* 使能TIM3在CCR1上的预装载寄存器 */
		TIM_OC2PreloadConfig( TIM3, TIM_OCPreload_Enable );                     /* 使能TIM3在CCR2上的预装载寄存器 */
		TIM_OC3PreloadConfig( TIM3, TIM_OCPreload_Enable );                     /* 使能TIM3在CCR3上的预装载寄存器 */
		TIM_OC4PreloadConfig( TIM3, TIM_OCPreload_Enable );                     /* 使能TIM3在CCR4上的预装载寄存器 */
	}
	
	 

  //TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE 主输出使能	
	
	TIM_ARRPreloadConfig(TIM1, ENABLE); //使能TIMx在ARR上的预装载寄存器

	TIM_Cmd( TIM3, ENABLE );                                                        /* 使能TIM3 */
}


void TIM3_IRQHandler( void )                                    /* TIM3中断 */
{
	if ( TIM_GetITStatus( TIM3, TIM_IT_Update ) != RESET )  /* 检查TIM3更新中断发生与否 */
	{
		TIM_ClearITPendingBit( TIM3, TIM_IT_Update );   /* 清除TIMx更新中断标志 */
	}
}

 

void TIM4_Int_Init( u16 arr4, u16 psc4 )
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;

	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM4, ENABLE );                  /* 时钟使能 */

	/* 定时器TIM3初始化 */
	TIM_TimeBaseStructure.TIM_Period	= arr4;                         /* 设置在下一个更新事件装入活动的自动重装载寄存器周期的值 */
	TIM_TimeBaseStructure.TIM_Prescaler	= psc4;                         /* 设置用来作为TIMx时钟频率除数的预分频值 */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;                            /* 设置时钟分割:TDTS = Tck_tim */
	TIM_TimeBaseStructure.TIM_CounterMode	= TIM_CounterMode_Up;           /* TIM向上计数模式 */
	TIM_TimeBaseInit( TIM4, &TIM_TimeBaseStructure );                       /* 根据指定的参数初始化TIMx的时间基数单位 */

	TIM_ITConfig( TIM4, TIM_IT_Update, ENABLE );                            /* 使能指定的TIM3中断,允许更新中断 */

	/* 中断优先级NVIC设置 */
	NVIC_InitStructure.NVIC_IRQChannel			= TIM4_IRQn;    /* TIM3中断 */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 0;            /* 先占优先级0级 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority		= 0;            /* 从优先级3级 */
	NVIC_InitStructure.NVIC_IRQChannelCmd			= ENABLE;       /* IRQ通道被使能 */
	NVIC_Init( &NVIC_InitStructure );                                       /* 初始化NVIC寄存器 */


	TIM_Cmd( TIM4, ENABLE );                                                /* 使能TIMx */
}



