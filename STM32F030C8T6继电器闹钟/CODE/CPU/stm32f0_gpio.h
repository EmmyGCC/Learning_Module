#ifndef STM32F0_GPIO_H
#define STM32F0_GPIO_H


#include <stdint.h>

/************************* LED宏定义 ***************************/
#define LEDR_GPIO_CLOCK_EN        GPIOA_CLOCK_EN

#define LEDR_GPIOX                GPIOA       
#define LEDR_PIN                  0
#define LEDR_PIN_MODE             ANALOG_MODE//OUTPUT_MODE
#define LEDR_PIN_TYPE             PUSH_PULL
#define LEDR_PIN_SPEED            LOW_SPEED
#define LEDR_PIN_PULL             NO_PULL

#define LEDR_ON()               (LEDR_GPIOX->BRR = (1<<LEDR_PIN))
#define LEDR_OFF()                (LEDR_GPIOX->BSRR = (1<<LEDR_PIN))
#define LEDR_INVERSE()           (LEDR_GPIOX->ODR ^= (1<<LEDR_PIN))




#define LEDG_GPIO_CLOCK_EN        GPIOA_CLOCK_EN

#define LEDG_GPIOX                GPIOA       
#define LEDG_PIN                  1
#define LEDG_PIN_MODE             OUTPUT_MODE
#define LEDG_PIN_TYPE             PUSH_PULL
#define LEDG_PIN_SPEED            LOW_SPEED
#define LEDG_PIN_PULL             NO_PULL

#define LEDG_OFF()                 (LEDG_GPIOX->BRR = (1<<LEDG_PIN))
#define LEDG_ON()                (LEDG_GPIOX->BSRR = (1<<LEDG_PIN))
#define LEDG_INVERSE()            (LEDG_GPIOX->ODR ^= (1<<LEDG_PIN))





/************************* 无线SDN宏定义 ***************************/
#define SDN_GPIO_CLOCK_EN        GPIOB_CLOCK_EN

#define SDN_GPIOX                GPIOB       
#define SDN_PIN                  7
#define SDN_PIN_MODE             OUTPUT_MODE
#define SDN_PIN_TYPE             PUSH_PULL
#define SDN_PIN_SPEED            LOW_SPEED
#define SDN_PIN_PULL             NO_PULL

#define SDN_H() 	              (SDN_GPIOX->BSRR = 1 << SDN_PIN)
#define SDN_L()		              (SDN_GPIOX->BRR = 1 << SDN_PIN)


#define WireLess_PWOFF()        (SDN_H())
#define WireLess_PWON()         (SDN_L())



/************************* 无线SI4438_PWR宏定义 ***************************/
#define PWR_GPIO_CLOCK_EN        GPIOC_CLOCK_EN

#define PWR_GPIOX                GPIOC
#define PWR_PIN                  14
#define PWR_PIN_MODE             OUTPUT_MODE
#define PWR_PIN_TYPE             PUSH_PULL
#define PWR_PIN_SPEED            LOW_SPEED
#define PWR_PIN_PULL             NO_PULL

#define SI4438_PWR_OFF()        (PWR_GPIOX->BSRR = 1 << PWR_PIN)
#define SI4438_PWR_ON()         (PWR_GPIOX->BRR = 1 << PWR_PIN)



/************************* 无线TXRX_Enable宏定义 ***************************/
#define WL_TX_GPIO_CLOCK_EN        GPIOB_CLOCK_EN

#define WL_TX_GPIOX                GPIOB       
#define WL_TX_PIN                  8
#define WL_TX_PIN_MODE             OUTPUT_MODE
#define WL_TX_PIN_TYPE             PUSH_PULL
#define WL_TX_PIN_SPEED            HIGH_SPEED
#define WL_TX_PIN_PULL             NO_PULL

#define WL_TX_EN() 	              (WL_TX_GPIOX->BSRR = 1 << WL_TX_PIN)
#define WL_TX_DIS()				  (WL_TX_GPIOX->BRR = 1 << WL_TX_PIN)


/*---------------------------------------------------------------*/
#define WL_RX_GPIO_CLOCK_EN        GPIOB_CLOCK_EN

#define WL_RX_GPIOX                GPIOB       
#define WL_RX_PIN                  9
#define WL_RX_PIN_MODE             OUTPUT_MODE
#define WL_RX_PIN_TYPE             PUSH_PULL
#define WL_RX_PIN_SPEED            LOW_SPEED
#define WL_RX_PIN_PULL             NO_PULL

#define WL_RX_EN() 	              (WL_RX_GPIOX->BSRR = 1 << WL_RX_PIN)
#define WL_RX_DIS()				  (WL_RX_GPIOX->BRR = 1 << WL_RX_PIN)



/************************* Switch宏定义 ***************************/
#define Switch1_GPIO_CLOCK_EN        GPIOB_CLOCK_EN

#define Switch1_GPIOX                GPIOB       
#define Switch1_PIN                  15
#define Switch1_PIN_MODE             OUTPUT_MODE
#define Switch1_PIN_TYPE             PUSH_PULL
#define Switch1_PIN_SPEED            LOW_SPEED
#define Switch1_PIN_PULL             NO_PULL

#define Switch1_ON()                 (Switch1_GPIOX->BSRR = (1<<Switch1_PIN))
#define Switch1_OFF()                (Switch1_GPIOX->BRR = (1<<Switch1_PIN))
#define Switch1_INVERSE()            (Switch1_GPIOX->ODR ^= (1<<Switch1_PIN))



