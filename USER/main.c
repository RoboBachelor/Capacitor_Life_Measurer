#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "can.h"
#include "timer.h"
#include "main.h"
#include "led.h"
#include "adc.h"
#include "key.h"
#include "math.h"
#include "oled.h"
#include "bsp_flash.h"
#include "ds18b20.h"

u8 trigger = 0;
u8 trigger_action = 0;

typedef enum {
    INIT = 0,
    CALIB,
    CHARGING,
    DISCHARGING,
} State_t;

__IO State_t state_flag = INIT;        //0 初始化/无操作 1 校准 2 充电 3 放电
__IO uint8_t circular_mode = 0;
__IO uint8_t calib_mode = 0;

float vol_cap = 0.0;    //电容电压
float vol_pow = 0.0;    //电源电压
float cur_cap = 0.0;    //电容电流
float charge_energy = 0.0f;        //充电电量
float discharge_energy = 0.0f;    //放电电量

float zero_cur = 1.65f;        //零点位移
float vol_cal = 1.0;//电压校准系数
float cur_cal = 1.0;//电流校准系数

float vol_cap_max = 0.f;    // 满电电压
float vol_cap_min = 0.f;    // 亏电电压

uint16_t s1_cnt = 0;
uint16_t s2_cnt = 0;
int32_t cycle_count;

float start_temp, full_temp, stop_temp;
uint32_t charging_time, discharging_time;


void collect_info() {
    float adc_sum = 0.0;
    float buf = 0.0;
    int i = 0;

    //采样电容电压
    adc_sum = 0;
    for (i = 0; i < 2; i++) {
        adc_sum += Get_Adc(0);
    }
    buf = (adc_sum / 2.0) * 3.3 / 4096.0;
    vol_cap = buf * 11.0 * vol_cal;

    //采样电源电压
    adc_sum = 0;
    for (i = 0; i < 2; i++) {
        adc_sum += Get_Adc(2);
    }
    buf = (adc_sum / 2.0) * 3.3 / 4096.0;
    vol_pow = buf * 11.0 * vol_cal;

    //采样电容电流
    adc_sum = 0;
    for (i = 0; i < 2; i++) {
        adc_sum += Get_Adc(1);
    }
    //buf=(adc_sum/2.0)* 3.3 / 4096.0;
    //cur_cap=(buf-1.65f)/100.0/0.005f;
    buf = (adc_sum / 2.0) * 3.3 / 4096.0;
    cur_cap = (buf - zero_cur) / 50.0 / 0.005f * cur_cal;
    //cur_cap=adc_sum/2;
}

void dis_info() {
    u8 lcd_str1[20];            //存放LCD ID字符串
    u8 lcd_str2[20];            //存放LCD ID字符串
    u8 lcd_str3[20];            //存放LCD ID字符串
    u8 lcd_str4[20];            //存放LCD ID字符串

    sprintf((char *) lcd_str1, "V:%2.1fV C:%2.1fA", vol_cap, cur_cap);//将LCD ID打印到lcd_id数组。
    sprintf((char *) lcd_str2, "Power:%2.1fV S:%d", vol_pow, state_flag);//将LCD ID打印到lcd_id数组。


    disp_string_8x16_16x16(1, 1, lcd_str1);
    disp_string_8x16_16x16(3, 1, lcd_str2);
    //disp_string_8x16_16x16(5,1,lcd_str3);
    //disp_string_8x16_16x16(7,1,lcd_str4);
}

/*
需要实现的功能：
1.充放电次数技术（1~1000000次） 写在FLASH里，掉电不遗失，同时按S1和S2才清零
2.检测充满：设定一个值，V_full_ref=2.5f, 电压误差和这个相近，且电流的变化量小于指定值，则认定充满
3.检测放光：设定一个值，V_zero_ref=0.0f, 电压误差和这个相近，且电流的变化量小于指定值，则认定放光
4.按下S1校准0点（已经实现）
5.按下S2开始循环测量，充电-》放电——》充电——》放电，直至再次按下S2
6.每一次放电结束，通过串口上传数据："N:充放次数 C:充电能量 D:放电能量 V:满电电压 亏电电压 状态值"
7.如果在充电、放电过程中出现full_scale, 状态值需要报警 红灯亮
*/
void all_off(void) {
    Trigger_OFF();
    EnableOut_OFF();
    SW1_OFF();
    SW2_OFF();
}


