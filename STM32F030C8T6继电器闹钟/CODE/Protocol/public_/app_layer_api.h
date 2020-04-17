#ifndef __APP_LAYER_API_H
#define __APP_LAYER_API_H

#include <stdint.h>



//帧控制命令对应的位定义
#define FCMD_CTLFUNC_BIT	(0X07<<0)	//功能码
#define FCMD_BUSY_BIT       (1<<5)      //忙碌等待标志
#define FCMD_ERROR_BIT		(1<<6)      //应答正确与否标志
#define FCMD_DIR_BIT		(1<<7)      //传输方向 1从站

//帧数据操作码对应的位定义
#define FDAC_UNITNUM_BIT	(0X7F<<0)	//设备单元
#define FDAC_RDWR_BIT		(1<<7)		//读写标志	1：读 0：写

//帧数据区标识头对应的位定义
#define FDATA_DATA_MARK_BIT			(1<<3)		//负载含有数据标识
#define FDATA_DATA_VALID_BIT		(1<<2)		//数据包含有效负载
#define FDATA_SRCADDR_VALID_BIT		(1<<1)		//源地址有效
#define FDATA_DESADDR_VALID_BIT		(1<<0)		//目标地址有效

#define NOSRCADDR_DATAVALID         1
#define NODATA_SRCADDRVALID         2
#define SRCADDR_DATA_VALID          3

//帧用户数据区的协议层标识固定的ID字段定义
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
/* 如果为事件帧，没有以下格式
应用层的用户数据区整体格式:

1，数据区标识头：
* x 1字节，备 | 备 | 备 | 备 | 负载含有数据标识 | 数据包含有效负载 | 源地址有效 | 目标地址有效

2，源地址：
* x 4字节

3，数据区有效负载：
4字节 负载长度
n字节 负载数据

4，协议层标识（按先后排序）（3+38字节）：
1字节 层数
2字节 协议层标识数据字节总长度

① 压缩处理层
1字节ProtocolId，固定为 1
1字节HeaderId，  固定为 3
---
4字节 original_size

② 碎片处理层
1字节ProtocolId，固定为 2
1字节HeaderId，  固定为 4
---
8字节 帧序列号
4字节 碎片编号
4字节 碎片总数

③ 可靠传输层
1字节ProtocolId，固定为 3
1字节HeaderId，  固定为 5
---
1字节 消息类型   [DATA = 0, ACK = 1, NAK = 2]
8字节 帧序列号   [DATA, ACK, NAK]
2字节 会话Id     [DATA]
1字节 起始标识码 [DATA]

④ 流量控制层
1字节ProtocolId，固定为 4
---

--暂无协议头数据


串口数据范例：
AC 00 00 01 03 00 22 [06] [00 00 01 03] [00 00 00 04] [00 01 06 33] [01] [00 12] [03] [05] [00] [00 00 00 00 00 00 00 01] [00 00] [01]  CRC16 53
                                          负载长度      负载数据    size  length  协议层标识数据，如：NAK ACK DATA ...
*/

/**
① 压缩处理层
 1字节ProtocolId，固定为 1
 1字节HeaderId，固定为 3
--- 
 4字节 original_size
*/
typedef struct
{
    uint8_t protocol_id;
    uint8_t header_id;
    uint32_t original_size;
} COMPRESSION_HANDLE_T;


/**
② 碎片处理层
 1字节ProtocolId，固定为 2
 1字节HeaderId，固定为 4
--- 
 8字节 帧序列号  需要时读取
 4字节 碎片编号  需要时读取，电器升级时需要写修改
 4字节 碎片总数  需要时读取
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
③ 可靠传输层
 1字节ProtocolId，固定为 3
 1字节HeaderId，固定为 5
--- 
 1字节 消息类型 [DATA = 0, ACK = 1]
 8字节 帧序列号 [DATA, ACK]
 2字节 会话Id [DATA]		//如果为ACK，不处理这个DATA字段
 1字节 起始标识码 [DATA]
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


//对帧字段各域的操作类型例举
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


_Bool AppFrameData_FormatVerify(uint8_t *p, uint8_t len);   //指定数据的起始地址对数据检查
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

