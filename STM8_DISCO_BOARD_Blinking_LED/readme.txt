/** @page STM8SO8_Discovery_FW STM8-SO8-DISCOVERY demonstration firmware Readme file

  @verbatim
  ******************** (C)COPYRIGHT 2017 STMicroelectronics *******************
  * @file    Blinking_LED/readme.txt
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    29-12-2017
  * @brief   Description of the Blinking_LED demonstration
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
  @endverbatim

  @par Description

  This demo provides a short description of how to use the GPIO peripheral to
  toggle LED LD1 infinitely with three different STM8-SO8 devices.

  This demo shows how to configure external interrupt lines.
  In this demo, 1 EXTI line is configured  to generate an interrupt on each falling edge.
  In the interrupt routine a BlinkSpeed variable is updated with each press of a button.

  In this demo:
    - EXTI Line7 is connected to PB7 pin (For STM8L0xxJ3 devices)
    - EXTI Line4 is connected to PC4 pin (For STM8S001J3 device)

  This demo provides how to use the TIM4 peripheral to generate time base
  equal to 1 ms. In this demo a "Delay" function is
  implemented based on the TIM4 time base.

  The TIM4CLK frequency is 16MHz, the Prescaler is 128 so the TIM4 counter
  clock is 125 KHz.
  The TIM4 is running at TIM4 frequency = TIM4 counter clock / (TIM4_PERIOD + 1)
                                        = 125000 / 125 = 1 KHz.
  So the TIM4 generates an Update Interrupt each 1 / 1000 = 1 ms.

  @par Directory content

  - Blinking_LED/STM8L001J3/src/main.c                   Main file containing the "main" function
  - Blinking_LED/STM8L001J3/src/stm8l10x_it.c            Interrupt routines source (interrupt handlers)
  - Blinking_LED/STM8L001J3/src/stm8_interrupt_vector.c  Basic interrupt vector table for STM8 devices
  - Blinking_LED/STM8L001J3/inc/stm8l10x_conf.h          Library Configuration file
  - Blinking_LED/STM8L001J3/inc/stm8l10x_it.h            Header file including all interrupt handlers prototypes

  - Blinking_LED/STM8L050J3/src/main.c                   Main file containing the "main" function
  - Blinking_LED/STM8L050J3/src/stm8l15x_it.c            Interrupt routines source (interrupt handlers)
  - Blinking_LED/STM8L050J3/src/stm8_interrupt_vector.c  Basic interrupt vector table for STM8 devices
  - Blinking_LED/STM8L050J3/inc/stm8l15x_conf.h          Library Configuration file
  - Blinking_LED/STM8L050J3/inc/stm8l15x_it.h            Header file including all interrupt handlers prototypes

  - Blinking_LED/STM8S001J3/src/main.c                   Main file containing the "main" function
  - Blinking_LED/STM8S001J3/src/stm8s_it.c               Interrupt routines source (interrupt handlers)
  - Blinking_LED/STM8S001J3/src/stm8_interrupt_vector.c  Basic interrupt vector table for STM8 devices
  - Blinking_LED/STM8S001J3/inc/stm8s_conf.h             Library Configuration file
  - Blinking_LED/STM8S001J3/inc/stm8s_it.h               Header file including all interrupt handlers prototypes

  @par Hardware and Software environment

  - This demo runs on STM8L001J3, STM8L050J3 and STM8S001J3 devices.

  - This demo has been tested with STM8-SO8-DISCOVERY board
     and can be easily tailored to any other development board.

  - STM8-SO8-DISCOVERY Set-up
     - One led LD1.
     - One Push Button B1.

  @par How to use it ?

  In order to make the program work, you must do the following :

  - Open your preferred toolchain
  - Set as active project the preferred STM8 Device configuration to use.
  - Clean the project (it's mandatory !)
  - Rebuild all files and load your image into target memory
  - Run the demo

@note
 - STM8L001J3, STM8L050J3 and STM8S001J3 devices are STM8 microcontrollers with a Flash memory
   density up to 8 Kbytes.

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
