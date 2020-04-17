#ifndef __RF_INTERFACE_H
#define __RF_INTERFACE_H

#include <stdint.h>
#include "protocol_data_def_struct.h"



#define DEFAULT_CHANNEL						10


/*******************  si4438 device status  **********************/
typedef enum
{
  Wireless_Free = 0,
  Wireless_RX_Receiving,
  Wireless_RX_Sync,
  Wireless_RX_Finish,
  Wireless_RX_Failure,
  Wireless_TX_Sending,
  Wireless_TX_Finish,
}Wireless_Status_TypDef;

/***********************  error code ***************************/
typedef enum
{
  Wireless_NoError = 0,
  Wireless_Busy,
  Wireless_ExecuteError
}Wireless_ErrorType;

/*****************  si4438 data receive  ********************/
typedef struct
{
  uint8_t Wireless_RSSI_Value;
  uint8_t Wireless_FrameHeader[4];
  
  uint8_t Wireless_RxData[APP_BUFF_MAX_LEN];
  uint8_t Wireless_TxData[APP_BUFF_MAX_LEN]; 
  
  uint8_t Sent_TimeOut_Cnt;
  uint8_t WirelessRx_Timeout_Cnt;
}WLS;





extern volatile Wireless_Status_TypDef WIRELESS_STATUS;




uint8_t Get_RF_Channel(void);
WLS *Get_pWirelessBuf(void);
void Set_SI4438_ReInit_Flag(uint8_t val);
void Clr_SI4438_ReInit_Flag(void);
uint8_t Get_SI4438_ReInit_val(void);

void Radio_Initfailed(void);
void Set_WireLessChannel(uint8_t* wire_chnel);
void SI4438_Chip_Init(void);

Wireless_ErrorType Si4438_Receive_Start(uint8_t channel_number);
Wireless_ErrorType Si4438_Transmit_Start(WLS *pWL, uint8_t channel_number, uint8_t *addr, uint8_t length);
Wireless_ErrorType Si4438_Transmit(WLS *pWL, uint8_t channel_number, uint8_t *addr, uint8_t length);

void WireLess_Process(void *p_srcdata);
void AppFrameData_TypeConfirm(uint8_t *p_data);

#endif

