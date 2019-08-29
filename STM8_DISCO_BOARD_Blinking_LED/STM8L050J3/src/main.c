/**
  ******************************************************************************
  * @file    Project/Blinking_LED/STM8L050J3/src/main.c
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
#include "stm8l15x_conf.h"
#include "stm8_so8_disco.h"

/** @addtogroup STM8-SO8_Discovery
  * @{
  */

/** @addtogroup Blinking_LED/STM8L050J3
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TIM4_PERIOD       10

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t TimingDelay;
volatile uint8_t BlinkSpeed =0;

/* Private function prototypes -----------------------------------------------*/
void LED_Blink(void);
static void CLK_Config(void);
static void TIM4_Config(void);
static void GPIO_Config(void);
void Delay(__IO uint32_t nTime);
void TimingDelay_Decrement(void);
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

  /* Timer configuration */
  TIM4_Config();

  /* GPIO configuration */
  GPIO_Config();

  while (1)
  {
    //LED_Blink();
  }
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

/**
  * @brief  Configure GPIO for the LED and the user button available on the STM8-SO8_DISCOVERY board
  * @param  None
  * @retval None
  */
static void GPIO_Config(void)
{
  /* Initialize LED mounted on STM8-SO8_DISCOVERY board */
  STM_DISCO_LEDInit(LED1);

  /* disable interrupts */
  disableInterrupts();

  /* Initialize Push Button mounted on STM8-SO8_DISCOVERY board */
  /* Pin configured in input floating mode with interrupt enabled
                 --> connected to EXTIx Interrupt, where x:0..7 */
  STM_DISCO_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);

  /* enable interrupts */
  enableInterrupts();
}

/**
  * @brief  Configure peripherals Clock
  * @param  None
  * @retval None
  */
static void CLK_Config(void)
{
  /* Select HSI as system clock source */
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);

  /* High speed internal clock prescaler: 1 */
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);

  /* Enable TIM4 CLK */
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);
}

/**
  * @brief  Configure TIM4 peripheral
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
  TIM4_TimeBaseInit(TIM4_Prescaler_16384, TIM4_PERIOD-1);

  /* Clear TIM4 update flag */
  TIM4_ClearFlag(TIM4_FLAG_Update);

  /* Enable update interrupt */
  TIM4_ITConfig(TIM4_IT_Update, ENABLE);

  /* Enable TIM4 */
  TIM4_Cmd(ENABLE);
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{
  double TIM4_period = 0;
  uint16_t TIMCLK_Freq = 2;
  uint8_t Tim4_PreNum = TIM4_GetPrescaler();
  
  while(--Tim4_PreNum)
	TIMCLK_Freq *= 2;
  
  TIM4_period = TIM4_PERIOD / (16000000/(TIMCLK_Freq+0.001-0.001));
  
  if (TimingDelay >= (nTime / (TIM4_period*1000)))
  {
	/* Toggle the Led */
	STM_DISCO_LEDToggle(LED1);
	TimingDelay = 0;
  }
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  TimingDelay++;
  
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
