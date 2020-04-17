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
    Retry_TypDef *p_retry;            //当前无线帧的所有对重试需要的信息
    
    for(i = 0; i < (Retry_Buf_Size - 2); i++)
    {
        if((p_framebuf->RetryBuf_Space & (1 << i)) == 0)
        {
            p_framebuf->RetryBuf_Space |= (1 << i);
            p_retry = &p_framebuf->Retry_Buf[i];
            p_retry->RetryBuf_Number = i;
            
            p_retry->Retry_SendLen = source_len;
            memmove(p_retry->Retry_DataBuf, p_source, source_len);	//把网络层数据放到buf中重试
            
            p_retry->Retry_DelayCnt = 0;				//计算重试的时间
            p_retry->RetryCnt = 2;		//最多重试（重发）2次
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
    uint8_t data_index = 0;     //协议层标识使用的数据下标
    
    //目前先看MAC是否一样决定判断是否与自己组网，未来将改为其他组网机制****************************************
    if (memcmp(AppFrameGet_pDesAddr(p_source), Get_pSelfDevMacInfo()->mac, MAC_Data_Len) == 0)
    {
        //只有密文或家庭组发生改变时才要重新保存并初始化相关AES信息
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
    
    //因需要扩展板添加AES信息到负载数据区，对可靠传输层缓存
    memmove(reliable_transport_temp, AppFrameGet_pReliable_Transport_Layer(p_source), 3 + RELIABLE_TRANSPORT_LAYER_LEN);
    
    data_index = AppFramePackage_DataHead(p_buf, user_data, sizeof(user_data), Have_datamark);
    memmove(AppFrameGet_pDataPayload(p_buf) + sizeof(user_data), reliable_transport_temp, 3 + RELIABLE_TRANSPORT_LAYER_LEN); //放置好数据头后放入协议层标识再打包
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
        p_retry = &p_framebuf->Retry_Buf[i];    //获取当前帧的重试信息
        
        if(p_retry->RetryCnt)                   //有重试的次数要求时
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
    
    //把无线接收到的网络层数据拿过来
    memmove(p_rf_data, Get_pWirelessBuf()->Wireless_RxData, pRadioConfiguration->Radio_PacketLength);
    memset(p_buf, 0, sizeof(*p_buf));
    memmove(data_ID, AppFrameGet_pDataPayload(p_source) + 1, 2);
    if (memcmp(data_ID, Get_pCMD_INIT(0), sizeof(*Get_pCMD_INIT(0))) == 0)
    {
        //组网动作
        reply = Device_JoinNet_Process(p_source, p_buf);
    }
    else if (memcmp(data_ID, Get_pCMD_STATUE(4), sizeof(*Get_pCMD_STATUE(4))) == 0)
    {
        //控制电源开关
        reply = Device_Execute_Process(p_source, p_buf);
    }
    
    if (reply)
    {
        uint8_t send_len = AppFramePackage_FormatVerify(p_buf);
        if (Get_pCarrier_Info()->IsData_from_carrier)
        {
            //通过载波把应用层数据发出
            CarrierFramePackage_SendData(Get_pCarrier_Info()->comSrcNode_Addr, p_buf, send_len);
        }
        //只有非组网帧才加密
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
        
        //以上的链路层数据已经处理完毕，下面是网络层封装
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
        else if (pending_val == 0x02)	//配对开关数据解密等
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
        //不是组网帧
        if(memcmp(AppFrameGet_pDataPayload(p_data) + 1, Get_pCMD_INIT(0), sizeof(*Get_pCMD_INIT(0))) != 0)
        {
#ifdef Use_Rout
            if (RF_RoutFrame_Transform(p_data, Get_RF_Channel(), *p_data_len))
                return;
#endif
            if(Get_LogicAddr_SetFlag())
            {
                //确认数据是给自己的
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
    //确认通信方向，处理来自主站的
    if (0 == AppFrameGet_Cmd(p_source, E_GET_transmissionDir))
    {
        if (AppFrameGet_Cmd(p_source, E_GET_crl_funcCode) == EVT_Reserve)
        {
            if (Get_pCarrier_Info()->IsData_from_carrier)
                HEXaddr_2_BCDaddr(AppFrameGet_pDataSrcAddr(p_source), Get_pCarrier_Info()->comSrcNode_Addr);  //保留下帧的源地址给载波用
            
            RemoteUp_CmdFrame_Process(p_source, Get_pHKFrame_Buf());
        }
        else if(AppFrameGet_Cmd(p_source, E_GET_crl_funcCode) == EVT_Event) //是否为清除事件帧
        {
            Clear_EventFrame_Process(p_source, Get_pHKFrame_Buf());
        }
    }
}



//////////////////////////////////// 上报处理 //////////////////////////////////////////
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
//启动上报
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
    
    //以上的应用层数据处理完毕，以下是网络层协议封装
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
        p_retry = &p_framebuf->Retry_Buf[i];	//获取需要重试的buf
        
        if(p_retry->RetryCnt > 0)				//还有重试的次数要求
        {
#ifndef Use_Rout
            if(++p_retry->Retry_DelayCnt > Frame_RetryTimes)			//不用路由为400ms超时
#else
                if(++p_retry->Retry_DelayCnt > Frame_Up_RetryTimes)		//用路由为500ms超时
#endif
                {
                    p_retry->Retry_DelayCnt = 0;		//重置重试的时间
                    
                    Si4438_Transmit_Start(Get_pWirelessBuf(), Get_RF_Channel(), p_retry->Retry_DataBuf, p_retry->Retry_SendLen);
                    if(--p_retry->RetryCnt == 0)		//超过要重试的次数就不重发了
                    {
                        p_framebuf->RetryBuf_Space &= ~(1 << p_retry->RetryBuf_Number);
                        
                        uint8_t frame_len = 0;
                        uint8_t default_addr[Carrier_MAC_LEN] = {0};
                        Encrypt_Convert(&p_retry->Retry_DataBuf[FRAME_ROUTER_LEN],
                                        p_retry->Retry_SendLen - FRAME_ROUTER_LEN - p_retry->Retry_DataBuf[RF_RoutTabLenNumber] - 4,
                                        &frame_len,
                                        DECRYPT);
                        //通过载波把应用层数据发出，防止在此之前没有拿到载波目标地址，此处直接转换首个无线主机地址
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




