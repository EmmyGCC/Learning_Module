#include <string.h>

#include "uart_interface.h"
#include "rf_interface.h"
#include "app_layer_api.h"
#include "net_layer_api.h"
#include "device_basic_info.h"
#include "carrier.h"
#include "hk_device.h"
#include "encrypt.h"
#include "crc16.h"
#include "m0_systick.h"
#include "stm32f0_gpio.h"



//给载波从节点用
static Uart1_RX_TypDef  Uart1RxBuf;
static Uart1_TX_TypDef  Uart1TxBuf;

//烧录MAC地址用
static Uart2_RX_TypDef  Uart2RxBuf;
static Uart2_TX_TypDef  Uart2TxBuf;


inline Uart1_RX_TypDef *Get_pCarrierUartRxBuf(void)
{
    return &Uart1RxBuf;
}

inline Uart1_TX_TypDef *Get_pCarrierUartTxBuf(void)
{
    return &Uart1TxBuf;
}

inline Uart2_RX_TypDef *Get_pDevUartRxBuf(void)
{
    return &Uart2RxBuf;
}

inline Uart2_TX_TypDef *Get_pDevUartTxBuf(void)
{
    return &Uart2TxBuf;
}









void MacCom_Process(void *p_srcdata)
{
    Uart1_RX_TypDef *prx_ubuf = (Uart1_RX_TypDef *)p_srcdata;
    uint8_t p_buf[1] = {MAC_FRAME_ERROE};
    
    if(prx_ubuf->Rx_Status == UartRx_Finished)
    {
        prx_ubuf->Rx_Status = UartRx_FrameHead; 
        if(0 == AppFrameData_FormatVerify(prx_ubuf->Frame_Data, prx_ubuf->FrameTotalLen))
        {
            if(prx_ubuf->Frame_Data[0] == AESFreamHeader)
                Set_DeviceState_Pending_Flag(SET_PendFlag, 0x01);
            else
                USART2Com_RXINT_EN();
        }
        else
        {
            USART2Com_RXINT_EN();   
        }
    }
    else if(Get_SYSTICK_CNT() > 1000)
    {
        Set_SYSTICK_CNT(0);
        DevUart_DataTx(Get_pDevUartTxBuf(), p_buf, 1);
    }
}



void DevUart_RX_INT_Process(uint8_t rx_data, void *p_srcdata)
{
    Uart2_RX_TypDef *pu_buf = (Uart2_RX_TypDef *)p_srcdata;
    switch(pu_buf->Rx_Status)  
    {
      case UartRx_FrameHead:              
        if(rx_data == AESFreamHeader)  
        {
            pu_buf->Frame_Data[0] = rx_data;
            pu_buf->Over_time_count = 1;
            pu_buf->Rx_Status = UartRx_AesCmd;
        }
        break;   
        
      case UartRx_AesCmd: 
        pu_buf->Frame_Data[1] = rx_data;
        pu_buf->Rx_Status = UartRx_Aeslen;
        break;
        
      case UartRx_Aeslen: 
        if(rx_data == 0)
        {
            pu_buf->Frame_Data[2] = 0;
            pu_buf->Rx_Status = UartRx_AesFrameCs1;
            pu_buf->FrameTotalLen = 3;
        }
        else
        {
            pu_buf->Frame_Data[2] = rx_data;
            pu_buf->Rx_Status = UartRx_AesData;
            pu_buf->FrameTotalLen = 3;
        }
        break;
        
      case UartRx_AesData:
        pu_buf->Frame_Data[pu_buf->FrameTotalLen] = rx_data;
        if(++pu_buf->FrameTotalLen >= (pu_buf->Frame_Data[2] + 3))
        {
            pu_buf->Rx_Status = UartRx_AesFrameCs1;
        }
        
        break;
        
      case UartRx_AesFrameCs1:
        pu_buf->Frame_Data[pu_buf->FrameTotalLen] = rx_data;
        pu_buf->FrameTotalLen++;
        pu_buf->Rx_Status = UartRx_AesFrameCs2;
        break;
        
      case UartRx_AesFrameCs2:
        pu_buf->Frame_Data[pu_buf->FrameTotalLen] = rx_data;
        pu_buf->FrameTotalLen++;  
        USART2Com_RXINT_DIS();
        pu_buf->Over_time_count = 0;
        pu_buf->Rx_Status = UartRx_Finished;
        break;
        
        
      default:
        break;
    }
}


