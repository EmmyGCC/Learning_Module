/**
  ******************************************************************************
  * @�ļ�         tim2.h
  * @����         LGG
  * @�汾         V1.0.0
  * @����         2017-11-15
  * @ժҪ         tim2ͷ�ļ�
  ******************************************************************************
*/ 

/* �����ֹ�ݹ���� ----------------------------------------------------------*/
#ifndef __TIM2__H
#define __TIM2__H

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm8s.h"


extern u16 msHcCount;

/* ��������-------------------------------------------------------------------*/
void TIM2_Count_Config(void);
void TIM2_PWM_Config(uint16_t CCR2_Val, uint16_t CCR3_Val);
void TIM2_OC_Active_ModeConfig(uint16_t CCR2_Val, uint16_t CCR3_Val);
void TIM2_OC_Inactive_ModeConfig(uint16_t CCR2_Val, uint16_t CCR3_Val);

void OpenTimerForHc(void);
void CloseTimerForHc(void);

#endif

/************************ Copyright (C)2017 LGG. All Rights Reserved *****END OF FILE****/

