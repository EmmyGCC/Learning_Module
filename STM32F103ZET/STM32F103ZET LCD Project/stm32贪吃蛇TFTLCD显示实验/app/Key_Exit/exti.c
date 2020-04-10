#include "stm32f10x.h"
#include "exti.h"
#include "lcd.h"
#include "delay.h"
extern u8 dir;
void KEY_GPIO_Config()
{
	GPIO_InitTypeDef  GPIO_Initstructure;
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE );
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_Initstructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_Initstructure);
}

static void KEY_NVIC_Config()
{
	NVIC_InitTypeDef    NVIC_InitStructure;
	NVIC_PriorityGroupConfig (NVIC_PriorityGroup_1);
	
	//�����ж�Դ ���� 1
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE ;
	NVIC_Init(&NVIC_InitStructure);
	
	//�����ж�Դ ���� 2
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE ;
	NVIC_Init(&NVIC_InitStructure);
	
	//�����ж�Դ ���� 3
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE ;
	NVIC_Init(&NVIC_InitStructure);
	
	//�����ж�Դ ���� 4
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE ;
	NVIC_Init(&NVIC_InitStructure);
}

//EXTI �ж�����
void EXTI_Key_Config()
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	//���ð���key1
	 KEY_GPIO_Config();
	//��AFIO ����ʱ��
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_AFIO, ENABLE );
	//����NVIC�ж�
	KEY_NVIC_Config();
	
	
	//ѡ��EXTI���ź�Դ
	GPIO_EXTILineConfig (GPIO_PortSourceGPIOA, GPIO_PinSource0|GPIO_PinSource1|GPIO_PinSource2|GPIO_PinSource3);
	
	//�ж���0
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	//�ж�ģʽ
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	//�������� ������
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	//ʹ���ж�
	EXTI_InitStructure.EXTI_LineCmd = ENABLE ;
	EXTI_Init(&EXTI_InitStructure);
	
	//�ж���1
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_Init(&EXTI_InitStructure);
	
	//�ж���2
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	EXTI_Init(&EXTI_InitStructure);
	
	//�ж���3
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_Init(&EXTI_InitStructure);
}




void EXTI0_IRQHandler(void)
{
	//ȷ���Ƿ����EXTI LINE �ж�
     if(EXTI_GetITStatus(EXTI_Line0) != RESET )
	{
		dir = 0;
		//LCD_ShowNum(180,290,dir,3,12); 
		LCD_ShowString(190,290,200,12,12,"straight"); //��ʾ��ǰ�ߵ�ǰ������
		EXTI_ClearITPendingBit(EXTI_Line0);
		 
	}
}

void EXTI1_IRQHandler(void)
{
	//ȷ���Ƿ����EXTI LINE �ж�
   if(EXTI_GetITStatus(EXTI_Line1) != RESET )
	{
			dir = 1;
			LCD_ShowString(190,290,200,12,12,"left    "); //��ʾ��ǰ�ߵ�ǰ������
			EXTI_ClearITPendingBit(EXTI_Line1);
	}
}
void EXTI2_IRQHandler(void)
{
	//ȷ���Ƿ����EXTI LINE �ж�
   if(EXTI_GetITStatus(EXTI_Line2) != RESET )
	{
			dir = 2;
			LCD_ShowString(190,290,200,12,12,"right      ");  //��ʾ��ǰ�ߵ�ǰ������
			EXTI_ClearITPendingBit(EXTI_Line2);
	}
}

void EXTI3_IRQHandler(void)                                                                                                               
{
	//ȷ���Ƿ����EXTI LINE �ж�
	if(EXTI_GetITStatus(EXTI_Line3) != RESET )
	{
			dir = 3;
			LCD_ShowString(190,290,200,12,12,"    down");  //��ʾ��ǰ�ߵ�ǰ������
			EXTI_ClearITPendingBit(EXTI_Line3);
	}
}

