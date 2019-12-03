/**
******************************************************************************
* @�ļ�         tim2.C
* @����         LGG
* @�汾         V1.0.0
* @����         2017-11-15
* @ժҪ         tim2Դ�ļ�
******************************************************************************
*/ 

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "tim2.h"


u16 msHcCount = 0;//ms����

void OpenTimerForHc(void)        //�򿪶�ʱ��
{
	TIM2_SetCounter(0);	//�������
	msHcCount = 0;
	//ʹ��TIMx����
	TIM2_Cmd(ENABLE);
}


void CloseTimerForHc(void)        //�رն�ʱ��
{
	TIM2_Cmd(DISABLE);		//ʧ��TIMx����
}


/**
* @������       TIM2_Count_Config
* @��  ��       ����TIM2��ʱ1ms�ж�һ��
* @��  ��       ��
* @����ֵ       ��
*/
void TIM2_Count_Config(void)
{
	/* 
	TIM2 ����:
	- TIM2��ʱ����16 MHz, 128��Ƶ����125kHz
	16 MHz / 128 = 125 000 Hz
	- Ԥװ��ֵ��Ϊ124��125-1��ʱ�������������ʱ��Ϊ1ms
	125000 / 125 = 1000Hz = 1ms
	*/
	
	/* 128��Ƶ��Ԥװ��ֵΪ��125-1�� */
	TIM2_TimeBaseInit(TIM2_PRESCALER_128, 124);
	
	/* ���TIM2�ĸ��±�־λ */
	TIM2_ClearFlag(TIM2_FLAG_UPDATE);
	
	/* ʹ�ܸ����ж� */
	TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
	
	/* ʹ��TIM2����ʼ���� */
	TIM2_Cmd(ENABLE);
}



/**
* @������       TIM2_PWM_Config
* @��  ��       ����TIM2���PWM Ƶ��16k
*               OC2ռ�ձ�CCR2_Val/1000     OC3ռ�ձ�CCR3_Val/1000
* @��  ��       CCR2_Val��OC2��ռ�ձ�����
*               CCR3_Val��OC3��ռ�ձ�����
* @����ֵ       ��
*/
void TIM2_PWM_Config(uint16_t CCR2_Val, uint16_t CCR3_Val)
{
	/* TIM2 1��Ƶ��Ԥװ��ֵ1000 */
	TIM2_TimeBaseInit(TIM2_PRESCALER_1, (1000 - 1));
	
	/* ����TIM2_OC2ΪPWM1ģʽ ,װ��ֵCCR2_Val��������ԣ���*/ 
	TIM2_OC2Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE,CCR2_Val, TIM2_OCPOLARITY_HIGH);
	TIM2_OC2PreloadConfig(ENABLE);        //TIM2_OC2Ԥװ��ʹ��
	
	/* ����TIM2_OC3ΪPWM1ģʽ ,װ��ֵCCR3_Val��������ԣ���*/       
	TIM2_OC3Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE,CCR3_Val, TIM2_OCPOLARITY_HIGH);
	TIM2_OC3PreloadConfig(ENABLE);        //TIM2_OC3Ԥװ��ʹ��
	
	TIM2_ARRPreloadConfig(ENABLE);        //TIM2Ԥװ��ֵʹ��
	
	/* ʹ��TIM2 */
	TIM2_Cmd(ENABLE);
}


/**
* @������       TIM2_OC_Active_ModeConfig
* @��  ��       ����TIM2��OC2 OC3 �����ͬ���ӳ��ź�
*               OC2�ӳ�500msȻ��һֱ�����     OC3�ӳ�250msȻ��һֱ�����
* @��  ��       CCR2_Val��OC2װ��ֵ����
*               CCR3_Val��OC3װ��ֵ����
* @����ֵ       ��
*/
void TIM2_OC_Active_ModeConfig(uint16_t CCR2_Val, uint16_t CCR3_Val)
{
	/* TIM2��ʼ������   32768��Ƶ��װ��ֵ65535*/      
	TIM2_TimeBaseInit(TIM2_PRESCALER_32768, 65535);
	
	/* 32768��Ƶ������װ�� */
	TIM2_PrescalerConfig(TIM2_PRESCALER_32768, TIM2_PSCRELOADMODE_IMMEDIATE);
	
	/* OC1������ */
	//  TIM2_OC1Init(TIM2_OCMODE_ACTIVE, TIM2_OUTPUTSTATE_ENABLE,CCR1_Val, TIM2_OCPOLARITY_HIGH);
	//  
	//  TIM2_OC1PreloadConfig(DISABLE);
	
	
	/* OC2������ */
	TIM2_OC2Init(TIM2_OCMODE_ACTIVE, TIM2_OUTPUTSTATE_ENABLE,CCR2_Val, TIM2_OCPOLARITY_HIGH); 
	TIM2_OC2PreloadConfig(DISABLE);
	
	/* OC3������ */
	TIM2_OC3Init(TIM2_OCMODE_ACTIVE, TIM2_OUTPUTSTATE_ENABLE,CCR3_Val, TIM2_OCPOLARITY_HIGH);
	TIM2_OC3PreloadConfig(DISABLE);
	TIM2_ARRPreloadConfig(ENABLE);        //Ԥװ��ʹ��
	
	/* TIM2 ʹ��*/
	TIM2_Cmd(ENABLE);
}


/**
* @������       TIM2_OC_Inactive_ModeConfig
* @��  ��       ����TIM2��OC2 OC3 �ڼ�����1000ms 500msȻ��ֱ�����ж�
*               OC2�ӳ�1000msȻ������ж�     OC3�ӳ�500msȻ������ж�
* @��  ��       CCR2_Val��OC2װ��ֵ����
*               CCR3_Val��OC3װ��ֵ����
* @����ֵ       ��
*/
void TIM2_OC_Inactive_ModeConfig(uint16_t CCR2_Val, uint16_t CCR3_Val)
{
	/* TIM2��ʼ������   32768��Ƶ��װ��ֵ65535*/  
	TIM2_TimeBaseInit(TIM2_PRESCALER_32768, 65535);
	
	/* 32768��Ƶ������װ�� */
	TIM2_PrescalerConfig(TIM2_PRESCALER_32768, TIM2_PSCRELOADMODE_IMMEDIATE);
	
	/* OC1������ */
	//TIM2_OC1Init(TIM2_OCMODE_INACTIVE, TIM2_OUTPUTSTATE_ENABLE,CCR1_Val, TIM2_OCPOLARITY_HIGH); 
	//TIM2_OC1PreloadConfig(DISABLE);
	
	/* OC2������ */
	TIM2_OC2Init(TIM2_OCMODE_INACTIVE, TIM2_OUTPUTSTATE_ENABLE,CCR2_Val, TIM2_OCPOLARITY_HIGH); 
	TIM2_OC2PreloadConfig(DISABLE);
	
	/* OC3������ */
	TIM2_OC3Init(TIM2_OCMODE_INACTIVE, TIM2_OUTPUTSTATE_ENABLE,CCR3_Val, TIM2_OCPOLARITY_HIGH); 
	TIM2_OC3PreloadConfig(DISABLE);
	
	TIM2_ARRPreloadConfig(ENABLE);//Ԥװ��ʹ��
	
	/* TIM2�ж�ʹ�� */
	TIM2_ITConfig(TIM2_IT_CC2, ENABLE);
	TIM2_ITConfig(TIM2_IT_CC3, ENABLE);
	
	/* TIM2 ʹ��*/
	TIM2_Cmd(ENABLE);
}

/************************ Copyright (C)2017 LGG. All Rights Reserved *****END OF FILE****/


