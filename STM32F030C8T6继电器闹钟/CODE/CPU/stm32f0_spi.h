#ifndef STM32F0_SPI_H
#define STM32F0_SPI_H


#include "stm32f0xx.h"
#include "stm32f0_peripheral.h"

/************************* 无线SPI引脚宏定义 ***************************/
#define WIRELESS_SPI                          SPI1
#define WIRELESS_SPI_CLKREG                   APB2ENR
#define WIRELESS_SPI_CLOCK_EN                 (1<<12)       


#define WIRELESS_NSEL_GPIO_CLOCK_EN           GPIOA_CLOCK_EN
#define WIRELESS_NSEL_GPIOX                   GPIOA       
#define WIRELESS_NSEL_PIN                     15

#define WIRELESS_NSEL_PIN_MODE                OUTPUT_MODE
//#define WIRELESS_NSEL_PIN_AFSEL               0
#define WIRELESS_NSEL_PIN_TYPE                PUSH_PULL
#define WIRELESS_NSEL_PIN_SPEED               MEDIUM_SPEED
#define WIRELESS_NSEL_PIN_PULL                NO_PULL

#define WIRELESS_SCK_GPIO_CLOCK_EN           GPIOB_CLOCK_EN
#define WIRELESS_SCK_GPIOX                   GPIOB       
#define WIRELESS_SCK_PIN                     3

#define WIRELESS_SCK_PIN_MODE                AF_MODE
#define WIRELESS_SCK_PIN_AFSEL               0
#define WIRELESS_SCK_PIN_TYPE                PUSH_PULL
#define WIRELESS_SCK_PIN_SPEED               HIGH_SPEED
#define WIRELESS_SCK_PIN_PULL                PULL_UP

#define WIRELESS_MOSI_GPIO_CLOCK_EN           GPIOB_CLOCK_EN
#define WIRELESS_MOSI_GPIOX                   GPIOB       
#define WIRELESS_MOSI_PIN                     5

#define WIRELESS_MOSI_PIN_MODE                AF_MODE
#define WIRELESS_MOSI_PIN_AFSEL               0
#define WIRELESS_MOSI_PIN_TYPE                PUSH_PULL
#define WIRELESS_MOSI_PIN_SPEED               HIGH_SPEED
#define WIRELESS_MOSI_PIN_PULL                PULL_UP

#define WIRELESS_MISO_GPIO_CLOCK_EN           GPIOB_CLOCK_EN
#define WIRELESS_MISO_GPIOX                   GPIOB       
#define WIRELESS_MISO_PIN                     4

#define WIRELESS_MISO_PIN_MODE                AF_MODE
#define WIRELESS_MISO_PIN_AFSEL               0
#define WIRELESS_MISO_PIN_TYPE                PUSH_PULL
#define WIRELESS_MISO_PIN_SPEED               HIGH_SPEED
#define WIRELESS_MISO_PIN_PULL                PULL_UP


/************************* SPI功能属性宏定义 ***************************/
#define MSB_FIRST                         0X00
#define LSB_FIRST                         (1<<7)

#define BR_PCLK_DIV2                      (0X00)
#define BR_PCLK_DIV4                      (1<<3)
#define BR_PCLK_DIV8                      (2<<3)
#define BR_PCLK_DIV16                     (3<<3)
#define BR_PCLK_DIV32                     (4<<3)
#define BR_PCLK_DIV64                     (5<<3)
#define BR_PCLK_DIV128                    (6<<3)
#define BR_PCLK_DIV256                    (7<<3)

#define SLAVE_CONFIG                      (0X00)
#define MASTER_CONFIG                     (1<<2)

#define CPOL_IDLE0                        (0X00)
#define CPOL_IDLE1                        (1<<1)

#define CPHA_EDGE1                        (0X00)
#define CPHA_EDGE2                        (1<<0)

#define RX_SIZE16                         (0X00)
#define RX_SIZE8                          (1<<12)

#define TX_SIZE8                          (0X7<<8)
#define TX_SIZE16                         (0XF<<8)

#define WIRELESS_SPI_EN()                 (WIRELESS_SPI->CR1 |= 1<<6)
#define WIRELESS_SPI_DIS()                (WIRELESS_SPI->CR1 &= ~(1<<6))





/*****************************************************************/
#define WIRELESS_NSEL_H() 	      (GPIOA->BSRR = 1<<15)
#define WIRELESS_NSEL_L()	        (GPIOA->BRR = 1<<15)







/************************* 外部函数 ***************************/

void WIRELESS_SPI_Init(void);
uint8_t SPI_RWbyte(uint8_t sdata);
void SPI_ReadData(uint8_t byteCount,uint8_t* pData);
void SPI_WriteData(uint8_t byteCount,uint8_t* pData);











#endif




