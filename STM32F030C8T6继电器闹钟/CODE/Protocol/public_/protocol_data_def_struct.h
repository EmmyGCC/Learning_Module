#ifndef __PROTOCOL_DATA_DEF_STRUCT_H
#define __PROTOCOL_DATA_DEF_STRUCT_H

#include <stdint.h>


#define DEV_CONTROL_CODE	(1<<0)
#define DEV_JOINNET_CODE	(1<<1)
#define DEV_MAINNODE_CODE	(1<<2)


////////////////////////////////协议帧的宏定义/////////////////////////////////////////////////
#define HKFrameHEAD		0xAC
#define HKFreamEND		0x53

#define CARRIERDATA_LEN     188
#define APP_BUFF_MAX_LEN	256
#define GROUP_ADDR_LEN		3




//帧各个域的下标访问定位
#define Region_HeaderNumber		0       //帧头
#define Region_AddrNumber		1       //帧目的地址
#define Region_CmdNumber		5       //帧命令码
#define Region_DataLenNumber	6       //帧数据长度

#define Region_DataDACNumber	7       //帧数据操作码
#define Region_DataIDNumber		8		//帧数据ID
#define Region_DataValNumber	10      //帧数据值


/*
应用层的用户数据区整体格式:
1，数据区标识头：
* x 1字节，备 | 备 | 备 | 备 | 负载含有数据标识 | 数据包含有效负载 | 源地址有效 | 目标地址有效

2，源地址：
* x MAC_Data_Len字节

3，数据区有效负载：（1.部分的"数据包含有效负载"bit有效时才有）：
4字节 负载长度
n字节 负载数据
*/
#define PayloadLen_NUM              4
#define Region_DataMarkHeadNumber		0
#define Region_DataSrcAddrNumber		1
#define Region_DataPayloadLenNumber		Region_DataSrcAddrNumber + MAC_Data_Len
#define Region_DataPayloadNumber		Region_DataPayloadLenNumber + PayloadLen_NUM




//MAC帧格式
#define AESFreamHeader			0x68
#define MACWrite_Cmd_Request	0x20
#define MACErase_Cmd_Request    0x30 

//MAC帧各命令帧长度
#define MAC_Data_Len	4


//功能性事件代码
typedef enum
{
    EVT_Reserve = 0,
	EVT_Event,
	EVT_View_Change,
	EVT_Uart_RxReady,
} EVENT_CODE_E;


//错误代码
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
  Data_error = 0,		//数据格式错误
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


