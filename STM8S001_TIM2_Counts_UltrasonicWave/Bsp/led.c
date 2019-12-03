/**
  ******************************************************************************
  * @�ļ�         led.c
  * @����         LGG
  * @�汾         V1.0.0
  * @����         2017-11-15
  * @ժҪ         ledԴ�ļ�
  ******************************************************************************
*/ 

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "led.h"


/**
  * @������       LED_Init
  * @��  ��       ����LDE1 LED2 ��IO��
  * @��  ��       ��
  * @����ֵ       ��
  */
void LED_Init(void)
{
  /* ����LED1 IO��Ϊ���ģʽ */
  GPIO_Init(LED1_GPIO_PORT, (GPIO_Pin_TypeDef)LED1_GPIO_PINS, GPIO_MODE_OUT_PP_LOW_FAST);
  
  /* ����LED2 IO��Ϊ���ģʽ */
  //GPIO_Init(LED2_GPIO_PORT, (GPIO_Pin_TypeDef)LED2_GPIO_PINS, GPIO_MODE_OUT_PP_LOW_FAST);
}


/************************ Copyright (C)2017 LGG. All Rights Reserved *****END OF FILE****/

