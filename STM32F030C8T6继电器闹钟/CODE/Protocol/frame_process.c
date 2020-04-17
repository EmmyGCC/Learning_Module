#include <string.h>

#include "frame_process.h"
#include "net_layer_api.h"
#include "app_layer_api.h"
#include "dev_mate_interface.h"
#include "uart_interface.h"
#include "rf_interface.h"
#include "carrier.h"
#include "encrypt.h"
#include "device_basic_info.h"
#include "m0_systick.h"
#include "radio.h"
#include "net_route_info.h"



static HKFrame_TypDef HKFrame_Buf;



inline HKFrame_TypDef *Get_pHKFrame_Buf(void)
{
    return &HKFrame_Buf;
}




void Retry_Start(HKFrame_TypDef *p_framebuf, uint8_t *p_source, uint8_t source_len)
{
    uint8_t i;
    Retry_TypDef *p_retry;            //��ǰ����֡�����ж�������Ҫ����Ϣ
    
    for(i = 0; i < (Retry_Buf_Size - 2); i++)
    {
        if((p_framebuf->RetryBuf_Space & (1 << i)) == 0)
        {
            p_framebuf->RetryBuf_Space |= (1 << i);
            p_retry = &p_framebuf->Retry_Buf[i];
            p_retry->RetryBuf_Number = i;
            
            p_retry->Retry_SendLen = source_len;
            memmove(p_retry->Retry_DataBuf, p_source, source_len);	//����������ݷŵ�buf������
            
            p_retry->Retry_DelayCnt = 0;				//�������Ե�ʱ��
            p_retry->RetryCnt = 2;		//������ԣ��ط���2��
            break;
        }
    }
}
/////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////////


_Bool Device_JoinNet_Process(uint8_t *p_source, uint8_t *p_buf)
{
    uint8_t *p_Payload = AppFrameGet_pDataPayload(p_source);
    uint8_t reliable_transport_temp[3 + RELIABLE_TRANSPORT_LAYER_LEN] = {0};
    uint8_t user_data[3 + 3] = {0};
    uint8_t data_index = 0;     //Э����ʶʹ�õ������±�
    
    //Ŀǰ�ȿ�MAC�Ƿ�һ�������ж��Ƿ����Լ�������δ������Ϊ������������****************************************
    if (memcmp(AppFrameGet_pDesAddr(p_source), Get_pSelfDevMacInfo()->mac, MAC_Data_Len) == 0)
    {
        //ֻ�����Ļ��ͥ�鷢���ı�ʱ��Ҫ���±��沢��ʼ�����AES��Ϣ
        if(memcmp(p_Payload + 3, Get_pDevAesInfo(), sizeof(DEVICE_AES_T)) != 0)
        {
            memmove(Get_pDevAesInfo()->aes, p_Payload + 3, RsaByte_Size);
            memmove(Get_pDevAesInfo()->addr_GA, p_Payload + 3 + RsaByte_Size, GROUP_ADDR_LEN);
            uint8_t dev_id = p_Payload[3 + RsaByte_Size + GROUP_ADDR_LEN];
            if (dev_id != 0 || dev_id != 0Xff)
                Get_pDevAesInfo()->self_ID = dev_id;
            
            Set_Mem_MacInfo();
            Set_LogicAddr_SetFlag(1);
            Get_pHKFrame_Buf()->OnLine_Staus = 1;
            
            Aes_Key_Init();
            
            SI4438_Chip_Init();
            Si4438_Receive_Start(DEFAULT_CHANNEL);   //Start Receive
        }
    }
    else
        return false;
    
    memmove(user_data, p_Payload, 3);
    user_data[3 + 0] = Version_Number >> 16;
    user_data[3 + 1] = (uint8_t)(Version_Number >> 8);
    user_data[3 + 2] = Version_Number & 0xff;
    
    AppFramePackage_DesAddr(p_buf, AppFrameGet_pDataSrcAddr(p_source));
    AppFramePackage_Cmd(p_buf, EVT_Reserve, E_SET_crl_funcCode);
    AppFramePackage_Cmd(p_buf, 1, E_SET_transmissionDir);
    
    //����Ҫ��չ�����AES��Ϣ���������������Կɿ�����㻺��
    memmove(reliable_transport_temp, AppFrameGet_pReliable_Transport_Layer(p_source), 3 + RELIABLE_TRANSPORT_LAYER_LEN);
    
    data_index = AppFramePackage_DataHead(p_buf, user_data, sizeof(user_data), Have_datamark);
    memmove(AppFrameGet_pDataPayload(p_buf) + sizeof(user_data), reliable_transport_temp, 3 + RELIABLE_TRANSPORT_LAYER_LEN); //���ú�����ͷ�����Э����ʶ�ٴ��
    data_index += AppFramePackage_DataProtocolMark(&p_buf[data_index], E_SET_compression_layer, ACK);
    data_index += AppFramePackage_DataProtocolMark(&p_buf[data_index], E_SET_fragment_layer, ACK);
    data_index += AppFramePackage_DataProtocolMark(&p_buf[data_index], E_SET_transport_layer, ACK);
    
    AppFramePackage_DataLen(p_buf, data_index - Region_DataDACNumber);
    
    return true;
}











