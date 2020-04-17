#include "stm32f0xx.h"
#include "stm32f0_eeprom.h"



void Eeprom_ErasePage(uint32_t page_addr)
{
  Flash_Unlock();
  FLASH->SR = 0;
  while(Flash_Busy());
  PAGE_ERASE_EN();
  FLASH->AR = page_addr + Eeprom_StartAddr;
  PAGE_ERASE_START();
  while(Flash_Busy());
  PAGE_ERASE_DIS();
  Flash_Lock();
}
  
  
void Eeprom_WriteHalfWord(uint32_t addr, uint16_t dat)
{
  addr += Eeprom_StartAddr;
  while(Flash_Busy());
  PROGRAMMING_EN();
  *(volatile uint16_t *)addr = dat;
  while(Flash_Busy());
  PROGRAMMING_DIS();
} 


uint16_t Eeprom_ReadHalfWord(uint32_t addr)
{
  addr += Eeprom_StartAddr;
  return *(volatile uint16_t *)addr;
}



void Eeprom_Write(uint32_t start_addr,uint16_t *p_buf,uint16_t len)   
{ 			 		 
	uint16_t i;
  
  Flash_Unlock();
  FLASH->SR = 0;
	for(i = 0;i < len; i++)
	{
		Eeprom_WriteHalfWord(start_addr, p_buf[i]);
	  start_addr += 2;
	}
  Flash_Lock();
} 


void Eeprom_Read(uint32_t start_addr,uint16_t *p_buf,uint16_t len)   
{ 			 		 
	uint16_t i;
  
	for(i = 0;i < len; i++)
	{
    p_buf[i] = Eeprom_ReadHalfWord(start_addr);
	  start_addr += 2;
	}  
}





