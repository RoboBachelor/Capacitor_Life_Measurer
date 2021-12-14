#include "can.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "MPU6050.h"
#include "delay.h"

/*
 * CAN初始化
 * tsjw:重新同步跳跃时间单元.范围:CAN_SJW_1tq~ CAN_SJW_4tq
 * tbs2:时间段2的时间单元.   范围:CAN_BS2_1tq~CAN_BS2_8tq;
 * tbs1:时间段1的时间单元.   范围:CAN_BS1_1tq ~CAN_BS1_16tq
 * brp :波特率分频器.范围:1~1024;  tq=(brp)*tpclk1
 * 波特率=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
 * mode:CAN_Mode_Normal,普通模式;CAN_Mode_LoopBack,回环模式;
 * Fpclk1的时钟在初始化的时候设置为36M,如果设置CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_LoopBack);
 * 则波特率为:36M/((8+9+1)*4)=500Kbps
 * 返回值:0,初始化OK;
 * 其他,初始化失败;
 */

u8 CAN_Mode_Init(u8 mode )
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	CAN_InitTypeDef		CAN_InitStructure;
	CAN_FilterInitTypeDef	CAN_FilterInitStructure;

	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );   /* 使能PORTA时钟 */

	RCC_APB1PeriphClockCmd( RCC_APB1Periph_CAN1, ENABLE );                          /* 使能CAN1时钟 */


	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;                              /*复用推挽 */
	GPIO_Init( GPIOA, &GPIO_InitStructure );                                        /* 初始化IO */

	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IPU;                                /*上拉输入 */
	GPIO_Init( GPIOA, &GPIO_InitStructure );                                        /* 初始化IO */


	/* CAN单元设置 */
	CAN_InitStructure.CAN_TTCM	= DISABLE;                                      /* 非时间触发通信模式 */
	CAN_InitStructure.CAN_ABOM	= ENABLE;                                       /* 软件自动离线管理 */
	CAN_InitStructure.CAN_AWUM	= DISABLE;                                      /* 睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位) */
	CAN_InitStructure.CAN_NART	= ENABLE;                                       /* 禁止报文自动传送 */
	CAN_InitStructure.CAN_RFLM	= DISABLE;                                      /* 报文不锁定,新的覆盖旧的 */
	CAN_InitStructure.CAN_TXFP	= ENABLE;                                       /* 优先级由报文标识符决定 */
	CAN_InitStructure.CAN_Mode	= mode;                                         /* 模式设置： mode:0,普通模式;1,回环模式; */
	/* 设置波特率 */
	CAN_InitStructure.CAN_SJW	= CAN_SJW_1tq;                                         /* 重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq */
	CAN_InitStructure.CAN_BS1	= CAN_BS1_3tq;                                         /* Tbs1=tbs1+1个时间单位CAN_BS1_1tq ~CAN_BS1_16tq */
	CAN_InitStructure.CAN_BS2	= CAN_BS2_2tq;                                         /* Tbs2=tbs2+1个时间单位CAN_BS2_1tq ~	CAN_BS2_8tq */
	CAN_InitStructure.CAN_Prescaler = 6;                                          /* 分频系数(Fdiv)为brp+1 */
	CAN_Init( CAN1, &CAN_InitStructure );                                           /* 初始化CAN1 */


	CAN_FilterInitStructure.CAN_FilterNumber		= 10;
	CAN_FilterInitStructure.CAN_FilterMode		= CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale		= CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh		= 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow		= 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh		= 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow		= 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment	= 0;
	CAN_FilterInitStructure.CAN_FilterActivation		= ENABLE;
	CAN_FilterInit( &CAN_FilterInitStructure );
	CAN_ITConfig( CAN1, CAN_IT_FMP0, ENABLE );
	CAN_ITConfig( CAN1, CAN_IT_TME, ENABLE );
	

	/* NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); */

	NVIC_InitStructure.NVIC_IRQChannel			= USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 0;                    /* 主优先级为1 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority		= 1;                    /* 次优先级为0 */
	NVIC_InitStructure.NVIC_IRQChannelCmd			= ENABLE;
	NVIC_Init( &NVIC_InitStructure );

	return(0);
}

 



