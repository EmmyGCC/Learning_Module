#include <string.h>

#include "app_layer_api.h"
#include "protocol_data_def_struct.h"
#include "device_basic_info.h"
#include "crc16.h"


/*************************************************************************************************************************/
/*---------------------------------------------- 设置应用层协议各字段的函数 ---------------------------------------------*/
/*************************************************************************************************************************/

_Bool AppFrameData_FormatVerify(uint8_t *p, uint8_t len)   
{
    uint16_t crc16_val = 0;
    uint8_t crc_len = 0;
    
    if((p[Region_HeaderNumber] != HKFrameHEAD) && (p[Region_HeaderNumber] != AESFreamHeader))
        return 1;
    
    if(p[Region_HeaderNumber] == HKFrameHEAD)
        crc_len = len-3;
    else
        crc_len = len-2;
    crc16_val = CRC16_2(p,crc_len);
    if((crc16_val >> 8) != p[crc_len])
        return 1;  
    if((crc16_val & 0xff) != p[crc_len + 1])
        return 1;
    
    
    return 0;
}


/**
给帧数据最后加CRC16和帧尾，最终帧长度
*/
uint8_t AppFramePackage_FormatVerify(uint8_t *p_data)
{
    uint8_t ComposeFrame_Len = 0;
    uint16_t crc16_val = 0;

	AppFramePackage_Head(p_data);
	
    ComposeFrame_Len = AppFrameGet_DataLen(p_data) + Region_DataDACNumber;

    crc16_val = CRC16_2(p_data, ComposeFrame_Len);
    p_data[ComposeFrame_Len] = crc16_val >> 8;
    p_data[ComposeFrame_Len + 1] = crc16_val & 0xff;
    p_data[ComposeFrame_Len + 2] = HKFreamEND;
	
    return ComposeFrame_Len += 3;
}


void AppFramePackage_Head(uint8_t *p_data)
{
	p_data[Region_HeaderNumber] = HKFrameHEAD;
}


void AppFramePackage_DesAddr(uint8_t *p_data, uint8_t *src_addr)
{
	memmove(&p_data[Region_AddrNumber], src_addr, MAC_Data_Len);
}


void AppFramePackage_Cmd(uint8_t *p_data, uint8_t code, ACTION_TYPE_T bit_type)
{
	switch(bit_type)
	{
		case E_SET_crl_funcCode:
			if(code)
				p_data[Region_CmdNumber] |= (code & FCMD_CTLFUNC_BIT);
			else
				p_data[Region_CmdNumber] &= ~(FCMD_CTLFUNC_BIT);
			break;
        case E_SET_busydelay:
            if (code)
                p_data[Region_CmdNumber] |= FCMD_BUSY_BIT;
            else
                p_data[Region_CmdNumber] &= ~FCMD_BUSY_BIT;
            break;
		case E_SET_errorFlag:
			if(code)
				p_data[Region_CmdNumber] |= FCMD_ERROR_BIT;
			else
				p_data[Region_CmdNumber] &= ~FCMD_ERROR_BIT;
			break;
		case E_SET_transmissionDir:
			if(code)
				p_data[Region_CmdNumber] |= FCMD_DIR_BIT;
			else
				p_data[Region_CmdNumber] &= ~FCMD_DIR_BIT;
			break;
		default:
			break;
	}
}


void AppFramePackage_DataLen(uint8_t *p_data, uint8_t data_len)
{
	p_data[Region_DataLenNumber] = data_len;
}


void AppFramePackage_DataDAC(uint8_t *p_data, uint8_t code, ACTION_TYPE_T bit_type)
{
	switch(bit_type)
	{
		case E_SET_devUnitNum:
			if(code)
				p_data[Region_DataDACNumber] |= (code & FDAC_UNITNUM_BIT);
			else
				p_data[Region_DataDACNumber] &= ~FDAC_UNITNUM_BIT;
			break;
		case E_SET_rd_wrFlag:
			if(code)
				p_data[Region_DataDACNumber] |= FDAC_RDWR_BIT;
			else
				p_data[Region_DataDACNumber] &= ~FDAC_RDWR_BIT;
			break;
		default:
			break;
	}
}


