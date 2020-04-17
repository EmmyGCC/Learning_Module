#include <string.h>

#include "net_layer_api.h"
#include "app_layer_api.h"
#include "dev_mate_interface.h"
#include "device_basic_info.h"
#include "74.h"
#include "crc16.h"
#include "radio.h"
#include "net_route_info.h"


/* ������������⣬�ܶຯ���Ĳ������Է��뿪��ģ����ģ��֮����� */
Network_Protocol_TypDef Net_Buf;



inline Network_Protocol_TypDef *Get_pNet_Buf(void)
{
    return &Net_Buf;
}


inline void Set_NetBuf_Pending_flag(uint8_t flag_val)
{
    Get_pNet_Buf()->Pending_flag = flag_val;
}


inline uint8_t *Get_NetBuf_pWL_Data_Buf(void)
{
    return Get_pNet_Buf()->WL_Data_Buf;
}


inline WL_Status_TypDef Get_Net_Status(void)
{
    return Get_pNet_Buf()->Status;
}


inline void Set_Net_Status(WL_Status_TypDef cur_status)
{
    Get_pNet_Buf()->Status = cur_status;
}


//���ڳ�ʼ�����б�־Data_Error(pNet)
WL_Process_Status Data_Error(Network_Protocol_TypDef* pNet)
{
    WL_CMD_typ* p_cmd = &(pNet->WL_CMD);
    
    p_cmd->Self_Frame = 0;
    p_cmd->Work_Mode = 0;
    p_cmd->Use_rout = 0;
    p_cmd->Comm_Mode = 0;
    p_cmd->Com_Type = 0;
    p_cmd->Self_Rout = 0;
    p_cmd->Alive = 0;
    p_cmd->Direction = 0;
    
    return Data_error;
}




/**
��վ��ַ�Ĵ洢����
**/
static void Master_ID_Update(Network_Protocol_TypDef* pNet, uint8_t *p_data)
{
    memmove(&pNet->Master_Device[2], &pNet->Master_Device[1], MAC_Data_Len);
    memmove(&pNet->Master_Device[1], &pNet->Master_Device[0], MAC_Data_Len);
    memmove(&pNet->Master_Device[0], &(p_data[RF_SourceNumber]), MAC_Data_Len);
}


