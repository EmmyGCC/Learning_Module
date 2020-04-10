#ifndef __MYEXTI_H
#define __MYEXTI_H
#include "stm32f10x.h"
#define KEY1   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)
#define KEY2   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)
#define KEY3   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)
#define KEY4   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)

void KEY_GPIO_Config(void);
static void KEY_NVIC_Config(void);
void EXTI_Key_Config(void);
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);

#endif

