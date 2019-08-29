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

/* Define to prevent recursive inclusion -------------------------------------*/
#if !defined(__STM8_SO8_DISCOVERY_H)
#define __STM8_SO8_DISCOVERY_H

/* Includes ------------------------------------------------------------------*/
#include "stm8_so8_disco.h"
#if defined(STM8L001)
#include "stm8l10x_conf.h"
#elif defined(STM8L05X_LD_VL)
#include "stm8l15x_conf.h"
#elif defined(STM8S001)
#include "stm8s_conf.h"
#endif
/** @addtogroup Utilities
  * @{
  */

/** @addtogroup STM8_SO8_DISCOVERY
  * @{
  */

/** @addtogroup STM8_SO8_DISCOVERY_LOW_LEVEL
  * @{
  */

/** @defgroup STM8_SO8_DISCOVERY_Exported_Types
  * @{
  */
typedef enum
{
  LED1 = 0
}Led_TypeDef;

typedef enum
{
  BUTTON_USER = 0
}Button_TypeDef;

typedef enum
{
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
} ButtonMode_TypeDef;

/** @defgroup STM8_SO8_DISCOVERY_LOW_LEVEL_Exported_Constants
  * @{
  */

/** @addtogroup STM8_SO8_DISCOVERY_LOW_LEVEL_LED
  * @{
  */
#define LEDn                  1

#if defined (STM8L001) || defined(STM8L05X_LD_VL)
#define LED1_GPIO_PORT        GPIOD
#define LED1_GPIO_PIN         GPIO_Pin_0
#elif defined(STM8S001)
#define LED1_GPIO_PORT        GPIOA
#define LED1_GPIO_PIN         GPIO_PIN_3
#endif /* STM8L001 || STM8L05X_LD_VL */

/**
  * @}
  */

/** @addtogroup STM8_SO8_DISCOVERY_LOW_LEVEL_BUTTON
  * @{
  */
#define BUTTONn                     1

/**
 * @brief USER push-button
 */
#if defined(STM8L001) || defined(STM8L05X_LD_VL)
#define USER_BUTTON_PORT             GPIOB
#define USER_BUTTON_PIN              GPIO_Pin_7
#define USER_BUTTON_EXTI             EXTI_Pin_7
#elif defined(STM8S001)
#define USER_BUTTON_PORT             GPIOC
#define USER_BUTTON_PIN              GPIO_PIN_4
#define USER_BUTTON_EXTI             EXTI_PORT_GPIOC
#endif /* STM8L001 || STM8L05X_LD_VL */

/**
  * @}
  */

/**
  * @}
  */

/* Exported Macros ------------------------------------------------------------*/

/** @defgroup STM8_SO8_DISCOVERY_LOW_LEVEL_Exported_Functions
  * @{
  */
void STM_DISCO_LEDInit(Led_TypeDef Led);
void STM_DISCO_LEDOn(Led_TypeDef Led);
void STM_DISCO_LEDOff(Led_TypeDef Led);
void STM_DISCO_LEDToggle(Led_TypeDef Led);
void STM_DISCO_PBInit(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode);
uint8_t STM_DISCO_PBGetState(Button_TypeDef Button);
/**
  * @}
  */

#endif /* __STM8_SO8_DISCOVERY_H */

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
