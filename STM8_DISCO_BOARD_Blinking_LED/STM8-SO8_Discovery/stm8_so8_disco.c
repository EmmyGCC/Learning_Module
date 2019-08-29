/**
  ******************************************************************************
  * @file    stm8_so8_disco.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    29-12-2017
  * @brief   This file contains definitions for STM8-SO8_Discovery Kit's Leds and
  *              push-button hardware resources.
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
#include "stm8_so8_disco.h"

/** @addtogroup Utilities
  * @{
  */

/** @addtogroup STM8_SO8_DISCOVERY
  * @{
  */

/** @defgroup STM8_SO8_DISCOVERY_LOW_LEVEL
  * @brief This file provides firmware functions to manage Leds, push-buttons,
  *         available on STM8-SO8 Discovery board from STMicroelectronics.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_TypeDef* LED_PORT[LEDn] = {LED1_GPIO_PORT};

CONST uint8_t LED_PIN[LEDn] =
  {
    LED1_GPIO_PIN
  };

GPIO_TypeDef* BUTTON_PORT[BUTTONn] =
  {
    USER_BUTTON_PORT
  };

CONST uint8_t BUTTON_PIN[BUTTONn] =
  {
    USER_BUTTON_PIN
  };

CONST uint8_t BUTTON_EXTI[BUTTONn] =
  {
    USER_BUTTON_EXTI
  };

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup STM8_SO8_DISCOVERYLOW_LEVEL_Private_Functions
  * @{
  */

/**
  * @brief  Configures LED GPIO.
  * @param  Led: Specifies the Led to be configured.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  * @retval None
  */
void STM_DISCO_LEDInit(Led_TypeDef Led)
{
  /* Configure the GPIO_LED pin */
#ifdef STM8S001
  GPIO_Init(LED_PORT[Led], (GPIO_Pin_TypeDef) LED_PIN[Led], GPIO_MODE_OUT_PP_HIGH_FAST);
#else
  GPIO_Init(LED_PORT[Led], LED_PIN[Led], GPIO_Mode_Out_PP_High_Fast);
#endif /* STM8S001 */
}

/**
  * @brief  Turns selected LED On.
  * @param  Led: Specifies the Led to be set on.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  * @retval None
  */
void STM_DISCO_LEDOn(Led_TypeDef Led)
{
  LED_PORT[Led]->ODR &= (uint8_t)~LED_PIN[Led];
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led: Specifies the Led to be set off.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  * @retval None
  */
void STM_DISCO_LEDOff(Led_TypeDef Led)
{
  LED_PORT[Led]->ODR |= (uint8_t)LED_PIN[Led];
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led: Specifies the Led to be toggled.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  * @retval None
  */
void STM_DISCO_LEDToggle(Led_TypeDef Led)
{
  LED_PORT[Led]->ODR ^= (uint8_t)LED_PIN[Led];
}

/**
  * @brief  Configures Button GPIO and EXTI Line.
  * @param  Button: Specifies the Button to be configured.
  *   This parameter can be one of following parameters:
  *     @arg BUTTON_USER: User Push Button
  * @param  Button_Mode: Specifies Button mode.
  *   This parameter can be one of following parameters:
  *     @arg BUTTON_MODE_GPIO: Button will be used as simple IO
  *     @arg BUTTON_MODE_EXTI: Button will be connected to EXTI pin (sensitive to falling trigger)
  * @retval None
  */
void STM_DISCO_PBInit(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode)
{
#ifdef STM8S001
    if (Button_Mode == BUTTON_MODE_EXTI)
  { /* Pin configured in input floating mode with interrupt enabled
                       --> connected to EXTIx Interrupt, where x:0..7 */
    GPIO_Init(BUTTON_PORT[Button], (GPIO_Pin_TypeDef) BUTTON_PIN[Button], GPIO_MODE_IN_FL_IT);
    EXTI_SetExtIntSensitivity((EXTI_Port_TypeDef)BUTTON_EXTI[(uint8_t)Button], EXTI_SENSITIVITY_FALL_ONLY);
  }
  else
  { /* Pin configured in input floating mode with interrupt disabled */
    GPIO_Init(BUTTON_PORT[Button], (GPIO_Pin_TypeDef) BUTTON_PIN[Button], GPIO_MODE_IN_FL_NO_IT);
  }
#else
  if (Button_Mode == BUTTON_MODE_EXTI)
  { /* Pin configured in input floating mode with interrupt enabled
                       --> connected to EXTIx Interrupt, where x:0..7 */
    GPIO_Init(BUTTON_PORT[Button], BUTTON_PIN[Button], GPIO_Mode_In_FL_IT);
    EXTI_SetPinSensitivity((EXTI_Pin_TypeDef)BUTTON_EXTI[(uint8_t)Button], EXTI_Trigger_Falling);
  }
  else
  { /* Pin configured in input floating mode with interrupt disabled */
    GPIO_Init(BUTTON_PORT[Button], BUTTON_PIN[Button], GPIO_Mode_In_FL_No_IT);
  }
#endif /* STM8S001 */
}

/**
  * @brief  Returns the selected Button state.
  * @param  Button: Specifies the Button to be checked.
  *   This parameter can be one of following parameters:
  *     @arg BUTTON_USER: Key Push Button
  * @retval The Button GPIO pin value.
  */
uint8_t STM_DISCO_PBGetState(Button_TypeDef Button)
{
#ifdef STM8S001
  return GPIO_ReadInputPin(BUTTON_PORT[Button], (GPIO_Pin_TypeDef)BUTTON_PIN[Button]);
#else
 return GPIO_ReadInputDataBit(BUTTON_PORT[Button], (GPIO_Pin_TypeDef)BUTTON_PIN[Button]);
#endif /* STM8S001 */
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
