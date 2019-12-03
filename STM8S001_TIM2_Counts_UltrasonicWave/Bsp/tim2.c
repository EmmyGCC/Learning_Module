/**
******************************************************************************
* @文件         tim2.C
* @作者         LGG
* @版本         V1.0.0
* @日期         2017-11-15
* @摘要         tim2源文件
******************************************************************************
*/ 

/* 包含头文件 ----------------------------------------------------------------*/
#include "tim2.h"


u16 msHcCount = 0;//ms计数

void OpenTimerForHc(void)        //打开定时器
{
	TIM2_SetCounter(0);	//清除计数
	msHcCount = 0;
	//使能TIMx外设
	TIM2_Cmd(ENABLE);
}


void CloseTimerForHc(void)        //关闭定时器
{
	TIM2_Cmd(DISABLE);		//失能TIMx外设
}


/**
* @函数名       TIM2_Count_Config
* @功  能       配置TIM2定时1ms中断一次
* @参  数       无
* @返回值       无
*/
void TIM2_Count_Config(void)
{
	/* 
	TIM2 配置:
	- TIM2的时钟是16 MHz, 128分频后是125kHz
	16 MHz / 128 = 125 000 Hz
	- 预装载值设为124（125-1）时，计数器溢出的时间为1ms
	125000 / 125 = 1000Hz = 1ms
	*/
	
	/* 128分频，预装载值为（125-1） */
	TIM2_TimeBaseInit(TIM2_PRESCALER_128, 124);
	
	/* 清除TIM2的更新标志位 */
	TIM2_ClearFlag(TIM2_FLAG_UPDATE);
	
	/* 使能更新中断 */
	TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
	
	/* 使能TIM2，开始计数 */
	TIM2_Cmd(ENABLE);
}



/**
* @函数名       TIM2_PWM_Config
* @功  能       配置TIM2输出PWM 频率16k
*               OC2占空比CCR2_Val/1000     OC3占空比CCR3_Val/1000
* @参  数       CCR2_Val：OC2的占空比输入
*               CCR3_Val：OC3的占空比输入
* @返回值       无
*/
void TIM2_PWM_Config(uint16_t CCR2_Val, uint16_t CCR3_Val)
{
	/* TIM2 1分频，预装载值1000 */
	TIM2_TimeBaseInit(TIM2_PRESCALER_1, (1000 - 1));
	
	/* 配置TIM2_OC2为PWM1模式 ,装载值CCR2_Val，输出极性：高*/ 
	TIM2_OC2Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE,CCR2_Val, TIM2_OCPOLARITY_HIGH);
	TIM2_OC2PreloadConfig(ENABLE);        //TIM2_OC2预装载使能
	
	/* 配置TIM2_OC3为PWM1模式 ,装载值CCR3_Val，输出极性：高*/       
	TIM2_OC3Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE,CCR3_Val, TIM2_OCPOLARITY_HIGH);
	TIM2_OC3PreloadConfig(ENABLE);        //TIM2_OC3预装载使能
	
	TIM2_ARRPreloadConfig(ENABLE);        //TIM2预装载值使能
	
	/* 使能TIM2 */
	TIM2_Cmd(ENABLE);
}


/**
* @函数名       TIM2_OC_Active_ModeConfig
* @功  能       配置TIM2的OC2 OC3 输出不同的延迟信号
*               OC2延迟500ms然后一直输出高     OC3延迟250ms然后一直输出高
* @参  数       CCR2_Val：OC2装载值输入
*               CCR3_Val：OC3装载值输入
* @返回值       无
*/
void TIM2_OC_Active_ModeConfig(uint16_t CCR2_Val, uint16_t CCR3_Val)
{
	/* TIM2初始化配置   32768分频，装载值65535*/      
	TIM2_TimeBaseInit(TIM2_PRESCALER_32768, 65535);
	
	/* 32768分频，立即装载 */
	TIM2_PrescalerConfig(TIM2_PRESCALER_32768, TIM2_PSCRELOADMODE_IMMEDIATE);
	
	/* OC1的配置 */
	//  TIM2_OC1Init(TIM2_OCMODE_ACTIVE, TIM2_OUTPUTSTATE_ENABLE,CCR1_Val, TIM2_OCPOLARITY_HIGH);
	//  
	//  TIM2_OC1PreloadConfig(DISABLE);
	
	
	/* OC2的配置 */
	TIM2_OC2Init(TIM2_OCMODE_ACTIVE, TIM2_OUTPUTSTATE_ENABLE,CCR2_Val, TIM2_OCPOLARITY_HIGH); 
	TIM2_OC2PreloadConfig(DISABLE);
	
	/* OC3的配置 */
	TIM2_OC3Init(TIM2_OCMODE_ACTIVE, TIM2_OUTPUTSTATE_ENABLE,CCR3_Val, TIM2_OCPOLARITY_HIGH);
	TIM2_OC3PreloadConfig(DISABLE);
	TIM2_ARRPreloadConfig(ENABLE);        //预装载使能
	
	/* TIM2 使能*/
	TIM2_Cmd(ENABLE);
}


/**
* @函数名       TIM2_OC_Inactive_ModeConfig
* @功  能       配置TIM2的OC2 OC3 在计数到1000ms 500ms然后分别进入中断
*               OC2延迟1000ms然后计入中断     OC3延迟500ms然后进入中断
* @参  数       CCR2_Val：OC2装载值输入
*               CCR3_Val：OC3装载值输入
* @返回值       无
*/
void TIM2_OC_Inactive_ModeConfig(uint16_t CCR2_Val, uint16_t CCR3_Val)
{
	/* TIM2初始化配置   32768分频，装载值65535*/  
	TIM2_TimeBaseInit(TIM2_PRESCALER_32768, 65535);
	
	/* 32768分频，立即装载 */
	TIM2_PrescalerConfig(TIM2_PRESCALER_32768, TIM2_PSCRELOADMODE_IMMEDIATE);
	
	/* OC1的配置 */
	//TIM2_OC1Init(TIM2_OCMODE_INACTIVE, TIM2_OUTPUTSTATE_ENABLE,CCR1_Val, TIM2_OCPOLARITY_HIGH); 
	//TIM2_OC1PreloadConfig(DISABLE);
	
	/* OC2的配置 */
	TIM2_OC2Init(TIM2_OCMODE_INACTIVE, TIM2_OUTPUTSTATE_ENABLE,CCR2_Val, TIM2_OCPOLARITY_HIGH); 
	TIM2_OC2PreloadConfig(DISABLE);
	
	/* OC3的配置 */
	TIM2_OC3Init(TIM2_OCMODE_INACTIVE, TIM2_OUTPUTSTATE_ENABLE,CCR3_Val, TIM2_OCPOLARITY_HIGH); 
	TIM2_OC3PreloadConfig(DISABLE);
	
	TIM2_ARRPreloadConfig(ENABLE);//预装载使能
	
	/* TIM2中断使能 */
	TIM2_ITConfig(TIM2_IT_CC2, ENABLE);
	TIM2_ITConfig(TIM2_IT_CC3, ENABLE);
	
	/* TIM2 使能*/
	TIM2_Cmd(ENABLE);
}

/************************ Copyright (C)2017 LGG. All Rights Reserved *****END OF FILE****/


