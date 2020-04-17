#ifndef STM32F0_ADC_H
#define STM32F0_ADC_H


#include "stm32f0xx.h"
#include "stm32f0_peripheral.h"

/************************* IO口引脚属性宏定义 ***************************/
/*
#define INPUT_MODE                  0X00
#define OUTPUT_MODE                 0X01
#define AF_MODE                     0X02
#define ANALOG_MODE                 0X03

#define PUSH_PULL                   0X00
#define OPEN_DRAIN                  0X01

#define LOW_SPEED                   0X00
#define MEDIUM_SPEED                0X01
#define HIGH_SPEED                  0X03

#define NO_PULL                     0X00
#define PULL_UP                     0X01
#define PULL_DOWN                   0X02
*/


/************************* ADC功能属性宏定义 ***************************/
#define PCLK_DIV2                 0x40000000
#define PCLK_DIV4                 0x80000000
#define RESOLUTION_12B            0X00
#define RESOLUTION_10B            (1<<3)
#define RESOLUTION_8B             (2<<3)
#define RIGHT_ALIGN               0X00
#define LEFT_ALIGN                (1<<5) 
#define SINGLE_MODE               0X00
#define CONTINUE_MODE             (1<<13)
#define DMA_CIRCULAR_DIS          0X00
#define DMA_CIRCULAR_EN           (1<<1)
#define DMA_DIS                   0X00
#define DMA_EN                    (1<<0)


#define SAMP_1CLOCK5               0X00
#define SAMP_7CLOCK5               0X01
#define SAMP_13CLOCK5              0X02
#define SAMP_28CLOCK5              0X03
#define SAMP_41CLOCK5              0X04
#define SAMP_55CLOCK5              0X05
#define SAMP_71CLOCK5              0X06
#define SAMP_239CLOCK5             0X07



/********************** ChargeADC接口宏定义 ***************************/
/*
#define ChargeADC_GPIO_CLOCK_EN        GPIOB_CLOCK_EN
#define ChargeADC_GPIOX                 GPIOB       
#define ChargeADC_PIN                   0
#define ChargeADC_PIN_MODE              ANALOG_MODE
//#define ChargeADC_PIN_TYPE              PUSH_PULL
//#define ChargeADC_PIN_SPEED             HIGH_SPEED
#define ChargeADC_PIN_PULL              NO_PULL

#define ChargeADC_CHANNEL              (1<<8)      //ADC_IN8
*/




/********************** BatteryADC接口宏定义 ***************************/
#define BatteryADC                       ADC1

#define BatteryADC_GPIO_CLOCK_EN        GPIOA_CLOCK_EN       
#define BatteryADC_CLKREG               APB2ENR
#define BatteryADC_CLOCK_EN             (1<<9)

#define BatteryADC_GPIOX                 GPIOA       
#define BatteryADC_PIN                   3
#define BatteryADC_PIN_MODE              ANALOG_MODE
//#define BatteryADC_PIN_TYPE              PUSH_PULL
//#define BatteryADC_PIN_SPEED             HIGH_SPEED
#define BatteryADC_PIN_PULL              NO_PULL

#define BatteryADC_CHANNEL              (1<<3)      //ADC_IN3



#define BatteryADC_EN()                  (BatteryADC->CR |= 1<<0)
#define BatteryADC_DIS()                 (BatteryADC->CR |= 1<<1)

#define BatteryADC_START()               (BatteryADC->CR |= 1<<2)
#define BatteryADC_STOP()                (BatteryADC->CR |= 1<<4)


#define BatteryADC_EOC_RD()             (BatteryADC->ISR & 1<<2)
#define BatteryADC_EOC_CLS()            (BatteryADC->ISR &= ~(1<<2))


//电池电压
#define BatVol_ResFactor                      0.31973f    //120/171 = 0.70175        实际电压的折扣
#define Vol_Factor                            0.8059f     //4095/3300 = 1.241       1毫伏需要的AD值数
#define Bat_Factor                            0.8708f    //1*0.70175*1.2409 = 0.8708 实际1mV电压代表的AD采样值因子数                             

#if(1)
#define BatFull_Val                           3614     //4.15V     4150*0.8708f = 3613.8
#define BatEmpty_Val                          3178     //3.65V     3650*0.8708f = 3178.4
#define BatFactor_Val                         4.36f    //1614-1457/100

#define ChargeBattery_Error                   113      //130*0.8708f = 113.2
#define Battery_Error_H                       3701      //4.25V     4250*0.8708f = 3700.9