void DevUart_DataTx(Uart2_TX_TypDef *pu_buf, uint8_t *p_buf, uint8_t len)
{
    if(pu_buf->Tx_Status != UartTx_Finished)
        return;
    pu_buf->Frame_Len = len;
    
    memmove(pu_buf->Frame_Data, p_buf, len);
    pu_buf->Byte_Cnt = 0;
    pu_buf->Tx_Status = UartTx_Start;
    USART2Com_TXINT_EN();
}


uint8_t DevUart_TX_INT_Process(void *p_srcdata)
{
    Uart2_TX_TypDef *pu_buf = (Uart2_TX_TypDef *)p_srcdata;
    uint8_t tx_data = 0;
    
    if(pu_buf->Tx_Status == UartTx_Start)
    {
        tx_data = pu_buf->Frame_Data[pu_buf->Byte_Cnt];
        if(++pu_buf->Byte_Cnt >= pu_buf->Frame_Len)
        {
            pu_buf->Tx_Status = UartTx_End;
            USART2Com_TXINT_DIS();
        }
    }
    
    return tx_data;
}


/*************************************************************************************************************************/
/*---------------------------------------------- 以下为载波串口接口函数 -------------------------------------------------*/
/*************************************************************************************************************************/


//载波串口收到数据
void CarrierCom_Process(void *p_srcdata)
{
    Uart1_RX_TypDef *prx_ubuf = (Uart1_RX_TypDef *)p_srcdata;
    
    if (prx_ubuf->Rx_Status == CarrierRx_Finished)
    {
        prx_ubuf->Rx_Status = CarrierRx_Head;
        
        //对接收到的应用层数据校验
        if (0 == AppFrameData_FormatVerify(&prx_ubuf->Frame_Data[Carrier_Data_Num],
                                    prx_ubuf->FrameTotalLen - Carrier_Data_Num - 2))
        {
            Set_LedIndicate_Cnt(1);
            memmove(Get_pNet_Buf()->WL_Data_Buf, prx_ubuf->Frame_Data + Carrier_Data_Num,
                    prx_ubuf->FrameTotalLen - Carrier_Data_Num - 2);
            Get_pNet_Buf()->Frame_Data_Len = prx_ubuf->FrameTotalLen - Carrier_Data_Num - 2;
            
            Get_pCarrier_Info()->IsData_from_carrier = true;
            AppFrameData_TypeConfirm(Get_NetBuf_pWL_Data_Buf());
        }
        else
            USART1Com_RX_En();
    }
    if (prx_ubuf->Rx_Status == CarrierRx_MacRequest)
    {
        prx_ubuf->Rx_Status = CarrierRx_Head;
        
        //设置从节点的设备地址
        Set_NodeAddr(&(Get_pCarrier_Info()->carrierSelfNode_Addr[0]));
        
        USART1Com_RX_En();
    }
}


