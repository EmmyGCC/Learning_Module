/*! @file radio.c
 * @brief This file contains functions to interface with the radio chip.
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2012 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */

#include "radio.h"
#include "data_base.h"
#include "radio_config.h"
#include "si446x_api_lib.h"
#include "radio_hal.h"
#include "rf_interface.h"
#include <time.h>
#include <stdlib.h>
#include "stm32f0_gpio.h"


/*****************************************************************************
 *  Local Macros & Definitions
 *****************************************************************************/
static uint8_t TRANSMIT_REMAIN = 0;
static uint8_t TRANSMIT_TIMES = 0;
static uint8_t RECEIVE_TIMES = 0;
static uint8_t RECEIVE_REMAIN = 0;

//通道转成物理频道的对应关系
const uint8_t Channel_Frequency_Index[66] = {24,66, 0,39, 1,40, 2,41, 3,42, 4,43, 5,44, 6,45, 7,46, 8,47, 9,48,10,49,11,50,12,51,13,52,14,53,15,
                                               54,16,55,17,56,18,57,19,58,20,59,21,60,22,61,23,62,25,63,26,64,27,65,28,67,29,68,30,69,31,70,32,71};

/*****************************************************************************
 *  Global Variables
 *****************************************************************************/

U8 Radio_Configuration_Data_Array[]=RADIO_CONFIGURATION_DATA_ARRAY;
tRadioConfiguration RadioConfiguration=RADIO_CONFIGURATION_DATA;

tRadioConfiguration* pRadioConfiguration=&RadioConfiguration;

/*****************************************************************************
 *  Local Function Declarations
 *****************************************************************************/
void TRANSMIT_TIMES_INIT(void)
{
    TRANSMIT_TIMES = 0;
}
void RECEIVE_TIMES_INIT(void)
{
    RECEIVE_TIMES = 0;
}
void RECEIVE_REMAIN_INIT(void)
{
    RECEIVE_REMAIN = 0;
}



void RADIO_COMM_ERROR_CALLBACK_(void)
{
    Set_SI4438_ReInit_Flag(1);
}


/*!
 *  Power up the Radio.
 *
 *  @note
 *
 */
void vRadio_PowerUp(void)
{
  U16 wDelay = 0;
  radio_hal_SetNsel();
  /* Hardware reset the chip */
  si446x_reset();

  /* Wait until reset timeout or Reset IT signal */
  for (; wDelay < pRadioConfiguration->Radio_Delay_Cnt_After_Reset; wDelay++);
}


/*!
 *  Radio Initialization.
 *
 *  @author Sz. Papp
 *
 *  @note
 *
 */
void vRadio_Init(void)
{
    Clr_SI4438_ReInit_Flag();
  /* Power Up the radio chip */
  vRadio_PowerUp();
  
  si446x_part_info();
  if(Si446xCmd.PART_INFO.PART!=0x4438)//part information check
  {
    while(1)
    {
      /* ERROR!!!!  SPI communicate error  or part information error */
      #ifdef RADIO_COMM_ERROR_CALLBACK
        RADIO_COMM_ERROR_CALLBACK_();
		break;
      #endif
    }
  }
  
  
  /* Load radio configuration */
  while (SI446X_SUCCESS != si446x_configuration_init(pRadioConfiguration->Radio_ConfigurationArray))
  {
	#ifdef RADIO_COMM_ERROR_CALLBACK
	RADIO_COMM_ERROR_CALLBACK_();
	break;
	#endif
  }
  
  // Read ITs, clear pending ones
  si446x_get_int_status(0u, 0u, 0u);
}

/*!
 *  Check if Packet sent IT flag or Packet Received IT is pending.
 *
 *  @return   SI4455_CMD_GET_INT_STATUS_REP_PACKET_SENT_PEND_BIT / SI4455_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT
 *
 *  @note
 *
 */
