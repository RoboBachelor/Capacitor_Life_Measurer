#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "delay.h"
#include "sys.h"
#include "usart.h"


typedef struct __MPU6050_Data__
{
    float Accel_X;  //ת����ʵ�ʵ�X����ٶȣ�
    float Accel_Y;  //ת����ʵ�ʵ�Y����ٶȣ�
    float Accel_Z;  //ת����ʵ�ʵ�Z����ٶȣ�
    float Temp;     //ת����ʵ�ʵ��¶ȣ���λΪ���϶�
    float Gyro_X;   //ת����ʵ�ʵ�X��Ǽ��ٶȣ�
    float Gyro_Y;   //ת����ʵ�ʵ�Y��Ǽ��ٶȣ�
    float Gyro_Z;   //ת����ʵ�ʵ�Z��Ǽ��ٶ�
	  float Angle_X;  // ģ���˲����x��Ƕ�
	  float Angle_Y;  //ģ���˲����y��Ƕ�
	  float Angle_Z;  //ģ���˲����z��Ƕ�
	
	
}MPU6050_REAL_DATA;


 void MPU_Data_Read(void);
 void Gimbal_Control(void) ;
 void MPU_Init(void) ;
 void LCD_display(void);


#endif