void CarrierUart_RX_INT_Process(uint8_t rx_data, void *p_srcdata)
{
    Uart1_RX_TypDef *pu_buf = (Uart1_RX_TypDef *)p_srcdata;
    static uint8_t rec_len = 0;
    switch(pu_buf->Rx_Status)
    {
      case CarrierRx_Head:
        {
            if (rx_data == CARRIER_HEAD)
            {
                pu_buf->Frame_Data[Carrier_Head_Num] = rx_data;
                pu_buf->Over_time_count = 1;
                pu_buf->Rx_Status = CarrierRx_Addr;
                rec_len = Carrier_Addr_Num;
            }
            if (rx_data == 0xFE)
            {
                pu_buf->Frame_Data[0] = rx_data;
                rec_len = 1;
                pu_buf->Rx_Status = CarrierRx_MacCmd;
            }
        }
        break;
        
      case CarrierRx_Addr:
        {
            pu_buf->Frame_Data[rec_len++] = rx_data;
            if(rec_len >= Carrier_Start_Num)
            {
                pu_buf->Rx_Status = CarrierRx_Start;
            }
        }
        break;
        
      case CarrierRx_Start:
        {
            pu_buf->Frame_Data[Carrier_Start_Num] = rx_data;
            pu_buf->Rx_Status = CarrierRx_Cmd;
        }
        break;
        
      case CarrierRx_Cmd:
        {
            pu_buf->Frame_Data[Carrier_Cmd_Num] = rx_data;
            pu_buf->Rx_Status = CarrierRx_Len;
        }
        break;
        
      case CarrierRx_Len:
        {
            pu_buf->Frame_Data[Carrier_Len_Num] = rx_data;
            pu_buf->Rx_Status = CarrierRx_Data;
            rec_len = Carrier_Data_Num;
        }
        break;
        
      case CarrierRx_Data:
        {
            pu_buf->Frame_Data[rec_len++] = rx_data;
            if(rec_len >= (pu_buf->Frame_Data[Carrier_Len_Num] + Carrier_Data_Num))
            {
                pu_buf->Rx_Status = CarrierRx_Checksum;
            }
        }
        break;
        
      case CarrierRx_Checksum:
        {
            pu_buf->Frame_Data[rec_len++] = rx_data;
            pu_buf->Rx_Status = CarrierRx_End;
        }
        break;
        
      case CarrierRx_End:
        {
            pu_buf->Frame_Data[rec_len++] = rx_data;
            pu_buf->FrameTotalLen = rec_len;
            pu_buf->Over_time_count = 0;
            pu_buf->Rx_Status = CarrierRx_Finished;
            USART1Com_RXINT_DIS();
        }
        break;
        
      case CarrierRx_MacCmd:
        {
            pu_buf->Frame_Data[rec_len++] = rx_data;
            if (rec_len > 4)
                pu_buf->Rx_Status = CarrierRx_MacData;
        }
        break;
        
      case CarrierRx_MacData:
        if (rx_data == 0XAA)
        {
            pu_buf->Frame_Data[rec_len] = rx_data;
            rec_len++;
            pu_buf->Over_time_count = 0;
            pu_buf->Rx_Status = CarrierRx_MacRequest;
        }
        else
        {
            pu_buf->Over_time_count = 0;
            pu_buf->Rx_Status = CarrierRx_Head;
        }
        break;
        
      default:
        break;
    }
}


void CarrierUart_DataTx(Uart1_TX_TypDef *pu_buf, uint8_t *p_buf, uint8_t len)
{
    if(pu_buf->Tx_Status != UartTx_Finished) return;
    pu_buf->Frame_Len = len;
    
    memmove(pu_buf->Frame_Data, p_buf, len);
    pu_buf->Byte_Cnt = 0;
    pu_buf->Tx_Status = UartTx_Start;
    USART1Com_TXINT_EN();
}



uint8_t CarrierUart_TX_INT_Process(void *p_srcdata)
{
    Uart1_TX_TypDef *pu_buf = (Uart1_TX_TypDef *)p_srcdata;
    uint8_t tx_data = 0;
    
    if(pu_buf->Tx_Status == UartTx_Start)
    {
        tx_data = pu_buf->Frame_Data[pu_buf->Byte_Cnt];
        if(++pu_buf->Byte_Cnt >= pu_buf->Frame_Len)
        {
            pu_buf->Tx_Status = UartTx_Finished;
            USART1Com_TXINT_DIS();
        }
    }
    return tx_data;
}


void USART1Com_RX_En(void)
{
    Get_pCarrier_Info()->IsData_from_carrier = false;
    USART1Com_RXINT_EN();
}


static void CarrierUartSendData(USART_TypeDef *USARTx, uint8_t byte)
{
    while (USART1Com_RDTC() == 0)
        ;
    USARTx->TDR = byte;
}


void CarrierUartSendBytes(USART_TypeDef *USARTx, uint8_t *buf, uint16_t len)
{
    uint16_t i = 0;
	
    for (i = 0; i < len; i++)
    {
        CarrierUartSendData(USARTx, *buf++);
    }
}




