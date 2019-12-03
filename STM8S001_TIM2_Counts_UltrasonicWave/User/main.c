/**
******************************************************************************
* @文件         main.c
* @作者         LGG
* @版本         V1.0.0
* @日期         2017-11-15
* @摘要         TIM2_Inactive_Mode例程
******************************************************************************
* @attention
* 技新网         www.jixin.pro
* 硬件平台       技新STM8S001J3核心板
* 程序效果       LED1以2Hz的频率闪烁
******************************************************************************
*/ 

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm8s.h"
#include "led.h"
#include "tim2.h"



/* 宏定义 --------------------------------------------------------------------*/
//根据C编译环境进行宏定义
#ifdef _RAISONANCE_
#define PUTCHAR_PROTOTYPE int putchar (char c)
#define GETCHAR_PROTOTYPE int getchar (void)
#elif defined (_COSMIC_)
#define PUTCHAR_PROTOTYPE char putchar (char c)
#define GETCHAR_PROTOTYPE char getchar (void)
#else /* _IAR_ */
#define PUTCHAR_PROTOTYPE int putchar (int c)
#define GETCHAR_PROTOTYPE int getchar (void)
#endif /* _RAISONANCE_ */


/*宏定义按键的 PORT 与 IO */
#define KEY_GPIO_PORT  (GPIOC)
#define KEY_GPIO_PINS  (GPIO_PIN_5)

#define TRI_GPIO_PORT  (GPIOA)
#define TRI_GPIO_PINS  (GPIO_PIN_3)

#define ECHO_GPIO_PORT  (GPIOD)
#define ECHO_GPIO_PINS  (GPIO_PIN_6)


/* 函数声明 ------------------------------------------------------------------*/
static void CLK_Config(void);
void Delay_Ms(uint16_t time);
float Hcsr04GetLength(void);


/**
* @函数名       main
* @功  能       主函数入口
* @参  数       无
* @返回值       无
*/
void main(void)
{
	/*******************初始化按键，防锁代码*************************/
	GPIO_Init(KEY_GPIO_PORT, (GPIO_Pin_TypeDef)KEY_GPIO_PINS, GPIO_MODE_IN_PU_NO_IT);
	if(!GPIO_ReadInputPin(KEY_GPIO_PORT,KEY_GPIO_PINS))
	{
		while(1);
	}
	GPIO_DeInit(KEY_GPIO_PORT);
	/****************************************************************/
	
	CLK_Config(); //配置系统时钟为HSI/1 = 16M
	GPIO_Init(TRI_GPIO_PORT, (GPIO_Pin_TypeDef)TRI_GPIO_PINS, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(ECHO_GPIO_PORT, (GPIO_Pin_TypeDef)ECHO_GPIO_PINS, GPIO_MODE_IN_PU_NO_IT);
	
	//LED_Init();   //LED初始化
	UART1_DeInit();               
	
	/* UART1 configured as follow:
	- BaudRate = 38400 baud   38400
	- Word Length = 8 Bits  数据位：8位
	- One Stop Bit  停止位：1位
	- No parity     无奇偶校验
	- Receive and transmit enabled  接受发送使能
	- UART1 Clock disabled  //时钟失能
	*/
	UART1_Init((uint32_t)38400, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, \
			   UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TX_ENABLE);
	
	/* TIM2 配置 ---------------------------------------------------------------*/
	TIM2_Count_Config();
	
	enableInterrupts();   //开总中断
	
	u16 length = 0;
	while (1)
	{
		Delay_Ms(500);
		
		length = Hcsr04GetLength();
		printf("距离为:%.3f\n",length);
	}
}


/**
* @函数名       CLK_Config
* @功  能       配置系统时钟为HSI/1 = 16M
* @参  数       无
* @返回值       无
*/
static void CLK_Config(void)
{
	/* Initialization of the clock */
	/* Clock divider to HSI/1 */
	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
}


/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: Delay_Ms_Ms
** 功能描述: 延时1MS (可通过仿真来判断他的准确度)            
** 参数描述：time (ms) 注意time<65535
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Delay_Ms(uint16_t time)  //延时函数
{
	uint16_t i,j;
	for(i=0;i<time;i++)
		for(j=0;j<10260;j++);
}


/**
  * @函数名       Delay_Us
  * @功  能       软件延迟
  * @参  数       nCount
  * @返回值       无
  */
void Delay_Us(uint16_t nCount)
{
  /* 减少 nCount 值 */
  while (nCount != 0)
  {
    nCount--;
  }
}

//获取定时器时间
u32 GetEchoTimer(void)
{
	u32 t = 0;
	t = msHcCount*1000;		//得到S
	t += TIM2_GetCounter();	//得到MS
	TIM2_SetCounter(0);		//将TIM2计数寄存器的计数值清零
	Delay_Ms(10);
	return t;
}


/*****************
@Description: 超声波测距
@Parameter: NULL
@Return: 平均值结果
@Note: 
******************/
float Hcsr04GetLength(void)
{
	u32 t = 0;
	int i = 0;
	float lengthTemp = 0;
	float sum = 0;
	while(i!=5)
	{
		GPIO_WriteHigh(TRI_GPIO_PORT, TRI_GPIO_PINS);	//发送口高电平输出
		Delay_Us(20);
		GPIO_WriteLow(TRI_GPIO_PORT, TRI_GPIO_PINS);	//发送口高电平输出
		
		while(GPIO_ReadInputPin(ECHO_GPIO_PORT, ECHO_GPIO_PINS) == RESET);      //等待接收口高电平输出
		OpenTimerForHc();        //打开定时器
		i = i + 1;
		while(GPIO_ReadInputPin(ECHO_GPIO_PORT, ECHO_GPIO_PINS) == SET);      //等待接收口低电平输出
		CloseTimerForHc();        //关闭定时器
		
		t = GetEchoTimer();        //获取时间,分辨率为1US
		lengthTemp = ((float)t/58.0);//cm
		sum = lengthTemp + sum ;
	}
	
	lengthTemp = sum/5.0;
	return lengthTemp;
}


/**
  * @函数名       PUTCHAR_PROTOTYPE
  * @功  能       重定向printf函数
  * @参  数       c：发送的字符
  * @返回值       发送的字符
  */
PUTCHAR_PROTOTYPE
{
  /* Write a character to the UART1 */
  UART1_SendData8(c);
  /* Loop until the end of transmission */
  while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);

  return (c);
}


#ifdef USE_FULL_ASSERT

/**
* @brief  Reports the name of the source file and the source line number
*   where the assert_param error has occurred.
* @param file: pointer to the source file name
* @param line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
	/* User can add his own implementation to report the file name and line number,
	ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	
	/* Infinite loop */
	while (1)
	{
	}
}
#endif

/************************ Copyright (C)2017 LGG. All Rights Reserved *****END OF FILE****/
