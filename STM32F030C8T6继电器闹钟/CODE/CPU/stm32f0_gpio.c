#include "stm32f0xx.h"
#include "stm32f0_gpio.h"
#include "stm32f0_peripheral.h"


volatile uint8_t LedIndicate_Cnt;



inline uint8_t Get_LedIndicate_Cnt(void)
{
    return LedIndicate_Cnt;
}


inline void Set_LedIndicate_Cnt(uint8_t cnt_val)
{
    LedIndicate_Cnt = cnt_val;
}



void Led_Init(void)
{
    RCC->GPIO_CLKREG |= LEDG_GPIO_CLOCK_EN;
    
    LEDG_GPIOX->MODER &= ~(3 << (LEDG_PIN*2));
    LEDG_GPIOX->MODER |= LEDG_PIN_MODE << (LEDG_PIN*2);
    LEDG_GPIOX->OTYPER &= ~(1 << LEDG_PIN);
    LEDG_GPIOX->OTYPER |= LEDG_PIN_TYPE << LEDG_PIN;
    LEDG_GPIOX->OSPEEDR &= ~(3 << (LEDG_PIN*2));
    LEDG_GPIOX->OSPEEDR |= LEDG_PIN_SPEED << (LEDG_PIN*2);
    LEDG_GPIOX->PUPDR &= ~(3 << (LEDG_PIN*2));
    LEDG_GPIOX->PUPDR |= LEDG_PIN_PULL << (LEDG_PIN*2);
    LEDG_OFF();
}


void SI4438_PWRInit(void)
{
    RCC->GPIO_CLKREG |= PWR_GPIO_CLOCK_EN;
    PWR_GPIOX->MODER &= ~(3 << (PWR_PIN*2));
    PWR_GPIOX->MODER |= PWR_PIN_MODE << (PWR_PIN*2);
    PWR_GPIOX->OTYPER &= ~(1 << PWR_PIN);
    PWR_GPIOX->OTYPER |= PWR_PIN_TYPE << PWR_PIN;
    PWR_GPIOX->OSPEEDR &= ~(3 << (PWR_PIN*2));
    PWR_GPIOX->OSPEEDR |= PWR_PIN_SPEED << (PWR_PIN*2);
    PWR_GPIOX->PUPDR &= ~(3 << (PWR_PIN*2));
    PWR_GPIOX->PUPDR |= PWR_PIN_PULL << (PWR_PIN*2);
    
    SI4438_PWR_ON();
}


void SDN_Init(void)
{
    RCC->GPIO_CLKREG |= SDN_GPIO_CLOCK_EN;
    SDN_GPIOX->MODER &= ~(3 << (SDN_PIN*2));
    SDN_GPIOX->MODER |= SDN_PIN_MODE << (SDN_PIN*2);
    SDN_GPIOX->OTYPER &= ~(1 << SDN_PIN);
    SDN_GPIOX->OTYPER |= SDN_PIN_TYPE << SDN_PIN;
    SDN_GPIOX->OSPEEDR &= ~(3 << (SDN_PIN*2));
    SDN_GPIOX->OSPEEDR |= SDN_PIN_SPEED << (SDN_PIN*2);
    SDN_GPIOX->PUPDR &= ~(3 << (SDN_PIN*2));
    SDN_GPIOX->PUPDR |= SDN_PIN_PULL << (SDN_PIN*2);
} 


void WL_TXRX_Init(void)
{
    RCC->GPIO_CLKREG |= WL_TX_GPIO_CLOCK_EN;
    WL_TX_GPIOX->MODER &= ~(3 << (WL_TX_PIN*2));
    WL_TX_GPIOX->MODER |= WL_TX_PIN_MODE << (WL_TX_PIN*2);
    WL_TX_GPIOX->OTYPER &= ~(1 << WL_TX_PIN);
    WL_TX_GPIOX->OTYPER |= WL_TX_PIN_TYPE << WL_TX_PIN;
    WL_TX_GPIOX->OSPEEDR &= ~(3 << (WL_TX_PIN*2));
    WL_TX_GPIOX->OSPEEDR |= WL_TX_PIN_SPEED << (WL_TX_PIN*2);
    WL_TX_GPIOX->PUPDR &= ~(3 << (WL_TX_PIN*2));
    WL_TX_GPIOX->PUPDR |= WL_TX_PIN_PULL << (WL_TX_PIN*2);
    
    RCC->GPIO_CLKREG |= WL_RX_GPIO_CLOCK_EN;
    WL_RX_GPIOX->MODER &= ~(3 << (WL_RX_PIN*2));
    WL_RX_GPIOX->MODER |= WL_RX_PIN_MODE << (WL_RX_PIN*2);
    WL_RX_GPIOX->OTYPER &= ~(1 << WL_RX_PIN);
    WL_RX_GPIOX->OTYPER |= WL_RX_PIN_TYPE << WL_RX_PIN;
    WL_RX_GPIOX->OSPEEDR &= ~(3 << (WL_RX_PIN*2));
    WL_RX_GPIOX->OSPEEDR |= WL_RX_PIN_SPEED << (WL_RX_PIN*2);
    WL_RX_GPIOX->PUPDR &= ~(3 << (WL_RX_PIN*2));
    WL_RX_GPIOX->PUPDR |= WL_RX_PIN_PULL << (WL_RX_PIN*2);
    
}


void Switch_Init(void)
{
    RCC->GPIO_CLKREG |= Switch1_GPIO_CLOCK_EN;
    Switch1_GPIOX->MODER &= ~(3 << (Switch1_PIN*2));
    Switch1_GPIOX->MODER |= Switch1_PIN_MODE << (Switch1_PIN*2);
    Switch1_GPIOX->OTYPER &= ~(1 << Switch1_PIN);
    Switch1_GPIOX->OTYPER |= Switch1_PIN_TYPE << Switch1_PIN;
    Switch1_GPIOX->OSPEEDR &= ~(3 << (Switch1_PIN*2));
    Switch1_GPIOX->OSPEEDR |= Switch1_PIN_SPEED << (Switch1_PIN*2);
    Switch1_GPIOX->PUPDR &= ~(3 << (Switch1_PIN*2));
    Switch1_GPIOX->PUPDR |= Switch1_PIN_PULL << (Switch1_PIN*2);
    
    Switch1_ON();
}




void Touch_Init(void)
{
    RCC->GPIO_CLKREG |= Touch1_GPIO_CLOCK_EN;
    Touch1_GPIOX->MODER &= ~(3 << (Touch1_PIN*2));
    Touch1_GPIOX->MODER |= Touch1_PIN_MODE << (Touch1_PIN*2);
    Touch1_GPIOX->PUPDR &= ~(3 << (Touch1_PIN*2));
    Touch1_GPIOX->PUPDR |= Touch1_PIN_PULL << (Touch1_PIN*2);
    
}
