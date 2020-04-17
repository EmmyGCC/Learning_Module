#ifndef STM32F0_USART_H_
#define STM32F0_USART_H_

#include "stm32f0xx.h"


/************************* 串口功能属性宏定义 ***************************/
#define WORD_LENGTH_8               0x00 
#define WORD_LENGTH_9               (1<<12)
#define WORD_LENGTH_7               (1<<28)

#define STOP_1BIT                   0X00
#define STOP_2BIT                   (2<<12)
#define STOP_1BIT5                  (3<<12)

#define PARITY_DIS                  (0X00)
#define PARITY_EN                   (1<<10)
#define PARITY_EVEN                 (0X00)
#define PARITY_ODD                  (1<<9)

#define BAUD_2400                   0X4E20
#define BAUD_9600                   0X1388
#define BAUD_19200                  0X09C4
#define BAUD_38400                  0X04E2
#define BAUD_57600                  0X0341
#define BAUD_115200                 0X01A1


/********************** USART1Com接口宏定义 ***************************/
#define USART1Com                       USART1
#define USART1Com_GPIO_CLOCK_EN        GPIOA_CLOCK_EN       
#define USART1Com_CLKREG               APB2ENR
#define USART1Com_CLOCK_EN             (1<<14)

#define USART1Com_TX_GPIOX             GPIOA       
#define USART1Com_TX_PIN               9

#define USART1Com_TX_PIN_MODE          AF_MODE
#define USART1Com_TX_AFSEL             1
#define USART1Com_TX_PIN_TYPE          PUSH_PULL
#define USART1Com_TX_PIN_SPEED         MEDIUM_SPEED
#define USART1Com_TX_PIN_PULL          PULL_UP

#define USART1Com_RX_GPIOX             GPIOA 
#define USART1Com_RX_PIN               10
#define USART1Com_RX_AFSEL             1
#define USART1Com_RX_PIN_MODE          AF_MODE
#define USART1Com_RX_PIN_TYPE          PUSH_PULL
#define USART1Com_RX_PIN_SPEED         MEDIUM_SPEED
#define USART1Com_RX_PIN_PULL          PULL_UP 

#define USART1Com_IRQ                  USART1_IRQn    //Interrupt Numbers
#define USART1Com_PRIORITY             0X03

#define USART1Com_TXINT_EN()              (USART1Com->CR1 |= 1<<7)
#define USART1Com_TXINT_DIS()             (USART1Com->CR1 &= ~(1<<7))
#define USART1Com_TCIE_EN()               (USART1Com->CR1 |= 1<<6)
#define USART1Com_TCIE_DIS()              (USART1Com->CR1 &= ~(1<<6))
#define USART1Com_RXINT_EN()              (USART1Com->CR1 |= 1<<5)
#define USART1Com_RXINT_DIS()             (USART1Com->CR1 &= ~(1<<5))

#define USART1Com_TXEN()                  (USART1Com->CR1 |= 1<<3)
#define USART1Com_TXDIS()                 (USART1Com->CR1 &= ~(1<<3))
#define USART1Com_RXEN()                  (USART1Com->CR1 |= 1<<2)
#define USART1Com_RXDIS()                 (USART1Com->CR1 &= ~(1<<2))

#define USART1Com_EN()                    (USART1Com->CR1 |= 1<<0)
#define USART1Com_DIS()                   (USART1Com->CR1 &= ~(1<<0))

#define USART1Com_RDTXE()                 (USART1Com->ISR & 1<<7)
#define USART1Com_RDTXINTEN()             (USART1Com->CR1 & 1<<7)    //tx中断是否使能
#define USART1Com_RDTC()                  (USART1Com->ISR & 1<<6)
#define USART1Com_RDRXNE()                (USART1Com->ISR & 1<<5)



/********************** USART2Com接口宏定义 ***************************/

#define USART2Com                      USART2
#define USART2Com_GPIO_CLOCK_EN        GPIOA_CLOCK_EN       
#define USART2Com_CLKREG               APB1ENR
#define USART2Com_CLOCK_EN             (1<<17)

#define USART2Com_TX_GPIOX             GPIOA       
#define USART2Com_TX_PIN               2

#define USART2Com_TX_PIN_MODE          AF_MODE
#define USART2Com_TX_AFSEL             1
#define USART2Com_TX_PIN_TYPE          OPEN_DRAIN
#define USART2Com_TX_PIN_SPEED         MEDIUM_SPEED
#define USART2Com_TX_PIN_PULL          PULL_UP

#define USART2Com_RX_GPIOX             GPIOA 
#define USART2Com_RX_PIN               3

#define USART2Com_RX_PIN_MODE          AF_MODE
#define USART2Com_RX_AFSEL             1
#define USART2Com_RX_PIN_TYPE          OPEN_DRAIN
#define USART2Com_RX_PIN_SPEED         MEDIUM_SPEED
#define USART2Com_RX_PIN_PULL          PULL_UP


#define USART2Com_IRQ                  USART2_IRQn    //Interrupt Numbers
#define USART2Com_PRIORITY             0X02

#define USART2Com_TXINT_EN()              (USART2Com->CR1 |= 1<<7)
#define USART2Com_TXINT_DIS()             (USART2Com->CR1 &= ~(1<<7))
#define USART2Com_TCIE_EN()               (USART2Com->CR1 |= 1<<6)
#define USART2Com_TCIE_DIS()              (USART2Com->CR1 &= ~(1<<6))
#define USART2Com_RXINT_EN()              (USART2Com->CR1 |= 1<<5)
#define USART2Com_RXINT_DIS()             (USART2Com->CR1 &= ~(1<<5))

#define USART2Com_TXEN()                  (USART2Com->CR1 |= 1<<3)
#define USART2Com_TXDIS()                 (USART2Com->CR1 &= ~(1<<3))
#define USART2Com_RXEN()                  (USART2Com->CR1 |= 1<<2)
#define USART2Com_RXDIS()                 (USART2Com->CR1 &= ~(1<<2))

#define USART2Com_EN()                    (USART2Com->CR1 |= 1<<0)
#define USART2Com_DIS()                   (USART2Com->CR1 &= ~(1<<0))

#define USART2Com_RDTXE()                 (USART2Com->ISR & 1<<7)
#define USART2Com_RDTXINTEN()             (USART2Com->CR1 & 1<<7)    //tx中断是否使能
#define USART2Com_RDTC()                  (USART2Com->ISR & 1<<6)
#define USART2Com_RDRXNE()                (USART2Com->ISR & 1<<5)
/////////////////////////////////////////////////////////////////////////////////




void USART1_Com_Init(void);
void USART2_Com_Init(void);

#endif