/**
���������Э���������
**/
WL_Process_Status WireLess_RX_Process(uint8_t *p_netData, Network_Protocol_TypDef* pNet)
{
    static uint8_t Rout_Level = 0;	//��ǰ��·�ɼ���,��·�ɵĽṹ����Ҫ����˱���ʹ��
    WL_CMD_typ* p_cmd = &(pNet->WL_CMD);
    
    
    switch(pNet->Status)
    {
      case WL_RX_Cmd:
        {
            uint8_t Cmd = p_netData[RF_CmdNumber];
            if((Cmd & 0X01) == Com_Mode)          //ͨ��֡
                p_cmd->Com_Type = 1;
            else
                p_cmd->Com_Type = 0;
            
            if((Cmd & 0X02) == Ldc_Mode)
                p_cmd->Work_Mode = 1;
            else
                p_cmd->Work_Mode = 0;
            
            if((Cmd & 0X04) == Online)			//����
                p_cmd->Alive = 1;
            else
                p_cmd->Alive = 0;
            
            if((Cmd & 0X80) == Master)
                p_cmd->Direction = 1;
            else
                p_cmd->Direction = 0;
            
            pNet->Status = WL_RX_Addr;
        }
        break;
      case WL_RX_Addr:
        {
            if(p_cmd->Com_Type)
            {
                if(memcmp(&(p_netData[RF_GroupAddrNumber]), &(Get_pDevAesInfo()->addr_GA[1]), 2) != 0)
#ifdef _Error_Init_Var_
                    return Data_Error(pNet);
#else
                return Addr_error;
#endif
            }
            pNet->Status = WL_RX_Destin;
        }
        break;
      case WL_RX_Destin:
        {
            if(!(p_cmd->Direction))		//��վ����
            {
                if (memcmp(&(p_netData[RF_DestinNumber]), Get_pSelfDevMacInfo()->mac, MAC_Data_Len) == 0)
                    p_cmd->Self_Frame = 1;	//���Լ�Ҫ�����֡
                else
                {
                    p_cmd->Self_Frame = 0;
                    
                    if ((p_cmd->Com_Type) && (!(p_cmd->Alive)))	//����������ͨ��֡
                    {
                        if (Get_MateDev_ReadyFlag() && (AppFrameGet_Cmd(&p_netData[RF_PayloadNumber], E_GET_crl_funcCode) != EVT_Event))
                        {
                            if (memcmp(Get_pDev_Mate_Buf()->Self_SWITCH[0].Mate_Switch.Dev_MAC_Addr, &(p_netData[RF_DestinNumber]), MAC_Data_Len) == 0)
                            {
                                pNet->Mate_Flag = 2;    //�豸״̬ͬ������
                            }
                        }
                    }
                }
            }
            else
                p_cmd->Self_Frame = 0;
            
            pNet->Status = WL_RX_Source;
        }
        break;
      case WL_RX_Source:
        {
            if (!(p_cmd->Direction))		//��վ����
            {
                if (p_cmd->Self_Frame)
                {
                    //�ʹ洢����վ��ַ��һ��ʱ�͸����б�
                    if (memcmp(&pNet->Master_Device[0], &(p_netData[RF_SourceNumber]), MAC_Data_Len) != 0)
                        Master_ID_Update(pNet, p_netData);
                }
            }
            pNet->Status = WL_RX_RoutLevel;
        }
        break;
      case WL_RX_RoutLevel:
        {
            uint8_t level = p_netData[RF_RoutLevelNumber];
            uint8_t routTab_len = p_netData[RF_RoutTabLenNumber];
            
            Rout_Level = ((level & 0XF0) >> 4); //��ǰ�����·�ɼ���
            
            if ((p_cmd->Work_Mode) && (Rout_Level == routTab_len))	//����LDCԴ�豸��֡������¼�ֵܽڵ�
#ifdef _Error_Init_Var_
                return Data_Error(pNet);
#else
            return RoutLevel_error;
#endif
            //���·�ɼ����Ƿ���ȷ
            if ((Rout_Level <= RoutSeries_MaxSize) && (Rout_Level <= routTab_len))
            {
                if (routTab_len > 0)
                {
                    p_cmd->Use_rout = 1;
#ifdef Use_Rout
                    Dev_list_Record(p_netData, Rout_Level, routTab_len, p_cmd->Direction);
#endif
                }
                else    //������·�ɱ�
                {
                    p_cmd->Use_rout = 0;
                    
                    if (!p_cmd->Self_Frame)
                    {
                        WL_Process_Status ret_val = Data_error;
#ifdef Use_Rout
                        ret_val = Dev_list_Record(p_netData, Rout_Level, routTab_len, p_cmd->Direction);
#endif
                        if(pNet->Mate_Flag)
                        {
                            pNet->Status = WL_RX_FrameData;
                            break;
                        }
#ifdef _Error_Init_Var_
                        return Data_Error(pNet);
#else
                        return ret_val;
#endif
                    }
                    else
                    {
                        p_cmd->Self_Rout = 0;
                        pNet->Status = WL_RX_FrameData;
                        break;
                    }
                }
                /*uint8_t Rout_Mode = !!((level & 0X0F) & (1<<Rout_Level));   �ز��Ƿ��õ���//ȡ����λ�е�ͨѶ��ʽ*/
                
                pNet->Status = WL_RX_RoutPath;
            }
            else
#ifdef _Error_Init_Var_
                return Data_Error(pNet);
#else
            return RoutLevel_error;
#endif
        }
        break;
      case WL_RX_RoutPath:
        {
            pNet->Status = WL_RX_FrameData;
            
            if (!(p_cmd->Direction))    //��վ����ʱ������Ϊ��һ��
            {
                if(p_netData[RF_RoutPathNumber + Rout_Level + 1] == Get_pDevAesInfo()->self_ID)
                    p_cmd->Self_Rout = 1;
                else
                {
                    p_cmd->Self_Rout = 0;
#ifdef _Error_Init_Var_
                    return Data_Error(pNet);
#else
                    return Record_error;
#endif
                }
            }
            else
            {
                if(p_netData[RF_RoutPathNumber + Rout_Level] == Get_pDevAesInfo()->self_ID)
                    p_cmd->Self_Rout = 1;
                else
                {
                    p_cmd->Self_Rout = 0;
#ifdef _Error_Init_Var_
                    return Data_Error(pNet);
#else
                    return Record_error;
#endif
                }
            }
        }
        break;
      case WL_RX_FrameData:
        {
            pNet->Frame_Data_Len = p_netData[RF_LenNumber] - FRAME_ROUTER_LEN - p_netData[RF_RoutTabLenNumber];   //���ݳ���-Э�鳤��(����CRC�ͳ����ֽڱ���)-·�ɱ���
            //�˴�Ϊû��·�ɱ�������
            memmove(Get_NetBuf_pWL_Data_Buf(), &p_netData[RF_PayloadNumber], pNet->Frame_Data_Len);
            pNet->Status = WL_RX_Finish;
            
            if(pNet->Mate_Flag == 1)	//��ӳ�俪�ص�����
                return Slave_Mate_Data;
            else if(pNet->Mate_Flag == 2)
                return Master_Mate_Data;
        }
        break;
      default:
#ifdef _Error_Init_Var_
        return Data_Error(pNet);
#else
        return Status_error;
#endif
        break;
    }
    return Data_Correct;
}


