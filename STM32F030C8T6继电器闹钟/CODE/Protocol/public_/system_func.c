#include <string.h>

#include "system_func.h"
#include "stm32f0_eeprom.h"
#include "delay.h"
#include "rf_interface.h"
#include "uart_interface.h"
#include "hk_device.h"
#include "frame_process.h"
#include "device_basic_info.h"
#include "m0_systick.h"

//放到每个工程中测试********/////////////***************/////////////******************//////******************
/************** **********/
/////////
/*********
最后改成作为函数参数传入来初始化 
************/
void FLASH_Write(uint32_t WriteAddr,void *pBuffer,uint16_t NumToWrite)
{
    Eeprom_Write(WriteAddr, (uint16_t *)pBuffer, NumToWrite/2);
}

void FLASH_Read(uint32_t ReadAddr,void *pBuffer,uint16_t NumToRead)
{
    Eeprom_Read(ReadAddr, (uint16_t *)pBuffer, NumToRead/2);
}

void Com1_DataTx(void *usart_buf, void *p_buf, uint16_t len)
{
    CarrierUart_DataTx((Uart1_TX_TypDef *)usart_buf, (uint8_t *)p_buf, (uint8_t)len);
}

void Com2_DataTx(void *usart_buf, void *p_buf, uint16_t len)
{
    DevUart_DataTx((Uart2_TX_TypDef *)usart_buf, (uint8_t *)p_buf, (uint8_t)len);
}


static SYSTEM_FUNC_T SystemFunc = 
{
    .FLASH_Erase = Eeprom_ErasePage,
    .FLASH_Write = FLASH_Write,
    .FLASH_Read = FLASH_Read,
    
    .TICK_Process = System_8msTick_Process,
    
    .Rf_RxProcess = WireLess_Process,
    .Carrier_RxProcess = CarrierCom_Process,
    .Mac_RxProcess = MacCom_Process,
    
    .DevData_UpReport = UpReport_Start,
    
    .Com1RxData = CarrierUart_RX_INT_Process,
    .Com2RxData = DevUart_RX_INT_Process,
    .Com1TxData = Com1_DataTx,
    .Com2TxData = Com2_DataTx,
    
    .DevAction_Process = Device_Process,
    .DevStateInit = Read_Device_State,
    
    
};


SYSTEM_FUNC_T *Get_pSystemFunc(void)
{
    return &SystemFunc;
}



