#include "m0_systick.h"
#include "stm32f0_peripheral.h"
#include "rf_interface.h"
#include "hk_device.h"
#include "frame_process.h"
#include "stm32f0_gpio.h"
#include "radio.h"
#include "uart_interface.h"
#include "stm32f0_exint.h"
#include "dev_mate_interface.h"



static uint16_t Not_RX_Cnt;
volatile uint32_t SYSTICK_CNT;                
volatile uint8_t TICK_8ms;
volatile time_t time_dat;




void Set_Not_RX_Cnt(void)
{
    Not_RX_Cnt = 1;
}

uint16_t Get_Not_RX_Cnt(void)
{
    return Not_RX_Cnt;
}

void Clr_Not_RX_Cnt(void)
{
    Not_RX_Cnt = 0;
}

uint32_t Get_SYSTICK_CNT(void)
{
    return SYSTICK_CNT;
}

void Set_SYSTICK_CNT(uint32_t cnt_val)
{
    SYSTICK_CNT = cnt_val;
}


void SysTick_Init(void)
{
    NVIC_InitTypeDef  NVIC_TEMP;
    
    SysTick->LOAD = SYSTICK_LOADVAL;            
    SysTick->CTRL |=  SYSTICK_SOURCE_SYS;       
    SYSTICK_CLSCNT();                           
    
    NVIC_TEMP.NVIC_IRQChannel = SYSTICK_CHANNEL;		
	NVIC_TEMP.NVIC_IRQChannelPriority = SYSTICK_PRIORITY;		
	NVIC_TEMP.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_TEMP);
    SYSTICK_INTEN();                            
    
    SYSTICK_EN();                                
}

void SysDelay_Xms(uint32_t delay_val)
{
    uint32_t start_tickcnt = SYSTICK_CNT;
    while(SYSTICK_CNT - start_tickcnt < delay_val);
}




void SysTick_Handle(void)
{
    SYSTICK_CNT++;
    
    if (Not_RX_Cnt)
    {
        Not_RX_Cnt++;
    }
    if((SYSTICK_CNT & 0x07) == 0)
    {
        TICK_8ms = 1;
    }
}





static void WirelessStatusLed_Flash(void)
{
    if (!Get_DeviceState_MateChange_Flag(LOW_4BIT | HIGH_4BIT))
    {
        if (Get_LedIndicate_Cnt())
        {
            Set_LedIndicate_Cnt(Get_LedIndicate_Cnt() + 1);
            if ((Get_LedIndicate_Cnt() & 0x05) == 0x01)
            {
                LEDG_INVERSE();
            }
            if (Get_LedIndicate_Cnt() > (500/8))
            {
                Set_LedIndicate_Cnt(0);
            }
        }
        else
        {
            if (Get_pHKFrame_Buf()->OnLine_Staus == 1)
            {
                LEDG_ON();
                if (++(Get_pHKFrame_Buf()->OnLine_Cnt) > 60000/8) //1分钟后灭上线状态灯
                {
                    Get_pHKFrame_Buf()->OnLine_Cnt = 0;
                    Get_pHKFrame_Buf()->OnLine_Staus = 0;
                }
            }
            else
                LEDG_OFF();
        }
    }
}

void System_8msTick_Process(void)
{
    static uint16_t reInit_val = 0;
    reInit_val = Get_SI4438_ReInit_val();
    
    if((WIRELESS_STATUS == Wireless_RX_Receiving) && (NIRQ_RDPIN() == 0))
    {
        if(Get_Not_RX_Cnt() > 375)
        {
            Clr_Not_RX_Cnt();
            SI4438_Chip_Init();
            Si4438_Receive_Start(DEFAULT_CHANNEL);				//Restart Receive
        }
    }
#if 1
    else if(WIRELESS_STATUS != Wireless_RX_Receiving)
    {
        if(Get_Not_RX_Cnt() > 625)
        {
            Clr_Not_RX_Cnt();
            Si4438_Receive_Start(DEFAULT_CHANNEL);			//Restart Receive
        }
    }
#endif
    if (reInit_val)
    {
        Set_SI4438_ReInit_Flag(++reInit_val);
        
        if (reInit_val > 375)   //RF模块断电3S重新初始化
        {
            Clr_SI4438_ReInit_Flag();
            SI4438_PWR_ON();
            
            SysDelay_Xms(50);
            SI4438_Chip_Init();
            Si4438_Receive_Start(DEFAULT_CHANNEL);				//Restart Receive
        }
    }
    if(Get_pWirelessBuf()->WirelessRx_Timeout_Cnt)  
    {
        if(++Get_pWirelessBuf()->WirelessRx_Timeout_Cnt > WirelessRx_Timeout_Threshold)
        {
            Get_pWirelessBuf()->WirelessRx_Timeout_Cnt = 0;
            WIRELESS_STATUS = Wireless_Free;
            Si4438_Receive_Start(DEFAULT_CHANNEL);  
        }
    }
    if(Get_pWirelessBuf()->Sent_TimeOut_Cnt > 0)
    {
        if(++Get_pWirelessBuf()->Sent_TimeOut_Cnt > WLSent_TimeOut_Val)
        {
            Get_pWirelessBuf()->Sent_TimeOut_Cnt = 0;
            WIRELESS_STATUS = Wireless_TX_Finish;
        }
    }
    if(Get_pDevUartRxBuf()->Over_time_count)
    {
        if(++Get_pDevUartRxBuf()->Over_time_count > Up_TimeOut_Val)
        {
            Get_pDevUartRxBuf()->Over_time_count = 0;
            Get_pDevUartRxBuf()->Rx_Status = UartRx_FrameHead;
        }
    }
    if(Get_pCarrierUartRxBuf()->Over_time_count)
    {
        if(++Get_pCarrierUartRxBuf()->Over_time_count > Up_TimeOut_Val)
        {
              Get_pCarrierUartRxBuf()->Over_time_count = 0;
              Get_pCarrierUartRxBuf()->Rx_Status = CarrierRx_Head;
        }
    }
    
    Set_ChangeDis_Cnt(Get_pDeviceState_Buf());
    
    WirelessStatusLed_Flash();
    Mate_LED_Process();
    Retransmission_Process(Get_pHKFrame_Buf());   //UpReport Retry Function
    
    //////////////////////////////////////////
}




