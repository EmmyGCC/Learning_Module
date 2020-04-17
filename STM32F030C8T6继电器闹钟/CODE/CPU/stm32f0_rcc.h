#ifndef STM32F0_RCC_H
#define STM32F0_RCC_H

#include "stm32f0xx.h"


/************************* RCC属性宏定义 ***************************/
#define PLL_RDY               (1<<25)
#define PLL_ON                (1<<24)
#define HSE_RDY               (1<<17)
#define HSE_ON                (1<<16)
#define HSI_RDY               (1<<1)
#define HSI_ON                (1<<0)

#define PLL_SOURCE_HSI        0X00
#define PLL_SOURCE_HSE        (1<<16)

#define PLL_DIV(X)            (X-1)
#define PLL_MUL(X)            ((X-2)<<18)

#define APB_DIV1              (0X00)
#define APB_DIV2              (4<<8)
#define APB_DIV4              (5<<8)
#define APB_DIV8              (6<<8)
#define APB_DIV16             (7<<8)

#define AHB_DIV1              (0X00)
#define AHB_DIV2              (8<<4)
#define AHB_DIV4              (9<<4)
#define AHB_DIV8              (10<<4)
#define AHB_DIV16             (11<<4)
#define AHB_DIV64             (12<<4)
#define AHB_DIV128            (13<<4)
#define AHB_DIV256            (14<<4)
#define AHB_DIV512            (15<<4)

#define SYSCLK_HSI            0X00
#define SYSCLK_HSE            0X01
#define SYSCLK_PLL            0X02

#define SYSCLK_STAHSI         0X00
#define SYSCLK_STAHSE         (1<<2)
#define SYSCLK_STAPLL         (2<<2)




/************************* 外部函数 ***************************/
void Rcc_Init(void);
void PWR_EnterSTANDBYMode(void);
void PWR_EnterSleepMode(void);
void PWR_EnterSTOPMode(void);





#endif



