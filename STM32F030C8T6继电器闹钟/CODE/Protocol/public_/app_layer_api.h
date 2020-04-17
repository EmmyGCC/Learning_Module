#ifndef __APP_LAYER_API_H
#define __APP_LAYER_API_H

#include <stdint.h>



//֡���������Ӧ��λ����
#define FCMD_CTLFUNC_BIT	(0X07<<0)	//������
#define FCMD_BUSY_BIT       (1<<5)      //æµ�ȴ���־
#define FCMD_ERROR_BIT		(1<<6)      //Ӧ����ȷ����־
#define FCMD_DIR_BIT		(1<<7)      //���䷽�� 1��վ

//֡���ݲ������Ӧ��λ����
#define FDAC_UNITNUM_BIT	(0X7F<<0)	//�豸��Ԫ
#define FDAC_RDWR_BIT		(1<<7)		//��д��־	1���� 0��д

//֡��������ʶͷ��Ӧ��λ����
#define FDATA_DATA_MARK_BIT			(1<<3)		//���غ������ݱ�ʶ
#define FDATA_DATA_VALID_BIT		(1<<2)		//���ݰ�����Ч����
#define FDATA_SRCADDR_VALID_BIT		(1<<1)		//Դ��ַ��Ч
#define FDATA_DESADDR_VALID_BIT		(1<<0)		//Ŀ���ַ��Ч

#define NOSRCADDR_DATAVALID         1
#define NODATA_SRCADDRVALID         2
#define SRCADDR_DATA_VALID          3

//֡�û���������Э����ʶ�̶���ID�ֶζ���
#define FDATA_COMPRESSION_PROTOCOL_ID	(1)
#define FDATA_COMPRESSION_HEADER_ID		(3)

#define FDATA_FRAGMENT_PROTOCOL_ID		(2)
#define FDATA_FRAGMENT_HEADER_ID		(4)

#define FDATA_TRANSPORT_PROTOCOL_ID		(3)
#define FDATA_TRANSPORT_HEADER_ID		(5)


#define COMPRESSION_HANDLE_LAYER_LEN	(1 + 1 + 4)
#define FRAGMENT_HANDLE_T_LAYER_LEN		(1 + 1 + 8 + 4 + 4)
#define RELIABLE_TRANSPORT_LAYER_LEN	(1 + 1 + 1 + 8 + 2 + 1)


typedef enum
{
    No_datamark = 0,
    Have_datamark,
} HAVE_DATAMARK_E;
/* ���Ϊ�¼�֡��û�����¸�ʽ
Ӧ�ò���û������������ʽ:

1����������ʶͷ��
* x 1�ֽڣ��� | �� | �� | �� | ���غ������ݱ�ʶ | ���ݰ�����Ч���� | Դ��ַ��Ч | Ŀ���ַ��Ч

2��Դ��ַ��
* x 4�ֽ�

3����������Ч���أ�
4�ֽ� ���س���
n�ֽ� ��������

4��Э����ʶ�����Ⱥ����򣩣�3+38�ֽڣ���
1�ֽ� ����
2�ֽ� Э����ʶ�����ֽ��ܳ���

�� ѹ�������
1�ֽ�ProtocolId���̶�Ϊ 1
1�ֽ�HeaderId��  �̶�Ϊ 3
---
4�ֽ� original_size

�� ��Ƭ�����
1�ֽ�ProtocolId���̶�Ϊ 2
1�ֽ�HeaderId��  �̶�Ϊ 4
---
8�ֽ� ֡���к�
4�ֽ� ��Ƭ���
4�ֽ� ��Ƭ����

�� �ɿ������
1�ֽ�ProtocolId���̶�Ϊ 3
1�ֽ�HeaderId��  �̶�Ϊ 5
---
1�ֽ� ��Ϣ����   [DATA = 0, ACK = 1, NAK = 2]
8�ֽ� ֡���к�   [DATA, ACK, NAK]
2�ֽ� �ỰId     [DATA]
1�ֽ� ��ʼ��ʶ�� [DATA]

�� �������Ʋ�
1�ֽ�ProtocolId���̶�Ϊ 4
---

--����Э��ͷ����


�������ݷ�����
AC 00 00 01 03 00 22 [06] [00 00 01 03] [00 00 00 04] [00 01 06 33] [01] [00 12] [03] [05] [00] [00 00 00 00 00 00 00 01] [00 00] [01]  CRC16 53
                                          ���س���      ��������    size  length  Э����ʶ���ݣ��磺NAK ACK DATA ...
*/

/**
�� ѹ�������
 1�ֽ�ProtocolId���̶�Ϊ 1
 1�ֽ�HeaderId���̶�Ϊ 3
--- 
 4�ֽ� original_size
*/
typedef struct
{
    uint8_t protocol_id;
    uint8_t header_id;
    uint32_t original_size;
} COMPRESSION_HANDLE_T;


