/**
******************************************************************************
* @�ļ�         main.c
* @����         LGG
* @�汾         V1.0.0
* @����         2017-11-15
* @ժҪ         TIM2_Inactive_Mode����
******************************************************************************
* @attention
* ������         www.jixin.pro
* Ӳ��ƽ̨       ����STM8S001J3���İ�
* ����Ч��       LED1��2Hz��Ƶ����˸
******************************************************************************
*/ 

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm8s.h"
#include "led.h"
#include "tim2.h"



/* �궨�� --------------------------------------------------------------------*/
//����C���뻷�����к궨��
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


/*�궨�尴���� PORT �� IO */
#define KEY_GPIO_PORT  (GPIOC)
#define KEY_GPIO_PINS  (GPIO_PIN_5)

#define TRI_GPIO_PORT  (GPIOA)
#define TRI_GPIO_PINS  (GPIO_PIN_3)

#define ECHO_GPIO_PORT  (GPIOD)
#define ECHO_GPIO_PINS  (GPIO_PIN_6)


/* �������� ------------------------------------------------------------------*/
static void CLK_Config(void);
void Delay_Ms(uint16_t time);
float Hcsr04GetLength(void);


/**
* @������       main
* @��  ��       ���������
* @��  ��       ��
* @����ֵ       ��
*/
void main(void)
{
	/*******************��ʼ����������������*************************/
	GPIO_Init(KEY_GPIO_PORT, (GPIO_Pin_TypeDef)KEY_GPIO_PINS, GPIO_MODE_IN_PU_NO_IT);
	if(!GPIO_ReadInputPin(KEY_GPIO_PORT,KEY_GPIO_PINS))
	{
		while(1);
	}
	GPIO_DeInit(KEY_GPIO_PORT);
	/****************************************************************/
	
	CLK_Config(); //����ϵͳʱ��ΪHSI/1 = 16M
	GPIO_Init(TRI_GPIO_PORT, (GPIO_Pin_TypeDef)TRI_GPIO_PINS, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(ECHO_GPIO_PORT, (GPIO_Pin_TypeDef)ECHO_GPIO_PINS, GPIO_MODE_IN_PU_NO_IT);
	
	//LED_Init();   //LED��ʼ��
	UART1_DeInit();               
	
	/* UART1 configured as follow:
	- BaudRate = 38400 baud   38400
	- Word Length = 8 Bits  ����λ��8λ
	- One Stop Bit  ֹͣλ��1λ
	- No parity     ����żУ��
	- Receive and transmit enabled  ���ܷ���ʹ��
	- UART1 Clock disabled  //ʱ��ʧ��
	*/
	UART1_Init((uint32_t)38400, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, \
			   UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TX_ENABLE);
	
	/* TIM2 ���� ---------------------------------------------------------------*/
	TIM2_Count_Config();
	
	enableInterrupts();   //�����ж�
	
	u16 length = 0;
	while (1)
	{
		Delay_Ms(500);
		
		length = Hcsr04GetLength();
		printf("����Ϊ:%.3f\n",length);
	}
}


/**
* @������       CLK_Config
* @��  ��       ����ϵͳʱ��ΪHSI/1 = 16M
* @��  ��       ��
* @����ֵ       ��
*/
static void CLK_Config(void)
{
	/* Initialization of the clock */
	/* Clock divider to HSI/1 */
	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
}


/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ��������: Delay_Ms_Ms
** ��������: ��ʱ1MS (��ͨ���������ж�����׼ȷ��)            
** ����������time (ms) ע��time<65535
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Delay_Ms(uint16_t time)  //��ʱ����
{
	uint16_t i,j;
	for(i=0;i<time;i++)
		for(j=0;j<10260;j++);
}


/**
  * @������       Delay_Us
  * @��  ��       ����ӳ�
  * @��  ��       nCount
  * @����ֵ       ��
  */
void Delay_Us(uint16_t nCount)
{
  /* ���� nCount ֵ */
  while (nCount != 0)
  {
    nCount--;
  }
}

//��ȡ��ʱ��ʱ��
u32 GetEchoTimer(void)
{
	u32 t = 0;
	t = msHcCount*1000;		//�õ�S
	t += TIM2_GetCounter();	//�õ�MS
	TIM2_SetCounter(0);		//��TIM2�����Ĵ����ļ���ֵ����
	Delay_Ms(10);
	return t;
}


/*****************
@Description: ���������
@Parameter: NULL
@Return: ƽ��ֵ���
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
		GPIO_WriteHigh(TRI_GPIO_PORT, TRI_GPIO_PINS);	//���Ϳڸߵ�ƽ���
		Delay_Us(20);
		GPIO_WriteLow(TRI_GPIO_PORT, TRI_GPIO_PINS);	//���Ϳڸߵ�ƽ���
		
		while(GPIO_ReadInputPin(ECHO_GPIO_PORT, ECHO_GPIO_PINS) == RESET);      //�ȴ����տڸߵ�ƽ���
		OpenTimerForHc();        //�򿪶�ʱ��
		i = i + 1;
		while(GPIO_ReadInputPin(ECHO_GPIO_PORT, ECHO_GPIO_PINS) == SET);      //�ȴ����տڵ͵�ƽ���
		CloseTimerForHc();        //�رն�ʱ��
		
		t = GetEchoTimer();        //��ȡʱ��,�ֱ���Ϊ1US
		lengthTemp = ((float)t/58.0);//cm
		sum = lengthTemp + sum ;
	}
	
	lengthTemp = sum/5.0;
	return lengthTemp;
}


/**
  * @������       PUTCHAR_PROTOTYPE
  * @��  ��       �ض���printf����
  * @��  ��       c�����͵��ַ�
  * @����ֵ       ���͵��ַ�
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
