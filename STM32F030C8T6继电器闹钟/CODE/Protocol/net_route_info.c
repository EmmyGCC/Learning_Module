#include <string.h>

#include "net_route_info.h"
#include "rf_interface.h"
#include "net_layer_api.h"
#include "device_basic_info.h"
#include "frame_process.h"
#include "dev_mate_interface.h"
#include "stm32f0_eeprom.h"
#include "m0_systick.h"
#include "crc16.h"
#include "radio.h"



static R_Device Device_rout;								//��¼�豸��·����Ϣ

static _Bool Update_Flag = false;
static uint8_t Update_num[Rout_Device_MaxSize];		//��¼�����˵ĵ�ַ���ֵܽڵ��б���±�
uint8_t cur_upda_num = 0;



R_Device *Get_pDevice_rout(void)
{
    return &Device_rout;
}


/**
�Ƚ��ֵܽڵ��б��еĵ�ַ��RSSIֵ������
**/
static void Dev_list_Cmp(uint8_t rssi_value, R_Device *device_rout, uint8_t dev_id)
{
    uint8_t num = 1;
    
    if(device_rout->Dev_list[num].Device_ID == 0)
        goto ret;
    else
    {	//��ѯ���е�ַ
        while((num < Rout_Device_MaxSize) && (device_rout->Dev_list[num].Device_ID != 0))
        {
            if(dev_id == (device_rout->Dev_list[num].Device_ID))
            {
                if(device_rout->Dev_list[num].Device_RSSI != rssi_value)	//������ͬ��ַ��RSSIֵ
                    goto ret;
                else													//û�и��µĵ�ַ�����������
                    return;
            }
            num++;
        }
        if(num >= Rout_Device_MaxSize)								//ȫ����������û�п���λ�ô�Ŵ˴θ��µĵ�ַ
            return;
    }
  ret:
    Update_num[cur_upda_num++] = num;
    Update_Flag = true;
    device_rout->Dev_list[num].Device_ID = dev_id;
    device_rout->Dev_list[num].Device_RSSI = rssi_value;
    if(cur_upda_num >= Rout_Device_MaxSize)
        cur_upda_num = 0;
}


/**
��¼�ֵܽڵ�ĵ�ַ
**/
WL_Process_Status Dev_list_Record(uint8_t *p_data, uint8_t rout_level, uint8_t rout_len, uint8_t data_dir)
{
    uint8_t dev_id = 0;
    
    if(data_dir == 0)
    {
        if (rout_level != 0)            //���Ǵ���վ������
        {
            if(rout_level <= rout_len)  //ȡ��ǰ������·�ɽڵ�ID����ǰ����Ϊ��һ���ڵ�ļ���������õ�
            {
                dev_id = p_data[RF_RoutPathNumber + rout_level];
                
                Dev_list_Cmp(Get_pWirelessBuf()->Wireless_RSSI_Value, Get_pDevice_rout(), dev_id);
            }
            if(Get_pNet_Buf()->Mate_Flag == 2)  //��Ҫ�豸״̬ͬ���ģ�Ҫ����ȡ����
                return Data_Correct;
            else
                return Record_error;
        }
    }
    else                                //��վ��֡���ֵܽڵ�
    {
        uint8_t mate_flag = 0;
        
        if (rout_len == rout_level)     //Դ�ڵ�ID
            dev_id = p_data[RF_RoutPathNumber];
        else                            //ȡ��һ���ӽڵ�ID
            dev_id = p_data[RF_RoutPathNumber + rout_level + 1];
        
        Dev_list_Cmp(Get_pWirelessBuf()->Wireless_RSSI_Value, Get_pDevice_rout(), dev_id);
        
        if(!(Get_pNet_Buf()->WL_CMD.Alive))
        {
            if(Get_MateDev_ReadyFlag() && !Get_DeviceState_MateChange_Flag(LOW_4BIT))
            {
                if (memcmp(Get_pDev_Mate_Buf()->Self_SWITCH[0].Mate_Switch.Dev_MAC_Addr, &(p_data[RF_SourceNumber]), MAC_Data_Len) == 0)
                {
                    mate_flag = 1;
                }
            }
        }
        if(mate_flag)
        {
            Get_pNet_Buf()->Mate_Flag = 1;
            return Data_Correct;
        }
        else
        {
            Get_pNet_Buf()->Mate_Flag = 0;
            return Data_RoutDevice;
        }
    }
    return Record_error;
}


/**
��ȡ·�ɱ���Ϣ
**/
void Read_Rout(R_Device *device_rout)
{
    device_rout->rout_main.rout_len = Routpath_Read(device_rout->rout_main.rout_id, (uint8_t *)&(Get_pNet_Buf()->Master_Device[0]));
}


