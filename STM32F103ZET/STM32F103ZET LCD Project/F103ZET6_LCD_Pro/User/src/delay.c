#include "delay.h"
#include "stm32f1xx_hal.h"



void delay_us(uint32_t nus)
{
    uint32_t cur_cnt = HAL_GetTick();
    
    while((HAL_GetTick() - cur_cnt) < nus);
}

void delay_ms(uint32_t nms)
{
    uint32_t cur_cnt = HAL_GetTick();
    
    while((HAL_GetTick() - cur_cnt) < nms);
}





