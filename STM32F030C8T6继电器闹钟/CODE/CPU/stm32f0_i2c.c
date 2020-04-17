#include "stm32f0xx.h"
#include "stm32f0_i2c.h"





void I2c_Init(void)
{
  SDA_H();
  SCL_H();
  
  RCC->GPIO_CLKREG |= I2C_SDA_GPIO_CLOCK_EN;
  I2C_SDA_GPIOX->MODER &= ~(3 << (I2C_SDA_PIN*2));
  I2C_SDA_GPIOX->MODER |= I2C_SDA_PIN_MODE << (I2C_SDA_PIN*2);
  I2C_SDA_GPIOX->OTYPER &= ~(1 << I2C_SDA_PIN);
  I2C_SDA_GPIOX->OTYPER |= I2C_SDA_PIN_TYPE << I2C_SDA_PIN;
  I2C_SDA_GPIOX->OSPEEDR &= ~(3 << (I2C_SDA_PIN*2));
  I2C_SDA_GPIOX->OSPEEDR |= I2C_SDA_PIN_SPEED << (I2C_SDA_PIN*2);
  I2C_SDA_GPIOX->PUPDR &= ~(3 << (I2C_SDA_PIN*2));
  I2C_SDA_GPIOX->PUPDR |= I2C_SDA_PIN_PULL << (I2C_SDA_PIN*2);

  RCC->GPIO_CLKREG |= I2C_SCL_GPIO_CLOCK_EN;
  I2C_SCL_GPIOX->MODER &= ~(3 << (I2C_SCL_PIN*2));
  I2C_SCL_GPIOX->MODER |= I2C_SCL_PIN_MODE << (I2C_SCL_PIN*2);
  I2C_SCL_GPIOX->OTYPER &= ~(1 << I2C_SCL_PIN);
  I2C_SCL_GPIOX->OTYPER |= I2C_SCL_PIN_TYPE << I2C_SCL_PIN;
  I2C_SCL_GPIOX->OSPEEDR &= ~(3 << (I2C_SCL_PIN*2));
  I2C_SCL_GPIOX->OSPEEDR |= I2C_SCL_PIN_SPEED << (I2C_SCL_PIN*2);
  I2C_SCL_GPIOX->PUPDR &= ~(3 << (I2C_SCL_PIN*2));
  I2C_SCL_GPIOX->PUPDR |= I2C_SCL_PIN_PULL << (I2C_SCL_PIN*2);
   

}


void SysDelay_us(uint16_t xus)	
{
	
	while(xus--)
	{
    //__NOP;
    volatile uint8_t test1us_val = 2;    //需加volatile,否则会被优化
    uint8_t comp_val = 127;
		while(test1us_val--)
    {
      comp_val += 2;
    }
	}
}







void I2C_Start(void)
{
  SDA_H(); 
  SCL_H();
  SysDelay_us(1);
  SDA_L();
  SysDelay_us(1);
  //SCL_L();
}


void I2C_Stop()
{
  SDA_L();
  SCL_H();
  SysDelay_us(1);
  SDA_H();
  SysDelay_us(1);
  //SCL_L();
}



void I2C_Write1Byte(uint8_t buf)
{
  uint8_t i;  
  
	for(i = 0; i < 8; i++)		
	{
    SCL_L();
    if(buf & 0x80)
      SDA_H();   
		else 
      SDA_L();
		SysDelay_us(1);
		SCL_H();     
   	SysDelay_us(1);       
		buf <<= 1;
	}
  SCL_L();
	SysDelay_us(1);
  SDA_H();
  SysDelay_us(1);
  SCL_H();
	SysDelay_us(1); 
  while(SDA_RD() && (i<255)) i++;
	SCL_L();   //不能省
	SysDelay_us(1);      
}


uint8_t I2C_Read1Byte(void)
{
  uint8_t i, res_val;  
  

	for(i = 0; i < 8; i++)		
	{
    SCL_H();                
		SysDelay_us(1);   
		res_val <<= 1;  
    if(SDA_RD())
      res_val |= 1;  
		SCL_L();                
		SysDelay_us(1);  
   
	}
  //SDA_L();      //后面是停止信号，要用非应答
  //SysDelay_us(1);
  //SCL_H();
	//SysDelay_us(1); 
	//SCL_L();
	//SysDelay_us(1);
  return res_val;
}



