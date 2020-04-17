#include <string.h>

#include "frame_process.h"
#include "rf_interface.h"
#include "uart_interface.h"
#include "net_layer_api.h"
#include "device_basic_info.h"
#include "stm32f0_iwdg.h"
#include "stm32f0_gpio.h"
#include "stm32f0_exint.h"
#include "m0_systick.h"
#include "mcu.h"
#include "net_route_info.h"






void main(void)
{
    SysAll_Init();
    
    LEDG_ON();
    SysDelay_Xms(3000);
    LEDG_OFF();
    
    Switch_Init();
    Touch_Init();
    
    Gap_Tick(5);
    
    Exercise_Alarm();
    
    while(1);
    
    
}






