#include "delay.h"
#include "stm32f10x.h"
#include "lcd.h"
#include "stdlib.h"
#include "time.h"
#include "snake.h"
#include "exti.h"
u8 dir = 3;

//RTC��ʼ���������ڻ�ȡ��ǰʱ������������
u8 RTC_Init(void) 
{
	//����ǲ��ǵ�һ������ʱ��
	u8 temp=0;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��   
	PWR_BackupAccessCmd(ENABLE);	//ʹ�ܺ󱸼Ĵ�������  
	if (BKP_ReadBackupRegister(BKP_DR1) != 0x5050)		//��ָ���ĺ󱸼Ĵ����ж�������:��������д���ָ�����ݲ����
		{	 			
		BKP_DeInit();	//��λ�������� 	
		RCC_LSEConfig(RCC_LSE_ON);	//�����ⲿ���پ���(LSE),ʹ��������پ���
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET&&temp<250)	//���ָ����RCC��־λ�������,�ȴ����پ������
			{
			temp++;
			delay_ms(10);
			}
		if(temp>=250)return 1;//��ʼ��ʱ��ʧ��,����������	    
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��    
		RCC_RTCCLKCmd(ENABLE);	//ʹ��RTCʱ��  
		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_WaitForSynchro();		//�ȴ�RTC�Ĵ���ͬ��  
		RTC_ITConfig(RTC_IT_SEC, ENABLE);		//ʹ��RTC���ж�
		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_EnterConfigMode();/// ��������	
		RTC_SetPrescaler(32767); //����RTCԤ��Ƶ��ֵ
		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		//RTC_Set(2015,1,14,17,42,55);  //����ʱ��	
		RTC_ExitConfigMode(); //�˳�����ģʽ  
		BKP_WriteBackupRegister(BKP_DR1, 0X5050);	//��ָ���ĺ󱸼Ĵ�����д���û���������
		}
		
	   else//ϵͳ������ʱ
		{
		RTC_WaitForSynchro();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_ITConfig(RTC_IT_SEC, ENABLE);	//ʹ��RTC���ж�
		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		}
	return 0; //ok
}
 

int main(void)
 {  
	POINT_COLOR=BLUE;  //������ɫ 
	RTC_Init();
	Snake_Init();
    EXTI_Key_Config();
	LCD_Init();
	LCD_Fill(0,0,8,280,YELLOW);//Rectangle(8,8-1,85,120,0xaa);//����һ�����ο�, 0-127
	LCD_Fill(232,0,240,280,YELLOW);//
	LCD_Fill(0,0,240,8,YELLOW);//
	LCD_Fill(0,272,240,280,YELLOW);//
		
	LCD_ShowString(0,290,200,24,24,"Score:");//Show Score
	LCD_ShowString(75,300,200,12,12,"0  length:3");//Show Score
  	while(1) 	
	{  
	    Snake_Run(dir);
		delay_ms(200);
	} 
}
 