/********************************************************************
 ********************************************************************
 * Name：        void CAN1_RX0_IRQHandler(void) //CAN RX
 *
 * Description： CAN1的接收中断函数
 *
 *********************************************************************/
CanRxMsg rx_message;
float ZGyroModuleAngle=0.0;
short acc_can[3];
short gyro_can[3];
float euler_can[3];

void USB_LP_CAN1_RX0_IRQHandler( void )
{
	if ( CAN_GetITStatus( CAN1, CAN_IT_FMP0 ) != RESET )
	{
		CAN_ITConfig(CAN1,CAN_IT_TME,DISABLE);
		CAN_ClearITPendingBit( CAN1, CAN_IT_FMP0 );
		CAN_Receive( CAN1, CAN_FIFO0, &rx_message );
		
		if(rx_message.StdId==0x401)
		{
			ZGyroModuleAngle=(float)(0.00857142857143)*((int32_t)(rx_message.Data[0]<<24)|(int32_t)(rx_message.Data[1]<<16) | (int32_t)(rx_message.Data[2]<<8) | (int32_t)(rx_message.Data[3]));
		}
		
		if(rx_message.StdId == 0xA)
    {
        euler_can[0] = *(float *)&(rx_message.Data[0]);
        euler_can[1] = *(float *)&(rx_message.Data[4]);  
    }
    else if(rx_message.StdId == 0xB)
    {
        euler_can[2] = *(float *)&(rx_message.Data[0]);
        acc_can[0] = *(short *)&(rx_message.Data[4]);
        acc_can[1] = *(short *)&(rx_message.Data[6]);
    }
    else if(rx_message.StdId == 0xC)
    {
        acc_can[2] = *(short *)&(rx_message.Data[0]);
        gyro_can[0] = *(short *)&(rx_message.Data[2]);
        gyro_can[1] = *(short *)&(rx_message.Data[4]);
        gyro_can[2] = *(short *)&(rx_message.Data[6]);
    }

		CAN_ITConfig(CAN1,CAN_IT_TME,ENABLE); 
	}
}

extern u8 trigger;

/********************************************************************************
 * 给电调板发送指令
 *********************************************************************************/
void CAN1_send_data(void)
{
	u8		mbox;
	u16		i = 0;
	CanTxMsg	TxMessage;

	TxMessage.StdId = 0x201;
	TxMessage.IDE	= CAN_Id_Standard;
	TxMessage.RTR	= CAN_RTR_Data;
	TxMessage.DLC	= 0x08;
  
	if(trigger==1)
	{
		TxMessage.Data[0]	= 0x01;
		TxMessage.Data[1]	= 0x01;
		TxMessage.Data[2]	= 0x00;
		TxMessage.Data[3]	= 0x00;
		TxMessage.Data[4]	= 0x00;
		TxMessage.Data[5]	= 0x00;
		TxMessage.Data[6]	= 0x00;
		TxMessage.Data[7]	= 0x00;
	}
	else if(trigger==0)
	{
		TxMessage.Data[0]	= 0x00;
		TxMessage.Data[1]	= 0x00;
		TxMessage.Data[2]	= 0x00;
		TxMessage.Data[3]	= 0x00;
		TxMessage.Data[4]	= 0x00;
		TxMessage.Data[5]	= 0x00;
		TxMessage.Data[6]	= 0x00;
		TxMessage.Data[7]	= 0x00;		
	}
  
	mbox = CAN_Transmit( CAN1, &TxMessage );
	while ( (CAN_TransmitStatus( CAN1, mbox ) == CAN_TxStatus_Failed) && (i < 0XFFF) )
		i++;                                                        /* 等待发送完成 */
}

