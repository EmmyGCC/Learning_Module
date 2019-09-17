/**
  ******************************************************************************
  * File Name          : USART.h
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

/* USER CODE BEGIN Private defines */

typedef struct
{
  uint8_t tx_data[30];
  uint8_t tx_len;
}UART_TX_Tpy;

typedef struct
{
  uint8_t rx_data[30];
  uint8_t rx_len;
  volatile _Bool rx_cmp;
}UART_RX_Tpy;

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART3_UART_Init(void);

/* USER CODE BEGIN Prototypes */

UART_TX_Tpy *UART1_TX_BUF(void);
UART_RX_Tpy *UART1_RX_BUF(void);

UART_TX_Tpy *UART3_TX_BUF(void);
UART_RX_Tpy *UART3_RX_BUF(void);

void Uart_rxIT_process(UART_HandleTypeDef *huart);


void Uart1_Rx_Process(UART_RX_Tpy *uart1_buf);
void Uart3_Rx_Process(UART_RX_Tpy *uart3_buf);


/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
