#ifndef __SYSTEM_FUNC__
#define __SYSTEM_FUNC__

#include "stm32f0xx.h"



/*************** FLASH ***************/
typedef void (*pFlash_WriteFunc)(uint32_t, void *, uint16_t);
typedef void (*pFlash_ReadFunc)(uint32_t, void *, uint16_t);
typedef void (*pFlash_EraseFunc)(uint32_t);

/*************** TICK ***************/
typedef void (*pTickfunc)(void);

/*************** RXDATA ***************/
typedef void (*pRf_Rxfunc)(void *);
typedef void (*pCarrier_Rxfunc)(void *);
typedef void (*pMac_Rxfunc)(void *);

/*************** DEV UPREPORT ***************/
typedef void (*pUpReport_func)(void *);

/*************** COM ***************/
typedef void (*pComRx_func)(uint8_t, void *);
typedef void (*pComTx_func)(void *, void *, uint16_t);

/*************** DEV ACTION ***************/
typedef void (*pDevAction_func)(void *);
typedef void (*pDevStateInitfunc)(void);




typedef struct
{
    pFlash_EraseFunc FLASH_Erase;
    pFlash_WriteFunc FLASH_Write;
	pFlash_ReadFunc FLASH_Read;
    
    pTickfunc TICK_Process;
    
    pRf_Rxfunc Rf_RxProcess;
    pCarrier_Rxfunc Carrier_RxProcess;
    pMac_Rxfunc Mac_RxProcess;
    
    pUpReport_func DevData_UpReport;
    
    pComRx_func Com1RxData;
    pComRx_func Com2RxData;
    pComTx_func Com1TxData;
    pComTx_func Com2TxData;
    
    pDevAction_func DevAction_Process;
    pDevStateInitfunc DevStateInit;
    
}SYSTEM_FUNC_T;



SYSTEM_FUNC_T *Get_pSystemFunc(void);









#endif

