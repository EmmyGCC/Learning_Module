#ifndef STM32F0_EXINT_H
#define STM32F0_EXINT_H


#include "stm32f0xx.h"
#include "stm32f0_peripheral.h"


#define SYSCFG_CLOCK_EN()         (RCC->APB2ENR |= 1<<0)					//SYSCFG clock enable



/********************** 无线NIRQ的外部接口宏定义 ***************************/

#define NIRQ_GPIO_CLOCK_EN        GPIOB_CLOCK_EN       

#define NIRQ_GPIOX                GPIOB       
#define NIRQ_PIN                  6
#define NIRQ_PIN_MODE             INPUT_MODE
#define NIRQ_PIN_TYPE             PUSH_PULL
#define NIRQ_PIN_SPEED            LOW_SPEED
#define NIRQ_PIN_PULL             PULL_UP

#define NIRQ_EXTIMAP              1           //外部中断映射，PA口为0，PB口为1，PC口为2 ...
#define NIRQ_TRIGDIS              0X00
#define NIRQ_TRIGEN               (1<<NIRQ_PIN)

#define NIRQ_IMR_CHANNEL          EXTI4_15_IRQn    //IMR Interrupt Numbers
#define NIRQ_IMR_PRIORITY         0x02            //Interrupt priority

#define NIRQ_INTEN()              (EXTI->IMR |= 1 << NIRQ_PIN)
#define NIRQ_INTDIS()             (EXTI->IMR &= ~(1 << NIRQ_PIN))

#define NIRQ_RDPR()               (EXTI->PR & (1 << NIRQ_PIN))
#define NIRQ_CLSPR()              (EXTI->PR |= 1 << NIRQ_PIN)
#define NIRQ_RDPIN()              (NIRQ_GPIOX->IDR & (1<<NIRQ_PIN))  //读引脚的电平状态




/********************** Touch1的外部接口宏定义 ***************************/
#define Touch1_INT_GPIO_CLOCK_EN        GPIOA_CLOCK_EN       

#define Touch1_INT_GPIOX                GPIOA       
#define Touch1_INT_PIN                  7
#define Touch1_INT_PIN_MODE             INPUT_MODE
//#define Touch1_INT_PIN_TYPE             PUSH_PULL
//#define Touch1_INT_PIN_SPEED            LOW_SPEED
#define Touch1_INT_PIN_PULL             PULL_UP

#define Touch1_INT_EXTIMAP              0           //外部中断映射，PA口为0，PB口为1，PC口为2 ...
#define Touch1_INT_TRIGDIS              0X00
#define Touch1_INT_TRIGEN               (1<<Touch1_INT_PIN)

#define Touch1_INT_IMR_CHANNEL          EXTI4_15_IRQn    //Interrupt Numbers
#define Touch1_INT_IMR_PRIORITY         0x02        //Interrupt priority


#define Touch1_INT_INTEN()              (EXTI->IMR |= 1 << Touch1_INT_PIN)
#define Touch1_INT_INTDIS()             (EXTI->IMR &= ~(1 << Touch1_INT_PIN))

#define Touch1_INT_RDPR()               (EXTI->PR & (1 << Touch1_INT_PIN))
#define Touch1_INT_CLSPR()              (EXTI->PR |= 1 << Touch1_INT_PIN)
#define Touch1_INT_RDPIN()              (Touch1_INT_GPIOX->IDR & (1<<Touch1_INT_PIN))







/********************** Touch2的外部接口宏定义 ***************************/
#define Touch2_INT_GPIO_CLOCK_EN        GPIOA_CLOCK_EN       

#define Touch2_INT_GPIOX                GPIOA       
#define Touch2_INT_PIN                  5
#define Touch2_INT_PIN_MODE             INPUT_MODE
//#define Touch2_INT_PIN_TYPE             PUSH_PULL
//#define Touch2_INT_PIN_SPEED            LOW_SPEED
#define Touch2_INT_PIN_PULL             PULL_UP

#define Touch2_INT_EXTIMAP              0           //外部中断映射，PA口为0，PB口为1，PC口为2 ...
#define Touch2_INT_TRIGDIS              0X00
#define Touch2_INT_TRIGEN               (1<<Touch2_INT_PIN)

#define Touch2_INT_IMR_CHANNEL          EXTI4_15_IRQn    //Interrupt Numbers
#define Touch2_INT_IMR_PRIORITY         0x02        //Interrupt priority


#define Touch2_INT_INTEN()              (EXTI->IMR |= 1 << Touch2_INT_PIN)
#define Touch2_INT_INTDIS()             (EXTI->IMR &= ~(1 << Touch2_INT_PIN))

#define Touch2_INT_RDPR()               (EXTI->PR & (1 << Touch2_INT_PIN))
#define Touch2_INT_CLSPR()              (EXTI->PR |= 1 << Touch2_INT_PIN)
#define Touch2_INT_RDPIN()              (Touch2_INT_GPIOX->IDR & (1<<Touch2_INT_PIN))


















/************************* 外部函数 ***************************/


void NIRQ_Int_Init(void);

void Touch_INT_Init(void);


#endif


