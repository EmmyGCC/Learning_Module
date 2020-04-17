#include "stm32f0xx.h"
#include "delay.h"


void Timer6_Init(void)
{
		RCC->APB1ENR |= 1<<4;					//TIM6 timer clock enable
//		TIM6->PSC = 480-1;					//Prescaler Clock
//		TIM6->ARR = 100-1;					//Auto-reload value
		TIM6->CR1 |= 1<<7 ;     		//Register is Buffered
		TIM6->CR1 |= 1<<2;      		//Only counter overflow/underflow generates an update interrupt
//		TIM6->DIER &= ~(1<<0);			//Update interrupt Disenabled
}





void delay_us(uint16_t nus)
{		
		TIM6->PSC = 48-1;					//Prescaler Clock
		TIM6->ARR = nus;					//Auto-reload value
		TIM6->EGR |= 1<<0;				//Re-initializes the timer counter
		TIM6->CR1 |= 1<<0;				//Counter enabled
		while(!(TIM6->SR & 1<<0));	//Read interrupt flag 
		TIM6->CR1 &= ~(1<<0);				//Counter Disenabled
		TIM6->SR &= ~(1<<0);			//Cleared interrupt flag
}




void delay_ms(uint16_t nms)
{	 		  	  
		TIM6->PSC = 48000-1;					//Prescaler Clock
		TIM6->ARR = nms;					//Auto-reload value
		TIM6->EGR |= 1<<0;				//Re-initializes the timer counter
		TIM6->CR1 |= 1<<0;				//Counter enabled
		while(!(TIM6->SR & 1<<0));	//Read interrupt flag 
		TIM6->CR1 &= ~(1<<0);				//Counter Disenabled
		TIM6->SR &= ~(1<<0);			//Cleared interrupt flag
} 
