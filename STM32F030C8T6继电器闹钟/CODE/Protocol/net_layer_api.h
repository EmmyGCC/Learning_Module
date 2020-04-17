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
//#define _Error_Init_Var_	//���ݴ���ʱ�Ƿ�Ҫ��ʼ�����б�־


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


#define NSelf_NRoutFrame					0X01	//�����Լ���֡����Ҫ�Լ�·�ɵ�
#define Self_RoutFrame						0X02	//���Լ���֡������Ҫ·��
#define Self_NRoutFrame						0X04	//���Լ���֡������Ҫ·��
#define NSelf_RoutFrame						0X08	//�����Լ���֡��Ҫ�Լ�·�ɵ�
#define Self_AliveFrame						0x10	//����֡


/********************  Wireless struct define Start *******************/


typedef struct              //һЩ����λ������
{
  uint8_t Self_Frame:1;     //bit 0: 0:�����豸��֡ 1:�Լ���֡
  uint8_t Work_Mode: 1;     //bit 1: 0:����ģʽ 1:LDCģʽ
  uint8_t Use_rout:  1;     //bit 2: 0:û·�� 1:��·��
  uint8_t Comm_Mode: 1;     //bit 3: 0:PLC 1:WireLess
  uint8_t Com_Type:  1;     //bit 4: 0:����֡ 1:ͨ��֡
  uint8_t Self_Rout: 1;     //bit 5: 0:��Ҫ�Լ�·�� 1:Ҫ�Լ�·��
  uint8_t Alive:     1;		//bit 6: 0:������֡  1:����֡
  uint8_t Direction: 1;		//bit 7: 0:��վ����	 1����վ����
}WL_CMD_typ;

typedef struct
{
  uint8_t HK_Device_ID[MAC_Data_Len];
}Master_list;

typedef struct
{
  WL_Status_TypDef Status;              //�����Ŀǰ�����е�״̬
  Master_list Master_Device[3];			//��ǰ�豸����վID�б�
  uint8_t Frame_Data_Len;               //���Ӧ�ò����ݵĳ���
  uint8_t WL_Data_Buf[WLFrame_LenMax];  //���Ӧ�ò�����
  WL_CMD_typ WL_CMD;
  uint8_t Pending_flag;					//���ڱ�ʶ���ڵ������Э��������
  uint8_t Mate_Flag;					//1����վ��ӳ�俪���ϱ�֡	2����վ��ӳ�俪��ͬ��֡
}Network_Protocol_TypDef;

/****************************************************************************/
typedef enum
{
    DECODE = 0,
    ENCODE
}CODE74_TYPE_E;


//·�ɲ�Э��֡������ṹ
typedef struct
{
    uint8_t type : 1;	//֡���ͣ�1ͨ�ţ�0����
    uint8_t mode : 1;	//֡ģʽ��0������1LDC����ʡ��
    uint8_t heart : 1;	//����֡��־
    uint8_t nn : 4;		//Ԥ��
    uint8_t dir : 1;	//���䷽��0����1��
} FRAME_ROUTER_CTRL_T;


//·�ɼ�����ÿһ����Ӧ��ͨ�ŷ�ʽ
typedef struct
{
    uint8_t type : 4;  //ͨ�ŷ�ʽ����ÿһλ��ʶ����·�ɣ�0��ʾ�������ز���1��ʾ����
    uint8_t index : 4; //��ǰ·�ɵ��Ｖ��ָʾ
} FRAME_ROUTER_NUMBER_T;


//·�ɲ�Э����վ����֡�ṹ
typedef struct
{
    uint8_t head_h;
    uint8_t head_l;
    uint8_t frame_len;				 //֡���ȣ����������ֽڵ�������ǰ���ֽ�����
    uint8_t addr_len;				 //��ַ����,����֡�ĵ�ַ����ʱӦ��ʹ�ô��ֶ�
    FRAME_ROUTER_CTRL_T ctrl;        //֡����
    uint8_t netNum[2];               //����ţ�ȡȺ���ַ�ĵ�2���ֽ�
    uint8_t des_addr[MAC_Data_Len];	 //Ŀ���ַ
    uint8_t src_addr[MAC_Data_Len];	 //Դ��ַ
    FRAME_ROUTER_NUMBER_T routerNum; //·�ɼ���
    uint8_t router_len;              //·�ɱ���
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
uint8_t Frame_Check(uint8_t *p,uint8_t Len);				//���ڶԱ���������У��
uint8_t RealData_Detect(uint8_t *p,uint8_t Frame_len);		//���ڶԽ���ǰ������У��
uint8_t Decode_74(uint8_t *wl_data);
uint8_t Code_74(uint8_t *addr, uint8_t *p_TxData, uint8_t length);


#endif
