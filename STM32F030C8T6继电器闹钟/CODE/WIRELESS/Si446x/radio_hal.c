/*!
 * File:
 *  radio_hal.c
 *
 * Description:
 *  This file contains RADIO HAL.
 *
 * Silicon Laboratories Confidential
 * Copyright 2011 Silicon Laboratories, Inc.
 */

                /* ======================================= *
                 *              I N C L U D E              *
                 * ======================================= */

#include "data_base.h"
#include "stm32f0_spi.h"
#include "stm32f0_gpio.h"
#include "radio_hal.h"
#include "stm32f0_exint.h"


                /* ======================================= *
                 *          D E F I N I T I O N S          *
                 * ======================================= */

                /* ======================================= *
                 *     G L O B A L   V A R I A B L E S     *
                 * ======================================= */

                /* ======================================= *
                 *      L O C A L   F U N C T I O N S      *
                 * ======================================= */

                /* ======================================= *
                 *     P U B L I C   F U N C T I O N S     *
                 * ======================================= */

void radio_hal_AssertShutdown(void)
{
  SDN_H();
}

void radio_hal_DeassertShutdown(void)
{
  SDN_L();
}

void radio_hal_ClearNsel(void)
{
  WIRELESS_NSEL_L();
}

void radio_hal_SetNsel(void)
{
  WIRELESS_NSEL_H();
}

_Bool radio_hal_NirqLevel(void)
{
  return NIRQ_RDPIN();
}

void radio_hal_SpiWriteByte(U8 byteToWrite)
{
  SPI_RWbyte(byteToWrite);
}

U8 radio_hal_SpiReadByte(void)
{
  return SPI_RWbyte(0xFF);
}

void radio_hal_SpiWriteData(U8 byteCount, U8* pData)
{
  SPI_WriteData(byteCount, pData);
}

void radio_hal_SpiReadData(U8 byteCount, U8* pData)
{
  SPI_ReadData(byteCount, pData);
}

#ifdef RADIO_DRIVER_EXTENDED_SUPPORT
_Bool radio_hal_Gpio0Level(void)
{
  _Bool retVal = FALSE;

#ifdef SILABS_PLATFORM_DKMB
  retVal = FALSE;
#endif
#ifdef SILABS_PLATFORM_UDP
  retVal = EZRP_RX_DOUT;
#endif
#if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB)
  retVal = RF_GPIO0;
#endif
#if (defined SILABS_PLATFORM_WMB930)
  retVal = FALSE;
#endif
#if defined (SILABS_PLATFORM_WMB912)
  #ifdef SILABS_IO_WITH_EXTENDER
    //TODO
    retVal = FALSE;
  #endif
#endif

  return retVal;
}

_Bool radio_hal_Gpio1Level(void)
{
  _Bool retVal = FALSE;

#ifdef SILABS_PLATFORM_DKMB
  retVal = FSK_CLK_OUT;
#endif
#ifdef SILABS_PLATFORM_UDP
  retVal = FALSE; //No Pop
#endif
#if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB) || (defined SILABS_PLATFORM_WMB930)
  retVal = RF_GPIO1;
#endif
#if defined (SILABS_PLATFORM_WMB912)
  #ifdef SILABS_IO_WITH_EXTENDER
    //TODO
    retVal = FALSE;
  #endif
#endif

  return retVal;
}

_Bool radio_hal_Gpio2Level(void)
{
  _Bool retVal = FALSE;

#ifdef SILABS_PLATFORM_DKMB
  retVal = DATA_NFFS;
#endif
#ifdef SILABS_PLATFORM_UDP
  retVal = FALSE; //No Pop
#endif
#if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB) || (defined SILABS_PLATFORM_WMB930)
  retVal = RF_GPIO2;
#endif
#if defined (SILABS_PLATFORM_WMB912)
  #ifdef SILABS_IO_WITH_EXTENDER
    //TODO
    retVal = FALSE;
  #endif
#endif

  return retVal;
}

_Bool radio_hal_Gpio3Level(void)
{
  _Bool retVal = FALSE;

#if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB) || (defined SILABS_PLATFORM_WMB930)
  retVal = RF_GPIO3;
#elif defined (SILABS_PLATFORM_WMB912)
  #ifdef SILABS_IO_WITH_EXTENDER
    //TODO
    retVal = FALSE;
  #endif
#endif

  return retVal;
}

#endif
