/**
  ******************************************************************************
  * @文件         led.c
  * @作者         LGG
  * @版本         V1.0.0
  * @日期         2017-11-15
  * @摘要         led源文件
  ******************************************************************************
*/ 

/* 包含头文件 ----------------------------------------------------------------*/
#include "led.h"


/**
  * @函数名       LED_Init
  * @功  能       配置LDE1 LED2 的IO口
  * @参  数       无
  * @返回值       无
  */
void LED_Init(void)
{
  /* 配置LED1 IO口为输出模式 */
  GPIO_Init(LED1_GPIO_PORT, (GPIO_Pin_TypeDef)LED1_GPIO_PINS, GPIO_MODE_OUT_PP_LOW_FAST);
  
  /* 配置LED2 IO口为输出模式 */
  //GPIO_Init(LED2_GPIO_PORT, (GPIO_Pin_TypeDef)LED2_GPIO_PINS, GPIO_MODE_OUT_PP_LOW_FAST);
}


/************************ Copyright (C)2017 LGG. All Rights Reserved *****END OF FILE****/