/*
�����Э���õ�У�麯��
*/
uint8_t WL_FrameData_Detect(uint8_t *pNetdata, uint8_t len)
{
    uint16_t crc16_val = 0;
    
    if((pNetdata[RF_HeaderNumber] == RF_Mate_HEAD)
       && (pNetdata[RF_HeaderNumber + 1] == RF_Mate_HEAD))
        Set_MateFrame_Flag(1);
    else
        Set_MateFrame_Flag(0);
    
    crc16_val = CRC16_2(pNetdata,len);
    
    if((crc16_val >> 8) != pNetdata[len])
        return 1;
    if((crc16_val & 0xff) != pNetdata[len + 1])
        return 1;
    
    return 0;
}

/**
���߱�����У�鴦��
**/
uint8_t Frame_Check(uint8_t *p,uint8_t Len)
{
    uint16_t crc16_val = 0;
    
    p[RF_LenNumber] = Len;
    p[RF_LenNumber + 1] = (MAC_Data_Len & 0X07);
    
    crc16_val = CRC16_2(p,Len);
    
    p[Len++] = crc16_val >> 8;
    p[Len++] = crc16_val & 0xff;
    //֡β��CRC�Ĵ���Ӧ���ڱ����
    memset(&p[Len], RF_Frame_END, 2);
    Len+=2;
    
    return Len;
}




/**
���������Э���������
**/
WL_Process_Status Net_Protocol_Process(uint8_t *p_NetData)
{
    WL_Process_Status result = Data_Correct;		//������ߴ���Ľ��
    WL_CMD_typ* p_cmd = &(Get_pNet_Buf()->WL_CMD);
    
    if (WL_FrameData_Detect(p_NetData, p_NetData[RF_LenNumber]) == 0)  //check NetLevel Protocol
    {
        if (Get_MateFrame_Flag())	//�����֡��ʽ
        {
            while (result == Data_Correct)
            {
                result = WireLess_Mate_Process(p_NetData, Get_pDev_Mate_Buf());	//��������õ������Э�����
                
                if (Get_DevMate_Status() == WL_RX_End)
                {
                    //Ҫ�ظ������֡
                    Mate_Respond_process();
                    break;
                }
                else if(Get_DevMate_Status() == WL_RX_Finish)
                    break;
            }
            Set_DevMate_Status(WL_RX_Cmd);
            
            return result;
        }
        else
        {
            while (result == Data_Correct)
            {
                result = WireLess_RX_Process(p_NetData, Get_pNet_Buf());//�����Э�鴦��
                
                if (Get_Net_Status() == WL_RX_Finish)
                    break;
            }
            Set_Net_Status(WL_RX_Cmd);
            
            if ((result != Data_Correct) && (result != Data_RoutDevice))
                return result;
        }
    }
    else    //CRC check Error
    {
        result = Data_error;
        return result;
    }
    
    
    if ((!(p_cmd->Self_Frame)) && (!(p_cmd->Self_Rout)))	//�����Լ���֡����Ҫ�Լ�·�ɵģ��յ�·�ɽڵ��֡��
    {
        Set_NetBuf_Pending_flag(NSelf_NRoutFrame);
        return Data_RoutDevice;
    }
    if ((p_cmd->Self_Frame) && (p_cmd->Use_rout))			//���Լ���֡������Ҫ·�ɣ���ǰ�յ��Լ���֡-->�Ȳ��ܴ������
    {
        Set_NetBuf_Pending_flag(Self_RoutFrame);
        return Data_error;
    }
    if ((p_cmd->Self_Frame) && (!(p_cmd->Use_rout)))		//���Լ���֡������Ҫ·��
    {
        Set_NetBuf_Pending_flag(Self_NRoutFrame);
    }
    if ((!(p_cmd->Self_Frame)) && (p_cmd->Self_Rout))		//�����Լ���֡��Ҫ�Լ�·�ɵ�
    {
        Set_NetBuf_Pending_flag(NSelf_RoutFrame);
    }
    if ((p_cmd->Self_Frame) && (p_cmd->Alive))				//�Լ�������֡
    {
        Set_NetBuf_Pending_flag(Self_AliveFrame);
        return Data_Correct;
    }
    
    return result;
}


