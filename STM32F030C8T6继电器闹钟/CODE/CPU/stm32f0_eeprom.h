#ifndef STM32F0_EEPROM_H
#define STM32F0_EEPROM_H

#include "stm32f0xx.h"



#define STM32_FLASH_SIZE 	64 	 			
             		
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 			//STM32 FLASH����ʼ��ַ


#define Eeprom_StartAddr   (STM32_FLASH_BASE + 0xc000)    //48K


#define MAC_INFO_SAVE_ADDR      0
#define ROUT_INFO_SAVE_ADDR     1024            //��2K����ʼ����·�ɱ�Ĵ洢
#define INIT_UNIQUE_ID_ADDR     12288           //�洢������Ҫ����Կ
#define Device_State_Addr1      13312           //FLASH��������������豸��ǰ״̬
#define Device_State_Addr2      14336
#define Device_State_Addr3      15360

//FLASH������ֵ
#define FLASH_KEY1               0X45670123
#define FLASH_KEY2               0XCDEF89AB


#define PROGRAMMING_EN()           (FLASH->CR |= 1<<0)   //���ʹ��
#define PROGRAMMING_DIS()          (FLASH->CR &= ~(1<<0))

#define PAGE_ERASE_EN()            (FLASH->CR |= 1<<1)    //ҳ����
#define PAGE_ERASE_DIS()           (FLASH->CR &= ~(1<<1))

#define PAGE_ERASE_START()         (FLASH->CR |= 1<<6)


#define Flash_Unlock()             {FLASH->KEYR = FLASH_KEY1; FLASH->KEYR = FLASH_KEY2;}
#define Flash_Lock()               (FLASH->CR |= 1<<7)

#define Flash_Busy()               (FLASH->SR & 1<<0)



/************************* �ⲿ���� ***************************/

void Eeprom_ErasePage(uint32_t page_addr);
void Eeprom_Write(uint32_t start_addr,uint16_t *p_buf,uint16_t len);
void Eeprom_Read(uint32_t start_addr,uint16_t *p_buf,uint16_t len);



#endif


