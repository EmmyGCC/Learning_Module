#include <string.h>

#include "app_layer_api.h"
#include "protocol_data_def_struct.h"
#include "device_basic_info.h"
#include "crc16.h"


/*************************************************************************************************************************/
/*---------------------------------------------- ����Ӧ�ò�Э����ֶεĺ��� ---------------------------------------------*/
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
��֡��������CRC16��֡β������֡����
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
 @���û���������ǰ�����ָ�ʽ���
 @����:
	p_data��Ӧ�ò��û���������ʼ��ָ��
	src_data��Ҫ���븺��������������ָ��
	src_data_len���������ݵĳ���
	dataMark���������ݱ�ʶ
@����ֵ�������������±� (Ҳ���Կ��Ƿ������ݴ�����ָ�룬���ڽ��պ�ֱ��ʹ��)
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
	else	//û��"���ݰ�����Ч����"BIT���޸�����������
	{
		p_userdata[Region_DataMarkHeadNumber] &= ~(FDATA_DATA_VALID_BIT | FDATA_DATA_MARK_BIT);
		return Region_DataDACNumber + Region_DataPayloadLenNumber + 3;		//û�����������ٸ������ݳ��ȵ�4�ֽ�
	}
}


/**
 @�û���������Э����ʶ����Ĵ������ش����������±�
 @����:
	p_data���û���������ǰ�����ֽ�����ʼ��ָ��
	bit_type��Ҫ������һ�������
	msg_type���ڢ۲����Ϣ����
*/
uint8_t AppFramePackage_DataProtocolMark(uint8_t *p_data, ACTION_TYPE_T bit_type, MSG_TYPE_E msg_type)
{
	switch (bit_type)
	{
      case E_SET_compression_layer:		//ѹ�������
		{
			COMPRESSION_HANDLE_T *pData = (COMPRESSION_HANDLE_T *)p_data;
			
			if (pData->protocol_id == FDATA_COMPRESSION_PROTOCOL_ID)
			{/*********���ϲ���Ҫ�������ݴ���500Bytesʱ����***********************/
				return COMPRESSION_HANDLE_LAYER_LEN;
			}
		}
		break;
		
		case E_SET_fragment_layer:		//��Ƭ�����
		{
			FRAGMENT_HANDLE_T *pData = (FRAGMENT_HANDLE_T *)p_data;
			
			if (pData->protocol_id == FDATA_FRAGMENT_PROTOCOL_ID)
			{/*********���ϲ���Ҫ���为�����ݴ���116Bytesʱ����***********************/
				return FRAGMENT_HANDLE_T_LAYER_LEN;
			}
		}
		break;
		
		case E_SET_transport_layer:		//�ɿ������
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
/*---------------------------------------------- ��ȡӦ�ò�Э����ֶεĺ��� ---------------------------------------------*/
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
        && (data_mark_head & FDATA_DATA_VALID_BIT))		//��Դ��ַʱ���и�������ʱ
    {
        return NOSRCADDR_DATAVALID;
    }
    else if ((data_mark_head & FDATA_SRCADDR_VALID_BIT)
        && ((data_mark_head & FDATA_DATA_VALID_BIT) == 0))
    {
        return NODATA_SRCADDRVALID;
    }
    else if (data_mark_head & (FDATA_SRCADDR_VALID_BIT | FDATA_DATA_VALID_BIT))	//ȷ���и������ݺ�Դ��ַ
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
��ȡ�������ݵĳ���
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
��ȡ�������ݳ���Ϊ�׵�ַ
*/
uint8_t *AppFrameGet_pDataPayloadLen(uint8_t *p_data)
{
	uint8_t *p_userdata = AppFrameGet_pDataDAC(p_data);
	
	return &p_userdata[Region_DataPayloadLenNumber];
}


/*
��ȡ������������ʶͷ�������ĵ�ַ
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
��ȡЭ����ʶ���ɿ������ĵ�ַ
*/
uint8_t *AppFrameGet_pReliable_Transport_Layer(uint8_t *p_data)
{
	uint8_t *p_userdata = AppFrameGet_pDataDAC(p_data);
	uint8_t payload_len = AppFrameGet_DataPayloadLen(p_data);
	
	return &p_userdata[Region_DataPayloadNumber + payload_len];
}


/**
 @��ȡ�û���������Э����ʶ����������±�
 @����:
	p_data���û���������ǰ�����ֽ�����ʼ��ָ��
	bit_type��Ҫ��ȡ��һ�������
*/
uint8_t AppFrameGet_pDataProtocolMark(uint8_t *p_data, ACTION_TYPE_T bit_type)
{
	switch (bit_type)
	{
		case E_SET_compression_layer:		//ѹ�������
		{	
			if (p_data[0] == FDATA_COMPRESSION_PROTOCOL_ID)
			{/*********���ϲ���Ҫ�������ݴ���500Bytesʱ����***********************/
				return COMPRESSION_HANDLE_LAYER_LEN;
			}
		}
		break;
		
		case E_SET_fragment_layer:		//��Ƭ�����
		{	
			if (p_data[0] == FDATA_FRAGMENT_PROTOCOL_ID)
			{/*********���ϲ���Ҫ���为�����ݴ���116Bytesʱ����***********************/
				return FRAGMENT_HANDLE_T_LAYER_LEN;
			}
		}
		break;
		
		case E_SET_transport_layer:		//�ɿ������
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


