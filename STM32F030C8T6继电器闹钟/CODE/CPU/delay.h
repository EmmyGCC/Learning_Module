#ifndef _DELAY_H_
#define _DELAY_H_

#include "stm32f0xx.h"


void Timer6_Init(void);
void delay_us(uint16_t nus);
void delay_ms(uint16_t nms);


#endif


