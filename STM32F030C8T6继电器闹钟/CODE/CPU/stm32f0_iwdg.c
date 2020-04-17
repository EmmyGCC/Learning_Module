#include "stm32f0_iwdg.h"


void IWDG_Init(void)
{
	IWDG->KR = IWDG_START;		          
	IWDG->KR = IWDG_WRITE_ACCESS;		    
	IWDG->PR = IWDG_PR4;		            
	IWDG->RLR = IWDG_RELOAD_VAL;		   
  while(IWDG_RDSR());                 
	IWDG_FEED();		                    	
}