void Clear_EventFrame_Process(uint8_t *p_source, HKFrame_TypDef *p_framebuf)
{
    uint8_t i;
    Retry_TypDef *p_retry;
    
    for(i = 0; i < (Retry_Buf_Size - 2); i++)
    {
        p_retry = &p_framebuf->Retry_Buf[i];    //��ȡ��ǰ֡��������Ϣ
        
        if(p_retry->RetryCnt)                   //�����ԵĴ���Ҫ��ʱ
        {
            p_retry->RetryCnt = 0;
            p_framebuf->RetryBuf_Space &= ~(1 << p_retry->RetryBuf_Number);
            break;
        }
    }
}



static void RemoteUp_CmdFrame_Process(uint8_t *p_source, HKFrame_TypDef *p_framebuf)
{
    _Bool reply = false;
    uint8_t *p_buf = p_framebuf->FrameProcess_Buf;
    uint8_t *p_rf_data = &(p_framebuf->Retry_Buf[2].Retry_DataBuf[0]);
    uint8_t data_ID[2] = {0};
    uint8_t rf_channel = 0;
    
    //�����߽��յ�������������ù���
    memmove(p_rf_data, Get_pWirelessBuf()->Wireless_RxData, pRadioConfiguration->Radio_PacketLength);
    memset(p_buf, 0, sizeof(*p_buf));
    memmove(data_ID, AppFrameGet_pDataPayload(p_source) + 1, 2);
    if (memcmp(data_ID, Get_pCMD_INIT(0), sizeof(*Get_pCMD_INIT(0))) == 0)
    {
        //��������
        reply = Device_JoinNet_Process(p_source, p_buf);
    }
    else if (memcmp(data_ID, Get_pCMD_STATUE(4), sizeof(*Get_pCMD_STATUE(4))) == 0)
    {
        //���Ƶ�Դ����
        reply = Device_Execute_Process(p_source, p_buf);
    }
    
    if (reply)
    {
        uint8_t send_len = AppFramePackage_FormatVerify(p_buf);
        if (Get_pCarrier_Info()->IsData_from_carrier)
        {
            //ͨ���ز���Ӧ�ò����ݷ���
            CarrierFramePackage_SendData(Get_pCarrier_Info()->comSrcNode_Addr, p_buf, send_len);
        }
        //ֻ�з�����֡�ż���
        if (memcmp(AppFrameGet_pDataPayload(p_source) + 1, Get_pCMD_INIT(0), sizeof(*Get_pCMD_INIT(0))) != 0)
        {
            if (!(AppFrameGet_Cmd(p_source, E_GET_errorFlag)))
            {
                Encrypt_Convert(p_buf, send_len, &send_len, ENCRYPT);
            }
            rf_channel = Get_RF_Channel();
        }
        else
            rf_channel = DEFAULT_CHANNEL;
        
        //���ϵ���·�������Ѿ�������ϣ�������������װ
        send_len = WL_Data_process(p_rf_data, (uint8_t *)Get_pDevice_rout(), p_buf, send_len);
        
        Si4438_Transmit_Start(Get_pWirelessBuf(), rf_channel, p_rf_data, send_len);
    }
}




