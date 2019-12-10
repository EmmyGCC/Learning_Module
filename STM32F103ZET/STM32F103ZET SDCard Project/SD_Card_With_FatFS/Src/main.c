/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "sdio.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdint.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
DWORD free_size, tot_sect, fre_sect;
FATFS *f_s;

FATFS fs;                 // Work area (file system object) for logical drive
FIL fil;                  // file objects
FRESULT result = FR_OK;

uint32_t byteswritten;                /* File write counts */
uint32_t bytesread;                   /* File read counts */
uint8_t wtext[] = "This is STM32 working with FatFs"; /* File write buffer */
uint8_t rtext[100];                     /* File read buffers */
char filename[] = "STM32cube.txt";
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */


    /* MCU Configuration--------------------------------------------------------*/
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_SDIO_SD_Init();
    MX_USART1_UART_Init();
    MX_FATFS_Init();
    /* USER CODE BEGIN 2 */
	
    printf("\r\n ****** FatFs Example ******\r\n\r\n");
	
	
	/*##-1- Register the file system object to the FatFs module ##############*/
    result = f_mount(&fs, "", 0);
    if(result)
    {
        printf(" mount error : %d \r\n",result);
        Error_Handler();
    }
    else
        printf(" mount success!!! \r\n");
	
	result = f_mkfs("", 1, 0);
	if(result)
    {
        printf("\r\n f_mkfs result: %d\r\n", result);
        //Error_Handler();
    }
    else
        printf(" f_mkfs success!!! \r\n");
	
	result = f_getfree( "", &free_size, &f_s);
	if (result)
	{
		printf("\r\n f_mkfs failed, result: %d\r\n", result);
	}
	/* Get total sectors and free sectors */
    tot_sect = (f_s->n_fatent - 2) * f_s->csize;
    fre_sect = free_size * f_s->csize;

    /* Print the free space (assuming 512 bytes/sector) */
    printf("%10lu KiB total drive space.\n%10lu KiB available.\n", tot_sect / 2, fre_sect / 2);
	
	
	
    /*##-2- Create and Open new text file objects with write access ######*/
    result = f_open(&fil, filename, FA_CREATE_ALWAYS | FA_WRITE);
    if(result)
        printf(" open file error : %d\r\n",result);
    else
        printf(" open file sucess!!! \r\n");

    /*##-3- Write data to the text files ###############################*/
    result = f_write(&fil, wtext, sizeof(wtext), (void *)&byteswritten);
    if(result)
        printf(" write file error : %d\r\n",result);
    else
    {
        printf(" write file sucess!!! \r\n");
        printf(" write Data : %s\r\n",wtext);
    }

    /*##-4- Close the open text files ################################*/
    result = f_close(&fil);
    if(result)
        printf(" close error : %d\r\n",result);
    else
        printf(" close sucess!!! \r\n");

    /*##-5- Open the text files object with read access ##############*/
    result = f_open(&fil, filename, FA_READ);
    if(result)
        printf(" open file error : %d\r\n",result);
    else
        printf(" open file sucess!!! \r\n");

    /*##-6- Read data from the text files ##########################*/
    result = f_read(&fil, rtext, sizeof(rtext), (UINT*)&bytesread);
    if(result)
        printf(" read error!!! %d\r\n",result);
    else
    {
        printf(" read sucess!!! \r\n");
        printf(" read Data : %s\r\n",rtext);
    }

    /*##-7- Close the open text files ############################*/
    result = f_close(&fil);
    if(result)
        printf(" close error!!! %d\r\n",result);
    else
        printf(" close sucess!!! \r\n");

    /*##-8- Compare read data with the expected data ############*/
    if(bytesread == byteswritten)
    {
        printf(" FatFs is working well!!!\r\n");
    }
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    for (;;)
    {}
    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