#else
#define BatFull_Val                           3657     //4.2V     4200*0.8708f = 3657.3
#define BatEmpty_Val                          3222     //3.7V     3700*0.8708f = 3221.96
//#define BatFactor_Val                         3.48f  
#define BatFactor_Val                         4.35f 
#endif

#define BatQuantityVal_0                      1457     //3.7V     3700*0.39374f = 1456.8
#define BatQuantityVal_10                     1476     //3.75V    3750*0.39374f = 1476.5
#define BatQuantityVal_20                     1496     //3.8V     3800*0.39374f = 1496.2
#define BatQuantityVal_30                     1516     //3.85V    3850*0.39374f = 1515.9
#define BatQuantityVal_40                     1536     //3.9V     3900*0.39374f = 1535.5
#define BatQuantityVal_50                     1555     //3.95V    3950*0.39374f = 1555.3
#define BatQuantityVal_60                     1575     //4.0V     4000*0.39374f = 1574.9
#define BatQuantityVal_70                     1595     //4.05V    4050*0.39374f = 1594.6
#define BatQuantityVal_80                     1614     //4.1V     4100*0.39374f = 1614.3
#define BatQuantityVal_90                     1634     //4.15V    4150*0.39374f = 1634.0
#define BatQuantityVal_100                    1654     //4.2V     4200*0.39374f = 1653.7


//电池电量
#define BatQuantity_NoDetect                   0      //NoDetect
#define BatQuantityLevel_Low                   1      //3.7V以下
#define BatQuantityLevel_0                     2      //3.7-3.75V以上
#define BatQuantityLevel_10                    3      //3.75-3.8V以上
#define BatQuantityLevel_20                    4      //3.8-3.85V以上
#define BatQuantityLevel_30                    5      //3.85-3.9以上
#define BatQuantityLevel_40                    6      //3.9-3.95V以上
#define BatQuantityLevel_50                    7      //3.95-4.0V以上
#define BatQuantityLevel_60                    8      //4.0-4.05V以上
#define BatQuantityLevel_70                    9      //4.05-4.1V以上
#define BatQuantityLevel_80                    10     //4.1-4.15V以上
#define BatQuantityLevel_90                    11     //4.15-4.2V 
#define BatQuantityLevel_100                   12     //4.2V以上


//充电状态电压
//20K接5Vin  20K接CHRG 10K接地 充电时电压为1.25V 充满时为2.5V，闭锁状态1.66V
                              
#define Charging_Vol                           1240    //1250mV   1000*1.24 = 1240
#define Charge_Lock_Vol                        1808    //1666mV   1458*1.24 = 1807.9 
#define Charge_Finish_Vol                      2604   //2500mV  2100*1.24 = 2604


/*
#define Charge_Not                              0
#define Charging                                1
#define Charge_Lock                             2
#define Charge_Finish                           3                          
*/
/************************* DMA功能属性宏定义 ***************************/
#define ADC_DMA_CLKREG               AHBENR
#define ADC_DMA_CLOCK_EN             (1<<0)

#define ADC_DMACHANNEL               DMA1_Channel1

#define PERIPH_TO_MEMORY             0x00
#define MEMORY_TO_PERIPH             (1<<4)

#define PERINC_DIS                   0X00
#define PERINC_EN                    (1<<6)
#define MEMINC_DIS                   0X00 
#define MEMINC_EN                    (1<<7)

#define PERSIZE_8B                   0X00
#define PERSIZE_16B                  (1<<8)
#define PERSIZE_32B                  (2<<8)
#define MEMSIZE_8B                   0X00
#define MEMSIZE_16B                  (1<<10)
#define MEMSIZE_32B                  (2<<10)

#define CIRCULAR_DIS                 0X00
#define CIRCULAR_EN                  (1<<5)

#define PRIORITY_LOW                 0X00
#define PRIORITY_MEDIUM              (1<<12)
#define PRIORITY_HIGH                (2<<12)
#define PRIORITY_VHIGH               (3<<12)

#define HALF_INT_DIS                 0X00
#define HALF_INT_EN                  (1<<2)
#define COMP_INT_DIS                 0X00
#define COMP_INT_EN                  (1<<1)

#define ADC_DMA_PER_ADDR             (uint32_t)(&ADC1->DR)


#define ADC_DMA_IRQ                  DMA1_Channel1_IRQn    //Interrupt Numbers
#define ADC_DMA_PRIORITY             0X02



#define ADC_DMA_EN()                 (ADC_DMACHANNEL->CCR |= 1<<0)
#define ADC_DMA_DIS()                (ADC_DMACHANNEL->CCR &= ~(1<<0))