/**
�Դ�վ����֡�ķ�װ����
**/
void UpReportData_Package(uint8_t *p_buf, uint8_t *p_device_rout)
{
#ifdef Use_Rout
    R_Device *device_rout = (R_Device *)p_device_rout;
#endif
    
    memset(&p_buf[RF_HeaderNumber], RF_Frame_HEAD, 2);
    p_buf[RF_CmdNumber] = 0X81;
    memmove(&p_buf[RF_GroupAddrNumber], &(Get_pDevAesInfo()->addr_GA[1]), 2);
    memmove(&p_buf[RF_DestinNumber],&(Get_pNet_Buf()->Master_Device[0]),MAC_Data_Len);
    memmove(&p_buf[RF_SourceNumber], Get_pSelfDevMacInfo()->mac, MAC_Data_Len);
    
#ifdef Use_Rout
    p_buf[RF_RoutLevelNumber] = device_rout->rout_main.rout_len;
#else
    p_buf[RF_RoutLevelNumber] = 0X0F;
#endif
    
    p_buf[RF_RoutTabLenNumber] = p_buf[RF_RoutLevelNumber] >> 4;
    
#ifdef Use_Rout
    memmove(&p_buf[RF_RoutPathNumber], device_rout->rout_main.rout_id, p_buf[RF_RoutTabLenNumber] + 1);
#endif
}


/**
�����Э���������Ĵ���
**/
uint8_t WL_Data_process(uint8_t *p_data, uint8_t *p_device_rout, uint8_t *p_buf, uint8_t send_len)
{
    uint8_t rout_len = p_data[RF_RoutTabLenNumber];                 //·�ɱ���
    uint8_t direction = Get_pNet_Buf()->WL_CMD.Direction;           //��ȡ֡����
#ifdef Use_Rout
    uint8_t rout_level = (p_data[RF_RoutLevelNumber] & 0XF0) >> 4;  //��ȡ��ǰ·�ɼ���
    uint8_t com_mode = p_data[RF_RoutLevelNumber] & 0X0F;           //��ȡͨ��ģʽ
    R_Device *device_rout = (R_Device *)p_device_rout;
#endif
    
    if(Get_pNet_Buf()->Pending_flag == Self_NRoutFrame)							//���Լ���֡������Ҫ·��
    {
        memmove(&p_data[RF_DestinNumber], &(Get_pNet_Buf()->Master_Device[0]), MAC_Data_Len);	//�������һ�θ��µ���վ��ַ
        memmove(&p_data[RF_SourceNumber], Get_pSelfDevMacInfo()->mac, MAC_Data_Len);
        p_data[RF_CmdNumber] |= 0x80;	//��վ��Ӧ
#ifdef Use_Rout
        device_rout->rout_main.rout_len = ((rout_len<<4) | com_mode);   //���뵱ǰ·�ɱ��ȣ����Լ���ʼ·�ɣ���ͨ�ŷ�ʽ
        memmove(device_rout->rout_main.rout_id, &p_data[RF_RoutPathNumber], rout_len + 1);
        
        Routpath_Save(device_rout->rout_main.rout_len, device_rout->rout_main.rout_id, (uint8_t *)(Get_pNet_Buf()->Master_Device));
        
        p_data[RF_RoutLevelNumber] = device_rout->rout_main.rout_len;
#endif
        p_data[RF_RoutPathNumber] = Get_pDevAesInfo()->self_ID;
    }
	
#ifdef Use_Rout
    if(Get_pNet_Buf()->Pending_flag == NSelf_RoutFrame) //�����Լ���֡��Ҫ�Լ�·�ɵ�
    {
        if (direction)                                  //��վ���ķ���ÿ����һ����ݼ�
        {
            if(rout_len >= rout_level)
                rout_level =  rout_level - 1;
        }
        else
        {
            if(rout_len > rout_level)
                rout_level =  rout_level + 1;
            else if(rout_len == rout_level)             //�������·�ɼ�������һ����Ŀ�ĵ�
                rout_level = 0;
        }
        
        p_data[RF_RoutLevelNumber] = ((rout_level << 4) | com_mode);
    }
#endif
    memmove(&p_data[RF_PayloadNumber + rout_len], p_buf, send_len);
    //У��ĳ���Ϊ����������+Э�������ȣ�����·�ɱ��ȣ�
    send_len = Frame_Check(p_data, send_len + RF_PayloadNumber + rout_len);
    
    return send_len;
}


