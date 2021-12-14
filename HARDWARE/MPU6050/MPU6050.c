/************************************************************************************
  File Name     :  MPU6050.c 
  cpu           :  STM32F405RGT6
  Create Date   :  2015/3/13
  Author        :  wx
  Description   :   MPU6050主要用于数据的读取处理  
  
-------------------------------Revision Histroy-----------------------------------
No   Version    Date     Revised By       Item       Description   
1     1.0        3/13      wx             
************************************************************************************/	
#include "MPU6050.h"
#include "string.h"
#include "pidcontrol.h"
#include "Can.h"
#include "usart.h"
#include "lcd.h"
//-----------------------------------------------------------------------------
//外部变量声明
extern unsigned char Re_buf[11];
extern unsigned char	sign;
//-----------------------------------------------------------------------------
//全局变量定义
MPU6050_REAL_DATA   MPU6050_Data;

/*********************************************************************
Name：          void MPU_Init(void)  mpu6050 

Description：   mpu6050模块 重新初始化 使z轴角度归零 
              
*********************************************************************/
 void MPU_Init(void) 
{
	//发一段指令 使MPU6050模块重新初始化
	printf("%c",0xff);
	printf("%c",0xaa);
	printf("%c",0x52);	
}

/*********************************************************************
Name：          void MPU_Data_Read(void)  mpu6050 

Description：   mpu6050 数据读取
              
*********************************************************************/
float a[3];
 void MPU_Data_Read(void)
{    
	unsigned char Temp[11];
	
	memcpy(Temp,Re_buf,11); 
	if(Re_buf[0]==0x55)       //检查帧头
	{  
		switch(Re_buf[1])
		{
			 case 0x51: //标识这个包是加速度包
//					a[0] = ((short)(Temp[3]<<8 | Temp[2]))/32768.0*16;      //X轴加速度
//					a[1] = ((short)(Temp[5]<<8 | Temp[4]))/32768.0*16;      //Y轴加速度
//					a[2] = ((short)(Temp[7]<<8 | Temp[6]))/32768.0*16;      //Z轴加速度
				//	T    = ((short)(Temp[9]<<8 | Temp[8]))/340.0+36.25;      //温度
					break;
			 case 0x52: //标识这个包是角速度包
				 MPU6050_Data.Gyro_X = ((short)(Temp[3]<<8| Temp[2]))/32768.0*2000;      //X轴角速度
				 MPU6050_Data.Gyro_Y = ((short)(Temp[5]<<8| Temp[4]))/32768.0*2000;      //Y轴角速度
				 MPU6050_Data.Gyro_Z = ((short)(Temp[7]<<8| Temp[6]))/32768.0*2000;      //Z轴角速度
				 MPU6050_Data.Temp    = ((short)(Temp[9]<<8| Temp[8]))/100;      //温度
					break;
			 case 0x53: //标识这个包是角度包
					MPU6050_Data.Angle_X = ((short)(Temp[3]<<8| Temp[2]))/32768.0*180;   //X轴滚转角（x 轴）
					MPU6050_Data.Angle_Y = ((short)(Temp[5]<<8| Temp[4]))/32768.0*180;   //Y轴俯仰角（y 轴）
					MPU6050_Data.Angle_Z = ((short)(Temp[7]<<8| Temp[6]))/32768.0*180;   //Z轴偏航角（z 轴）
					MPU6050_Data.Temp    = ((short)(Temp[9]<<8| Temp[8]))/340.0+36.25;   //温度
					break;
			 default:  break;
		}
	}
}


float last_206_angle = 0.0;       //上次pitch轴的角度
float this_206_angle = 0.0;       //本次pitch轴的角度
float rate_206_angle = 0.0;       //pitch轴角速度 = 本次pitch轴角度 - 上次pitch轴角度
float velocity_206_output = 0.0;  //pitch轴速度环函数输出值
//float position_206_output = 0.0;  //pitch轴位置环函数输出值

#define T   0.01
#define Tf  0.1
#define pi  3.1415926
float filter_in=0;
float last_filter_out=0;
float filter_out=0;
extern uint8_t count_shooting_flag;
/********************************************************************* 
Name：          void Gimbal_Control(void)  

Description：  云台控制程序
              向上运动电流 为正值
*********************************************************************/
extern int16_t encoder_206;
 void Gimbal_Control(void)  
{
	
	//position_206_output = Position_Control_206(encoder_206,2000);//计算位置闭环输出量，传给速度闭环 
	
  //velocity_206_output=Velocity_Control_206(MPU6050_Data.Gyro_Y,20);//速度环执行PID
	
	velocity_206_output=User_PidSpeed_LocationControl(MPU6050_Data.Gyro_Y, 3000);
		
   //CAN1_send_data(5000, velocity_206_output);//往云台电机送入数据
	
}


u8 table[100];

void LCD_display(void)
{
//   sprintf((char*)table,"%f",MPU6050_Data.Gyro_X);//将LCD ID打印到lcd_id数组。	
//	 LCD_ShowString(30,40,210,24,24,table);
//	
//	 sprintf((char*)table,"%f",MPU6050_Data.Gyro_Y);//将LCD ID打印到lcd_id数组。	
//	 LCD_ShowString(30,60,210,24,24,table);
//	
//	 sprintf((char*)table,"%f",MPU6050_Data.Gyro_Z);//将LCD ID打印到lcd_id数组。	
//	 LCD_ShowString(30,80,210,24,24,table);
//	
//	sprintf((char*)table,"%f", MPU6050_Data.Temp);//将LCD ID打印到lcd_id数组。	
//	 LCD_ShowString(30,100,210,24,24,table);
//	
//	sprintf((char*)table,"%f",MPU6050_Data.Angle_X);//将LCD ID打印到lcd_id数组。	
//	 LCD_ShowString(30,120,210,24,24,table);
//	
//	sprintf((char*)table,"%f",MPU6050_Data.Angle_Y);//将LCD ID打印到lcd_id数组。	
//	 LCD_ShowString(30,140,210,24,24,table);
//	
//	sprintf((char*)table,"%f",MPU6050_Data.Angle_Z);//将LCD ID打印到lcd_id数组。	
//	 LCD_ShowString(30,160,210,24,24,table);
   
}