/*********************DMA 中断标志 **********************/
#define RD_DMA1CHANNEL1_GIF()                     (DMA1->ISR & (1<<0))
#define RD_DMA1CHANNEL1_TCIF()                    (DMA1->ISR & (1<<1))
#define RD_DMA1CHANNEL1_HTIF()                    (DMA1->ISR & (1<<2))
#define RD_DMA1CHANNEL1_TEIF()                    (DMA1->ISR & (1<<3))

#define CLS_DMA1CHANNEL1_CGIF()                    (DMA1->IFCR |= (1<<0))
#define CLS_DMA1CHANNEL1_CTCIF()                   (DMA1->IFCR |= (1<<1))
#define CLS_DMA1CHANNEL1_CHTIF()                   (DMA1->IFCR |= (1<<2))
#define CLS_DMA1CHANNEL1_CTEIF()                   (DMA1->IFCR |= (1<<3))

#define RD_DMA1CHANNEL2_GIF()                     (DMA1->ISR & (1<<4))
#define RD_DMA1CHANNEL2_TCIF()                    (DMA1->ISR & (1<<5))
#define RD_DMA1CHANNEL2_HTIF()                    (DMA1->ISR & (1<<6))
#define RD_DMA1CHANNEL2_TEIF()                    (DMA1->ISR & (1<<7))

#define CLS_DMA1CHANNEL2_CGIF()                    (DMA1->IFCR |= (1<<4))
#define CLS_DMA1CHANNEL2_CTCIF()                   (DMA1->IFCR |= (1<<5))
#define CLS_DMA1CHANNEL2_CHTIF()                   (DMA1->IFCR |= (1<<6))
#define CLS_DMA1CHANNEL2_CTEIF()                   (DMA1->IFCR |= (1<<7))

#define RD_DMA1CHANNEL3_GIF()                     (DMA1->ISR & (1<<8))
#define RD_DMA1CHANNEL3_TCIF()                    (DMA1->ISR & (1<<9))
#define RD_DMA1CHANNEL3_HTIF()                    (DMA1->ISR & (1<<10))
#define RD_DMA1CHANNEL3_TEIF()                    (DMA1->ISR & (1<<11))

#define CLS_DMA1CHANNEL3_CGIF()                    (DMA1->IFCR |= (1<<8))
#define CLS_DMA1CHANNEL3_CTCIF()                   (DMA1->IFCR |= (1<<9))
#define CLS_DMA1CHANNEL3_CHTIF()                   (DMA1->IFCR |= (1<<10))
#define CLS_DMA1CHANNEL3_CTEIF()                   (DMA1->IFCR |= (1<<11))

#define RD_DMA1CHANNEL4_GIF()                     (DMA1->ISR & (1<<12))
#define RD_DMA1CHANNEL4_TCIF()                    (DMA1->ISR & (1<<13))
#define RD_DMA1CHANNEL4_HTIF()                    (DMA1->ISR & (1<<14))
#define RD_DMA1CHANNEL4_TEIF()                    (DMA1->ISR & (1<<15))

#define CLS_DMA1CHANNEL4_CGIF()                    (DMA1->IFCR |= (1<<12))
#define CLS_DMA1CHANNEL4_CTCIF()                   (DMA1->IFCR |= (1<<13))
#define CLS_DMA1CHANNEL4_CHTIF()                   (DMA1->IFCR |= (1<<14))
#define CLS_DMA1CHANNEL4_CTEIF()                   (DMA1->IFCR |= (1<<15))

#define RD_DMA1CHANNEL5_GIF()                     (DMA1->ISR & (1<<16))
#define RD_DMA1CHANNEL5_TCIF()                    (DMA1->ISR & (1<<17))
#define RD_DMA1CHANNEL5_HTIF()                    (DMA1->ISR & (1<<18))
#define RD_DMA1CHANNEL5_TEIF()                    (DMA1->ISR & (1<<19))

#define CLS_DMA1CHANNEL5_CGIF()                    (DMA1->IFCR |= (1<<16))
#define CLS_DMA1CHANNEL5_CTCIF()                   (DMA1->IFCR |= (1<<17))
#define CLS_DMA1CHANNEL5_CHTIF()                   (DMA1->IFCR |= (1<<18))
#define CLS_DMA1CHANNEL5_CTEIF()                   (DMA1->IFCR |= (1<<19))



/************************* 外部函数 ***************************/
//extern uint8_t      aResultDMA[680];

void BatteryADC_Init(void);
uint16_t Get_BatteryADC_AverValue(uint32_t ADC_Channel);
//void ADC_DMA_Init(uint8_t *me_addr, uint16_t len);

#endif

