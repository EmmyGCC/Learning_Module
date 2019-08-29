/**
  ******************************************************************************
  * @file    Project/Blinking_LED/STM8L001J3/src/main.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    29-12-2017
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2017 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm8l10x_conf.h"
#include "stm8_so8_disco.h"

/** @addtogroup STM8-SO8_DISCOVERY
  * @{
  */

/** @addtogroup Blinking_LED/STM8L001J3
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TIM4_PERIOD                    124
#define TIM2_PERIOD                    62499

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;
volatile uint8_t BlinkSpeed =0;

/* Private function prototypes -----------------------------------------------*/
void LED_Blink(void);
static void CLK_Config(void);
static void TIM4_Config(void);
static void GPIO_Config(void);
void Delay(__IO uint32_t nTime);
void TimingDelay_Decrement(void);

static void AWU_Config(void);
uint32_t LSI_Measurement(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
  /* Clock configuration */
  CLK_Config();

  /* AWU configuration */
  //AWU_Config();
  
  /* Timer configuration */
  TIM4_Config();

  /* GPIO configuration */
  GPIO_Config();

  /* enable interrupts */
  enableInterrupts();

  //halt();
  while (1)
  {
    LED_Blink();
	
  }
}


uint32_t LSI_Measurement(void)
{
  uint32_t LSI_Clock_Freq_value = 0;
  
  AWU->CSR |= AWU_CSR_MSR;
  LSI_Clock_Freq_value = TIM2_ComputeLsiClockFreq(CLK_GetClockFreq());
  AWU->CSR &= (uint8_t)(~AWU_CSR_MSR);//禁止LSI测量，断开LSI与捕获通道的连接
  
  return LSI_Clock_Freq_value;
}


/**
  * @brief  Blinks LED with four frequencies depending on User press button.
  * @param  None
  * @retval None
  */
void LED_Blink(void)
{
  /* Toggle the Led */
  STM_DISCO_LEDToggle(LED1);

  switch (BlinkSpeed)
  {
    case 0 :
    {
      /* Wait for 1000ms */
      Delay(1000);
      break;
    }
    case 1:
    {
      /* Wait for 500ms */
      Delay(500);
      break;
    }
    case 2:
    {
      /* Wait for 200ms */
      Delay(200);
      break;
    }
    case 3:
    {
      /* wait for 50ms */
      Delay(50);
      break;
    }
    default:
    {
      BlinkSpeed = 0;
      break;
    }
  }
}


void AWU_Config(void)
{
  AWU_LSICalibrationConfig(LSI_Measurement());
  AWU_Init(AWU_Timebase_2s);
}




/**
  * @brief  Configure TIM4 to generate an update interrupt each 1ms
  * @param  None
  * @retval None
  */
static void TIM4_Config(void)
{
  /* TIM4 configuration:
  - TIM4CLK is set to 16 MHz, the TIM4 Prescaler is equal to 128 so the TIM4 counter
  clock used is 16 MHz / 128 = 125 000 Hz
  - With 125 000 Hz we can generate time base:
  max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
  min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
  - In this example we need to generate a time base equal to 1 ms
  so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 */

  /* Time base configuration */
  TIM4_TimeBaseInit(TIM4_Prescaler_128, TIM4_PERIOD);

  /* Clear TIM4 update flag */
  TIM4_ClearFlag(TIM4_FLAG_Update);

  /* Enable update interrupt */
  TIM4_ITConfig(TIM4_IT_Update, ENABLE);

  /* Enable TIM4 */
  TIM4_Cmd(ENABLE);
}

/**
  * @brief  Configure system clock to run at 16Mhz
  * @param  None
  * @retval None
  */
static void CLK_Config(void)
{
  /* Initialization of the clock */

  /* Clock divider to HSI/1 */
  CLK_MasterPrescalerConfig(CLK_MasterPrescaler_HSIDiv1);

  /* ENABLE TIM4 peripheral clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);
  
  /* ENABLE AWU peripheral clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_AWU, ENABLE);
  
  /* ENABLE TIM2 peripheral clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
  
}

/**
  * @brief  Configure GPIO for the LED and the user button available on the STM8-SO8_DISCOVERY board
  * @param  None
  * @retval None
  */
static void GPIO_Config(void)
{
  /* Initialize LED mounted on STM8-SO8_DISCOVERY board */
  STM_DISCO_LEDInit(LED1);

  /* Initialize Push Button mounted on STM8-SO8_DISCOVERY board */
  /* Pin configured in input floating mode with interrupt enabled
                 --> connected to EXTIx Interrupt, where x:0..7 */
  /* disable interrupts */
  disableInterrupts();

  STM_DISCO_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);

  /* enable interrupts */
  enableInterrupts();
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in 1 ms.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if(TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