/**
 @对用户数据区的前三部分格式打包
 @参数:
	p_data：应用层用户数据区开始的指针
	src_data：要放入负载数据区的数据指针
	src_data_len：负载数据的长度
	dataMark：有无数据标识
@返回值：处理后的数据下标 (也可以考虑返回数据处理后的指针，便于接收后直接使用)
*/
uint8_t AppFramePackage_DataHead(uint8_t *p_data, uint8_t *src_data, uint8_t src_data_len, HAVE_DATAMARK_E dataMark)
{
	uint8_t *p_userdata = AppFrameGet_pDataDAC(p_data);
	
	p_userdata[Region_DataMarkHeadNumber] = FDATA_SRCADDR_VALID_BIT;
	memmove(&p_userdata[Region_DataSrcAddrNumber], Get_pSelfDevMacInfo()->mac, MAC_Data_Len);
	
	if (src_data_len)
	{
		uint8_t data_len[PayloadLen_NUM] = {0,0,0,src_data_len};
		
		p_userdata[Region_DataMarkHeadNumber] |= FDATA_DATA_VALID_BIT;
		if (dataMark)
			p_userdata[Region_DataMarkHeadNumber] |= FDATA_DATA_MARK_BIT;
		
		memmove(&p_userdata[Region_DataPayloadLenNumber], &data_len, PayloadLen_NUM);
		memmove(&p_userdata[Region_DataPayloadNumber], src_data, src_data_len);
		return Region_DataDACNumber + Region_DataPayloadNumber + src_data_len + 3;
	}
	else	//没有"数据包含有效负载"BIT则无负载数据区域
	{
		p_userdata[Region_DataMarkHeadNumber] &= ~(FDATA_DATA_VALID_BIT | FDATA_DATA_MARK_BIT);
		return Region_DataDACNumber + Region_DataPayloadLenNumber + 3;		//没有数据区，少负载数据长度的4字节
	}
}


/**
 @用户数据区的协议层标识各层的处理，返回处理后的数据下标
 @参数:
	p_data：用户数据区的前三部分结束后开始的指针
	bit_type：要处理哪一层的数据
	msg_type：第③层的消息类型
*/
uint8_t AppFramePackage_DataProtocolMark(uint8_t *p_data, ACTION_TYPE_T bit_type, MSG_TYPE_E msg_type)
{
	switch (bit_type)
	{
      case E_SET_compression_layer:		//压缩处理层
		{
			COMPRESSION_HANDLE_T *pData = (COMPRESSION_HANDLE_T *)p_data;
			
			if (pData->protocol_id == FDATA_COMPRESSION_PROTOCOL_ID)
			{/*********在上层需要传输数据大于500Bytes时会有***********************/
				return COMPRESSION_HANDLE_LAYER_LEN;
			}
		}
		break;
		
		case E_SET_fragment_layer:		//碎片处理层
		{
			FRAGMENT_HANDLE_T *pData = (FRAGMENT_HANDLE_T *)p_data;
			
			if (pData->protocol_id == FDATA_FRAGMENT_PROTOCOL_ID)
			{/*********在上层需要传输负载数据大于116Bytes时会有***********************/
				return FRAGMENT_HANDLE_T_LAYER_LEN;
			}
		}
		break;
		
		case E_SET_transport_layer:		//可靠传输层
		{
			RELIABLE_TRANSPORT_T *pData = (RELIABLE_TRANSPORT_T *)p_data;
			
			if (pData->protocol_id == FDATA_TRANSPORT_PROTOCOL_ID)
			{
				pData->message_type = msg_type;
				
				return RELIABLE_TRANSPORT_LAYER_LEN;
			}
		}
		break;
		
		case E_SET_flow_layer:
			break;
		
		default:
			break;
	}
	return 0;
}


/*************************************************************************************************************************/
/*---------------------------------------------- 获取应用层协议各字段的函数 ---------------------------------------------*/
/*************************************************************************************************************************/

inline uint8_t AppFrameGet_Head(uint8_t *p_data)
{
	return p_data[Region_HeaderNumber];
}


inline uint8_t *AppFrameGet_pDesAddr(uint8_t *p_data)
{
	return &p_data[Region_AddrNumber];
}


inline uint8_t AppFrameGet_Addr_DevMacType(uint8_t *p_data)
{
	return p_data[Region_CmdNumber - 1];
}


uint8_t AppFrameGet_Cmd(uint8_t *p_data,ACTION_TYPE_T bit_type)
{
	uint8_t ret_val = 0;
	
	switch (bit_type)
    {
      case E_GET_crl_funcCode:
        ret_val = p_data[Region_CmdNumber] & FCMD_CTLFUNC_BIT;
        break;
      case E_GET_errorFlag:
        ret_val = p_data[Region_CmdNumber] & FCMD_ERROR_BIT;
        break;
      case E_GET_transmissionDir:
        ret_val = p_data[Region_CmdNumber] & FCMD_DIR_BIT;
        break;
      default :
        break;
    }
	return ret_val;
}


inline uint8_t AppFrameGet_DataLen(uint8_t *p_data)
{
	return p_data[Region_DataLenNumber];
}


inline uint8_t *AppFrameGet_pDataDAC(uint8_t *p_data)
{
	return &p_data[Region_DataDACNumber];
}


inline uint8_t *AppFrameGet_pDataID(uint8_t *p_data)
{
	return &p_data[Region_DataIDNumber];
}


inline uint8_t *AppFrameGet_pDataVal(uint8_t *p_data)
{
	return &p_data[Region_DataValNumber];
}


