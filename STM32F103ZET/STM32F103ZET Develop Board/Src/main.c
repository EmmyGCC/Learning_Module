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
#include "crc.h"
#include "dma.h"
#include "rtc.h"
#include "sdio.h"
#include "spi.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "W25QXX.h"
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Reset_RTC_Time(void);
void Get_RTC_DateTime(void);
void Get_CrcVal(void);
void Get_DevID(void);
void SDCard_RW_Operation(void);
void W25Q16_Test(void);
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
    MX_DMA_Init();
    MX_SDIO_SD_Init();
    MX_USART1_UART_Init();
    MX_CRC_Init();
    MX_RTC_Init();
    MX_USB_DEVICE_Init();
    MX_SPI2_Init();
    /* USER CODE BEGIN 2 */
    printf("\n\r UART Printf Example: retarget the C library printf function to the UART\n\r");
#if 0
    SDCard_RW_Operation();
#endif
	
#if 0
    Get_DevID();
#endif
	
#if 0
    Get_CrcVal();
#endif
	
#if 0
    W25Q16_Test();
#endif

	//添加用户代码
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
#if 0
        if (HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_RESET)
        {
            Get_RTC_DateTime();
        }
        if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET)
        {
            Reset_RTC_Time();
        }
#endif
        //添加用户代码

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
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
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
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USB;
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

void SDCard_RW_Operation(void)
{
    uint8_t Buffer_Tx[512] = {0}, Buffer_Rx[512] = {0};
    HAL_StatusTypeDef sdcard_status = HAL_OK;

    printf(" Warning: this program may erase all the TF card data. \r\n");

    printf("\r\n Initialize SD card successfully!\r\n\r\n");
    printf(" SD card information! \r\n");
    printf(" CardCapacity  : %d \r\n",hsd.SdCard.BlockNbr );
    printf(" CardBlockSize : %d \r\n",hsd.SdCard.LogBlockSize );
    printf(" RCA           : %d \r\n",hsd.SdCard.RelCardAdd );
    printf(" CardType      : %d \r\n",hsd.SdCard.CardType );

    /*------------------- Block Write --------------------------*/
    memset(Buffer_Tx,0x15,sizeof(Buffer_Tx));

    /*------------------- Block Erase -------------------------------*/
    /*  sdcard_status = HAL_SD_Erase(&hsd, 0, 512);
      if (sdcard_status == HAL_OK)
    	printf("\r\n Erase block successfully!\r\n");
      else
          printf("\r\n Erase block fail!\r\n");

    HAL_Delay(300);
    */

    /* 向SD卡块写入数据 */
    printf("------------------- Write SD card block data Test ------------------\r\n");
    sdcard_status = HAL_SD_WriteBlocks(&hsd,(uint8_t *)Buffer_Tx,3,1,0xffff);
    if(sdcard_status == HAL_OK)
    {
        printf("Write block data ok \r\n" );
    }
    else
    {
        printf("Write block data fail!\r\n " );
    }
    HAL_Delay(200);
    /* 读取操作之后的数据 */
    printf("------------------- Read SD card block data after Write ------------------\r\n");
    sdcard_status = HAL_SD_ReadBlocks(&hsd,(uint8_t *)Buffer_Rx,3,1,0xffff);
    if(sdcard_status == HAL_OK)
    {
        printf("Read block data ok \r\n" );
        for(uint16_t i = 0; i < sizeof(Buffer_Rx); i++)
        {
            printf("0x%02x ", Buffer_Rx[i]);
            if((i+1)%16 == 0)
            {
                printf("\r\n");
            }
        }
    }
}

void Get_DevID(void)
{
#define CHIP_UID	(uint32_t *)0x1FFFF7E8
#if 0
    uint32_t chip_uid[3] = {0};
    uint32_t *id_addr = CHIP_UID;

    chip_uid[0] = HAL_GetUIDw0();
    chip_uid[1] = HAL_GetUIDw1();
    chip_uid[2] = HAL_GetUIDw2();
#else
    uint32_t chip_uid[3] = {0};
    uint32_t *id_addr = CHIP_UID;

    for (uint8_t i = 0; i < 3; i++)
    {
        chip_uid[i] = *id_addr;
        id_addr += 1;
    }
#endif
    printf("chip uid = %4x %4x %4x \r\n", chip_uid[0],chip_uid[1],chip_uid[2]);
}

void Get_CrcVal(void)
{
    uint32_t ready_data[] = {0x68,0x20,0x08,0x11,0x02,0x00,0x01,0x00,0x00,0x00,0x01};

    uint32_t crc_val = HAL_CRC_Accumulate(&hcrc, ready_data, sizeof(ready_data));
    printf("\n\r crc_val: 0x%x \n\r", crc_val);
}

void Get_RTC_DateTime(void)
{
    RTC_DateTypeDef sdatestructure;
    RTC_TimeTypeDef stimestructure;

    /* Get the RTC current Time ,must get time first*/
    HAL_RTC_GetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN);
    /* Get the RTC current Date */
    HAL_RTC_GetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN);

    /* Display date Format : yy/mm/dd */
    printf("%02d/%02d/%02d\r\n",2000 + sdatestructure.Year, sdatestructure.Month, sdatestructure.Date);
    /* Display time Format : hh:mm:ss */
    printf("%02d:%02d:%02d\r\n",stimestructure.Hours, stimestructure.Minutes, stimestructure.Seconds);

    printf("\r\n");
}

void Reset_RTC_Time(void)
{
    RTC_TimeTypeDef sTime = {0};

    sTime.Hours = 0x11;
    sTime.Minutes = 0x20;
    sTime.Seconds = 0x0;

    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
    {
        Error_Handler();
    }
}

void W25Q16_Test(void)
{
    uint8_t wData[0x100];
    uint8_t rData[0x100];
    uint32_t i;
    uint8_t ID[2];

    printf("\r\n SPI-W25Qxxx Example \r\n\r\n");

    /*##-1- Read the device ID  ########################*/
    BSP_W25Qx_Init();
    BSP_W25Qx_Read_ID(ID);
    printf(" W25Qxxx ID is : 0x%02X 0x%02X \r\n\r\n",ID[0],ID[1]);

    /*##-2- Erase Block ##################################*/
    if(BSP_W25Qx_Erase_Block(0) == W25Qx_OK)
        printf(" SPI Erase Block ok\r\n");
    else
        Error_Handler();

    /*##-3- Written to the flash ########################*/
    /* fill buffer */
    for(i =0; i<0x100; i ++)
    {
        wData[i] = i;
        rData[i] = 0;
    }

    if(BSP_W25Qx_Write(wData,0x00,0x100)== W25Qx_OK)
        printf(" SPI Write ok\r\n");
    else
        Error_Handler();

    /*##-4- Read the flash     ########################*/
    if(BSP_W25Qx_Read(rData,0x00,0x100)== W25Qx_OK)
        printf(" SPI Read ok\r\n\r\n");
    else
        Error_Handler();

    printf("SPI Read Data : \r\n");
    for(i =0; i<0x100; i++)
        printf("0x%02X  ",rData[i]);
    printf("\r\n\r\n");

    /*##-5- check date          ########################*/
    if(memcmp(wData,rData,0x100) == 0 )
        printf(" W25Q16FV SPI Test OK\r\n");
    else
        printf(" W25Q16FV SPI Test False\r\n");
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
	printf(" Error_Handler is Running \r\n\r\n");
    for(;;);
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
