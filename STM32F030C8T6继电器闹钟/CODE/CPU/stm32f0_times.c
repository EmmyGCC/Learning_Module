#include "stm32f0xx.h"
#include "stm32f0_peripheral.h"
#include "stm32f0_times.h"



void LedBig_Init(void)
{
  RCC->GPIO_CLKREG |= LedBig_GPIO_CLOCK_EN;

  LedBig_GPIOX->MODER &= ~(3 << (LedBig_PIN*2));
  LedBig_GPIOX->MODER |= LedBig_PIN_MODE << (LedBig_PIN*2);
  LedBig_GPIOX->AFR[LedBig_PIN>>3] &= ~(0xf << ((LedBig_PIN & 0x07)*4));
  LedBig_GPIOX->AFR[LedBig_PIN>>3] |= LedBig_AFSEL << ((LedBig_PIN&0x07)*4);
  LedBig_GPIOX->OTYPER &= ~(1 << LedBig_PIN);
  LedBig_GPIOX->OTYPER |= LedBig_PIN_TYPE << LedBig_PIN;
  LedBig_GPIOX->OSPEEDR &= ~(3 << (LedBig_PIN*2));
  LedBig_GPIOX->OSPEEDR |= LedBig_PIN_SPEED << (LedBig_PIN*2);
  LedBig_GPIOX->PUPDR &= ~(3 << (LedBig_PIN*2));
  LedBig_GPIOX->PUPDR |= LedBig_PIN_PULL << (LedBig_PIN*2);
  
  RCC->LedBig_TIME_CLKREG |= LedBig_TIME_CLOCK_EN;
  LedBig_TIME->PSC = 192-1;        //4us      
  LedBig_TIME->ARR = 255;          //1Khz
  LedBig_TIME->CCR1 = 0;         //初始化为50%
  //LedBig_TIME->CCR2 = 0;         //初始化为50%
  LedBig_TIME->CCMR1 = 0;
  /*
  LedBig_TIME->CCMR1 &= ~(7<<4 | 7<<12);
  LedBig_TIME->CCMR1 |= 6<<4 | 6<<12;     //PWM1模式
  LedBig_TIME->CCMR1 |= 1<<3 | 1<<11;       //输出比较预装载使能（运行中需改变）
  LedBig_TIME->CCMR1 |= 1<<2 | 1<<10;       //快速触发使能
  LedBig_TIME->CCMR1 &= ~(3<<0 | 3<<8);       //通道1和通道2配置为比较输出模式
  */
  
  LedBig_TIME->CCMR1 &= ~(7<<4);
  LedBig_TIME->CCMR1 |= 6<<4;     //PWM1模式
  LedBig_TIME->CCMR1 |= 1<<3;       //输出比较预装载使能（运行中需改变）
  LedBig_TIME->CCMR1 |= 1<<2;       //快速触发使能
  LedBig_TIME->CCMR1 &= ~(3<<0);
  
  LedBig_TIME->CCER = 0;
  //LedBig_TIME->CCER |= CC1P_H;         //输出极性配置
  //LedBig_TIME->CCER |= 1<<2 | 1<<6;      //CH1N 与CH2N输出使能
  LedBig_TIME->CR1 = 0;
  LedBig_TIME->CR1 |= ARPE_EN;         //有影子寄存器
  LedBig_TIME->CR1 |= OPM_DIS;          //一直计数
  LedBig_TIME->CR1 |= URS_OVER;         //仅溢出产生中断
  LedBig_TIME->CR1 |= UDIS_EN;          //允许更新
  LedBig_TIME_UGEN();                   //初始化计数器
  
  LedBig_TIME_UIE_DIS();              //更新中断禁止  
  LedBig_TIME_COMPEN();               //CH1N比较输出使能
  LedBig_TIME_MOEN();                 //带有此功能的一定要打开
  LedBig_TIME_EN();
  //*/
}



