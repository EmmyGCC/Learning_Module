#ifndef __PROTOCOL_DATA_DEF_STRUCT_H
#define __PROTOCOL_DATA_DEF_STRUCT_H

#include <stdint.h>


#define DEV_CONTROL_CODE	(1<<0)
#define DEV_JOINNET_CODE	(1<<1)
#define DEV_MAINNODE_CODE	(1<<2)


////////////////////////////////Э��֡�ĺ궨��/////////////////////////////////////////////////
#define HKFrameHEAD		0xAC
#define HKFreamEND		0x53

#define CARRIERDATA_LEN     188
#define APP_BUFF_MAX_LEN	256
#define GROUP_ADDR_LEN		3




//֡��������±���ʶ�λ
#define Region_HeaderNumber		0       //֡ͷ
#define Region_AddrNumber		1       //֡Ŀ�ĵ�ַ
#define Region_CmdNumber		5       //֡������
#define Region_DataLenNumber	6       //֡���ݳ���

#define Region_DataDACNumber	7       //֡���ݲ�����
#define Region_DataIDNumber		8		//֡����ID
#define Region_DataValNumber	10      //֡����ֵ


/*
Ӧ�ò���û������������ʽ:
1����������ʶͷ��
* x 1�ֽڣ��� | �� | �� | �� | ���غ������ݱ�ʶ | ���ݰ�����Ч���� | Դ��ַ��Ч | Ŀ���ַ��Ч

2��Դ��ַ��
* x MAC_Data_Len�ֽ�

3����������Ч���أ���1.���ֵ�"���ݰ�����Ч����"bit��Чʱ���У���
4�ֽ� ���س���
n�ֽ� ��������
*/
#define PayloadLen_NUM              4
#define Region_DataMarkHeadNumber		0
#define Region_DataSrcAddrNumber		1
#define Region_DataPayloadLenNumber		Region_DataSrcAddrNumber + MAC_Data_Len
#define Region_DataPayloadNumber		Region_DataPayloadLenNumber + PayloadLen_NUM




//MAC֡��ʽ
#define AESFreamHeader			0x68
#define MACWrite_Cmd_Request	0x20
#define MACErase_Cmd_Request    0x30 

//MAC֡������֡����
#define MAC_Data_Len	4


//�������¼�����
typedef enum
{
    EVT_Reserve = 0,
	EVT_Event,
	EVT_View_Change,
	EVT_Uart_RxReady,
} EVENT_CODE_E;


//�������
typedef enum
{
    ERR_No_Request_Data = 1,
    ERR_Dev_Not_Ready,
    ERR_Link_Response_Overtime,
    ERR_Data_Format,
    ERR_Invaild_FuncMark,
    ERR_Aes_Resolve,
    ERR_Aes_Not_Ready,
    ERR_Crc16_VerifyFailed,
} ERROR_CODE_E;


typedef enum
{
  WL_RX_Cmd = 0,
  WL_RX_Addr,
  WL_RX_Destin,
  WL_RX_Source,
  WL_RX_RoutLevel,
  WL_RX_RoutLen,
  WL_RX_RoutPath,
  WL_RX_FrameData,
  WL_RX_Crc,
  WL_RX_End,
  WL_RX_Finish,
}WL_Status_TypDef;


typedef enum
{
  Data_error = 0,		//���ݸ�ʽ����
  Data_Correct,
  Slave_Mate_Data,
  Master_Mate_Data,
  Data_RoutDevice,
  
  Addr_error,
  Source_error,
  RoutLevel_error,
  RoutLen_error,
  Status_error,
  Record_error,
}WL_Process_Status;



#endif


