#include "stm32f0xx.h"
#include "stm32f0_rcc.h"


void Rcc_Init(void)
{
  /*
  __IO uint32_t cfgr_reg;
  cfgr_reg = RCC->CFGR;
  cfgr_reg |= PLL_SOURCE_HSE;
  cfgr_reg |= PLL_MUL(6);
  cfgr_reg |= SYSCLK_PLL;
  cfgr_reg |= AHB_DIV1;
  cfgr_reg |= APB_DIV1;
  RCC->CFGR = cfgr_reg;
  */
  
  //RCC->CFGR |= PLL_SOURCE_HSE;        
  RCC->CFGR |= PLL_SOURCE_HSI;  
  RCC->CFGR2 = PLL_DIV(1);            
  RCC->CFGR |= PLL_MUL(12);            
  RCC->CFGR |= SYSCLK_PLL;            
  RCC->CFGR |= AHB_DIV1;              
  RCC->CFGR |= APB_DIV1;              
  
  
 
  //RCC->CR |= HSE_ON;                    
  //while(!(RCC->CR & HSE_RDY));         
  RCC->CR |= HSI_ON;
  while(!(RCC->CR & HSI_RDY));
  RCC->CR |= PLL_ON;                   
  while(!(RCC->CR & PLL_RDY));         
   
}




void PWR_EnterSTANDBYMode(void)
{

  //PWR->CR |= PWR_CR_PDDS;
  PWR->CR |= 1<<1;

  //SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  SCB->SCR |= 1<<2;

  __WFI();
}


void PWR_EnterSleepMode(void)
{
 

  //SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);
  
  SCB->SCR &= ~((uint32_t)(1<<2));
    __WFI();

}



void PWR_EnterSTOPMode(void)
{

  //tmpreg |= PWR_Regulator;
  

  //PWR->CR = tmpreg;
  PWR->CR |= ~((uint32_t)(1<<3));
  PWR->CR |= 1<<0;
  

  //SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  
  SCB->SCR |= 1<<2;
    __WFI();

   // SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP_Msk); 
  SCB->SCR &= ~((uint32_t)(1<<2));
  
}

