#include "can.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "MPU6050.h"
#include "delay.h"

/*
 * CAN��ʼ��
 * tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:CAN_SJW_1tq~ CAN_SJW_4tq
 * tbs2:ʱ���2��ʱ�䵥Ԫ.   ��Χ:CAN_BS2_1tq~CAN_BS2_8tq;
 * tbs1:ʱ���1��ʱ�䵥Ԫ.   ��Χ:CAN_BS1_1tq ~CAN_BS1_16tq
 * brp :�����ʷ�Ƶ��.��Χ:1~1024;  tq=(brp)*tpclk1
 * ������=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
 * mode:CAN_Mode_Normal,��ͨģʽ;CAN_Mode_LoopBack,�ػ�ģʽ;
 * Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ36M,�������CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_LoopBack);
 * ������Ϊ:36M/((8+9+1)*4)=500Kbps
 * ����ֵ:0,��ʼ��OK;
 * ����,��ʼ��ʧ��;
 */

u8 CAN_Mode_Init(u8 mode )
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	CAN_InitTypeDef		CAN_InitStructure;
	CAN_FilterInitTypeDef	CAN_FilterInitStructure;

	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );   /* ʹ��PORTAʱ�� */

	RCC_APB1PeriphClockCmd( RCC_APB1Periph_CAN1, ENABLE );                          /* ʹ��CAN1ʱ�� */


	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;                              /*�������� */
	GPIO_Init( GPIOA, &GPIO_InitStructure );                                        /* ��ʼ��IO */

	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IPU;                                /*�������� */
	GPIO_Init( GPIOA, &GPIO_InitStructure );                                        /* ��ʼ��IO */


	/* CAN��Ԫ���� */
	CAN_InitStructure.CAN_TTCM	= DISABLE;                                      /* ��ʱ�䴥��ͨ��ģʽ */
	CAN_InitStructure.CAN_ABOM	= ENABLE;                                       /* ����Զ����߹��� */
	CAN_InitStructure.CAN_AWUM	= DISABLE;                                      /* ˯��ģʽͨ���������(���CAN->MCR��SLEEPλ) */
	CAN_InitStructure.CAN_NART	= ENABLE;                                       /* ��ֹ�����Զ����� */
	CAN_InitStructure.CAN_RFLM	= DISABLE;                                      /* ���Ĳ�����,�µĸ��Ǿɵ� */
	CAN_InitStructure.CAN_TXFP	= ENABLE;                                       /* ���ȼ��ɱ��ı�ʶ������ */
	CAN_InitStructure.CAN_Mode	= mode;                                         /* ģʽ���ã� mode:0,��ͨģʽ;1,�ػ�ģʽ; */
	/* ���ò����� */
	CAN_InitStructure.CAN_SJW	= CAN_SJW_1tq;                                         /* ����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq */
	CAN_InitStructure.CAN_BS1	= CAN_BS1_3tq;                                         /* Tbs1=tbs1+1��ʱ�䵥λCAN_BS1_1tq ~CAN_BS1_16tq */
	CAN_InitStructure.CAN_BS2	= CAN_BS2_2tq;                                         /* Tbs2=tbs2+1��ʱ�䵥λCAN_BS2_1tq ~	CAN_BS2_8tq */
	CAN_InitStructure.CAN_Prescaler = 6;                                          /* ��Ƶϵ��(Fdiv)Ϊbrp+1 */
	CAN_Init( CAN1, &CAN_InitStructure );                                           /* ��ʼ��CAN1 */


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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 0;                    /* �����ȼ�Ϊ1 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority		= 1;                    /* �����ȼ�Ϊ0 */
	NVIC_InitStructure.NVIC_IRQChannelCmd			= ENABLE;
	NVIC_Init( &NVIC_InitStructure );

	return(0);
}

 



/********************************************************************
 ********************************************************************
 * Name��        void CAN1_RX0_IRQHandler(void) //CAN RX
 *
 * Description�� CAN1�Ľ����жϺ���
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
 * ������巢��ָ��
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
		i++;                                                        /* �ȴ�������� */
}