void Si4438_Interrupt_Handler(U8 *p_data)
{
    WLS *pWL = (WLS *)p_data;
    
  uint8_t Rx_fifo_cnt = 0;
  /* Read ITs, clear pending ones */
  si446x_get_int_status(0u, 0u, 0u);
  
  /*CRC ERROR*/
  if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PH_STATUS_CRC_ERROR_BIT)
  {
	WIRELESS_STATUS = Wireless_RX_Failure;
  }
  /*SYNC_DETECT*/
  if(Si446xCmd.GET_INT_STATUS.MODEM_PEND & SI446X_CMD_GET_INT_STATUS_REP_MODEM_PEND_SYNC_DETECT_PEND_BIT)
  {
	pWL->WirelessRx_Timeout_Cnt = 1;
  }
  /*RSSI_PEND
  if(Si446xCmd.GET_INT_STATUS.MODEM_PEND & SI446X_CMD_GET_INT_STATUS_REP_MODEM_PEND_RSSI_PEND_BIT)
  {
	WIRELESS_ERROR_CODE = Wireless_Busy;
  }*/
  /*TX_FIFO_ALMOST_EMPTY*/
  if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_TX_FIFO_ALMOST_EMPTY_PEND_BIT)
  {
	if(TRANSMIT_REMAIN > TX_THRESHOLD)
	{
	  /* Fill the TX fifo with datas */
	  si446x_write_tx_fifo(TX_THRESHOLD, &pWL->Wireless_TxData[60 + TX_THRESHOLD * TRANSMIT_TIMES]);
	  TRANSMIT_TIMES ++;
	  TRANSMIT_REMAIN -= TX_THRESHOLD;
	}
	else if(TRANSMIT_REMAIN > 0)
	{
	  si446x_write_tx_fifo(TRANSMIT_REMAIN, &pWL->Wireless_TxData[60 + TX_THRESHOLD * TRANSMIT_TIMES]);
	  TRANSMIT_REMAIN = 0;
	}
  }
  /*SENT_PEND*/
  if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_SENT_PEND_BIT)
  {
	TRANSMIT_REMAIN = 0;
	WIRELESS_STATUS = Wireless_TX_Finish;
	pWL->Sent_TimeOut_Cnt = 0;
  }
  
  /*RX_FIFO_ALMOST_FULL*/
  if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_RX_FIFO_ALMOST_FULL_PEND_BIT)
  {
	if(RECEIVE_TIMES == 0)
	{
	  si446x_read_rx_fifo(4, &pWL->Wireless_FrameHeader[0]);
	  
	  si446x_fifo_info(0x00);
	  Rx_fifo_cnt = Si446xCmd.FIFO_INFO.RX_FIFO_COUNT;
	  si446x_read_rx_fifo(Rx_fifo_cnt, &pWL->Wireless_RxData[0]);
	  
	  RECEIVE_REMAIN = Rx_fifo_cnt;
	  RECEIVE_TIMES = 1;
	}
	else
	{
	  si446x_fifo_info(0x00);
	  Rx_fifo_cnt = Si446xCmd.FIFO_INFO.RX_FIFO_COUNT;
	  si446x_read_rx_fifo(Rx_fifo_cnt, &pWL->Wireless_RxData[RECEIVE_REMAIN]);
	  
	  RECEIVE_REMAIN += Rx_fifo_cnt;
	  RECEIVE_TIMES ++;
	}
  }
  /*PACKET_RX_PEND*/
  if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_RX_PEND_BIT)
  {
	/* Packet RX */
	if(RECEIVE_TIMES == 0)				///在64字节内就会收完进此操作
	{
	  si446x_read_rx_fifo(4, &pWL->Wireless_FrameHeader[0]);
	  
	  si446x_fifo_info(0x00);
	  si446x_read_rx_fifo(Si446xCmd.FIFO_INFO.RX_FIFO_COUNT, &pWL->Wireless_RxData[0]);
	  
	}
	else if(RECEIVE_REMAIN > 0)			//在64字节内不能收完的进此操作
	{
	  si446x_fifo_info(0x00);
	  si446x_read_rx_fifo(Si446xCmd.FIFO_INFO.RX_FIFO_COUNT, &pWL->Wireless_RxData[RECEIVE_REMAIN]);
		
	  RECEIVE_REMAIN = 0;
	}
	
	pRadioConfiguration->Radio_PacketLength = pWL->Wireless_FrameHeader[0] - 4;
	pRadioConfiguration->Radio_ChannelNumber = pWL->Wireless_FrameHeader[1];
	
    si446x_get_modem_status(0X00);
    pWL->Wireless_RSSI_Value = Si446xCmd.GET_MODEM_STATUS.LATCH_RSSI;
	
	WIRELESS_STATUS = Wireless_RX_Finish;
	pWL->WirelessRx_Timeout_Cnt = 0;
	
	
  }
}


/*!
 *  Set Radio to RX mode. .
 *
 *  @param channel Freq. Channel,  packetLength : 0 Packet handler fields are used , nonzero: only Field1 is used
 *
 *  @note
 *
 */
void vRadio_StartRX(U8 channel, U8 packetLenght )
{
  /* Leave RX state */
  si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEXT_STATE1_NEW_STATE_ENUM_READY);
  
  // Read ITs, clear pending ones
  si446x_get_int_status(0u, 0u, 0u);

   // Reset the Rx Fifo
   si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_FIFO_RX_BIT);

  /* Start Receiving packet, channel 0, START immediately, Packet length used or not according to packetLength */
  si446x_start_rx(channel, 0u, packetLenght,
                  SI446X_CMD_START_RX_ARG_NEXT_STATE1_RXTIMEOUT_STATE_ENUM_NOCHANGE,
                  SI446X_CMD_START_RX_ARG_NEXT_STATE2_RXVALID_STATE_ENUM_READY,
                  SI446X_CMD_START_RX_ARG_NEXT_STATE3_RXINVALID_STATE_ENUM_RX );
}




/*!
 *  Set Radio to TX mode, variable packet length.
 *
 *  @param channel Freq. Channel, Packet to be sent length of of the packet sent to TXFIFO
 *
 *  @note
 *
 */
void vRadio_StartTx_Variable_Packet(U8 channel, U8 *pioRadioPacket, U8 length)
{
  /* Leave RX state */
  si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEXT_STATE1_NEW_STATE_ENUM_READY);

  /* Read ITs, clear pending ones */
  si446x_get_int_status(0u, 0u, 0u);

  /* Reset the Tx Fifo */
  si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_FIFO_TX_BIT);

  /* Fill the TX fifo with datas */
  if(length > 60)
  {
    TRANSMIT_REMAIN = length - 60;
    si446x_write_tx_fifo(4, Get_pWirelessBuf()->Wireless_FrameHeader);
    si446x_write_tx_fifo(60, pioRadioPacket);
  }
  else
  {
    TRANSMIT_REMAIN = 0;
    si446x_write_tx_fifo(4, Get_pWirelessBuf()->Wireless_FrameHeader);
    si446x_write_tx_fifo(length, pioRadioPacket);
  }

  /* Start sending packet, channel , START immediately */
   si446x_start_tx(channel, 0x80, length);
}