uint8_t Filter_DataMark_Head(uint8_t *p_data)
{
    uint8_t *p_userdata = AppFrameGet_pDataDAC(p_data);
    uint8_t data_mark_head = p_userdata[Region_DataMarkHeadNumber];
    
    if (((data_mark_head & FDATA_SRCADDR_VALID_BIT) == 0)
        && (data_mark_head & FDATA_DATA_VALID_BIT))		//无源地址时但有负载数据时
    {
        return NOSRCADDR_DATAVALID;
    }
    else if ((data_mark_head & FDATA_SRCADDR_VALID_BIT)
        && ((data_mark_head & FDATA_DATA_VALID_BIT) == 0))
    {
        return NODATA_SRCADDRVALID;
    }
    else if (data_mark_head & (FDATA_SRCADDR_VALID_BIT | FDATA_DATA_VALID_BIT))	//确认有负载数据和源地址
    {
        return SRCADDR_DATA_VALID;
    }
    return NULL;
}


uint8_t *AppFrameGet_pDataSrcAddr(uint8_t *p_data)
{
    uint8_t *p_userdata = AppFrameGet_pDataDAC(p_data);
    
    return &p_userdata[Region_DataSrcAddrNumber];
}


/*
获取负载数据的长度
*/
uint8_t AppFrameGet_DataPayloadLen(uint8_t *p_data)
{
	uint8_t *p_userdata = AppFrameGet_pDataDAC(p_data);
    uint8_t data_markhead = Filter_DataMark_Head(p_data);
    uint8_t ret = 0;
    
    switch(data_markhead)
    {
        case NOSRCADDR_DATAVALID:
            ret = p_userdata[Region_DataSrcAddrNumber + PayloadLen_NUM - 1];
            break;
        case NODATA_SRCADDRVALID:
            break;
        case SRCADDR_DATA_VALID:
            ret = p_userdata[Region_DataPayloadNumber - 1];
            break;
        default:
            break;
    }
    
	return ret;
}


/*
获取负载数据长度为首地址
*/
uint8_t *AppFrameGet_pDataPayloadLen(uint8_t *p_data)
{
	uint8_t *p_userdata = AppFrameGet_pDataDAC(p_data);
	
	return &p_userdata[Region_DataPayloadLenNumber];
}


/*
获取负载数据区标识头处理结果的地址
*/
uint8_t *AppFrameGet_pDataPayload(uint8_t *p_data)
{
    uint8_t *p_userdata = AppFrameGet_pDataDAC(p_data);
    uint8_t data_markhead = Filter_DataMark_Head(p_data);
    uint8_t *ret = NULL;
    
    switch(data_markhead)
    {
        case NOSRCADDR_DATAVALID:
            ret = &p_userdata[Region_DataSrcAddrNumber + PayloadLen_NUM];
            break;
        case NODATA_SRCADDRVALID:
            ret = &p_userdata[Region_DataPayloadLenNumber];
            break;
        case SRCADDR_DATA_VALID:
            ret = &p_userdata[Region_DataPayloadNumber];
            break;
        default:
            break;
    }
    
    return ret;
}


/*
获取协议层标识区可靠传输层的地址
*/
uint8_t *AppFrameGet_pReliable_Transport_Layer(uint8_t *p_data)
{
	uint8_t *p_userdata = AppFrameGet_pDataDAC(p_data);
	uint8_t payload_len = AppFrameGet_DataPayloadLen(p_data);
	
	return &p_userdata[Region_DataPayloadNumber + payload_len];
}


/**
 @获取用户数据区的协议层标识各层的数据下标
 @参数:
	p_data：用户数据区的前三部分结束后开始的指针
	bit_type：要获取哪一层的数据
*/
uint8_t AppFrameGet_pDataProtocolMark(uint8_t *p_data, ACTION_TYPE_T bit_type)
{
	switch (bit_type)
	{
		case E_SET_compression_layer:		//压缩处理层
		{	
			if (p_data[0] == FDATA_COMPRESSION_PROTOCOL_ID)
			{/*********在上层需要传输数据大于500Bytes时会有***********************/
				return COMPRESSION_HANDLE_LAYER_LEN;
			}
		}
		break;
		
		case E_SET_fragment_layer:		//碎片处理层
		{	
			if (p_data[0] == FDATA_FRAGMENT_PROTOCOL_ID)
			{/*********在上层需要传输负载数据大于116Bytes时会有***********************/
				return FRAGMENT_HANDLE_T_LAYER_LEN;
			}
		}
		break;
		
		case E_SET_transport_layer:		//可靠传输层
		{	
			if (p_data[0] == FDATA_TRANSPORT_PROTOCOL_ID)
			{
				return RELIABLE_TRANSPORT_LAYER_LEN;
			}
		}
		break;
		
		case E_SET_flow_layer:
			break;
		
		default:
			break;
	}
	return 0;
}


