#ifndef __UART_INTERFACE_H
#define __UART_INTERFACE_H


#include <stdint.h>
#include "protocol_data_def_struct.h"
#include "stm32f0_usart.h"



#define Tx_GapTime_Size             6 

////////////////////////////////协议帧的宏定义/////////////////////////////////////////////////

#define Up_TimeOut_Val                              30     //250Byte约62.5ms





typedef enum
{
    Uart_NoError = 0,
    Uart_TxBusy,
    Uart_ParaError,
}Uart_ErrorType;


typedef enum
{
    UartRx_FrameHead = 0,      
    UartRx_FrameAddr,         
    UartRx_FrameSeq,          
    UartRx_DataCmd,           
    UartRx_Datalen,           
    UartRx_Data,              
    UartRx_FrameCs,           
    UartRx_FrameEnd,          
    
    UartRx_AesCmd,           
    UartRx_Aeslen,           
    UartRx_AesData,             
    UartRx_AesFrameCs1,          
    UartRx_AesFrameCs2,          
    
    UartRx_Finished,          //接收完成
}UART_RxSta_TypDef;

typedef enum
{
    CarrierRx_Head = 0,
    CarrierRx_Addr,
    CarrierRx_Start,
    CarrierRx_Cmd,
    CarrierRx_Len,
    CarrierRx_Data,
    CarrierRx_Checksum,
    CarrierRx_End,

    CarrierRx_MacCmd,
    CarrierRx_MacData,
    
    CarrierRx_MacRequest,
    CarrierRx_Finished,          //接收完成
}CARRIER_RxSta_TypDef;


typedef enum
{
    UartTx_Finished = 0,  //发送完成
    UartTx_Start,         //启动发送
    UartTx_End,         //发送结束
}Uart_TxSta_TypDef;


/******************************Carrier Uart****************************/
typedef struct
{
    Uart_TxSta_TypDef Tx_Status;
    uint8_t Frame_Len;
    uint8_t Byte_Cnt;
    uint8_t Frame_Data[CARRIERDATA_LEN];
}Uart1_TX_TypDef;

typedef struct
{
    volatile CARRIER_RxSta_TypDef Rx_Status;
    uint8_t FrameTotalLen;
    uint8_t Frame_Data[CARRIERDATA_LEN];
    volatile uint8_t Over_time_count;
}Uart1_RX_TypDef;


/******************************Dev Uart****************************/
typedef struct
{
    Uart_TxSta_TypDef Tx_Status;
    uint8_t Frame_Len;
    uint8_t Byte_Cnt;
    uint8_t Frame_Data[MAC_Data_Len*3]; 
}Uart2_TX_TypDef;

typedef struct
{
    volatile UART_RxSta_TypDef Rx_Status;
    
    uint8_t FrameTotalLen;
    uint8_t Frame_Data[MAC_Data_Len*3];
    volatile uint8_t Over_time_count;
}Uart2_RX_TypDef;








Uart1_RX_TypDef *Get_pCarrierUartRxBuf(void);
Uart1_TX_TypDef *Get_pCarrierUartTxBuf(void);
Uart2_RX_TypDef *Get_pDevUartRxBuf(void);
Uart2_TX_TypDef *Get_pDevUartTxBuf(void);



void MacCom_Process(void *prx_ubuf);

void DevUart_DataTx(Uart2_TX_TypDef *pu_buf, uint8_t *p_buf, uint8_t len);
uint8_t DevUart_TX_INT_Process(void *p_srcdata);
void DevUart_RX_INT_Process(uint8_t rx_data, void *p_srcdata);


void CarrierCom_Process(void *p_srcdata);
void CarrierUart_DataTx(Uart1_TX_TypDef *pu_buf, uint8_t *p_buf, uint8_t len);
uint8_t CarrierUart_TX_INT_Process(void *p_srcdata);
void CarrierUart_RX_INT_Process(uint8_t rx_data, void *p_srcdata);

void USART1Com_RX_En(void);
void CarrierUartSendBytes(USART_TypeDef *USARTx, uint8_t *buf, uint16_t len);



#endif

