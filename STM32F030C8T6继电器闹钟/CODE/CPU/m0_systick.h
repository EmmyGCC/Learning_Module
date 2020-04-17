#ifndef M0_SYSTICK_H
#define M0_SYSTICK_H

#include "stm32f0xx.h"
#include <time.h>

#define SYSTICK_LOADVAL               48000       //1ms(此值根据系统时钟）

#define SYSTICK_SOURCE_EX             0X00
#define SYSTICK_SOURCE_SYS            (1<<2)


#define SYSTICK_INTEN()          (SysTick->CTRL |= 1<<1)
#define SYSTICK_INTDIS()         (SysTick->CTRL &= ~(1<<1))

#define SYSTICK_EN()             (SysTick->CTRL |= 1<<0)
#define SYSTICK_DIS()            (SysTick->CTRL &= ~(1<<0))

#define SYSTICK_RDFLAG()         (SysTick->CTRL & 1<<16)
#define SYSTICK_CLSFLAG()        (SysTick->CTRL)
#define SYSTICK_CLSCNT()         (SysTick->VAL = 0)


#define SYSTICK_CHANNEL          SysTick_IRQn    //Interrupt Numbers
#define SYSTICK_PRIORITY         0x01            //Interrupt priority




/************************* 外部变量 ***************************/
extern volatile uint32_t SYSTICK_CNT;    
extern volatile uint8_t TICK_8ms;
extern volatile time_t time_dat;

/************************* 外部函数 ***************************/
void Set_Not_RX_Cnt(void);
uint16_t Get_Not_RX_Cnt(void);
void Clr_Not_RX_Cnt(void);
uint32_t Get_SYSTICK_CNT(void);
void Set_SYSTICK_CNT(uint32_t cnt_val);

void SysTick_Init(void);
void SysDelay_Xms(uint32_t delay_val);
void SysTick_Handle(void);
void System_8msTick_Process(void);


#endif


