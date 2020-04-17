#ifndef STM32F0_TIMES_H
#define STM32F0_TIMES_H

#include "stm32f0xx.h"

/************************* 定时器功能属性宏定义 ***************************/
//普通模式
#define ARPE_DIS                    (0x00)
#define ARPE_EN                     (1<<7)
#define OPM_DIS                     (0X00)
#define OPM_EN                      (1<<3)
#define URS_ALL                     (0X00)
#define URS_OVER                    (1<<2)
#define UDIS_EN                     (0X00)
#define UDIS_DIS                    (1<<1)

//比较输出模式
#define MODE_PWM1                   (6<<4)
#define MODE_PWM2                   (7<<4)
#define OC1P_DIS                    (0X00)
#define OC1P_EN                     (1<<3)
#define OC1F_DIS                    (0X00)
#define OC1F_EN                     (1<<2)
#define CC1_OUT                     (0X00)
#define CC1_IN                      (1<<0)

#define CC1P_H                      (0x00)
#define CC1P_L                      (1<<1)




/************************* LedBig输出引脚宏定义 ***************************/
#define LedBig_GPIO_CLOCK_EN        GPIOA_CLOCK_EN

#define LedBig_GPIOX                GPIOA       
#define LedBig_PIN                  7
#define LedBig_PIN_MODE             AF_MODE
#define LedBig_AFSEL                2
#define LedBig_PIN_TYPE             PUSH_PULL
#define LedBig_PIN_SPEED            MEDIUM_SPEED
#define LedBig_PIN_PULL             NO_PULL





/********************** LedBig_TIME宏定义 ***************************/
#define LedBig_TIME                     TIM1

#define LedBig_TIME_CLKREG              APB2ENR
#define LedBig_TIME_CLOCK_EN            (1<<11)
#define LedBig_TIME_UGEN()              (LedBig_TIME->EGR |= 1<<0)

//#define LedBig_TIME_IRQ                 TIM16_IRQn
//#define LedBig_TIME_PRIORITY            0x03

#define LedBig_TIME_EN()                (LedBig_TIME->CR1 |= 1<<0)
#define LedBig_TIME_DIS()               {LedBig_TIME->CR1 &= ~(1<<0); LedBig_TIME->CNT = 0;}

#define LedBig_TIME_UIE_EN()            (LedBig_TIME->DIER |= 1<<0)
#define LedBig_TIME_UIE_DIS()           (LedBig_TIME->DIER &= ~(1<<0))

#define LedBig_TIME_RDUIF()             (LedBig_TIME->SR & 1<<0)
#define LedBig_TIME_CLSUIF()            (LedBig_TIME->SR &= ~(1<<0))

/*
#define LedBig_TIME_COMPEN()            (LedBig_TIME->CCER |= 1<<2 | 1<<6)
#define LedBig_TIME_COMPDIS()           (LedBig_TIME->CCER &= ~(1<<2 | 1<<6))
*/
#define LedBig_TIME_COMPEN()            (LedBig_TIME->CCER |= 1<<2)
#define LedBig_TIME_COMPDIS()           (LedBig_TIME->CCER &= ~(1<<2)
#define LedBig_TIME_MOEN()              (LedBig_TIME->BDTR |= 1<<15)
#define LedBig_TIME_MODIS()             (LedBig_TIME->BDTR &= ~(1<<15))



#define LedBig_SetDuty(duty_val)      (LedBig_TIME->CCR1 = duty_val)




/************************* LedBlue输出引脚宏定义 ***************************/
#define LedBlue_GPIO_CLOCK_EN        GPIOA_CLOCK_EN

#define LedBlue_GPIOX                GPIOA       
#define LedBlue_PIN                  2
#define LedBlue_PIN_MODE             AF_MODE
#define LedBlue_AFSEL                0
#define LedBlue_PIN_TYPE             PUSH_PULL
#define LedBlue_PIN_SPEED            MEDIUM_SPEED
#define LedBlue_PIN_PULL             NO_PULL


/********************** LedBlue_TIME宏定义 ***************************/
#define LedBlue_TIME                     TIM15

#define LedBlue_TIME_CLKREG              APB2ENR
#define LedBlue_TIME_CLOCK_EN            (1<<16)
#define LedBlue_TIME_UGEN()              (LedBlue_TIME->EGR |= 1<<0)