int main(void) {
    u8 lcd_str[20];            //存放LCD ID字符串
    u16 i = 0;
    u8 key_value = 0;
    float adc_sum = 0.0f;
    delay_init(72);             //延时函数初始
    Led_Configuration();   //初始化LED
    Sw_Configuration();    //初始化开关
    LEDG_ON();
    LEDR_ON();
            SW1_ON();//闭合放电开关
            SW2_ON();//闭合放电开关
    ADC1_init();
    KEY_Init();//按键初始化
    TIM4_Int_Init(14375, 4);//72分频，周期为1ms	// 中断程序，用于定时数据处理
    uart1_3_init();
		
		cycle_count = read_count();
		ds18b20_init();

    OLED_Init();            //初始化OLED
    initial_olcd();          //初始化
    clear_screen();            //清屏
		printf("COUNT, Charge(J), Discharge(J), Vmax, Vmin, Start Temp, Full Temp, Stop Temp, Charging Time(ms), Discharging Time(ms)\r\n");
    while (1) {
				collect_info();
        dis_info();
        // key_value = KEY_Scan(0);

				sprintf((char *) lcd_str, "S2: Start");
				disp_string_8x16_16x16(5, 1, lcd_str);
			
        if (calib_mode) {
            all_off();//打开所有开关，然后校准0状态

            TIM_Cmd(TIM4, DISABLE);                                                /* 禁止TIMx */

            state_flag = CALIB;
            dis_info();

            delay_ms(100);
            adc_sum = 0.0f;
            for (i = 0; i < 100; i++) {
                adc_sum += Get_Adc(1);
                sprintf((char *) lcd_str, "Cal Cur:%3.1f %%", (i / 100.0f) * 100.0f);//将LCD ID打印到lcd_id数组。
                disp_string_8x16_16x16(5, 1, lcd_str);
                delay_ms(1);
            }
            zero_cur = (adc_sum / 100.0f) * 3.3f / 4096.0f;
            //printf((char*)lcd_str,"                ");//清空
            //disp_string_8x16_16x16(5,1,lcd_str);
            sprintf((char *) lcd_str, "Zero Cur:%2.3f", zero_cur);//将LCD ID打印到lcd_id数组。
            disp_string_8x16_16x16(5, 1, lcd_str);
            state_flag = INIT;

            TIM_Cmd(TIM4, ENABLE);                                                /* 使能TIMx */
            dis_info();
						calib_mode = 0;
        } 
				
				while(circular_mode) {
					
						sprintf((char *) lcd_str, "Circular: ON ");
						disp_string_8x16_16x16(7, 1, lcd_str);
					
            all_off();//打开所有开关

						// 记录初始温度
						start_temp = ds18b20_read();
					
            // 充电过程
            charge_energy = 0;    //清零充电电量计数
            state_flag = CHARGING;            //充电标志
            dis_info();                //显示信息
            //Trigger_ON();		//闭合，给电容充电（继电器）
            EnableOut_ON();        //闭合，PMOS,给电容充电
						charging_time = 0;
						discharging_time = 0;
					
            // 等待充电完成
            while (state_flag == CHARGING){
								dis_info();
								delay_ms(40);
								sprintf((char *) lcd_str, "%5d|CRG:%3.0fJ", cycle_count, charge_energy);
								disp_string_8x16_16x16(5, 1, lcd_str);
							
								if(circular_mode == 0){									
										sprintf((char *) lcd_str, "Circular: OFF");
										disp_string_8x16_16x16(7, 1, lcd_str);
								} else{
										sprintf((char *) lcd_str, "Circular: ON ");
										disp_string_8x16_16x16(7, 1, lcd_str);
								}
						}

            state_flag = INIT;            //转为空闲标志
            //Trigger_OFF();	//打开，不给电容充电（继电器）
            EnableOut_OFF();    //打开，PMOS,不给电容充电
            sprintf((char *) lcd_str, "                ");//清空
            disp_string_8x16_16x16(5, 1, lcd_str);
            sprintf((char *) lcd_str, "Charged:%3.0fJ", charge_energy);//将LCD ID打印到lcd_id数组。
            disp_string_8x16_16x16(5, 1, lcd_str);

            // 获取满电电压
            collect_info();
            vol_cap_max = vol_cap;

						
						// 记录满电温度
						full_temp = ds18b20_read();
						
            all_off();//打开所有开关
            //放电过程
            discharge_energy = 0; //清零充电电量计数
            state_flag = DISCHARGING; //放电标志
            dis_info();//显示信息
            SW1_ON();//闭合放电开关
            SW2_ON();//闭合放电开关

						// 等待放电结束
            while (state_flag == DISCHARGING){
								dis_info();
								delay_ms(40);
								sprintf((char *) lcd_str, "%5d|DCG:%3.0fJ", cycle_count, -discharge_energy);
								disp_string_8x16_16x16(5, 1, lcd_str);
							
								if(circular_mode == 0){
										sprintf((char *) lcd_str, "Circular: OFF");
										disp_string_8x16_16x16(7, 1, lcd_str);
								} else{
										sprintf((char *) lcd_str, "Circular: ON ");
										disp_string_8x16_16x16(7, 1, lcd_str);
								}
						}

            state_flag = INIT; //转为空闲标志
            SW1_OFF();
            SW2_OFF();
            sprintf((char *) lcd_str, "                ");//清空
            disp_string_8x16_16x16(5, 1, lcd_str);
            sprintf((char *) lcd_str, "Dis:%5.4fJ", discharge_energy);//将LCD ID打印到lcd_id数组。
            disp_string_8x16_16x16(5, 1, lcd_str);

            // 获取亏电电压
            collect_info();
            vol_cap_min = vol_cap;

						
						// 记录放电后温度
						stop_temp = ds18b20_read();
						
            //发送数据
            printf("%d, %f, %f, %f, %f, %f, %f, %f, %d, %d\r\n", 
								cycle_count++, charge_energy, discharge_energy, vol_cap_max, vol_cap_min,
								start_temp, full_temp, stop_temp, charging_time, discharging_time);
						if(!(cycle_count & 31)){
								write_count(cycle_count);
						}
						
        }
    }

}

