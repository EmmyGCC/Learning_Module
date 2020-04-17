#include "stm32f0xx.h"
#include "stm32f0_exint.h"
#include "stm32f0_peripheral.h"







void NIRQ_Int_Init(void)
{
  NVIC_InitTypeDef  NVIC_TEMP;
  
  RCC->GPIO_CLKREG |= NIRQ_GPIO_CLOCK_EN;
  NIRQ_GPIOX->MODER &= ~(3 << (NIRQ_PIN*2));
  NIRQ_GPIOX->MODER |= NIRQ_PIN_MODE << (NIRQ_PIN*2);
  //NIRQ_GPIOX->OTYPER &= ~(1 << NIRQ_PIN);
  //NIRQ_GPIOX->OTYPER |= NIRQ_PIN_TYPE << NIRQ_PIN;
  //NIRQ_GPIOX->OSPEEDR &= ~(3 << (NIRQ_PIN*2));
  //NIRQ_GPIOX->OSPEEDR |= NIRQ_PIN_SPEED << (NIRQ_PIN*2);
  NIRQ_GPIOX->PUPDR &= ~(3 << (NIRQ_PIN*2));
  NIRQ_GPIOX->PUPDR |= NIRQ_PIN_PULL << (NIRQ_PIN*2);
  
  SYSCFG_CLOCK_EN();
  SYSCFG->EXTICR[NIRQ_PIN >> 2] &= ~(0X0F <<((NIRQ_PIN & 0X03) << 2));			//external interrupt mapping
  SYSCFG->EXTICR[NIRQ_PIN >> 2] |= NIRQ_EXTIMAP << ((NIRQ_PIN & 0X03) << 2);			

  EXTI->FTSR |= NIRQ_TRIGEN;                //Falling trigger enabled
  EXTI->RTSR |= NIRQ_TRIGDIS;               //Rising trigger disabled
  
  NVIC_TEMP.NVIC_IRQChannel = NIRQ_IMR_CHANNEL;		//Specifies the IRQ channel
	NVIC_TEMP.NVIC_IRQChannelPriority = NIRQ_IMR_PRIORITY;		//Specifies the priority
	NVIC_TEMP.NVIC_IRQChannelCmd = ENABLE;			//NVIC_IRQChannel will be enabled
	NVIC_Init(&NVIC_TEMP);
  NIRQ_INTEN();
}





void Touch_INT_Init(void)
{
  NVIC_InitTypeDef  NVIC_TEMP;
  
  RCC->GPIO_CLKREG |= Touch1_INT_GPIO_CLOCK_EN;
  Touch1_INT_GPIOX->MODER &= ~(3 << (Touch1_INT_PIN*2));
  Touch1_INT_GPIOX->MODER |= Touch1_INT_PIN_MODE << (Touch1_INT_PIN*2);
  //Touch1_INT_GPIOX->OTYPER &= ~(1 << Touch1_INT_PIN);
  //Touch1_INT_GPIOX->OTYPER |= Touch1_INT_PIN_TYPE << Touch1_INT_PIN;
  //Touch1_INT_GPIOX->OSPEEDR &= ~(3 << (Touch1_INT_PIN*2));
  //Touch1_INT_GPIOX->OSPEEDR |= Touch1_INT_PIN_SPEED << (Touch1_INT_PIN*2);
  Touch1_INT_GPIOX->PUPDR &= ~(3 << (Touch1_INT_PIN*2));
  Touch1_INT_GPIOX->PUPDR |= Touch1_INT_PIN_PULL << (Touch1_INT_PIN*2);
  

  SYSCFG_CLOCK_EN();
  SYSCFG->EXTICR[Touch1_INT_PIN >> 2] &= ~(0X0F <<((Touch1_INT_PIN & 0X03) << 2));			//external interrupt mapping
  SYSCFG->EXTICR[Touch1_INT_PIN >> 2] |= Touch1_INT_EXTIMAP << ((Touch1_INT_PIN & 0X03) << 2);			

  EXTI->FTSR &= ~Touch1_INT_TRIGEN;                //Falling trigger disabled
  EXTI->RTSR |= Touch1_INT_TRIGEN;               //Rising trigger enabled
  
  NVIC_TEMP.NVIC_IRQChannel = Touch1_INT_IMR_CHANNEL;		//Specifies the IRQ channel
	NVIC_TEMP.NVIC_IRQChannelPriority = Touch1_INT_IMR_PRIORITY;		//Specifies the priority
	NVIC_TEMP.NVIC_IRQChannelCmd = ENABLE;			//NVIC_IRQChannel will be enabled
	NVIC_Init(&NVIC_TEMP);
  
  Touch1_INT_INTEN();
  
  
  RCC->GPIO_CLKREG |= Touch2_INT_GPIO_CLOCK_EN;
  Touch2_INT_GPIOX->MODER &= ~(3 << (Touch2_INT_PIN*2));
  Touch2_INT_GPIOX->MODER |= Touch2_INT_PIN_MODE << (Touch2_INT_PIN*2);
  //Touch2_INT_GPIOX->OTYPER &= ~(1 << Touch2_INT_PIN);
  //Touch2_INT_GPIOX->OTYPER |= Touch2_INT_PIN_TYPE << Touch2_INT_PIN;
  //Touch2_INT_GPIOX->OSPEEDR &= ~(3 << (Touch2_INT_PIN*2));
  //Touch2_INT_GPIOX->OSPEEDR |= Touch2_INT_PIN_SPEED << (Touch2_INT_PIN*2);
  Touch2_INT_GPIOX->PUPDR &= ~(3 << (Touch2_INT_PIN*2));
  Touch2_INT_GPIOX->PUPDR |= Touch2_INT_PIN_PULL << (Touch2_INT_PIN*2);
  

  SYSCFG_CLOCK_EN();
  SYSCFG->EXTICR[Touch2_INT_PIN >> 2] &= ~(0X0F <<((Touch2_INT_PIN & 0X03) << 2));			//external interrupt mapping
  SYSCFG->EXTICR[Touch2_INT_PIN >> 2] |= Touch2_INT_EXTIMAP << ((Touch2_INT_PIN & 0X03) << 2);			

  EXTI->FTSR &= ~Touch2_INT_TRIGEN;                //Falling trigger disabled
  EXTI->RTSR |= Touch2_INT_TRIGEN;               //Rising trigger enabled
  
  NVIC_TEMP.NVIC_IRQChannel = Touch2_INT_IMR_CHANNEL;		//Specifies the IRQ channel
	NVIC_TEMP.NVIC_IRQChannelPriority = Touch2_INT_IMR_PRIORITY;		//Specifies the priority
	NVIC_TEMP.NVIC_IRQChannelCmd = ENABLE;			//NVIC_IRQChannel will be enabled
	NVIC_Init(&NVIC_TEMP);
  
  Touch2_INT_INTEN();

}