/**
�� ��Ƭ�����
 1�ֽ�ProtocolId���̶�Ϊ 2
 1�ֽ�HeaderId���̶�Ϊ 4
--- 
 8�ֽ� ֡���к�  ��Ҫʱ��ȡ
 4�ֽ� ��Ƭ���  ��Ҫʱ��ȡ����������ʱ��Ҫд�޸�
 4�ֽ� ��Ƭ����  ��Ҫʱ��ȡ
*/
typedef struct
{
    uint8_t protocol_id;
    uint8_t header_id;
    uint64_t frame_seq;
	uint32_t fragment_seq;
	uint32_t fragment_total;
} FRAGMENT_HANDLE_T;


/**
�� �ɿ������
 1�ֽ�ProtocolId���̶�Ϊ 3
 1�ֽ�HeaderId���̶�Ϊ 5
--- 
 1�ֽ� ��Ϣ���� [DATA = 0, ACK = 1]
 8�ֽ� ֡���к� [DATA, ACK]
 2�ֽ� �ỰId [DATA]		//���ΪACK�����������DATA�ֶ�
 1�ֽ� ��ʼ��ʶ�� [DATA]
*/
typedef enum
{
	DATA = 0,
	ACK,
	NACK,
}MSG_TYPE_E;
typedef struct
{
    uint8_t protocol_id;
    uint8_t header_id;
    uint8_t message_type;
	uint64_t frame_seq;
	uint16_t conversation_id;
	uint8_t start_mark;
} RELIABLE_TRANSPORT_T;


//��֡�ֶθ���Ĳ�����������
typedef enum
{
	E_SET_crl_funcCode,
    E_SET_busydelay,
	E_SET_errorFlag,
	E_SET_transmissionDir,
	
	E_GET_crl_funcCode,
    E_GET_busydelay,
	E_GET_errorFlag,
	E_GET_transmissionDir,
	
	E_SET_devUnitNum,
	E_SET_rd_wrFlag,
	
	E_GET_devUnitNum,
	E_GET_rd_wrFlag,
	
	E_SET_compression_layer,
	E_SET_fragment_layer,
	E_SET_transport_layer,
	E_SET_flow_layer,
	
}ACTION_TYPE_T;


_Bool AppFrameData_FormatVerify(uint8_t *p, uint8_t len);   //ָ�����ݵ���ʼ��ַ�����ݼ��
uint8_t AppFramePackage_FormatVerify(uint8_t *p);
void AppFramePackage_Head(uint8_t *p_data);
void AppFramePackage_DesAddr(uint8_t *p_data, uint8_t *src_addr);
void AppFramePackage_Cmd(uint8_t *p_data, uint8_t code, ACTION_TYPE_T bit_type);
void AppFramePackage_DataLen(uint8_t *p_data, uint8_t data_len);
void AppFramePackage_DataDAC(uint8_t *p_data, uint8_t code, ACTION_TYPE_T bit_type);
uint8_t AppFramePackage_DataHead(uint8_t *p_data, uint8_t *src_data, uint8_t src_data_len, HAVE_DATAMARK_E dataMark);
uint8_t AppFramePackage_DataProtocolMark(uint8_t *p_data, ACTION_TYPE_T bit_type, MSG_TYPE_E msg_type);


uint8_t AppFrameGet_Head(uint8_t *p_data);
uint8_t *AppFrameGet_pDesAddr(uint8_t *p_data);
uint8_t AppFrameGet_Addr_DevMacType(uint8_t *p_data);
uint8_t AppFrameGet_Cmd(uint8_t *p_data,ACTION_TYPE_T bit_type);
uint8_t AppFrameGet_DataLen(uint8_t *p_data);
uint8_t *AppFrameGet_pDataDAC(uint8_t *p_data);
uint8_t *AppFrameGet_pDataID(uint8_t *p_data);
uint8_t *AppFrameGet_pDataVal(uint8_t *p_data);
uint8_t Filter_DataMark_Head(uint8_t *p_data);
uint8_t *AppFrameGet_pDataSrcAddr(uint8_t *p_data);
uint8_t AppFrameGet_DataPayloadLen(uint8_t *p_data);
uint8_t *AppFrameGet_pDataPayloadLen(uint8_t *p_data);
uint8_t *AppFrameGet_pDataPayload(uint8_t *p_data);
uint8_t *AppFrameGet_pReliable_Transport_Layer(uint8_t *p_data);
uint8_t AppFrameGet_pDataProtocolMark(uint8_t *p_data, ACTION_TYPE_T bit_type);

#endif

