#ifndef STM32F0_IWDG_H
#define STM32F0_IWDG_H

#include "stm32f0xx.h"



#define USE_IWDG



#define IWDG_START                0xcccc
#define IWDG_WRITE_ACCESS         0x5555
#define IWDG_REFRESH              0xaaaa

#define IWDG_PR4                  0x00
#define IWDG_PR8                  0x01
#define IWDG_PR16                 0x02
#define IWDG_PR32                 0x03
#define IWDG_PR64                 0x04
#define IWDG_PR128                0x05
#define IWDG_PR256                0x06

#define IWDG_RELOAD_VAL           4000          //只有12位，最大只能是4095
#define IWDG_RDSR()               (IWDG->SR)
#define IWDG_FEED()               (IWDG->KR = IWDG_REFRESH)
#define WDT_FEED()                (IWDG_FEED())


/************************* 外部函数 ***************************/

void IWDG_Init(void);

#endif








