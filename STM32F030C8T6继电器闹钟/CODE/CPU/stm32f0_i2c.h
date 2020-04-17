#ifndef STM32F0_I2C_H
#define STM32F0_I2C_H

#include "stm32f0xx.h"
#include "stm32f0_peripheral.h"












/********************** I2C接口宏定义 ***************************/
#define I2C_SDA_GPIO_CLOCK_EN        GPIOB_CLOCK_EN       

#define I2C_SDA_GPIOX                 GPIOB       
#define I2C_SDA_PIN                   11
#define I2C_SDA_PIN_MODE              OUTPUT_MODE
#define I2C_SDA_PIN_TYPE              OPEN_DRAIN 
//#define I2C_SDA_PIN_SPEED             HIGH_SPEED
#define I2C_SDA_PIN_SPEED             LOW_SPEED     //高速低速都可
#define I2C_SDA_PIN_PULL              NO_PULL

#define SDA_H()                       (I2C_SDA_GPIOX->BSRR = (1<<I2C_SDA_PIN))
#define SDA_L()                       (I2C_SDA_GPIOX->BRR = (1<<I2C_SDA_PIN))
#define SDA_RD()                      (I2C_SDA_GPIOX->IDR & (1<<I2C_SDA_PIN))



#define I2C_SCL_GPIO_CLOCK_EN        GPIOB_CLOCK_EN       

#define I2C_SCL_GPIOX                 GPIOB       
#define I2C_SCL_PIN                   10
#define I2C_SCL_PIN_MODE              OUTPUT_MODE
#define I2C_SCL_PIN_TYPE              OPEN_DRAIN 
//#define I2C_SCL_PIN_SPEED             HIGH_SPEED
#define I2C_SCL_PIN_SPEED             LOW_SPEED     //高速低速都可
#define I2C_SCL_PIN_PULL              NO_PULL

#define SCL_H()                       (I2C_SCL_GPIOX->BSRR = (1<<I2C_SCL_PIN))
#define SCL_L()                       (I2C_SCL_GPIOX->BRR = (1<<I2C_SCL_PIN))















/************************* 外部函数 ***************************/
void I2c_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Write1Byte(uint8_t buf);
uint8_t I2C_Read1Byte(void);


#endif