/* 定时器4中断服务程序 */
// 1ms周期
unsigned char led_state = 0;

void TIM4_IRQHandler(void)                                    /* TIM4中断 */
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  /* 检查TIM4更新中断发生与否 */
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);   /* 清除TIMx更新中断标志 */
        if (state_flag == CHARGING)//充电
        {
						collect_info();
            charge_energy += cur_cap * vol_cap * 0.001f;
            if (vol_cap > 2.6f) {
                state_flag = INIT;
            }
						++charging_time;
        }
				else if (state_flag == DISCHARGING)//放电
        {
						collect_info();
            discharge_energy += cur_cap * vol_cap * 0.001f;
            if (vol_cap < 0.1f) {
                state_flag = INIT;
            }
						++discharging_time;
        }
				
				if(S1 == 0){
						s1_cnt++;
				} else{
						s1_cnt = 0;
				}
				if(S2 == 0){
						s2_cnt++;
				} else{
						s2_cnt = 0;
				}
				if(s2_cnt == 100){
						if(!circular_mode){
								circular_mode = 1;
						} else{
								circular_mode = 0;
								write_count(cycle_count + 1);
						}
				}
				if(s1_cnt > 1000 && s2_cnt > 1000){
						write_count(cycle_count = 0);
						s1_cnt = 0;
				}
    }
}
