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

__IO State_t state_flag = INIT;        //0 ��ʼ��/�޲��� 1 У׼ 2 ��� 3 �ŵ�
__IO uint8_t circular_mode = 0;
__IO uint8_t calib_mode = 0;

float vol_cap = 0.0;    //���ݵ�ѹ
float vol_pow = 0.0;    //��Դ��ѹ
float cur_cap = 0.0;    //���ݵ���
float charge_energy = 0.0f;        //������
float discharge_energy = 0.0f;    //�ŵ����

float zero_cur = 1.65f;        //���λ��
float vol_cal = 1.0;//��ѹУ׼ϵ��
float cur_cal = 1.0;//����У׼ϵ��

float vol_cap_max = 0.f;    // �����ѹ
float vol_cap_min = 0.f;    // �����ѹ

uint16_t s1_cnt = 0;
uint16_t s2_cnt = 0;
int32_t cycle_count;

float start_temp, full_temp, stop_temp;
uint32_t charging_time, discharging_time;


void collect_info() {
    float adc_sum = 0.0;
    float buf = 0.0;
    int i = 0;

    //�������ݵ�ѹ
    adc_sum = 0;
    for (i = 0; i < 2; i++) {
        adc_sum += Get_Adc(0);
    }
    buf = (adc_sum / 2.0) * 3.3 / 4096.0;
    vol_cap = buf * 11.0 * vol_cal;

    //������Դ��ѹ
    adc_sum = 0;
    for (i = 0; i < 2; i++) {
        adc_sum += Get_Adc(2);
    }
    buf = (adc_sum / 2.0) * 3.3 / 4096.0;
    vol_pow = buf * 11.0 * vol_cal;

    //�������ݵ���
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
    u8 lcd_str1[20];            //���LCD ID�ַ���
    u8 lcd_str2[20];            //���LCD ID�ַ���
    u8 lcd_str3[20];            //���LCD ID�ַ���
    u8 lcd_str4[20];            //���LCD ID�ַ���

    sprintf((char *) lcd_str1, "V:%2.1fV C:%2.1fA", vol_cap, cur_cap);//��LCD ID��ӡ��lcd_id���顣
    sprintf((char *) lcd_str2, "Power:%2.1fV S:%d", vol_pow, state_flag);//��LCD ID��ӡ��lcd_id���顣


    disp_string_8x16_16x16(1, 1, lcd_str1);
    disp_string_8x16_16x16(3, 1, lcd_str2);
    //disp_string_8x16_16x16(5,1,lcd_str3);
    //disp_string_8x16_16x16(7,1,lcd_str4);
}

/*
��Ҫʵ�ֵĹ��ܣ�
1.��ŵ����������1~1000000�Σ� д��FLASH����粻��ʧ��ͬʱ��S1��S2������
2.���������趨һ��ֵ��V_full_ref=2.5f, ��ѹ�������������ҵ����ı仯��С��ָ��ֵ�����϶�����
3.���Ź⣺�趨һ��ֵ��V_zero_ref=0.0f, ��ѹ�������������ҵ����ı仯��С��ָ��ֵ�����϶��Ź�
4.����S1У׼0�㣨�Ѿ�ʵ�֣�
5.����S2��ʼѭ�����������-���ŵ硪������硪�����ŵ磬ֱ���ٴΰ���S2
6.ÿһ�ηŵ������ͨ�������ϴ����ݣ�"N:��Ŵ��� C:������� D:�ŵ����� V:�����ѹ �����ѹ ״ֵ̬"
7.����ڳ�硢�ŵ�����г���full_scale, ״ֵ̬��Ҫ���� �����
*/
void all_off(void) {
    Trigger_OFF();
    EnableOut_OFF();
    SW1_OFF();
    SW2_OFF();
}