/**
ɸѡ��Ψһ����ţ���С�������кô��
**/
void List_Filter(uint8_t *Update_num)
{
    uint8_t i = 0,j = 0,num = 1;
    uint8_t temp = 0;
    _Bool Flag = true;
    
    for(i=0;(i<Rout_Device_MaxSize-1) && (Flag);i++)	//ð������С����
    {
        Flag = false;
        for(j=0;j<(Rout_Device_MaxSize-i-1);j++)
        {
            if(Update_num[j] > Update_num[j+1])
            {
                temp = Update_num[j+1];
                Update_num[j+1] = Update_num[j];
                Update_num[j] = temp;
                Flag = true;
            }
        }
    }
    for(i=1;i<Rout_Device_MaxSize;i++)		//����ԭ���б�
    {
        if(Update_num[num-1] != Update_num[i])	//��һ����ǰһ����ͬ��ֵ������
        {
            Update_num[num++] = Update_num[i];
        }
    }
}


/**
·�ɱ�ͱ�Ҫ��Ϣ�洢��FLASH
**/
void Routpath_Save(uint8_t rout_len,uint8_t *p_rout_id, uint8_t *addr)
{
    Master_list *master_addr = (Master_list *)addr;
    uint8_t Save_len_rout[16] = {0};
    
    Save_len_rout[0] = rout_len;
    Save_len_rout[1] = *p_rout_id;
    p_rout_id++;
    Save_len_rout[2] = *p_rout_id;
    p_rout_id++;
    Save_len_rout[3] = *p_rout_id;
    
    memmove(&Save_len_rout[4], master_addr, 3*MAC_Data_Len);
    
    Get_pSystemFunc()->FLASH_Erase(ROUT_INFO_SAVE_ADDR);
    Get_pSystemFunc()->FLASH_Write(ROUT_INFO_SAVE_ADDR, &Save_len_rout, sizeof(Save_len_rout));
}


/**
·�ɱ�ͱ�Ҫ��Ϣ����
**/
uint8_t Routpath_Read(uint8_t *rout, uint8_t *addr)
{
    Master_list *master_addr = (Master_list *)addr;
    uint8_t p_buf[16] = {0};
    uint8_t rout_len;
    
    Get_pSystemFunc()->FLASH_Read(ROUT_INFO_SAVE_ADDR, &p_buf, sizeof(p_buf));
    rout_len = p_buf[0];
    rout[0] = p_buf[1];
    rout[1] = p_buf[2];
    rout[2] = p_buf[3];
    
    memmove(&master_addr[0], &p_buf[4], MAC_Data_Len);
    memmove(&master_addr[1], &p_buf[8], MAC_Data_Len);
    memmove(&master_addr[2], &p_buf[12], MAC_Data_Len);
    
    return rout_len;
}




/**
����֡�Ļ�Ӧ����
**/
uint8_t Alive_Process(uint8_t *p_data, R_Device *device_rout)
{
    uint8_t *p_buf = Get_pHKFrame_Buf()->FrameProcess_Buf;
    memmove(Get_pHKFrame_Buf()->FrameProcess_Buf, p_data, pRadioConfiguration->Radio_PacketLength);
    volatile uint8_t List_cnt = 0;
    //�����յ������ݻ�Ӧ����֡��·�ɱ�����
    uint8_t len = p_buf[RF_RoutTabLenNumber];
    uint8_t com_mode = p_buf[RF_RoutLevelNumber] & 0X0F;
    
    memset(&p_buf[RF_HeaderNumber], RF_Frame_HEAD, 2);
    p_buf[RF_CmdNumber] = 0X81;
    p_buf[RF_CmdNumber] |= (1<<2);
    memmove(&p_buf[RF_GroupAddrNumber], &(Get_pDevAesInfo()->addr_GA[1]), 2);
    memmove(&p_buf[RF_DestinNumber], &(Get_pNet_Buf()->Master_Device[0]), MAC_Data_Len);
    memmove(&p_buf[RF_SourceNumber], Get_pSelfDevMacInfo()->mac, MAC_Data_Len);
    p_buf[RF_RoutLevelNumber] = ((len<<4) | com_mode);
    
    
    if(Update_Flag)					//�и���������б�
    {
        uint8_t temp_list[Rout_Device_MaxSize * 2] = {0};
        Device_list dev_list[Rout_Device_MaxSize] = {0};
        uint8_t list_num = 1;
        
        List_Filter(Update_num);
        
        for(list_num=1;list_num<Rout_Device_MaxSize;list_num++)
        {
            if(Update_num[list_num] <= Update_num[list_num-1])		//��һ��С��ǰһ�����±����ѱ�������
            {
                list_num--;
                break;
            }
            else
                dev_list[list_num] = device_rout->Dev_list[ Update_num[list_num] ];
        }
        
        do
        {
            if(dev_list[list_num].Device_ID != 0)
            {
                temp_list[List_cnt++] = dev_list[list_num].Device_ID;
                temp_list[List_cnt++] = dev_list[list_num].Device_RSSI;
            }
        }
        while((list_num--) != 0)
            ;
        
        memmove(&p_buf[RF_RoutPathNumber + p_buf[RF_RoutTabLenNumber]],temp_list,List_cnt);	//�����ֵܽڵ�ĸ����б�
        Update_Flag = false;
        cur_upda_num = 0;
        //������µ�ַ�±���б�
        memset(Update_num,0,Rout_Device_MaxSize);
    }
    
    Set_NetBuf_Pending_flag(0);
    return Frame_Check(p_buf,List_cnt + RF_RoutPathNumber + p_buf[RF_RoutTabLenNumber]);
}