/**
���ϱ����ݵ�������װ
**/
void UpReport_WL_Process(uint8_t *p_src_data, uint8_t *send_len)
{
    uint8_t *p_send = Get_NetBuf_pWL_Data_Buf();
    
#ifdef Use_Rout
    Read_Rout(Get_pDevice_rout());
#endif
    
    UpReportData_Package(p_send, (uint8_t *)Get_pDevice_rout());
    
    memmove(&p_send[RF_PayloadNumber + p_send[RF_RoutTabLenNumber]], p_src_data, *send_len);
    
    *send_len = Frame_Check(p_send, *send_len + RF_PayloadNumber + p_send[RF_RoutTabLenNumber]);
}




#ifdef Use_74dcode
/**
74����Ĵ������
**/
uint8_t Decode_74(uint8_t *wl_data)
{
    uint8_t wl_len = pRadioConfiguration->Radio_PacketLength;
    uint8_t wireless_RealData[APP_BUFF_MAX_LEN] = {0};
    
    if(wl_data[RF_LenNumber] <= (ENCODE_MAX_DATALEN * 2))			//����δ���룬�������붯��
    {
        uint8_t Datacnt = RF_GroupAddrNumber;
        uint8_t Realcnt = 0;
        volatile uint16_t dest;
        uint8_t Datalength = (wl_len - RF_GroupAddrNumber - 2)/2;   //������ŵ�У������ʼ����
        
        while(Datalength--)
        {
            dest = (wl_data[Datacnt]);
            dest = dest << 8;
            dest |= wl_data[Datacnt+1];                             //���յ����ǵ�λ1Byte��74�����ݣ������2Byte����ʹ��
            wireless_RealData[Realcnt++] = decode16_74bit(dest);    //�������ʵ����
            Datacnt+=2;
        }
        memset(&wl_data[RF_GroupAddrNumber], (uint8_t)0, wl_len);	//�ѽ��յ��ı���֡����������������������֡�ĺ�׺����
        memmove(&wl_data[RF_GroupAddrNumber], wireless_RealData, Realcnt);
        pRadioConfiguration->Radio_PacketLength = Realcnt + RF_GroupAddrNumber + 2; //���ݳ���+ͷ��δ���볤��+֡β
        wl_data[RF_LenNumber] = Realcnt + RF_GroupAddrNumber - 2;   //-֡ͷ����
        memset(&wl_data[Realcnt + RF_GroupAddrNumber], RF_Frame_END, 2);
    }
    else
        return 0;
    
    return 1;
}


/**
74����Ĵ������
**/
uint8_t Code_74(uint8_t *addr, uint8_t *p_TxData, uint8_t length)
{
    if(addr[RF_LenNumber] <= ENCODE_MAX_DATALEN)    //��������������������
    {
        uint8_t i;
        uint8_t code_cnt = 0;                       //���������ݳ���
        volatile uint16_t dest = 0;
        uint8_t temp[2] = {0};
        uint8_t wireless_RealData[APP_BUFF_MAX_LEN] = {0};
        
        for(i = RF_GroupAddrNumber;i<length-2;i++)  //��CMD���浽CRC16У�������74����
        {
            dest = code16_74bit(addr[i]);
            temp[0] = dest >> 8;
            temp[1] = dest & 0Xff;
            wireless_RealData[code_cnt] = temp[0];
            wireless_RealData[code_cnt+1] = temp[1];
            code_cnt+=2;
        }
        memset(&p_TxData[RF_GroupAddrNumber], (uint8_t)0, length*2);    //��Ҫ���͵�ԭ֡����������������������֡�ĺ�׺����
        memmove(&p_TxData[RF_GroupAddrNumber], wireless_RealData, code_cnt);
        length = code_cnt + 2 + 1;                       //��������ݳ���+FL+AL+CMD
        p_TxData[RF_LenNumber] = length;
        memset(&p_TxData[2 + length], RF_Frame_END, 2);
    }
    return length + 2 + 2;
}


/**
���߱���ǰ��У�鴦��
**/
uint8_t RealData_Detect(uint8_t *p_data,uint8_t Frame_len)
{
    if((p_data[RF_HeaderNumber] != RF_Frame_HEAD) || (p_data[RF_HeaderNumber + 1] != RF_Frame_HEAD))		//��·��֡��֡ͷ
    {
        if ((p_data[RF_HeaderNumber] != RF_Mate_HEAD)
            || (p_data[RF_HeaderNumber + 1] != RF_Mate_HEAD))	//�����֡��֡ͷ
            return 1;
    }
    
    if((p_data[Frame_len - 2] != RF_Frame_END) || (p_data[Frame_len - 1] != RF_Frame_END))
        return 1;
    
    return 0;
}

#endif

