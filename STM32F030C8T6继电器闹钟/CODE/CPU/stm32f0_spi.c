#include "stm32f0xx.h"
#include "stm32f0_spi.h"



void WIRELESS_SPI_Init(void)
{
  RCC->WIRELESS_SPI_CLKREG |= WIRELESS_SPI_CLOCK_EN;
  
  RCC->GPIO_CLKREG |= WIRELESS_NSEL_GPIO_CLOCK_EN;
  RCC->GPIO_CLKREG |= WIRELESS_SCK_GPIO_CLOCK_EN;
  RCC->GPIO_CLKREG |= WIRELESS_MOSI_GPIO_CLOCK_EN;
  RCC->GPIO_CLKREG |= WIRELESS_MISO_GPIO_CLOCK_EN;
  
  //WIRELESS_NSEL_GPIOX->AFR[WIRELESS_NSEL_PIN>>3] &= ~(0xf << ((WIRELESS_NSEL_PIN & 0x07)*4));
  //WIRELESS_NSEL_GPIOX->AFR[WIRELESS_NSEL_PIN>>3] |= WIRELESS_NSEL_PIN_AFSEL << ((WIRELESS_NSEL_PIN&0x07)*4);
  WIRELESS_NSEL_GPIOX->MODER &= ~(3 << (WIRELESS_NSEL_PIN*2));
  WIRELESS_NSEL_GPIOX->MODER |= WIRELESS_NSEL_PIN_MODE << (WIRELESS_NSEL_PIN*2);
  WIRELESS_NSEL_GPIOX->OTYPER &= ~(1 << WIRELESS_NSEL_PIN);
  WIRELESS_NSEL_GPIOX->OTYPER |= WIRELESS_NSEL_PIN_TYPE << WIRELESS_NSEL_PIN;
  WIRELESS_NSEL_GPIOX->OSPEEDR &= ~(3 << (WIRELESS_NSEL_PIN*2));
  WIRELESS_NSEL_GPIOX->OSPEEDR |= WIRELESS_NSEL_PIN_SPEED << (WIRELESS_NSEL_PIN*2);
  WIRELESS_NSEL_GPIOX->PUPDR &= ~(3 << (WIRELESS_NSEL_PIN*2));
  WIRELESS_NSEL_GPIOX->PUPDR |= WIRELESS_NSEL_PIN_PULL << (WIRELESS_NSEL_PIN*2);
  
  WIRELESS_SCK_GPIOX->AFR[WIRELESS_SCK_PIN>>3] &= ~(0xf << ((WIRELESS_SCK_PIN & 0x07)*4));
  WIRELESS_SCK_GPIOX->AFR[WIRELESS_SCK_PIN>>3] |= WIRELESS_SCK_PIN_AFSEL << ((WIRELESS_SCK_PIN&0x07)*4);
  WIRELESS_SCK_GPIOX->MODER &= ~(3 << (WIRELESS_SCK_PIN*2));
  WIRELESS_SCK_GPIOX->MODER |= WIRELESS_SCK_PIN_MODE << (WIRELESS_SCK_PIN*2);
  WIRELESS_SCK_GPIOX->OTYPER &= ~(1 << WIRELESS_SCK_PIN);
  WIRELESS_SCK_GPIOX->OTYPER |= WIRELESS_SCK_PIN_TYPE << WIRELESS_SCK_PIN;
  WIRELESS_SCK_GPIOX->OSPEEDR &= ~(3 << (WIRELESS_SCK_PIN*2));
  WIRELESS_SCK_GPIOX->OSPEEDR |= WIRELESS_SCK_PIN_SPEED << (WIRELESS_SCK_PIN*2);
  WIRELESS_SCK_GPIOX->PUPDR &= ~(3 << (WIRELESS_SCK_PIN*2));
  WIRELESS_SCK_GPIOX->PUPDR |= WIRELESS_SCK_PIN_PULL << (WIRELESS_SCK_PIN*2);
  
  WIRELESS_MOSI_GPIOX->AFR[WIRELESS_MOSI_PIN>>3] &= ~(0xf << ((WIRELESS_MOSI_PIN & 0x07)*4));
  WIRELESS_MOSI_GPIOX->AFR[WIRELESS_MOSI_PIN>>3] |= WIRELESS_MOSI_PIN_AFSEL << ((WIRELESS_MOSI_PIN&0x07)*4);
  WIRELESS_MOSI_GPIOX->MODER &= ~(3 << (WIRELESS_MOSI_PIN*2));
  WIRELESS_MOSI_GPIOX->MODER |= WIRELESS_MOSI_PIN_MODE << (WIRELESS_MOSI_PIN*2);
  WIRELESS_MOSI_GPIOX->OTYPER &= ~(1 << WIRELESS_MOSI_PIN);
  WIRELESS_MOSI_GPIOX->OTYPER |= WIRELESS_MOSI_PIN_TYPE << WIRELESS_MOSI_PIN;
  WIRELESS_MOSI_GPIOX->OSPEEDR &= ~(3 << (WIRELESS_MOSI_PIN*2));
  WIRELESS_MOSI_GPIOX->OSPEEDR |= WIRELESS_MOSI_PIN_SPEED << (WIRELESS_MOSI_PIN*2);
  WIRELESS_MOSI_GPIOX->PUPDR &= ~(3 << (WIRELESS_MOSI_PIN*2));
  WIRELESS_MOSI_GPIOX->PUPDR |= WIRELESS_MOSI_PIN_PULL << (WIRELESS_MOSI_PIN*2);
  
  WIRELESS_MISO_GPIOX->AFR[WIRELESS_MISO_PIN>>3] &= ~(0xf << ((WIRELESS_MISO_PIN & 0x07)*4));
  WIRELESS_MISO_GPIOX->AFR[WIRELESS_MISO_PIN>>3] |= WIRELESS_MISO_PIN_AFSEL << ((WIRELESS_MISO_PIN&0x07)*4);
  WIRELESS_MISO_GPIOX->MODER &= ~(3 << (WIRELESS_MISO_PIN*2));
  WIRELESS_MISO_GPIOX->MODER |= WIRELESS_MISO_PIN_MODE << (WIRELESS_MISO_PIN*2);
  //WIRELESS_MISO_GPIOX->OTYPER &= ~(1 << WIRELESS_MISO_PIN);
  //WIRELESS_MISO_GPIOX->OTYPER |= WIRELESS_MISO_PIN_TYPE << WIRELESS_MISO_PIN;
  //WIRELESS_MISO_GPIOX->OSPEEDR &= ~(3 << (WIRELESS_MISO_PIN*2));
  //WIRELESS_MISO_GPIOX->OSPEEDR |= WIRELESS_MISO_PIN_SPEED << (WIRELESS_MISO_PIN*2);
  WIRELESS_MISO_GPIOX->PUPDR &= ~(3 << (WIRELESS_MISO_PIN*2));
  WIRELESS_MISO_GPIOX->PUPDR |= WIRELESS_MISO_PIN_PULL << (WIRELESS_MISO_PIN*2);
  
  WIRELESS_SPI->CR1 = 0;
  WIRELESS_SPI->CR1 |= MSB_FIRST;           
  WIRELESS_SPI->CR1 |= BR_PCLK_DIV8;        
  WIRELESS_SPI->CR1 |= MASTER_CONFIG;       
  WIRELESS_SPI->CR1 |= CPOL_IDLE0;          
  WIRELESS_SPI->CR1 |= CPHA_EDGE1;          
  
  WIRELESS_SPI->CR2 = 0;
  WIRELESS_SPI->CR2 |= RX_SIZE8;            
  WIRELESS_SPI->CR2 |= TX_SIZE8;            
  
  //WIRELESS_SPI->CR1 &= ~(1<<9);           
  //WIRELESS_SPI->CR2 |= 1<<2; 					    
  //WIRELESS_SPI->CR2 |= 1<<3;					    
  
  WIRELESS_SPI->CR1 |= 1<<9;          
  WIRELESS_SPI->CR1 |= 1<<8;         
  
  WIRELESS_SPI_EN();
  SPI_RWbyte(0xff);          
}







uint8_t SPI_RWbyte(uint8_t sdata)
{ 
  volatile uint8_t *temp;
  
  //__disable_interrupt();
  while(!(WIRELESS_SPI->SR & 1<<1));           
  *(uint8_t *)&WIRELESS_SPI->DR = sdata;      
  while(!(WIRELESS_SPI->SR & 1<<0));          
  temp = (uint8_t *)&WIRELESS_SPI->DR;
  //__enable_interrupt();
  return *temp;
}


void SPI_ReadData(uint8_t byteCount,uint8_t* pData)
{
  uint8_t i;
  
  for(i=0;i<byteCount;i++)
  {
	*pData=SPI_RWbyte(0xff);
	pData++;
  }
}


void SPI_WriteData(uint8_t byteCount,uint8_t* pData)
{
  uint8_t i;

  for(i=0;i<byteCount;i++)
  {
	SPI_RWbyte(pData[i]);
  }
}




