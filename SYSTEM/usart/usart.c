#include "sys.h"
#include "usart.h"
#include "delay.h"
RC_Ctl_t RC_Ctl;
volatile u8 sbus_rx_buffer[25] = {0}; 
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数 
u16 yuntai_flag=0;
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif 

void uart1_3_init(void) 
{ 
	/* -------------- Enable Module Clock Source ----------------------------*/ 
   
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
 	 
	/* -------------- Configure NVIC ----------------------------*/ 
	{ 
		NVIC_InitTypeDef  nvic; 
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		
		nvic.NVIC_IRQChannel = DMA1_Channel6_IRQn; 
		nvic.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
		nvic.NVIC_IRQChannelSubPriority = 0; 
		nvic.NVIC_IRQChannelCmd   = ENABLE; 
		NVIC_Init(&nvic);  
	}  


	/* -------------- Configure UART1  ---------------------------------------*/  

	{
		GPIO_InitTypeDef  gpio; 
		USART_InitTypeDef usart1;

		gpio.GPIO_Pin = GPIO_Pin_9 ;
		gpio.GPIO_Mode=GPIO_Mode_AF_PP;
		gpio.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &gpio);

		USART_DeInit(USART1);
		usart1.USART_BaudRate = 115200;
		usart1.USART_WordLength   = USART_WordLength_8b;
		usart1.USART_StopBits = USART_StopBits_1;
		usart1.USART_Parity = USART_Parity_No; 
		usart1.USART_Mode = USART_Mode_Tx; 
		usart1.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
		USART_Init(USART1,&usart1); 
		USART_Cmd(USART1,ENABLE); 
	}  
	/* -------------- Configure UART3  ---------------------------------------*/ 
	{
		GPIO_InitTypeDef  gpio; 
		USART_InitTypeDef usart3;

		gpio.GPIO_Pin = GPIO_Pin_10 ;
		gpio.GPIO_Mode=GPIO_Mode_AF_PP;
		gpio.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &gpio);

		USART_DeInit(USART3);
		usart3.USART_BaudRate = 115200;
		usart3.USART_WordLength   = USART_WordLength_8b;
		usart3.USART_StopBits = USART_StopBits_1;
		usart3.USART_Parity = USART_Parity_No; 
		usart3.USART_Mode = USART_Mode_Tx; 
		usart3.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
		USART_Init(USART3,&usart3); 
		USART_Cmd(USART3,ENABLE); 
	}
}

void uart2_dma_init(void){
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
 	 
/* -------------- Configure NVIC ----------------------------*/ 
	{ 
		NVIC_InitTypeDef  nvic; 
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		
		nvic.NVIC_IRQChannel = DMA1_Channel6_IRQn; 
		nvic.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
		nvic.NVIC_IRQChannelSubPriority = 0; 
		nvic.NVIC_IRQChannelCmd   = ENABLE; 
		NVIC_Init(&nvic);  
	}  

	 /* -------------- Configure UART2(Recevier) ---------------------------------------*/ 

	{
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}

	{
		USART_InitTypeDef usart2;
		USART_DeInit(USART2);
		usart2.USART_BaudRate = 100000;
		usart2.USART_WordLength   = USART_WordLength_8b;
		usart2.USART_StopBits = USART_StopBits_1;
		usart2.USART_Parity = USART_Parity_Even; 
		usart2.USART_Mode = USART_Mode_Rx|USART_Mode_Tx; 
		usart2.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
		USART_Init(USART2,&usart2); 
		USART_Cmd(USART2,ENABLE); 
		USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
	} 

	
/* -------------- Configure UART2 DMA -----------------------------------------*/  
	{ 
		DMA_InitTypeDef DMA_InitStructure;
		DMA_DeInit(DMA1_Channel6); 
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->DR);
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)sbus_rx_buffer;
		DMA_InitStructure.DMA_DIR =  DMA_DIR_PeripheralSRC;
		DMA_InitStructure.DMA_BufferSize = 18;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
		DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		DMA_Init(DMA1_Channel6, &DMA_InitStructure);
		DMA_Cmd(DMA1_Channel6,ENABLE);
		DMA_ITConfig(DMA1_Channel6,DMA_IT_TC, ENABLE);
	}                    //使能串口1 

}
	
void DMA1_Channel6_IRQHandler(void)
{  
	if(DMA_GetITStatus(DMA1_IT_TC6))  
	{ 
		DMA_ClearFlag(DMA1_FLAG_TC6); 
		DMA_ClearITPendingBit(DMA1_IT_TC6); 
	
		RC_Ctl.rc.ch0 = (sbus_rx_buffer[0]| (sbus_rx_buffer[1] << 8)) & 0x07ff;                                       //!< Channel 0
		RC_Ctl.rc.ch1 = ((sbus_rx_buffer[1] >> 3) | (sbus_rx_buffer[2] << 5)) & 0x07ff;                               //!< Channel 1 
		RC_Ctl.rc.ch2 = ((sbus_rx_buffer[2] >> 6) | (sbus_rx_buffer[3] << 2) |  (sbus_rx_buffer[4] << 10)) & 0x07ff;  //!< Channel 2  
		RC_Ctl.rc.ch3 = ((sbus_rx_buffer[4] >> 1) | (sbus_rx_buffer[5] << 7)) & 0x07ff;                               //!< Channel 3 
		RC_Ctl.rc.s1  = ((sbus_rx_buffer[5] >> 4)& 0x000C) >> 2;                                                      //!< Switch left
		RC_Ctl.rc.s2  = ((sbus_rx_buffer[5] >> 4)& 0x0003);                                                           //!< Switch right  
		RC_Ctl.mouse.x = sbus_rx_buffer[6] | (sbus_rx_buffer[7] << 8);                                                //!< Mouse X axis 
		RC_Ctl.mouse.y = sbus_rx_buffer[8] | (sbus_rx_buffer[9] << 8);                                                //!< Mouse Y axis 
		RC_Ctl.mouse.z = sbus_rx_buffer[10] | (sbus_rx_buffer[11] << 8);                                              //!< Mouse Z axis 
		RC_Ctl.mouse.press_l = sbus_rx_buffer[12];                                                                    //!< Mouse Left Is Press ? 
		RC_Ctl.mouse.press_r = sbus_rx_buffer[13];                                                                    //!< Mouse Right Is Press ? 
		RC_Ctl.key.v = sbus_rx_buffer[14] | (sbus_rx_buffer[15] << 8);                                                //!< KeyBoard value */
	} 
} 





