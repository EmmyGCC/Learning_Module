/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */


uint8_t Data_Uart3Rec;         //串口3中断接收一个字符
uint8_t Data_Uart1Rec;


UART_RX_TYPE Uart1_rx;
UART_RX_TYPE Uart3_rx;
/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 38400;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_HalfDuplex_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}
/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 38400;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */
	
  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();
  
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART3 GPIO Configuration    
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX 
    */
    GPIO_InitStruct.Pin = BL_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(BL_TX_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = BL_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(BL_RX_GPIO_Port, &GPIO_InitStruct);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspInit 1 */
	
  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();
  
    /**USART3 GPIO Configuration    
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX 
    */
    HAL_GPIO_DeInit(GPIOB, BL_TX_Pin|BL_RX_Pin);

    /* USART3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        Uart1_rx.rx_buf[Uart1_rx.rx_count] = Data_Uart1Rec;

        if((Uart1_rx.rx_buf[Uart1_rx.rx_count-1] == 'O') && (Uart1_rx.rx_buf[Uart1_rx.rx_count] == 'K'))
        {
            Uart1_rx.rx_buf[++Uart1_rx.rx_count] = '\0';
            Uart1_rx.rx_size = Uart1_rx.rx_count;
            Uart1_rx.rx_count = 0;
            Uart1_rx.rx_cpl = 1;
        }
        else
            Uart1_rx.rx_count++;
    }
    if(huart->Instance == USART3)
    {
        Uart3_rx.rx_buf[Uart3_rx.rx_count] = Data_Uart3Rec;
        
        if((Uart3_rx.rx_buf[Uart3_rx.rx_count-1] == 'O') && (Uart3_rx.rx_buf[Uart3_rx.rx_count] == 'K'))
        {
            Uart3_rx.rx_buf[++Uart3_rx.rx_count] = '\0';
            Uart3_rx.rx_size = Uart3_rx.rx_count;
            Uart3_rx.rx_count = 0;
            Uart3_rx.rx_cpl = 1;
        }
        else
            Uart3_rx.rx_count++;
    }
}

void UART_Transmit_Str(UART_HandleTypeDef *huart,uint8_t tx_len,uint8_t *p_data)
{
    if(HAL_OK == HAL_HalfDuplex_EnableTransmitter(huart))
    {
        while(tx_len--)
        {
            HAL_UART_Transmit(huart, p_data, 1, 0xFFFF);
            p_data++;
        }
    }
}



static void Printf_BL_Info(void)
{
  if(memcmp(&Uart3_rx.rx_buf[Uart3_rx.rx_size-2],"OK",2) == 0)
  {
	//printf("%s\n",(const char*)Uart3_rx.rx_buf);
	//从回复的第一段中提取出':'后面的内容
	const char *temp = strtok((char *)Uart3_rx.rx_buf,"\r\n");
	if(temp != NULL)
	{
	  char *p_ret = strchr(temp,':');
	  if(p_ret != NULL)
		printf("%s\n",++p_ret);
	}
  }
}
void HC_05_STATE(void)
{
  uint8_t *p_Tx_cmd = "AT+STATE?\r\n";
  
  UART_Transmit_Str(&huart3,strlen((const char*)p_Tx_cmd),p_Tx_cmd);
  HAL_Delay(70);
  
  if(Uart3_rx.rx_cpl)
  {
	Uart3_rx.rx_cpl = 0;
	
	printf("BL's STATE:");
	Printf_BL_Info();
  }
}

void HC_05_PSWD(void)
{
  uint8_t *p_Tx_cmd = "AT+PSWD?\r\n";
  
  UART_Transmit_Str(&huart3,strlen((const char*)p_Tx_cmd),p_Tx_cmd);
  HAL_Delay(70);
  
  if(Uart3_rx.rx_cpl)
  {
	Uart3_rx.rx_cpl = 0;
	
	printf("BL's PSWD:");
	Printf_BL_Info();
  }
}



#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
 
  return ch;
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
