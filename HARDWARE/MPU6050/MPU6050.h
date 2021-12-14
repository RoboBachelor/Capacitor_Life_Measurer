#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "delay.h"
#include "sys.h"
#include "usart.h"


typedef struct __MPU6050_Data__
{
    float Accel_X;  //转换成实际的X轴加速度，
    float Accel_Y;  //转换成实际的Y轴加速度，
    float Accel_Z;  //转换成实际的Z轴加速度，
    float Temp;     //转换成实际的温度，单位为摄氏度
    float Gyro_X;   //转换成实际的X轴角加速度，
    float Gyro_Y;   //转换成实际的Y轴角加速度，
    float Gyro_Z;   //转换成实际的Z轴角加速度
	  float Angle_X;  // 模块滤波后的x轴角度
	  float Angle_Y;  //模块滤波后的y轴角度
	  float Angle_Z;  //模块滤波后的z轴角度
	
	
}MPU6050_REAL_DATA;


 void MPU_Data_Read(void);
 void Gimbal_Control(void) ;
 void MPU_Init(void) ;
 void LCD_display(void);


#endif