//#define LedBlue_TIME_IRQ                 TIM15_IRQn
//#define LedBlue_TIME_PRIORITY            0x03

#define LedBlue_TIME_EN()                (LedBlue_TIME->CR1 |= 1<<0)
#define LedBlue_TIME_DIS()               {LedBlue_TIME->CR1 &= ~(1<<0); LedBlue_TIME->CNT = 0;}

#define LedBlue_TIME_UIE_EN()            (LedBlue_TIME->DIER |= 1<<0)
#define LedBlue_TIME_UIE_DIS()           (LedBlue_TIME->DIER &= ~(1<<0))

#define LedBlue_TIME_RDUIF()             (LedBlue_TIME->SR & 1<<0)
#define LedBlue_TIME_CLSUIF()            (LedBlue_TIME->SR &= ~(1<<0))

/*
#define LedBlue_TIME_COMPEN()            (LedBlue_TIME->CCER |= 1<<2 | 1<<6)
#define LedBlue_TIME_COMPDIS()           (LedBlue_TIME->CCER &= ~(1<<2 | 1<<6))
*/
#define LedBlue_TIME_COMPEN()            (LedBlue_TIME->CCER |= 1<<0)
#define LedBlue_TIME_COMPDIS()           (LedBlue_TIME->CCER &= ~(1<<0)
#define LedBlue_TIME_MOEN()              (LedBlue_TIME->BDTR |= 1<<15)
#define LedBlue_TIME_MODIS()             (LedBlue_TIME->BDTR &= ~(1<<15))



#define LedBlue_SetDuty(duty_val)      (LedBlue_TIME->CCR1 = duty_val)



/************************* LedRed输出引脚宏定义 ***************************/
#define LedRed_GPIO_CLOCK_EN        GPIOA_CLOCK_EN

#define LedRed_GPIOX                GPIOA       
#define LedRed_PIN                  4
#define LedRed_PIN_MODE             AF_MODE
#define LedRed_AFSEL                4
#define LedRed_PIN_TYPE             PUSH_PULL
#define LedRed_PIN_SPEED            MEDIUM_SPEED
#define LedRed_PIN_PULL             NO_PULL


/********************** LedRed_TIME宏定义 ***************************/
#define LedRed_TIME                     TIM14

#define LedRed_TIME_CLKREG              APB1ENR
#define LedRed_TIME_CLOCK_EN            (1<<8)
#define LedRed_TIME_UGEN()              (LedRed_TIME->EGR |= 1<<0)

//#define LedRed_TIME_IRQ                 TIM16_IRQn
//#define LedRed_TIME_PRIORITY            0x03

#define LedRed_TIME_EN()                (LedRed_TIME->CR1 |= 1<<0)
#define LedRed_TIME_DIS()               {LedRed_TIME->CR1 &= ~(1<<0); LedRed_TIME->CNT = 0;}

#define LedRed_TIME_UIE_EN()            (LedRed_TIME->DIER |= 1<<0)
#define LedRed_TIME_UIE_DIS()           (LedRed_TIME->DIER &= ~(1<<0))

#define LedRed_TIME_RDUIF()             (LedRed_TIME->SR & 1<<0)
#define LedRed_TIME_CLSUIF()            (LedRed_TIME->SR &= ~(1<<0))

/*
#define LedRed_TIME_COMPEN()            (LedRed_TIME->CCER |= 1<<2 | 1<<6)
#define LedRed_TIME_COMPDIS()           (LedRed_TIME->CCER &= ~(1<<2 | 1<<6))
*/
#define LedRed_TIME_COMPEN()            (LedRed_TIME->CCER |= 1<<0)
#define LedRed_TIME_COMPDIS()           (LedRed_TIME->CCER &= ~(1<<0)
#define LedRed_TIME_MOEN()              (LedRed_TIME->BDTR |= 1<<15)
#define LedRed_TIME_MODIS()             (LedRed_TIME->BDTR &= ~(1<<15))



#define LedRed_SetDuty(duty_val)      (LedRed_TIME->CCR1 = duty_val)




/************************* 外部函数 ***************************/

void LedBig_Init(void);
void LedRed_Init(void);
void LedBlue_Init(void);




#endif