int main(void) {
    u8 lcd_str[20];            //���LCD ID�ַ���
    u16 i = 0;
    u8 key_value = 0;
    float adc_sum = 0.0f;
    delay_init(72);             //��ʱ������ʼ
    Led_Configuration();   //��ʼ��LED
    Sw_Configuration();    //��ʼ������
    LEDG_ON();
    LEDR_ON();
            SW1_ON();//�պϷŵ翪��
            SW2_ON();//�պϷŵ翪��
    ADC1_init();
    KEY_Init();//������ʼ��
    TIM4_Int_Init(14375, 4);//72��Ƶ������Ϊ1ms	// �жϳ������ڶ�ʱ���ݴ���
    uart1_3_init();
		
		cycle_count = read_count();
		ds18b20_init();

    OLED_Init();            //��ʼ��OLED
    initial_olcd();          //��ʼ��
    clear_screen();            //����
		printf("COUNT, Charge(J), Discharge(J), Vmax, Vmin, Start Temp, Full Temp, Stop Temp, Charging Time(ms), Discharging Time(ms)\r\n");
    while (1) {
				collect_info();
        dis_info();
        // key_value = KEY_Scan(0);

				sprintf((char *) lcd_str, "S2: Start");
				disp_string_8x16_16x16(5, 1, lcd_str);
			
        if (calib_mode) {
            all_off();//�����п��أ�Ȼ��У׼0״̬

            TIM_Cmd(TIM4, DISABLE);                                                /* ��ֹTIMx */

            state_flag = CALIB;
            dis_info();

            delay_ms(100);
            adc_sum = 0.0f;
            for (i = 0; i < 100; i++) {
                adc_sum += Get_Adc(1);
                sprintf((char *) lcd_str, "Cal Cur:%3.1f %%", (i / 100.0f) * 100.0f);//��LCD ID��ӡ��lcd_id���顣
                disp_string_8x16_16x16(5, 1, lcd_str);
                delay_ms(1);
            }
            zero_cur = (adc_sum / 100.0f) * 3.3f / 4096.0f;
            //printf((char*)lcd_str,"                ");//���
            //disp_string_8x16_16x16(5,1,lcd_str);
            sprintf((char *) lcd_str, "Zero Cur:%2.3f", zero_cur);//��LCD ID��ӡ��lcd_id���顣
            disp_string_8x16_16x16(5, 1, lcd_str);
            state_flag = INIT;

            TIM_Cmd(TIM4, ENABLE);                                                /* ʹ��TIMx */
            dis_info();
						calib_mode = 0;
        } 
				
				while(circular_mode) {
					
						sprintf((char *) lcd_str, "Circular: ON ");
						disp_string_8x16_16x16(7, 1, lcd_str);
					
            all_off();//�����п���

						// ��¼��ʼ�¶�
						start_temp = ds18b20_read();
					
            // ������
            charge_energy = 0;    //�������������
            state_flag = CHARGING;            //����־
            dis_info();                //��ʾ��Ϣ
            //Trigger_ON();		//�պϣ������ݳ�磨�̵�����
            EnableOut_ON();        //�պϣ�PMOS,�����ݳ��
						charging_time = 0;
						discharging_time = 0;
					
            // �ȴ�������
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

            state_flag = INIT;            //תΪ���б�־
            //Trigger_OFF();	//�򿪣��������ݳ�磨�̵�����
            EnableOut_OFF();    //�򿪣�PMOS,�������ݳ��
            sprintf((char *) lcd_str, "                ");//���
            disp_string_8x16_16x16(5, 1, lcd_str);
            sprintf((char *) lcd_str, "Charged:%3.0fJ", charge_energy);//��LCD ID��ӡ��lcd_id���顣
            disp_string_8x16_16x16(5, 1, lcd_str);

            // ��ȡ�����ѹ
            collect_info();
            vol_cap_max = vol_cap;

						
						// ��¼�����¶�
						full_temp = ds18b20_read();
						
            all_off();//�����п���
            //�ŵ����
            discharge_energy = 0; //�������������
            state_flag = DISCHARGING; //�ŵ��־
            dis_info();//��ʾ��Ϣ
            SW1_ON();//�պϷŵ翪��
            SW2_ON();//�պϷŵ翪��

						// �ȴ��ŵ����
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

            state_flag = INIT; //תΪ���б�־
            SW1_OFF();
            SW2_OFF();
            sprintf((char *) lcd_str, "                ");//���
            disp_string_8x16_16x16(5, 1, lcd_str);
            sprintf((char *) lcd_str, "Dis:%5.4fJ", discharge_energy);//��LCD ID��ӡ��lcd_id���顣
            disp_string_8x16_16x16(5, 1, lcd_str);

            // ��ȡ�����ѹ
            collect_info();
            vol_cap_min = vol_cap;

						
						// ��¼�ŵ���¶�
						stop_temp = ds18b20_read();
						
            //��������
            printf("%d, %f, %f, %f, %f, %f, %f, %f, %d, %d\r\n", 
								cycle_count++, charge_energy, discharge_energy, vol_cap_max, vol_cap_min,
								start_temp, full_temp, stop_temp, charging_time, discharging_time);
						if(!(cycle_count & 31)){
								write_count(cycle_count);
						}
						
        }
    }

}

/* ��ʱ��4�жϷ������ */
// 1ms����
unsigned char led_state = 0;

void TIM4_IRQHandler(void)                                    /* TIM4�ж� */
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  /* ���TIM4�����жϷ������ */
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);   /* ���TIMx�����жϱ�־ */
        if (state_flag == CHARGING)//���
        {
						collect_info();
            charge_energy += cur_cap * vol_cap * 0.001f;
            if (vol_cap > 2.6f) {
                state_flag = INIT;
            }
						++charging_time;
        }
				else if (state_flag == DISCHARGING)//�ŵ�
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