#define Switch2_GPIO_CLOCK_EN        GPIOF_CLOCK_EN

#define Switch2_GPIOX                GPIOF       
#define Switch2_PIN                  6
#define Switch2_PIN_MODE             OUTPUT_MODE
#define Switch2_PIN_TYPE             PUSH_PULL
#define Switch2_PIN_SPEED            LOW_SPEED
#define Switch2_PIN_PULL             NO_PULL

#define Switch2_ON()                 (Switch2_GPIOX->BSRR = (1<<Switch2_PIN))
#define Switch2_OFF()                (Switch2_GPIOX->BRR = (1<<Switch2_PIN))
#define Switch2_INVERSE()            (Switch2_GPIOX->ODR ^= (1<<Switch2_PIN))



#define Switch3_GPIO_CLOCK_EN        GPIOB_CLOCK_EN

#define Switch3_GPIOX                GPIOB       
#define Switch3_PIN                  14
#define Switch3_PIN_MODE             OUTPUT_MODE
#define Switch3_PIN_TYPE             PUSH_PULL
#define Switch3_PIN_SPEED            LOW_SPEED
#define Switch3_PIN_PULL             NO_PULL

#define Switch3_ON()                 (Switch3_GPIOX->BSRR = (1<<Switch3_PIN))
#define Switch3_OFF()                (Switch3_GPIOX->BRR = (1<<Switch3_PIN))
#define Switch3_INVERSE()            (Switch3_GPIOX->ODR ^= (1<<Switch3_PIN))



/************************* Switch_Led宏定义 ***************************/

#define Switch2_Led_GPIO_CLOCK_EN        GPIOB_CLOCK_EN

#define Switch2_Led_GPIOX                GPIOB       
#define Switch2_Led_PIN                  2
#define Switch2_Led_PIN_MODE             OUTPUT_MODE
#define Switch2_Led_PIN_TYPE             PUSH_PULL
#define Switch2_Led_PIN_SPEED            LOW_SPEED
#define Switch2_Led_PIN_PULL             NO_PULL

#define Switch2_Led_ON()                 (Switch2_Led_GPIOX->BSRR = (1<<Switch2_Led_PIN))
#define Switch2_Led_OFF()                (Switch2_Led_GPIOX->BRR = (1<<Switch2_Led_PIN))
#define Switch2_Led_INVERSE()            (Switch2_Led_GPIOX->ODR ^= (1<<Switch2_Led_PIN))


#define Switch3_Led_GPIO_CLOCK_EN        GPIOB_CLOCK_EN

#define Switch3_Led_GPIOX                GPIOB       
#define Switch3_Led_PIN                  11
#define Switch3_Led_PIN_MODE             OUTPUT_MODE
#define Switch3_Led_PIN_TYPE             PUSH_PULL
#define Switch3_Led_PIN_SPEED            LOW_SPEED
#define Switch3_Led_PIN_PULL             NO_PULL

#define Switch3_Led_ON()                 (Switch3_Led_GPIOX->BSRR = (1<<Switch3_Led_PIN))
#define Switch3_Led_OFF()                (Switch3_Led_GPIOX->BRR = (1<<Switch3_Led_PIN))
#define Switch3_Led_INVERSE()            (Switch3_Led_GPIOX->ODR ^= (1<<Switch3_Led_PIN))



/********************** Touch1的外部接口宏定义 ***************************/
#define Touch1_GPIO_CLOCK_EN        GPIOA_CLOCK_EN       

#define Touch1_GPIOX                GPIOA       
#define Touch1_PIN                  7
#define Touch1_PIN_MODE             INPUT_MODE
//#define Touch1_PIN_TYPE             PUSH_PULL
//#define Touch1_PIN_SPEED            LOW_SPEED
#define Touch1_PIN_PULL             PULL_UP

#define Touch1_RDPIN()              (!!(Touch1_GPIOX->IDR & (1<<Touch1_PIN)))


/********************** Touch2的外部接口宏定义 ***************************/
#define Touch2_GPIO_CLOCK_EN        GPIOA_CLOCK_EN       

#define Touch2_GPIOX                GPIOA       
#define Touch2_PIN                  5
#define Touch2_PIN_MODE             INPUT_MODE
//#define Touch2_PIN_TYPE             PUSH_PULL
//#define Touch2_PIN_SPEED            LOW_SPEED
#define Touch2_PIN_PULL             PULL_UP

#define Touch2_RDPIN()              (!!(Touch2_GPIOX->IDR & (1<<Touch2_PIN)))


/********************** Touch3的外部接口宏定义 ***************************/
#define Touch3_GPIO_CLOCK_EN        GPIOC_CLOCK_EN       

#define Touch3_GPIOX                GPIOC       
#define Touch3_PIN                  13
#define Touch3_PIN_MODE             INPUT_MODE
//#define Touch3_PIN_TYPE             PUSH_PULL
//#define Touch3_PIN_SPEED            LOW_SPEED
#define Touch3_PIN_PULL             PULL_UP

#define Touch3_RDPIN()              (!!(Touch3_GPIOX->IDR & (1<<Touch3_PIN)))



/************************* 外部函数 ***************************/
uint8_t Get_LedIndicate_Cnt(void);
void Set_LedIndicate_Cnt(uint8_t cnt_val);

void Led_Init(void);
void SI4438_PWRInit(void);
void SDN_Init(void);
void Switch_Init(void);
void Switch_Led_Init(void);
void Touch_Init(void);
void WL_TXRX_Init(void);


#endif