/**
����֡�Ĵ������Լ��ķ���ȥ�������Լ���׼��ת��
**/
void Alive_Upreport(uint8_t net_Pending_flag, uint8_t* p_cmd_data)
{
    uint8_t send_len = 0;
    WL_CMD_typ *p_net_cmd = (WL_CMD_typ *)p_cmd_data;
    
    if(net_Pending_flag == Self_AliveFrame)
    {
        send_len = Alive_Process(Get_pWirelessBuf()->Wireless_RxData, Get_pDevice_rout());
        if(send_len)
            Si4438_Transmit_Start(Get_pWirelessBuf(), Get_RF_Channel(), Get_pHKFrame_Buf()->FrameProcess_Buf, send_len);
    }
    else if(p_net_cmd->Self_Rout)
    {
        p_net_cmd->Self_Rout = 0;
        Set_DeviceState_Pending_Flag(SET_PendFlag, 0x40);
    }
    p_net_cmd->Alive = 0;
}


void Filed_Retransmission_Pro(void)
{
    static uint8_t current_ID_num = 0;
    Retry_TypDef *p_retry;
    p_retry = &(Get_pHKFrame_Buf()->Retry_Buf[Retry_Buf_Size-2]);
    
    if(WIRELESS_STATUS == Wireless_TX_Sending)return;
    if(p_retry->RetryCnt > 0)
    {
        if(++p_retry->Retry_DelayCnt > Frame_Up_RetryTimes)		//��·��Ϊ600ms��ʱ
        {
            p_retry->Retry_DelayCnt = 0;							//�������Ե�ʱ��
            uint8_t *p_retrydata = p_retry->Retry_DataBuf;
            memmove(&p_retrydata[RF_DestinNumber],&(Get_pNet_Buf()->Master_Device[++current_ID_num]),MAC_Data_Len);
            
            uint16_t crc16_val;
            crc16_val = CRC16_2(p_retrydata,p_retry->Retry_SendLen - 4);
            p_retrydata[p_retry->Retry_SendLen - 4] = crc16_val >> 8;
            p_retrydata[p_retry->Retry_SendLen - 3] = crc16_val & 0xff;
            
            Si4438_Transmit_Start(Get_pWirelessBuf(), Get_RF_Channel(), p_retrydata, p_retry->Retry_SendLen);
            
            if(--p_retry->RetryCnt == 0)							//����Ҫ���ԵĴ���
            {
                if(++current_ID_num > 2)
                {
                    current_ID_num = 0;
                    return;
                }
                p_retry->RetryCnt = Rout_Retry_Cnt;					//������ԣ��ط���3��
            }
        }
    }
}


_Bool RF_RoutFrame_Transform(uint8_t *p_data, uint8_t channel_number, uint8_t data_len)
{
    if(Get_pNet_Buf()->Pending_flag == NSelf_RoutFrame)											//�����Լ���֡,Ҫ�Լ�·�ɵ�
    {
        memmove(Get_pHKFrame_Buf()->Retry_Buf[Retry_Buf_Size-1].Retry_DataBuf, Get_pWirelessBuf()->Wireless_RxData, pRadioConfiguration->Radio_PacketLength);
        uint8_t *wl_data = Get_pHKFrame_Buf()->Retry_Buf[Retry_Buf_Size-1].Retry_DataBuf;
        
        SysDelay_Xms(100);
        uint8_t send_len = WL_Data_process(wl_data, (uint8_t *)Get_pDevice_rout(), p_data, data_len);
#ifdef USE_IWDG
        WDT_FEED();
#endif
        Si4438_Transmit_Start(Get_pWirelessBuf(), channel_number, wl_data, send_len);
        
        Set_DeviceState_Pending_Flag(Clr_PendFlag, HIGH_4BIT);
        
        Set_NetBuf_Pending_flag(0);
        if(WIRELESS_STATUS != Wireless_TX_Sending)
            Si4438_Receive_Start(DEFAULT_CHANNEL);   //Start Receive
        return 1;
    }
    return 0;
}




