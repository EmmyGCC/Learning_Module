/**
******************************************************************************
* @file    Demonstrations/Src/stm32f0xx_it.c 
* @author  MCD Application Team
* @version V1.2.0
* @date    05-Dec-2014
* @brief   Main Interrupt Service Routines.
*          This file provides template for all exceptions handler and 
*          peripherals interrupt service routine.
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*   1. Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*   3. Neither the name of STMicroelectronics nor the names of its contributors
*      may be used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/

#include "stm32f0xx_it.h"
#include "uart_interface.h"
#include "stm32f0_usart.h"
#include "m0_systick.h"
#include "stm32f0_exint.h"
#include "rf_interface.h"
#include "radio.h"
#include "system_func.h"





void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

void SVC_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
    if(SYSTICK_RDFLAG())
    {
        SYSTICK_CLSFLAG();
        
        SysTick_Handle();
        /*
        if((SYSTICK_CNT % 1000) == 0)
        {
        time_dat++;
    }*/
    }
    
}


void TIM15_IRQHandler(void)
{
    
}


void TIM17_IRQHandler(void)
{
    
}




void TIM6_IRQHandler(void)
{
    
}



void EXTI0_1_IRQHandler(void)
{
    
}


void EXTI2_3_IRQHandler(void)				
{
}



void EXTI4_15_IRQHandler(void)
{
    if(NIRQ_RDPR())
    {
        Si4438_Interrupt_Handler((uint8_t *)Get_pWirelessBuf());
        
        NIRQ_CLSPR(); 
    }
}


void USART1_IRQHandler(void)
{
    if(USART1Com->ISR & (1<<3))
        USART1Com->ICR |= (1<<3);
    
    if(USART1Com_RDRXNE())
    {
        Get_pSystemFunc()->Com1RxData(USART1Com->RDR, Get_pCarrierUartRxBuf());
        /*调试看现象用CarrierUart_RxBuf.Frame_Data[CarrierUart_RxBuf.FrameTotalLen++] = USART1Com->RDR;*/
    }
    if(USART1Com_RDTC() && USART1Com_RDTXINTEN())
    {
        USART1Com->TDR = CarrierUart_TX_INT_Process(Get_pCarrierUartTxBuf());
    }
}


void USART2_IRQHandler(void)
{
    if(USART2Com->ISR & (1<<3))
        USART2Com->ICR |= (1<<3);
    
    if(USART2Com_RDRXNE())
    {//得到读取后的结果，给发送DATA类型的读取帧用
        Get_pSystemFunc()->Com2RxData(USART2Com->RDR, Get_pDevUartRxBuf());
    }
    if(USART2Com_RDTC() && USART2Com_RDTXINTEN())
    {
        USART2Com->TDR = DevUart_TX_INT_Process(Get_pDevUartTxBuf());
    }
}




void DMA1_Channel1_IRQHandler(void)
{
    
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
