#include "delay.h"
#include "stm32f10x.h"
#include "lcd.h"
#include "stdlib.h"
#include "time.h"
#include "snake.h"
#include "exti.h"
u8 dir = 3;

//RTC初始化函数用于获取当前时间产生真随机数
u8 RTC_Init(void) 
{
	//检查是不是第一次配置时钟
	u8 temp=0;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟   
	PWR_BackupAccessCmd(ENABLE);	//使能后备寄存器访问  
	if (BKP_ReadBackupRegister(BKP_DR1) != 0x5050)		//从指定的后备寄存器中读出数据:读出了与写入的指定数据不相乎
		{	 			
		BKP_DeInit();	//复位备份区域 	
		RCC_LSEConfig(RCC_LSE_ON);	//设置外部低速晶振(LSE),使用外设低速晶振
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET&&temp<250)	//检查指定的RCC标志位设置与否,等待低速晶振就绪
			{
			temp++;
			delay_ms(10);
			}
		if(temp>=250)return 1;//初始化时钟失败,晶振有问题	    
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
		RCC_RTCCLKCmd(ENABLE);	//使能RTC时钟  
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
		RTC_WaitForSynchro();		//等待RTC寄存器同步  
		RTC_ITConfig(RTC_IT_SEC, ENABLE);		//使能RTC秒中断
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
		RTC_EnterConfigMode();/// 允许配置	
		RTC_SetPrescaler(32767); //设置RTC预分频的值
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
		//RTC_Set(2015,1,14,17,42,55);  //设置时间	
		RTC_ExitConfigMode(); //退出配置模式  
		BKP_WriteBackupRegister(BKP_DR1, 0X5050);	//向指定的后备寄存器中写入用户程序数据
		}
		
	   else//系统继续计时
		{
		RTC_WaitForSynchro();	//等待最近一次对RTC寄存器的写操作完成
		RTC_ITConfig(RTC_IT_SEC, ENABLE);	//使能RTC秒中断
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
		}
	return 0; //ok
}
 

int main(void)
 {  
	POINT_COLOR=BLUE;  //画笔颜色 
	RTC_Init();
	Snake_Init();
    EXTI_Key_Config();
	LCD_Init();
	LCD_Fill(0,0,8,280,YELLOW);//Rectangle(8,8-1,85,120,0xaa);//绘制一个矩形框, 0-127
	LCD_Fill(232,0,240,280,YELLOW);//
	LCD_Fill(0,0,240,8,YELLOW);//
	LCD_Fill(0,272,240,280,YELLOW);//
		
	LCD_ShowString(0,290,200,24,24,"Score:");//Show Score
	LCD_ShowString(75,300,200,12,12,"0  length:3");//Show Score
  	while(1) 	
	{  
	    Snake_Run(dir);
		delay_ms(200);
	} 
}
 
