#ifndef FRAME_PROCESS_H
#define FRAME_PROCESS_H


#include "hk_device.h"
#include "protocol_data_def_struct.h"
//////////////////////// �궨�� //////////////////////////////////




#define Frame_RetryTimes                   50      //��λΪ8mS

#define Retry_Buf_Size                      4      //�ϱ�BUF����


/////////////////////////////////////////////////////////////////////////


//�ϱ�
#define Local_UpReportCmd                   0x88           //�����ϱ�֡
#define Remote_UpReportCmd                  0x98           //�ڵ��ϱ�֡


//���յ���֡��0X98λ�����ж�
#define Local_CmdFrame                      0x00      //��������֡
#define Local_EventFrame                    0x08      //�����¼�֡
#define RemoteUp_CmdFrame                   0x10      //����Զ������֡
#define RemoteUp_EventFrame                 0x18      //����Զ���¼�֡

//////////////////////////////////////////////////////////////////////////

typedef struct
{
  volatile uint8_t RetryCnt;      //��Ϊ0ʱ����ʾ��Ҫ���ԣ�����������ʱ��
  uint8_t Retry_SendLen;
  uint8_t RetryBuf_Number;   //��ʾ�õ��ڼ����ռ�
  uint8_t Retry_DataBuf[APP_BUFF_MAX_LEN];
  uint16_t Retry_DelayCnt;
}Retry_TypDef;

typedef struct
{
  Retry_TypDef Retry_Buf[Retry_Buf_Size];
  uint8_t RetryBuf_Space;   //�ڼ�bit����ڼ����ռ䣬0Ϊ���ã�1Ϊ��ռ��
  
  uint8_t OnLine_Staus;
  uint16_t OnLine_Cnt;
  uint8_t FrameProcess_Buf[APP_BUFF_MAX_LEN];
}HKFrame_TypDef;//�����ݴ������ʹ�õ�






HKFrame_TypDef *Get_pHKFrame_Buf(void);

void RemoteTranspond_Process(uint8_t *p_source, uint8_t len, uint8_t flag);

void RxData_Process(void);

void Frame_Process(uint8_t *p_source);
void UpReport_Start(void *p_srcdata);
void Retransmission_Process(HKFrame_TypDef *p_framebuf);


#endif

