#ifndef _ROUT_H
#define _ROUT_H

#include "stm32f0xx.h"
#include "protocol_data_def_struct.h"




#ifndef false
#define false	0
#endif

#ifndef true
#define true	(!false)
#endif
//#define _Error_Init_Var_	//数据错误时是否要初始化所有标志


#define FRAME_ROUTER_LEN	(2+2+1+2+MAC_Data_Len+MAC_Data_Len+1+1+1)

#define RF_Frame_HEAD   0x69
#define RF_Frame_END    0x96

#define RF_HeaderNumber                     0
#define RF_LenNumber                        2
#define RF_CmdNumber                        4
#define RF_GroupAddrNumber					5
#define RF_DestinNumber                     7
#define RF_SourceNumber                     RF_DestinNumber + MAC_Data_Len
#define RF_RoutLevelNumber                  RF_SourceNumber + MAC_Data_Len
#define RF_RoutTabLenNumber					RF_RoutLevelNumber + 1
#define RF_RoutPathNumber                   RF_RoutTabLenNumber + 1
#define RF_PayloadNumber                    RF_RoutPathNumber + 1

#define ENCODE_MAX_DATALEN                  80

#define WLFrame_LenMax                      APP_BUFF_MAX_LEN


#define Com_Mode                            0X01
#define Ldc_Mode                            0X02
#define Online								0X04
#define Master								0x80
#define Mate_Mode							0X08
#define Mate_Respond						0X10


#define NSelf_NRoutFrame					0X01	//不是自己的帧，不要自己路由的
#define Self_RoutFrame						0X02	//是自己的帧，还需要路由
#define Self_NRoutFrame						0X04	//是自己的帧，不需要路由
#define NSelf_RoutFrame						0X08	//不是自己的帧，要自己路由的
#define Self_AliveFrame						0x10	//心跳帧


/********************  Wireless struct define Start *******************/


typedef struct              //一些命令位的意义
{
  uint8_t Self_Frame:1;     //bit 0: 0:其他设备的帧 1:自己的帧
  uint8_t Work_Mode: 1;     //bit 1: 0:正常模式 1:LDC模式
  uint8_t Use_rout:  1;     //bit 2: 0:没路由 1:用路由
  uint8_t Comm_Mode: 1;     //bit 3: 0:PLC 1:WireLess
  uint8_t Com_Type:  1;     //bit 4: 0:组网帧 1:通信帧
  uint8_t Self_Rout: 1;     //bit 5: 0:不要自己路由 1:要自己路由
  uint8_t Alive:     1;		//bit 6: 0:非心跳帧  1:心跳帧
  uint8_t Direction: 1;		//bit 7: 0:主站发来	 1：从站发来
}WL_CMD_typ;

typedef struct
{
  uint8_t HK_Device_ID[MAC_Data_Len];
}Master_list;

typedef struct
{
  WL_Status_TypDef Status;              //网络层目前接收中的状态
  Master_list Master_Device[3];			//当前设备的主站ID列表
  uint8_t Frame_Data_Len;               //存放应用层数据的长度
  uint8_t WL_Data_Buf[WLFrame_LenMax];  //存放应用层数据
  WL_CMD_typ WL_CMD;
  uint8_t Pending_flag;					//用于标识现在的网络层协议解析结果
  uint8_t Mate_Flag;					//1：从站的映射开关上报帧	2：主站的映射开关同步帧
}Network_Protocol_TypDef;

/****************************************************************************/
typedef enum
{
    DECODE = 0,
    ENCODE
}CODE74_TYPE_E;


//路由层协议帧控制域结构
typedef struct
{
    uint8_t type : 1;	//帧类型，1通信，0组网
    uint8_t mode : 1;	//帧模式，0正常，1LDC休眠省电
    uint8_t heart : 1;	//心跳帧标志
    uint8_t nn : 4;		//预留
    uint8_t dir : 1;	//传输方向，0主，1从
} FRAME_ROUTER_CTRL_T;


//路由级数和每一级对应的通信方式
typedef struct
{
    uint8_t type : 4;  //通信方式，按每一位标识各级路由，0表示电力线载波，1表示无线
    uint8_t index : 4; //当前路由到达级数指示
} FRAME_ROUTER_NUMBER_T;


//路由层协议主站发出帧结构
typedef struct
{
    uint8_t head_h;
    uint8_t head_l;
    uint8_t frame_len;				 //帧长度（包括长度字节到结束符前面字节数）
    uint8_t addr_len;				 //地址长度,解析帧的地址索引时应该使用此字段
    FRAME_ROUTER_CTRL_T ctrl;        //帧控制
    uint8_t netNum[2];               //网络号，取群组地址的低2个字节
    uint8_t des_addr[MAC_Data_Len];	 //目标地址
    uint8_t src_addr[MAC_Data_Len];	 //源地址
    FRAME_ROUTER_NUMBER_T routerNum; //路由级数
    uint8_t router_len;              //路由表长度
} FRAME_ROUTER_MASTER_CMD_T;


/********************  Wireless struct define End  *******************/


Network_Protocol_TypDef *Get_pNet_Buf(void);
void Set_NetBuf_Pending_flag(uint8_t flag_val);
uint8_t *Get_NetBuf_pWL_Data_Buf(void);
WL_Status_TypDef Get_Net_Status(void);
void Set_Net_Status(WL_Status_TypDef cur_status);


WL_Process_Status WireLess_RX_Process(uint8_t *p_netData, Network_Protocol_TypDef* pNet);


uint8_t WL_Data_process(uint8_t *p_wl, uint8_t *p_device_rout, uint8_t *p_buf, uint8_t send_len);
WL_Process_Status Net_Protocol_Process(uint8_t *p_NetData);

void UpReport_WL_Process(uint8_t *p_src_data, uint8_t *send_len);


uint8_t WL_FrameData_Detect(uint8_t *pNetdata, uint8_t len);
uint8_t Frame_Check(uint8_t *p,uint8_t Len);				//用于对编码后的数据校验
uint8_t RealData_Detect(uint8_t *p,uint8_t Frame_len);		//用于对解码前的数据校验
uint8_t Decode_74(uint8_t *wl_data);
uint8_t Code_74(uint8_t *addr, uint8_t *p_TxData, uint8_t length);


#endif