void LedRed_Init(void)
{
  RCC->GPIO_CLKREG |= LedRed_GPIO_CLOCK_EN;

  LedRed_GPIOX->MODER &= ~(3 << (LedRed_PIN*2));
  LedRed_GPIOX->MODER |= LedRed_PIN_MODE << (LedRed_PIN*2);
  LedRed_GPIOX->AFR[LedRed_PIN>>3] &= ~(0xf << ((LedRed_PIN & 0x07)*4));
  LedRed_GPIOX->AFR[LedRed_PIN>>3] |= LedRed_AFSEL << ((LedRed_PIN&0x07)*4);
  LedRed_GPIOX->OTYPER &= ~(1 << LedRed_PIN);
  LedRed_GPIOX->OTYPER |= LedRed_PIN_TYPE << LedRed_PIN;
  LedRed_GPIOX->OSPEEDR &= ~(3 << (LedRed_PIN*2));
  LedRed_GPIOX->OSPEEDR |= LedRed_PIN_SPEED << (LedRed_PIN*2);
  LedRed_GPIOX->PUPDR &= ~(3 << (LedRed_PIN*2));
  LedRed_GPIOX->PUPDR |= LedRed_PIN_PULL << (LedRed_PIN*2);
  
  RCC->LedRed_TIME_CLKREG |= LedRed_TIME_CLOCK_EN;
  LedRed_TIME->PSC = 192-1;        //4us      
  LedRed_TIME->ARR = 255;          //1Khz
  LedRed_TIME->CCR1 = 0;         //初始化为50%
  //LedRed_TIME->CCR2 = 0;         //初始化为50%
  LedRed_TIME->CCMR1 = 0;
  /*
  LedRed_TIME->CCMR1 &= ~(7<<4 | 7<<12);
  LedRed_TIME->CCMR1 |= 6<<4 | 6<<12;     //PWM1模式
  LedRed_TIME->CCMR1 |= 1<<3 | 1<<11;       //输出比较预装载使能（运行中需改变）
  LedRed_TIME->CCMR1 |= 1<<2 | 1<<10;       //快速触发使能
  LedRed_TIME->CCMR1 &= ~(3<<0 | 3<<8);       //通道1和通道2配置为比较输出模式
  */
  
  LedRed_TIME->CCMR1 &= ~(7<<4);
  LedRed_TIME->CCMR1 |= 6<<4;     //PWM1模式
  LedRed_TIME->CCMR1 |= 1<<3;       //输出比较预装载使能（运行中需改变）
  LedRed_TIME->CCMR1 |= 1<<2;       //快速触发使能
  LedRed_TIME->CCMR1 &= ~(3<<0);
  
  LedRed_TIME->CCER = 0;
  //LedRed_TIME->CCER |= CC1P_H;         //输出极性配置
  //LedRed_TIME->CCER |= 1<<2 | 1<<6;      //CH1N 与CH2N输出使能
  LedRed_TIME->CR1 = 0;
  LedRed_TIME->CR1 |= ARPE_EN;         //有影子寄存器
  LedRed_TIME->CR1 |= OPM_DIS;          //一直计数
  LedRed_TIME->CR1 |= URS_OVER;         //仅溢出产生中断
  LedRed_TIME->CR1 |= UDIS_EN;          //允许更新
  LedRed_TIME_UGEN();                   //初始化计数器
  
  LedRed_TIME_UIE_DIS();              //更新中断禁止  
  LedRed_TIME_COMPEN();               //CH1N比较输出使能
  LedRed_TIME_MOEN();                 //带有此功能的一定要打开
  LedRed_TIME_EN();
  //*/
}







void LedBlue_Init(void)
{
  RCC->GPIO_CLKREG |= LedBlue_GPIO_CLOCK_EN;

  LedBlue_GPIOX->MODER &= ~(3 << (LedBlue_PIN*2));
  LedBlue_GPIOX->MODER |= LedBlue_PIN_MODE << (LedBlue_PIN*2);
  LedBlue_GPIOX->AFR[LedBlue_PIN>>3] &= ~(0xf << ((LedBlue_PIN & 0x07)*4));
  LedBlue_GPIOX->AFR[LedBlue_PIN>>3] |= LedBlue_AFSEL << ((LedBlue_PIN&0x07)*4);
  LedBlue_GPIOX->OTYPER &= ~(1 << LedBlue_PIN);
  LedBlue_GPIOX->OTYPER |= LedBlue_PIN_TYPE << LedBlue_PIN;
  LedBlue_GPIOX->OSPEEDR &= ~(3 << (LedBlue_PIN*2));
  LedBlue_GPIOX->OSPEEDR |= LedBlue_PIN_SPEED << (LedBlue_PIN*2);
  LedBlue_GPIOX->PUPDR &= ~(3 << (LedBlue_PIN*2));
  LedBlue_GPIOX->PUPDR |= LedBlue_PIN_PULL << (LedBlue_PIN*2);
  
  RCC->LedBlue_TIME_CLKREG |= LedBlue_TIME_CLOCK_EN;
  LedBlue_TIME->PSC = 192-1;        //4us      
  LedBlue_TIME->ARR = 255;          //1Khz
  LedBlue_TIME->CCR1 = 0;         //初始化为50%
  //LedBlue_TIME->CCR2 = 0;         //初始化为50%
  LedBlue_TIME->CCMR1 = 0;
  /*
  LedBlue_TIME->CCMR1 &= ~(7<<4 | 7<<12);
  LedBlue_TIME->CCMR1 |= 6<<4 | 6<<12;     //PWM1模式
  LedBlue_TIME->CCMR1 |= 1<<3 | 1<<11;       //输出比较预装载使能（运行中需改变）
  LedBlue_TIME->CCMR1 |= 1<<2 | 1<<10;       //快速触发使能
  LedBlue_TIME->CCMR1 &= ~(3<<0 | 3<<8);       //通道1和通道2配置为比较输出模式
  */
  
  LedBlue_TIME->CCMR1 &= ~(7<<4);
  LedBlue_TIME->CCMR1 |= 6<<4;     //PWM1模式
  LedBlue_TIME->CCMR1 |= 1<<3;       //输出比较预装载使能（运行中需改变）
  LedBlue_TIME->CCMR1 |= 1<<2;       //快速触发使能
  LedBlue_TIME->CCMR1 &= ~(3<<0);
  
  LedBlue_TIME->CCER = 0;
  //LedBlue_TIME->CCER |= CC1P_H;         //输出极性配置
  //LedBlue_TIME->CCER |= 1<<2 | 1<<6;      //CH1N 与CH2N输出使能
  LedBlue_TIME->CR1 = 0;
  LedBlue_TIME->CR1 |= ARPE_EN;         //有影子寄存器
  LedBlue_TIME->CR1 |= OPM_DIS;          //一直计数
  LedBlue_TIME->CR1 |= URS_OVER;         //仅溢出产生中断
  LedBlue_TIME->CR1 |= UDIS_EN;          //允许更新
  LedBlue_TIME_UGEN();                   //初始化计数器
  
  LedBlue_TIME_UIE_DIS();              //更新中断禁止  
  LedBlue_TIME_COMPEN();               //CH1N比较输出使能
  LedBlue_TIME_MOEN();                 //带有此功能的一定要打开
  LedBlue_TIME_EN();
  //*/
}

















