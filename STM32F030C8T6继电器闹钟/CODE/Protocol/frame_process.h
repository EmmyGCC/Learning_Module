#ifndef FRAME_PROCESS_H
#define FRAME_PROCESS_H


#include "hk_device.h"
#include "protocol_data_def_struct.h"
//////////////////////// 宏定义 //////////////////////////////////




#define Frame_RetryTimes                   50      //单位为8mS

#define Retry_Buf_Size                      4      //上报BUF数量


/////////////////////////////////////////////////////////////////////////


//上报
#define Local_UpReportCmd                   0x88           //本地上报帧
#define Remote_UpReportCmd                  0x98           //节点上报帧


//接收到的帧用0X98位与来判断
#define Local_CmdFrame                      0x00      //本地命令帧
#define Local_EventFrame                    0x08      //本地事件帧
#define RemoteUp_CmdFrame                   0x10      //上行远程命令帧
#define RemoteUp_EventFrame                 0x18      //上行远程事件帧

//////////////////////////////////////////////////////////////////////////

typedef struct
{
  volatile uint8_t RetryCnt;      //不为0时，表示需要重试，并计算重试时间
  uint8_t Retry_SendLen;
  uint8_t RetryBuf_Number;   //表示用到第几个空间
  uint8_t Retry_DataBuf[APP_BUFF_MAX_LEN];
  uint16_t Retry_DelayCnt;
}Retry_TypDef;

typedef struct
{
  Retry_TypDef Retry_Buf[Retry_Buf_Size];
  uint8_t RetryBuf_Space;   //第几bit代表第几个空间，0为可用，1为已占用
  
  uint8_t OnLine_Staus;
  uint16_t OnLine_Cnt;
  uint8_t FrameProcess_Buf[APP_BUFF_MAX_LEN];
}HKFrame_TypDef;//做数据处理过程使用的






HKFrame_TypDef *Get_pHKFrame_Buf(void);

void RemoteTranspond_Process(uint8_t *p_source, uint8_t len, uint8_t flag);

void RxData_Process(void);

void Frame_Process(uint8_t *p_source);
void UpReport_Start(void *p_srcdata);
void Retransmission_Process(HKFrame_TypDef *p_framebuf);


#endif