void RxData_Process(void)
{
    uint8_t pending_val = Get_DeviceState_Pending_Flag(LOW_4BIT);
    uint8_t error_flag = 0;
    uint8_t *p_data = Get_NetBuf_pWL_Data_Buf();
    uint8_t *p_data_len = &(Get_pNet_Buf()->Frame_Data_Len);
    
    if (pending_val)
    {
        if (pending_val == 0x01)
        {
            if (Get_pDevUartTxBuf()->Tx_Status != UartTx_Finished)
                return;
            AesFrame_Process(Get_pDevUartRxBuf()->Frame_Data);
        }
        else if (pending_val == 0x02)	//��Կ������ݽ��ܵ�
        {
            MateSwitch_DataProcess(p_data, p_data_len);
        }
        
        Set_DeviceState_Pending_Flag(Clr_PendFlag, LOW_4BIT);
        
        Si4438_Receive_Start(DEFAULT_CHANNEL);
        Get_pNet_Buf()->Mate_Flag = 0;
		USART1Com_RX_En();
		USART2Com_EN();
    }
    
    if(Get_DeviceState_Pending_Flag(HIGH_4BIT))
    {
        //��������֡
        if(memcmp(AppFrameGet_pDataPayload(p_data) + 1, Get_pCMD_INIT(0), sizeof(*Get_pCMD_INIT(0))) != 0)
        {
#ifdef Use_Rout
            if (RF_RoutFrame_Transform(p_data, Get_RF_Channel(), *p_data_len))
                return;
#endif
            if(Get_LogicAddr_SetFlag())
            {
                //ȷ�������Ǹ��Լ���
                if(memcmp(&(p_data[Region_AddrNumber]), Get_pSelfDevMacInfo()->mac, MAC_Data_Len) == 0)
                {
                    Get_pHKFrame_Buf()->OnLine_Staus = 1;
                    Get_pHKFrame_Buf()->OnLine_Cnt = 0;
                }
            }
            else
                error_flag = 1;
            
            Encrypt_Convert(p_data, *p_data_len, p_data_len, DECRYPT);
        }
        else
        {
#ifdef Use_Rout
            if (RF_RoutFrame_Transform(p_data, DEFAULT_CHANNEL, *p_data_len))
                return;
#endif
        }
        
        
        if(error_flag)
        {
            Set_DeviceState_Pending_Flag(Clr_PendFlag, HIGH_4BIT);
            Si4438_Receive_Start(DEFAULT_CHANNEL);
            USART1Com_RX_En();
            return;
        }
        
        Frame_Process(p_data);
        
        Set_DeviceState_Pending_Flag(Clr_PendFlag, HIGH_4BIT);
        Set_NetBuf_Pending_flag(0);
        
        if(WIRELESS_STATUS != Wireless_TX_Sending)
            Si4438_Receive_Start(DEFAULT_CHANNEL);
        USART1Com_RX_En();
    }
}







void Frame_Process(uint8_t *p_source)
{
    //ȷ��ͨ�ŷ��򣬴���������վ��
    if (0 == AppFrameGet_Cmd(p_source, E_GET_transmissionDir))
    {
        if (AppFrameGet_Cmd(p_source, E_GET_crl_funcCode) == EVT_Reserve)
        {
            if (Get_pCarrier_Info()->IsData_from_carrier)
                HEXaddr_2_BCDaddr(AppFrameGet_pDataSrcAddr(p_source), Get_pCarrier_Info()->comSrcNode_Addr);  //������֡��Դ��ַ���ز���
            
            RemoteUp_CmdFrame_Process(p_source, Get_pHKFrame_Buf());
        }
        else if(AppFrameGet_Cmd(p_source, E_GET_crl_funcCode) == EVT_Event) //�Ƿ�Ϊ����¼�֡
        {
            Clear_EventFrame_Process(p_source, Get_pHKFrame_Buf());
        }
    }
}



