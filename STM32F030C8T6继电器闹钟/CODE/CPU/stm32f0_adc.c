#include "stm32f0xx.h"
#include "stm32f0_adc.h"


//uint8_t      aResultDMA[680];

void BatteryADC_Init(void)        
{ 
  //GPIO��ʼ��
  RCC->GPIO_CLKREG |= BatteryADC_GPIO_CLOCK_EN;
  BatteryADC_GPIOX->MODER &= ~(3 << (BatteryADC_PIN*2));
  BatteryADC_GPIOX->MODER |= BatteryADC_PIN_MODE << (BatteryADC_PIN*2);

  BatteryADC_GPIOX->PUPDR &= ~(3 << (BatteryADC_PIN*2));
  BatteryADC_GPIOX->PUPDR |= BatteryADC_PIN_PULL << (BatteryADC_PIN*2);
  
  //Charge GPIO
  /*
  RCC->GPIO_CLKREG |= ChargeADC_GPIO_CLOCK_EN;
  ChargeADC_GPIOX->MODER &= ~(3 << (ChargeADC_PIN*2));
  ChargeADC_GPIOX->MODER |= ChargeADC_PIN_MODE << (ChargeADC_PIN*2);

  ChargeADC_GPIOX->PUPDR &= ~(3 << (ChargeADC_PIN*2));
  ChargeADC_GPIOX->PUPDR |= ChargeADC_PIN_PULL << (ChargeADC_PIN*2);
  */
  
  //ADC��ʼ��(����AD���������ͬһ��ADC��ֻ���ϲ�ͬ��GPIO��ʼ�����ɣ������ͻ)
  RCC->BatteryADC_CLKREG |= BatteryADC_CLOCK_EN;
  BatteryADC->CFGR2 = PCLK_DIV4;            //ADCʱ�� 48/4 = 12M
  
  BatteryADC->CFGR1 = 0;
  //BatteryADC->CFGR1 |= DMA_CIRCULAR_EN;    //DMAѭ��ģʽʹ��
  BatteryADC->CFGR1 |= DMA_CIRCULAR_DIS;
  BatteryADC->CFGR1 |= RESOLUTION_12B;      //ADC�ֱ���
  BatteryADC->CFGR1 |= RIGHT_ALIGN;        //���뷽ʽ
  BatteryADC->CFGR1 |= SINGLE_MODE;      //ת��ģʽ
  //BatteryADC->CFGR1 |= DMA_EN;             //�Ƿ�ʹ��DMA
  BatteryADC->CFGR1 |= DMA_DIS;
  //BatteryADC->CHSELR = ADC_CHANNEL;        //ת����ͨ��
  
  //BatteryADC->SMPR = SAMP_13CLOCK5;
  BatteryADC->SMPR = SAMP_41CLOCK5;       //41.5+12.5 = 54/12M = 4.5uS����һ��
  BatteryADC_EN();                        //ADCʹ��
}




uint16_t BatteryADC_Single_Convert(uint32_t ADC_Channel)
{
	uint16_t Conver_Value = 0;
        
	BatteryADC->CHSELR = ADC_Channel;			                  
  BatteryADC_START();                     
  while(!BatteryADC_EOC_RD());
  BatteryADC_EOC_CLS();
	//Conver_Value = BatteryADC->DRL;	        
	//Conver_Value |= BatteryADC->DRH  << 8;	
  Conver_Value = BatteryADC->DR & 0X0FFF;	
  return Conver_Value;
	//return (Conver_Value & 0x03ff);   
}



uint16_t Get_BatteryADC_AverValue(uint32_t ADC_Channel)
{
  uint8_t loop;
  uint16_t temp_val, max_val = 0, min_val = 0xffff,  sum_val = 0, average_val;
  
  BatteryADC->CHSELR = ADC_Channel;	         
  BatteryADC_START();                     
  for(loop = 0; loop < 10; loop++)	
	{
    //temp_val = 0;
    while(!BatteryADC_EOC_RD());
    BatteryADC_EOC_CLS();
    temp_val = BatteryADC->DR & 0X0FFF;	         	  
    if(loop < 9) BatteryADC_START();
		//temp_val = temp_val & 0x03ff;   
    sum_val += temp_val;
    max_val = max_val > temp_val ? max_val : temp_val;
    min_val = min_val < temp_val ? min_val : temp_val;  
	}
  average_val = (sum_val - max_val - min_val) >> 3;
  return average_val;
}








void ADC_DMA_Init(uint8_t *me_addr, uint16_t len) 
{
  NVIC_InitTypeDef  NVIC_TEMP;
  RCC->ADC_DMA_CLKREG |= ADC_DMA_CLOCK_EN;
  
  ADC_DMACHANNEL->CNDTR = len;             
  ADC_DMACHANNEL->CPAR = ADC_DMA_PER_ADDR;
  ADC_DMACHANNEL->CMAR = (uint32_t)me_addr;
  
  ADC_DMACHANNEL->CCR = 0;
  ADC_DMACHANNEL->CCR |= PERIPH_TO_MEMORY; //���˷���
  ADC_DMACHANNEL->CCR |= PERINC_DIS;       //��ַ�Ƿ�����
  ADC_DMACHANNEL->CCR |= MEMINC_EN;  
  ADC_DMACHANNEL->CCR |= PERSIZE_8B;       //�ֽڳ���
  ADC_DMACHANNEL->CCR |= MEMSIZE_8B;
  ADC_DMACHANNEL->CCR |= CIRCULAR_EN;      //�Ƿ�ѭ��
  ADC_DMACHANNEL->CCR |= PRIORITY_LOW;     //ͨ�������ȼ�
  //ADC_DMACHANNEL->CCR |= HALF_INT_DIS;      //�����жϽ�ֹ
  ADC_DMACHANNEL->CCR |= HALF_INT_DIS;
  ADC_DMACHANNEL->CCR |= COMP_INT_EN;       //����ж�ʹ��
  
  NVIC_TEMP.NVIC_IRQChannel = ADC_DMA_IRQ;		            //Specifies the IRQ channel
	NVIC_TEMP.NVIC_IRQChannelPriority = ADC_DMA_PRIORITY;		//Specifies the priority
	NVIC_TEMP.NVIC_IRQChannelCmd = ENABLE;			            //NVIC_IRQChannel will be enabled
	NVIC_Init(&NVIC_TEMP);
   
  ADC_DMA_EN();                         //DMAʹ��
}