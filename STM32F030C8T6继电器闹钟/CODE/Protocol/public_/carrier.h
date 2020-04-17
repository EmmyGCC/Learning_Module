#ifndef __CARRIER_H
#define __CARRIER_H

#include <stdint.h>
#include "carrier_layer_api.h"




typedef struct
{
    uint8_t carrierSelfNode_Addr[Carrier_MAC_LEN];  //从节点自身的通信地址
    uint8_t comSrcNode_Addr[Carrier_MAC_LEN];       //目的从节点的通信地址
    _Bool IsData_from_carrier;
} CARRIER_T;



CARRIER_T *Get_pCarrier_Info(void);

void Set_Carrier_Info(uint8_t *src_addr);
void HEXaddr_2_BCDaddr(uint8_t *hex_srcAddr, uint8_t *BCD_desAddr);

void Set_NodeAddr(uint8_t *des_addr);
void CarrierFramePackage_SendData(uint8_t *des_addr, uint8_t *src_data, uint16_t data_len);



#endif