//////////////////////////////////// �ϱ����� //////////////////////////////////////////
static void Clear_UpReport(HKFrame_TypDef *p_framebuf)
{
    uint8_t i = 0;
    Retry_TypDef *p_retry;
    
    p_framebuf->RetryBuf_Space = 0;
    for(i = 0; i < (Retry_Buf_Size - 2); i++)
    {
        p_retry = &p_framebuf->Retry_Buf[i];
        p_retry->RetryBuf_Number = 0;
        p_retry->Retry_SendLen = 0;
        p_retry->Retry_DelayCnt = 0;
        p_retry->RetryCnt = 0;
    }
}
//�����ϱ�
void UpReport_Start(void *p_srcdata)
{
    HKFrame_TypDef *p_framebuf = (HKFrame_TypDef *)p_srcdata;
    uint8_t *p_data = p_framebuf->FrameProcess_Buf;
    uint8_t send_len = 0;
    
    memset(p_data, 0, sizeof(p_framebuf->FrameProcess_Buf));
    send_len = UpReport_Process(p_data);
    if(!send_len)
        return;
    
    Clear_UpReport(p_framebuf);
    
    Encrypt_Convert(p_data, send_len, &send_len, ENCRYPT);
    
    //���ϵ�Ӧ�ò����ݴ�����ϣ������������Э���װ
    UpReport_WL_Process(p_data, &send_len);
    uint8_t *p_WL_data = Get_NetBuf_pWL_Data_Buf();
    Retry_Start(p_framebuf, p_WL_data, send_len);
    
    Si4438_Transmit_Start(Get_pWirelessBuf(), Get_RF_Channel(), p_WL_data, send_len);
}



void Retransmission_Process(HKFrame_TypDef *p_framebuf)
{
    uint8_t i;
    Retry_TypDef *p_retry;
    
    if(WIRELESS_STATUS == Wireless_TX_Sending)return;
    for(i = 0; i < (Retry_Buf_Size - 2); i++)
    {
        p_retry = &p_framebuf->Retry_Buf[i];	//��ȡ��Ҫ���Ե�buf
        
        if(p_retry->RetryCnt > 0)				//�������ԵĴ���Ҫ��
        {
#ifndef Use_Rout
            if(++p_retry->Retry_DelayCnt > Frame_RetryTimes)			//����·��Ϊ400ms��ʱ
#else
                if(++p_retry->Retry_DelayCnt > Frame_Up_RetryTimes)		//��·��Ϊ500ms��ʱ
#endif
                {
                    p_retry->Retry_DelayCnt = 0;		//�������Ե�ʱ��
                    
                    Si4438_Transmit_Start(Get_pWirelessBuf(), Get_RF_Channel(), p_retry->Retry_DataBuf, p_retry->Retry_SendLen);
                    if(--p_retry->RetryCnt == 0)		//����Ҫ���ԵĴ����Ͳ��ط���
                    {
                        p_framebuf->RetryBuf_Space &= ~(1 << p_retry->RetryBuf_Number);
                        
                        uint8_t frame_len = 0;
                        uint8_t default_addr[Carrier_MAC_LEN] = {0};
                        Encrypt_Convert(&p_retry->Retry_DataBuf[FRAME_ROUTER_LEN],
                                        p_retry->Retry_SendLen - FRAME_ROUTER_LEN - p_retry->Retry_DataBuf[RF_RoutTabLenNumber] - 4,
                                        &frame_len,
                                        DECRYPT);
                        //ͨ���ز���Ӧ�ò����ݷ�������ֹ�ڴ�֮ǰû���õ��ز�Ŀ���ַ���˴�ֱ��ת���׸�����������ַ
                        if (memcmp(Get_pCarrier_Info()->comSrcNode_Addr, default_addr, Carrier_MAC_LEN) == 0)
                        {
                            HEXaddr_2_BCDaddr((uint8_t *)&(Get_pNet_Buf()->Master_Device[0]), Get_pCarrier_Info()->comSrcNode_Addr);
                        }
                        CarrierFramePackage_SendData(Get_pCarrier_Info()->comSrcNode_Addr, &p_retry->Retry_DataBuf[FRAME_ROUTER_LEN], frame_len);
                    }
                    break;     
                }
        }
    }
}




