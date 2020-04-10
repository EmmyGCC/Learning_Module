#include "stm32f10x.h"
#include "exti.h"
#include "lcd.h"
#include "delay.h"
extern u8 dir;
void KEY_GPIO_Config()
{
	GPIO_InitTypeDef  GPIO_Initstructure;
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE );
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_Initstructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_Initstructure);
}

static void KEY_NVIC_Config()
{
	NVIC_InitTypeDef    NVIC_InitStructure;
	NVIC_PriorityGroupConfig (NVIC_PriorityGroup_1);
	
	//配置中断源 按键 1
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE ;
	NVIC_Init(&NVIC_InitStructure);
	
	//配置中断源 按键 2
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE ;
	NVIC_Init(&NVIC_InitStructure);
	
	//配置中断源 按键 3
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE ;
	NVIC_Init(&NVIC_InitStructure);
	
	//配置中断源 按键 4
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE ;
	NVIC_Init(&NVIC_InitStructure);
}

//EXTI 中断配置
void EXTI_Key_Config()
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	//配置按键key1
	 KEY_GPIO_Config();
	//打开AFIO 复用时钟
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_AFIO, ENABLE );
	//配置NVIC中断
	KEY_NVIC_Config();
	
	
	//选择EXTI的信号源
	GPIO_EXTILineConfig (GPIO_PortSourceGPIOA, GPIO_PinSource0|GPIO_PinSource1|GPIO_PinSource2|GPIO_PinSource3);
	
	//中断线0
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	//中断模式
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	//触发类型 上升沿
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	//使能中断
	EXTI_InitStructure.EXTI_LineCmd = ENABLE ;
	EXTI_Init(&EXTI_InitStructure);
	
	//中断线1
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_Init(&EXTI_InitStructure);
	
	//中断线2
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	EXTI_Init(&EXTI_InitStructure);
	
	//中断线3
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_Init(&EXTI_InitStructure);
}




void EXTI0_IRQHandler(void)
{
	//确认是否产生EXTI LINE 中断
     if(EXTI_GetITStatus(EXTI_Line0) != RESET )
	{
		dir = 0;
		//LCD_ShowNum(180,290,dir,3,12); 
		LCD_ShowString(190,290,200,12,12,"straight"); //显示当前蛇的前进方向
		EXTI_ClearITPendingBit(EXTI_Line0);
		 
	}
}

void EXTI1_IRQHandler(void)
{
	//确认是否产生EXTI LINE 中断
   if(EXTI_GetITStatus(EXTI_Line1) != RESET )
	{
			dir = 1;
			LCD_ShowString(190,290,200,12,12,"left    "); //显示当前蛇的前进方向
			EXTI_ClearITPendingBit(EXTI_Line1);
	}
}
void EXTI2_IRQHandler(void)
{
	//确认是否产生EXTI LINE 中断
   if(EXTI_GetITStatus(EXTI_Line2) != RESET )
	{
			dir = 2;
			LCD_ShowString(190,290,200,12,12,"right      ");  //显示当前蛇的前进方向
			EXTI_ClearITPendingBit(EXTI_Line2);
	}
}

void EXTI3_IRQHandler(void)                                                                                                               
{
	//确认是否产生EXTI LINE 中断
	if(EXTI_GetITStatus(EXTI_Line3) != RESET )
	{
			dir = 3;
			LCD_ShowString(190,290,200,12,12,"    down");  //显示当前蛇的前进方向
			EXTI_ClearITPendingBit(EXTI_Line3);
	}
}

