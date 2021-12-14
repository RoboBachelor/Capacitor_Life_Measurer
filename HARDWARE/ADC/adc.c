#include "adc.h"
#include "can.h"
#include "usart.h"
#include "sys.h"
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
void ADC1_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef  ADC_InitStructure;
 
    /*V_BAT*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
 
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //��������ģʽ
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;   //��ɨ��ģʽ
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//����ת��ģʽ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel =1;
    ADC_Init(ADC1,&ADC_InitStructure);
    ADC_Cmd(ADC1,ENABLE); //ʹ��ADC1
    ADC_ResetCalibration(ADC1); //ʹ�ܸ�λУ׼
    while(ADC_GetResetCalibrationStatus(ADC1)); //�ȴ���λУ׼����
    ADC_StartCalibration(ADC1); //����ADУ׼
    while(ADC_GetCalibrationStatus(ADC1));//�ȴ�У׼����
}

//��ȡĳ��ͨ����ADCֵ
float Get_Adc(unsigned char ch)
{
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    return ADC_GetConversionValue(ADC1);
} 