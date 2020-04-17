#include <string.h>

#include "carrier_layer_api.h"




/**************************************************************************/
/*					   	以下为载波打包数据使用API						  */
/**************************************************************************/

inline void CarrierFramePackage_Head1(uint8_t *p_data)
{
	p_data[Carrier_Head_Num] = CARRIER_HEAD;
}


inline void CarrierFramePackage_DesAddr(uint8_t *p_data, uint8_t *src_addr)
{
	memmove(&p_data[Carrier_Addr_Num], src_addr, Carrier_MAC_LEN);
}


inline void CarrierFramePackage_Head2(uint8_t *p_data)
{
	p_data[Carrier_Start_Num] = CARRIER_HEAD;
}


inline void CarrierFramePackage_Cmd(uint8_t *p_data, uint8_t code)
{
	p_data[Carrier_Cmd_Num] = code;
}


inline void CarrierFramePackage_DataLen(uint8_t *p_data, uint8_t data_len)
{
	p_data[Carrier_Len_Num] = data_len;
}


inline void CarrierFramePackage_Data(uint8_t *p_data, uint8_t *src_data, uint8_t src_data_len)
{
	memmove(&p_data[Carrier_Data_Num], src_data, src_data_len);
}


void CarrierFramePackage_FormatVerify(uint8_t *p_data)
{
	uint8_t checksum = 0;
	
	for(uint8_t cnt = Carrier_Head_Num; cnt < (Carrier_Data_Num + p_data[Carrier_Len_Num]); cnt++)
	{
		checksum += p_data[cnt];
	}
	
    p_data[Carrier_Data_Num + p_data[Carrier_Len_Num]] = checksum;
    p_data[Carrier_Data_Num + p_data[Carrier_Len_Num] + 1] = CARRIER_END;
}


/**************************************************************************/
/*					   	以下为载波获取数据使用API						  */
/**************************************************************************/

inline uint8_t CarrierFrameGet_DataLen(uint8_t *p_data)
{
	return p_data[Carrier_Len_Num];
}


