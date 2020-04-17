#include "mcu.h"
#include "stm32f0xx.h"
#include "stm32f0_rcc.h"
#include "stm32f0_gpio.h"
#include "stm32f0_exint.h"
#include "m0_systick.h"
#include "stm32f0_spi.h"
#include "stm32f0_usart.h"


void SysAll_Init(void)
{
  Rcc_Init();
  
  __disable_irq();
  Led_Init();
  
  SysTick_Init();
#ifndef NO_RF_DEBUG
  SI4438_PWRInit();
  WIRELESS_SPI_Init();
  SDN_Init();
  WL_TXRX_Init();
  NIRQ_Int_Init();
#endif
  
    USART2_Com_Init();
    USART1_Com_Init();

  __enable_irq();
}


