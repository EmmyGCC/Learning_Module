/**
  ******************************************************************************
  * @文件         tim2.h
  * @作者         LGG
  * @版本         V1.0.0
  * @日期         2017-11-15
  * @摘要         tim2头文件
  ******************************************************************************
*/ 

/* 定义防止递归包含 ----------------------------------------------------------*/
#ifndef __TIM2__H
#define __TIM2__H

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm8s.h"


extern u16 msHcCount;

/* 函数声明-------------------------------------------------------------------*/
void TIM2_Count_Config(void);
void TIM2_PWM_Config(uint16_t CCR2_Val, uint16_t CCR3_Val);
void TIM2_OC_Active_ModeConfig(uint16_t CCR2_Val, uint16_t CCR3_Val);
void TIM2_OC_Inactive_ModeConfig(uint16_t CCR2_Val, uint16_t CCR3_Val);

void OpenTimerForHc(void);
void CloseTimerForHc(void);

#endif

/************************ Copyright (C)2017 LGG. All Rights Reserved *****END OF FILE****/

