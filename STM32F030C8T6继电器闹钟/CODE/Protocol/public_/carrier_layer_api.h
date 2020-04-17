#ifndef _CARRIER_LAYER_API_H
#define _CARRIER_LAYER_API_H

#include <stdint.h>



#define Carrier_MAC_LEN         6

#define Carrier_Head_Num        0
#define Carrier_Addr_Num		1
#define Carrier_Start_Num		7
#define Carrier_Cmd_Num			8
#define Carrier_Len_Num         9
#define Carrier_Data_Num        10

#define CARRIER_HEAD        0X68
#define CARRIER_END         0X16

#define CARRIER_MAC_HEAD	0XFE



void CarrierFramePackage_Head1(uint8_t *p_data);
void CarrierFramePackage_Head2(uint8_t *p_data);
void CarrierFramePackage_DesAddr(uint8_t *p_data, uint8_t *src_addr);
void CarrierFramePackage_Cmd(uint8_t *p_data, uint8_t code);
void CarrierFramePackage_DataLen(uint8_t *p_data, uint8_t data_len);
void CarrierFramePackage_Data(uint8_t *p_data, uint8_t *src_data, uint8_t src_data_len);
void CarrierFramePackage_FormatVerify(uint8_t *p_data);

uint8_t CarrierFrameGet_DataLen(uint8_t *p_data);


#endif

