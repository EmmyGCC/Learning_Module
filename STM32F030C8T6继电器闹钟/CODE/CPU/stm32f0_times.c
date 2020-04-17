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
  LedBig_TIME->CCR1 = 0;         //��ʼ��Ϊ50%
  //LedBig_TIME->CCR2 = 0;         //��ʼ��Ϊ50%
  LedBig_TIME->CCMR1 = 0;
  /*
  LedBig_TIME->CCMR1 &= ~(7<<4 | 7<<12);
  LedBig_TIME->CCMR1 |= 6<<4 | 6<<12;     //PWM1ģʽ
  LedBig_TIME->CCMR1 |= 1<<3 | 1<<11;       //����Ƚ�Ԥװ��ʹ�ܣ���������ı䣩
  LedBig_TIME->CCMR1 |= 1<<2 | 1<<10;       //���ٴ���ʹ��
  LedBig_TIME->CCMR1 &= ~(3<<0 | 3<<8);       //ͨ��1��ͨ��2����Ϊ�Ƚ����ģʽ
  */
  
  LedBig_TIME->CCMR1 &= ~(7<<4);
  LedBig_TIME->CCMR1 |= 6<<4;     //PWM1ģʽ
  LedBig_TIME->CCMR1 |= 1<<3;       //����Ƚ�Ԥװ��ʹ�ܣ���������ı䣩
  LedBig_TIME->CCMR1 |= 1<<2;       //���ٴ���ʹ��
  LedBig_TIME->CCMR1 &= ~(3<<0);
  
  LedBig_TIME->CCER = 0;
  //LedBig_TIME->CCER |= CC1P_H;         //�����������
  //LedBig_TIME->CCER |= 1<<2 | 1<<6;      //CH1N ��CH2N���ʹ��
  LedBig_TIME->CR1 = 0;
  LedBig_TIME->CR1 |= ARPE_EN;         //��Ӱ�ӼĴ���
  LedBig_TIME->CR1 |= OPM_DIS;          //һֱ����
  LedBig_TIME->CR1 |= URS_OVER;         //����������ж�
  LedBig_TIME->CR1 |= UDIS_EN;          //�������
  LedBig_TIME_UGEN();                   //��ʼ��������
  
  LedBig_TIME_UIE_DIS();              //�����жϽ�ֹ  
  LedBig_TIME_COMPEN();               //CH1N�Ƚ����ʹ��
  LedBig_TIME_MOEN();                 //���д˹��ܵ�һ��Ҫ��
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
  LedRed_TIME->CCR1 = 0;         //��ʼ��Ϊ50%
  //LedRed_TIME->CCR2 = 0;         //��ʼ��Ϊ50%
  LedRed_TIME->CCMR1 = 0;
  /*
  LedRed_TIME->CCMR1 &= ~(7<<4 | 7<<12);
  LedRed_TIME->CCMR1 |= 6<<4 | 6<<12;     //PWM1ģʽ
  LedRed_TIME->CCMR1 |= 1<<3 | 1<<11;       //����Ƚ�Ԥװ��ʹ�ܣ���������ı䣩
  LedRed_TIME->CCMR1 |= 1<<2 | 1<<10;       //���ٴ���ʹ��
  LedRed_TIME->CCMR1 &= ~(3<<0 | 3<<8);       //ͨ��1��ͨ��2����Ϊ�Ƚ����ģʽ
  */
  
  LedRed_TIME->CCMR1 &= ~(7<<4);
  LedRed_TIME->CCMR1 |= 6<<4;     //PWM1ģʽ
  LedRed_TIME->CCMR1 |= 1<<3;       //����Ƚ�Ԥװ��ʹ�ܣ���������ı䣩
  LedRed_TIME->CCMR1 |= 1<<2;       //���ٴ���ʹ��
  LedRed_TIME->CCMR1 &= ~(3<<0);
  
  LedRed_TIME->CCER = 0;
  //LedRed_TIME->CCER |= CC1P_H;         //�����������
  //LedRed_TIME->CCER |= 1<<2 | 1<<6;      //CH1N ��CH2N���ʹ��
  LedRed_TIME->CR1 = 0;
  LedRed_TIME->CR1 |= ARPE_EN;         //��Ӱ�ӼĴ���
  LedRed_TIME->CR1 |= OPM_DIS;          //һֱ����
  LedRed_TIME->CR1 |= URS_OVER;         //����������ж�
  LedRed_TIME->CR1 |= UDIS_EN;          //�������
  LedRed_TIME_UGEN();                   //��ʼ��������
  
  LedRed_TIME_UIE_DIS();              //�����жϽ�ֹ  
  LedRed_TIME_COMPEN();               //CH1N�Ƚ����ʹ��
  LedRed_TIME_MOEN();                 //���д˹��ܵ�һ��Ҫ��
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
  LedBlue_TIME->CCR1 = 0;         //��ʼ��Ϊ50%
  //LedBlue_TIME->CCR2 = 0;         //��ʼ��Ϊ50%
  LedBlue_TIME->CCMR1 = 0;
  /*
  LedBlue_TIME->CCMR1 &= ~(7<<4 | 7<<12);
  LedBlue_TIME->CCMR1 |= 6<<4 | 6<<12;     //PWM1ģʽ
  LedBlue_TIME->CCMR1 |= 1<<3 | 1<<11;       //����Ƚ�Ԥװ��ʹ�ܣ���������ı䣩
  LedBlue_TIME->CCMR1 |= 1<<2 | 1<<10;       //���ٴ���ʹ��
  LedBlue_TIME->CCMR1 &= ~(3<<0 | 3<<8);       //ͨ��1��ͨ��2����Ϊ�Ƚ����ģʽ
  */
  
  LedBlue_TIME->CCMR1 &= ~(7<<4);
  LedBlue_TIME->CCMR1 |= 6<<4;     //PWM1ģʽ
  LedBlue_TIME->CCMR1 |= 1<<3;       //����Ƚ�Ԥװ��ʹ�ܣ���������ı䣩
  LedBlue_TIME->CCMR1 |= 1<<2;       //���ٴ���ʹ��
  LedBlue_TIME->CCMR1 &= ~(3<<0);
  
  LedBlue_TIME->CCER = 0;
  //LedBlue_TIME->CCER |= CC1P_H;         //�����������
  //LedBlue_TIME->CCER |= 1<<2 | 1<<6;      //CH1N ��CH2N���ʹ��
  LedBlue_TIME->CR1 = 0;
  LedBlue_TIME->CR1 |= ARPE_EN;         //��Ӱ�ӼĴ���
  LedBlue_TIME->CR1 |= OPM_DIS;          //һֱ����
  LedBlue_TIME->CR1 |= URS_OVER;         //����������ж�
  LedBlue_TIME->CR1 |= UDIS_EN;          //�������
  LedBlue_TIME_UGEN();                   //��ʼ��������
  
  LedBlue_TIME_UIE_DIS();              //�����жϽ�ֹ  
  LedBlue_TIME_COMPEN();               //CH1N�Ƚ����ʹ��
  LedBlue_TIME_MOEN();                 //���д˹��ܵ�һ��Ҫ��
  LedBlue_TIME_EN();
  //*/
}

















