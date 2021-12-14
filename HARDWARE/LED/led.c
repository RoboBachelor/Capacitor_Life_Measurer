#include "led.h"


void Led_Configuration(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	 //ʹ��PE,PF�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 //LED0-->PB0 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��PB0
 GPIO_SetBits(GPIOA,GPIO_Pin_4);						 //PPB0 �����

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	    		 //LED1-->PB1 �˿�����, �������
 GPIO_Init(GPIOA, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
 GPIO_SetBits(GPIOA,GPIO_Pin_5); 						 //PB1 �����  
}

void Sw_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	 //ʹ��PE,PF�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				 //Trigger ���̵��� �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��PB0
	GPIO_ResetBits(GPIOA,GPIO_Pin_3);						 //����� 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //EnableOut ��PMOS �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��PB0
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);						 //����� 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	    		 //SW1 �˿�����, �������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
	GPIO_ResetBits(GPIOA,GPIO_Pin_11); 						 //�����   

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	    		 //SW2 �˿�����, �������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
	GPIO_ResetBits(GPIOA,GPIO_Pin_11); 						 //�����  
}

 