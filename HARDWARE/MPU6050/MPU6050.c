/************************************************************************************
  File Name     :  MPU6050.c 
  cpu           :  STM32F405RGT6
  Create Date   :  2015/3/13
  Author        :  wx
  Description   :   MPU6050��Ҫ�������ݵĶ�ȡ����  
  
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
//�ⲿ��������
extern unsigned char Re_buf[11];
extern unsigned char	sign;
//-----------------------------------------------------------------------------
//ȫ�ֱ�������
MPU6050_REAL_DATA   MPU6050_Data;

/*********************************************************************
Name��          void MPU_Init(void)  mpu6050 

Description��   mpu6050ģ�� ���³�ʼ�� ʹz��Ƕȹ��� 
              
*********************************************************************/
 void MPU_Init(void) 
{
	//��һ��ָ�� ʹMPU6050ģ�����³�ʼ��
	printf("%c",0xff);
	printf("%c",0xaa);
	printf("%c",0x52);	
}

/*********************************************************************
Name��          void MPU_Data_Read(void)  mpu6050 

Description��   mpu6050 ���ݶ�ȡ
              
*********************************************************************/
float a[3];
 void MPU_Data_Read(void)
{    
	unsigned char Temp[11];
	
	memcpy(Temp,Re_buf,11); 
	if(Re_buf[0]==0x55)       //���֡ͷ
	{  
		switch(Re_buf[1])
		{
			 case 0x51: //��ʶ������Ǽ��ٶȰ�
//					a[0] = ((short)(Temp[3]<<8 | Temp[2]))/32768.0*16;      //X����ٶ�
//					a[1] = ((short)(Temp[5]<<8 | Temp[4]))/32768.0*16;      //Y����ٶ�
//					a[2] = ((short)(Temp[7]<<8 | Temp[6]))/32768.0*16;      //Z����ٶ�
				//	T    = ((short)(Temp[9]<<8 | Temp[8]))/340.0+36.25;      //�¶�
					break;
			 case 0x52: //��ʶ������ǽ��ٶȰ�
				 MPU6050_Data.Gyro_X = ((short)(Temp[3]<<8| Temp[2]))/32768.0*2000;      //X����ٶ�
				 MPU6050_Data.Gyro_Y = ((short)(Temp[5]<<8| Temp[4]))/32768.0*2000;      //Y����ٶ�
				 MPU6050_Data.Gyro_Z = ((short)(Temp[7]<<8| Temp[6]))/32768.0*2000;      //Z����ٶ�
				 MPU6050_Data.Temp    = ((short)(Temp[9]<<8| Temp[8]))/100;      //�¶�
					break;
			 case 0x53: //��ʶ������ǽǶȰ�
					MPU6050_Data.Angle_X = ((short)(Temp[3]<<8| Temp[2]))/32768.0*180;   //X���ת�ǣ�x �ᣩ
					MPU6050_Data.Angle_Y = ((short)(Temp[5]<<8| Temp[4]))/32768.0*180;   //Y�ḩ���ǣ�y �ᣩ
					MPU6050_Data.Angle_Z = ((short)(Temp[7]<<8| Temp[6]))/32768.0*180;   //Z��ƫ���ǣ�z �ᣩ
					MPU6050_Data.Temp    = ((short)(Temp[9]<<8| Temp[8]))/340.0+36.25;   //�¶�
					break;
			 default:  break;
		}
	}
}


float last_206_angle = 0.0;       //�ϴ�pitch��ĽǶ�
float this_206_angle = 0.0;       //����pitch��ĽǶ�
float rate_206_angle = 0.0;       //pitch����ٶ� = ����pitch��Ƕ� - �ϴ�pitch��Ƕ�
float velocity_206_output = 0.0;  //pitch���ٶȻ��������ֵ
//float position_206_output = 0.0;  //pitch��λ�û��������ֵ

#define T   0.01
#define Tf  0.1
#define pi  3.1415926
float filter_in=0;
float last_filter_out=0;
float filter_out=0;
extern uint8_t count_shooting_flag;
/********************************************************************* 
Name��          void Gimbal_Control(void)  

Description��  ��̨���Ƴ���
              �����˶����� Ϊ��ֵ
*********************************************************************/
extern int16_t encoder_206;
 void Gimbal_Control(void)  
{
	
	//position_206_output = Position_Control_206(encoder_206,2000);//����λ�ñջ�������������ٶȱջ� 
	
  //velocity_206_output=Velocity_Control_206(MPU6050_Data.Gyro_Y,20);//�ٶȻ�ִ��PID
	
	velocity_206_output=User_PidSpeed_LocationControl(MPU6050_Data.Gyro_Y, 3000);
		
   //CAN1_send_data(5000, velocity_206_output);//����̨�����������
	
}


u8 table[100];

void LCD_display(void)
{
//   sprintf((char*)table,"%f",MPU6050_Data.Gyro_X);//��LCD ID��ӡ��lcd_id���顣	
//	 LCD_ShowString(30,40,210,24,24,table);
//	
//	 sprintf((char*)table,"%f",MPU6050_Data.Gyro_Y);//��LCD ID��ӡ��lcd_id���顣	
//	 LCD_ShowString(30,60,210,24,24,table);
//	
//	 sprintf((char*)table,"%f",MPU6050_Data.Gyro_Z);//��LCD ID��ӡ��lcd_id���顣	
//	 LCD_ShowString(30,80,210,24,24,table);
//	
//	sprintf((char*)table,"%f", MPU6050_Data.Temp);//��LCD ID��ӡ��lcd_id���顣	
//	 LCD_ShowString(30,100,210,24,24,table);
//	
//	sprintf((char*)table,"%f",MPU6050_Data.Angle_X);//��LCD ID��ӡ��lcd_id���顣	
//	 LCD_ShowString(30,120,210,24,24,table);
//	
//	sprintf((char*)table,"%f",MPU6050_Data.Angle_Y);//��LCD ID��ӡ��lcd_id���顣	
//	 LCD_ShowString(30,140,210,24,24,table);
//	
//	sprintf((char*)table,"%f",MPU6050_Data.Angle_Z);//��LCD ID��ӡ��lcd_id���顣	
//	 LCD_ShowString(30,160,210,24,24,table);
   
}










