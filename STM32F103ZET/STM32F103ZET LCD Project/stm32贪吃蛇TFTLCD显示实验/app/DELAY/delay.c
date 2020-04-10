#include <stm32f10x.h>
#include <delay.h>

void delay_us(u32 nus)
{
	u32  i;
	SysTick_Config(SystemCoreClock/10000000);
	for(i = 0; i < nus; i++)
	{
		while( !((SysTick->CTRL)&(1 << 16)));
	}
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}
void delay_ms(u32 nms)
{
	u32  i;
	SysTick_Config(SystemCoreClock/10000);
	for(i = 0; i < nms; i++)
	{
		while( !((SysTick->CTRL)&(1 << 16)));
	}
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

