#ifndef __NET_ROUTE_INFO_H
#define __NET_ROUTE_INFO_H

#include <stdint.h>
#include "protocol_data_def_struct.h"



#define Frame_Up_RetryTimes		62			//上报重试的时间	单位8ms
#define Rout_Retry_Cnt			3			//路由上报的重试次数

#define Rout_Device_MaxSize					20		//作为兄弟节点的上限
#define RoutSeries_MaxSize                  2       //最大路由级数



typedef struct			//一条路径的结构
{
  uint8_t rout_len;		//当前路由表的总级数
  uint8_t rout_id[1 + RoutSeries_MaxSize];  //1字节自身ID+路由表ID
} R_device;

typedef struct			//兄弟节点地址和RSSI值
{
  uint8_t Device_ID;
  uint8_t Device_RSSI;
} Device_list;

typedef struct			//一条主路径，两条备用路径
{
  R_device rout_main;
  R_device rout_reserve1;
  R_device rout_reserve2;
  
  Device_list Dev_list[Rout_Device_MaxSize];
} R_Device;





R_Device *Get_pDevice_rout(void);

WL_Process_Status Dev_list_Record(uint8_t *p_data, uint8_t rout_level, uint8_t rout_len, uint8_t data_dir);

void Routpath_Save(uint8_t rout_len,uint8_t *p_rout_id, uint8_t *addr);
uint8_t Routpath_Read(uint8_t *rout, uint8_t *addr);
void Read_Rout(R_Device *device_rout);

uint8_t Alive_Process(uint8_t *p_data, R_Device *device_rout);
void Alive_Upreport(uint8_t net_Pending_flag, uint8_t* p_cmd_data);
_Bool RF_RoutFrame_Transform(uint8_t *p_data, uint8_t channel_number, uint8_t data_len);

void Filed_Retransmission_Pro(void);




#endif


