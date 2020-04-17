/*! @file radio.h
 * @brief This file is contains the public radio interface functions.
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2012 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */

#ifndef RADIO_H_
#define RADIO_H_


#include "data_base.h"
#include "si446x_cmd.h"

/*****************************************************************************
 *  Global Macros & Definitions
 *****************************************************************************/



#define TX_THRESHOLD                        0x30
#define RX_THRESHOLD                        0x30

#define WirelessRx_Timeout_Threshold              250
#define BUSY_TIMEOUT                              25    //200ms
#define WLSent_TimeOut_Val						  62
#define Two_Min_TxTime							  15000	//2分钟没有收到东西发送一些内容

#define RADIO_COMM_ERROR_CALLBACK

/*****************************************************************************
 *  Global Typedefs & Enums
 *****************************************************************************/
typedef struct
{
  U8   *Radio_ConfigurationArray;

  U8   Radio_ChannelNumber;
  U8   Radio_PacketLength;
  U8   Radio_State_After_Power_Up;

  U16  Radio_Delay_Cnt_After_Reset;
} tRadioConfiguration;




/*****************************************************************************
 *  Global Variable Declarations
 *****************************************************************************/
extern tRadioConfiguration* pRadioConfiguration;
extern tRadioConfiguration RadioConfiguration;

/*! Si446x configuration array */
extern U8 Radio_Configuration_Data_Array[];
extern uint8_t const Channel_Frequency_Index[66];

/*****************************************************************************
 *  Global Function Declarations
 *****************************************************************************/

void  vRadio_Init(void);
void  vRadio_StartRX(U8,U8);
void  vRadio_StartTx_Variable_Packet(U8, U8*, U8);
void Si4438_Interrupt_Handler(U8 *p_data);


void TRANSMIT_TIMES_INIT(void);
void RECEIVE_TIMES_INIT(void);
void RECEIVE_REMAIN_INIT(void);

#endif /* RADIO_H_ */
