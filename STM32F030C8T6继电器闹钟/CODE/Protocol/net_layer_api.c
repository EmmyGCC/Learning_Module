#include <string.h>

#include "net_layer_api.h"
#include "app_layer_api.h"
#include "dev_mate_interface.h"
#include "device_basic_info.h"
#include "74.h"
#include "crc16.h"
#include "radio.h"
#include "net_route_info.h"


/* 函数设计有问题，很多函数的参数可以分离开，模块与模块之间解耦 */
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


//用于初始化所有标志Data_Error(pNet)
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
主站地址的存储处理
**/
static void Master_ID_Update(Network_Protocol_TypDef* pNet, uint8_t *p_data)
{
    memmove(&pNet->Master_Device[2], &pNet->Master_Device[1], MAC_Data_Len);
    memmove(&pNet->Master_Device[1], &pNet->Master_Device[0], MAC_Data_Len);
    memmove(&pNet->Master_Device[0], &(p_data[RF_SourceNumber]), MAC_Data_Len);
}


/**
网络层无线协议解析过程
**/
WL_Process_Status WireLess_RX_Process(uint8_t *p_netData, Network_Protocol_TypDef* pNet)
{
    static uint8_t Rout_Level = 0;	//当前的路由级数,在路由的结构体中要定义此变量使用
    WL_CMD_typ* p_cmd = &(pNet->WL_CMD);
    
    
    switch(pNet->Status)
    {
      case WL_RX_Cmd:
        {
            uint8_t Cmd = p_netData[RF_CmdNumber];
            if((Cmd & 0X01) == Com_Mode)          //通信帧
                p_cmd->Com_Type = 1;
            else
                p_cmd->Com_Type = 0;
            
            if((Cmd & 0X02) == Ldc_Mode)
                p_cmd->Work_Mode = 1;
            else
                p_cmd->Work_Mode = 0;
            
            if((Cmd & 0X04) == Online)			//心跳
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
            if(!(p_cmd->Direction))		//主站发来
            {
                if (memcmp(&(p_netData[RF_DestinNumber]), Get_pSelfDevMacInfo()->mac, MAC_Data_Len) == 0)
                    p_cmd->Self_Frame = 1;	//是自己要处理的帧
                else
                {
                    p_cmd->Self_Frame = 0;
                    
                    if ((p_cmd->Com_Type) && (!(p_cmd->Alive)))	//不是心跳的通信帧
                    {
                        if (Get_MateDev_ReadyFlag() && (AppFrameGet_Cmd(&p_netData[RF_PayloadNumber], E_GET_crl_funcCode) != EVT_Event))
                        {
                            if (memcmp(Get_pDev_Mate_Buf()->Self_SWITCH[0].Mate_Switch.Dev_MAC_Addr, &(p_netData[RF_DestinNumber]), MAC_Data_Len) == 0)
                            {
                                pNet->Mate_Flag = 2;    //设备状态同步操作
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
            if (!(p_cmd->Direction))		//主站发来
            {
                if (p_cmd->Self_Frame)
                {
                    //和存储的主站地址不一致时就更新列表
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
            
            Rout_Level = ((level & 0XF0) >> 4); //当前到达的路由级数
            
            if ((p_cmd->Work_Mode) && (Rout_Level == routTab_len))	//来自LDC源设备的帧，不记录兄弟节点
#ifdef _Error_Init_Var_
                return Data_Error(pNet);
#else
            return RoutLevel_error;
#endif
            //检查路由级数是否正确
            if ((Rout_Level <= RoutSeries_MaxSize) && (Rout_Level <= routTab_len))
            {
                if (routTab_len > 0)
                {
                    p_cmd->Use_rout = 1;
#ifdef Use_Rout
                    Dev_list_Record(p_netData, Rout_Level, routTab_len, p_cmd->Direction);
#endif
                }
                else    //不存在路由表
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
                /*uint8_t Rout_Mode = !!((level & 0X0F) & (1<<Rout_Level));   载波是否用到？//取低四位中的通讯方式*/
                
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
            
            if (!(p_cmd->Direction))    //主站发来时，级数为上一级
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
            pNet->Frame_Data_Len = p_netData[RF_LenNumber] - FRAME_ROUTER_LEN - p_netData[RF_RoutTabLenNumber];   //数据长度-协议长度(包括CRC和长度字节本身)-路由表长度
            //此处为没有路由表的情况下
            memmove(Get_NetBuf_pWL_Data_Buf(), &p_netData[RF_PayloadNumber], pNet->Frame_Data_Len);
            pNet->Status = WL_RX_Finish;
            
            if(pNet->Mate_Flag == 1)	//是映射开关的数据
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
网络层协议用的校验函数
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
无线编码后的校验处理
**/
uint8_t Frame_Check(uint8_t *p,uint8_t Len)
{
    uint16_t crc16_val = 0;
    
    p[RF_LenNumber] = Len;
    p[RF_LenNumber + 1] = (MAC_Data_Len & 0X07);
    
    crc16_val = CRC16_2(p,Len);
    
    p[Len++] = crc16_val >> 8;
    p[Len++] = crc16_val & 0xff;
    //帧尾和CRC的处理应该在编码后
    memset(&p[Len], RF_Frame_END, 2);
    Len+=2;
    
    return Len;
}




/**
网络层无线协议解析过程
**/
WL_Process_Status Net_Protocol_Process(uint8_t *p_NetData)
{
    WL_Process_Status result = Data_Correct;		//存放无线处理的结果
    WL_CMD_typ* p_cmd = &(Get_pNet_Buf()->WL_CMD);
    
    if (WL_FrameData_Detect(p_NetData, p_NetData[RF_LenNumber]) == 0)  //check NetLevel Protocol
    {
        if (Get_MateFrame_Flag())	//是配对帧格式
        {
            while (result == Data_Correct)
            {
                result = WireLess_Mate_Process(p_NetData, Get_pDev_Mate_Buf());	//开关配对用的网络层协议解析
                
                if (Get_DevMate_Status() == WL_RX_End)
                {
                    //要回复的配对帧
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
                result = WireLess_RX_Process(p_NetData, Get_pNet_Buf());//网络层协议处理
                
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
    
    
    if ((!(p_cmd->Self_Frame)) && (!(p_cmd->Self_Rout)))	//不是自己的帧，不要自己路由的（收到路由节点的帧）
    {
        Set_NetBuf_Pending_flag(NSelf_NRoutFrame);
        return Data_RoutDevice;
    }
    if ((p_cmd->Self_Frame) && (p_cmd->Use_rout))			//是自己的帧，还需要路由（提前收到自己的帧-->先不管此情况）
    {
        Set_NetBuf_Pending_flag(Self_RoutFrame);
        return Data_error;
    }
    if ((p_cmd->Self_Frame) && (!(p_cmd->Use_rout)))		//是自己的帧，不需要路由
    {
        Set_NetBuf_Pending_flag(Self_NRoutFrame);
    }
    if ((!(p_cmd->Self_Frame)) && (p_cmd->Self_Rout))		//不是自己的帧，要自己路由的
    {
        Set_NetBuf_Pending_flag(NSelf_RoutFrame);
    }
    if ((p_cmd->Self_Frame) && (p_cmd->Alive))				//自己的心跳帧
    {
        Set_NetBuf_Pending_flag(Self_AliveFrame);
        return Data_Correct;
    }
    
    return result;
}


/**
对从站发出帧的封装处理
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
网络层协议解析结果的处理
**/
uint8_t WL_Data_process(uint8_t *p_data, uint8_t *p_device_rout, uint8_t *p_buf, uint8_t send_len)
{
    uint8_t rout_len = p_data[RF_RoutTabLenNumber];                 //路由表长度
    uint8_t direction = Get_pNet_Buf()->WL_CMD.Direction;           //获取帧方向
#ifdef Use_Rout
    uint8_t rout_level = (p_data[RF_RoutLevelNumber] & 0XF0) >> 4;  //获取当前路由级数
    uint8_t com_mode = p_data[RF_RoutLevelNumber] & 0X0F;           //获取通信模式
    R_Device *device_rout = (R_Device *)p_device_rout;
#endif
    
    if(Get_pNet_Buf()->Pending_flag == Self_NRoutFrame)							//是自己的帧，不需要路由
    {
        memmove(&p_data[RF_DestinNumber], &(Get_pNet_Buf()->Master_Device[0]), MAC_Data_Len);	//放入最后一次更新的主站地址
        memmove(&p_data[RF_SourceNumber], Get_pSelfDevMacInfo()->mac, MAC_Data_Len);
        p_data[RF_CmdNumber] |= 0x80;	//从站回应
#ifdef Use_Rout
        device_rout->rout_main.rout_len = ((rout_len<<4) | com_mode);   //放入当前路由表长度（从自己开始路由）和通信方式
        memmove(device_rout->rout_main.rout_id, &p_data[RF_RoutPathNumber], rout_len + 1);
        
        Routpath_Save(device_rout->rout_main.rout_len, device_rout->rout_main.rout_id, (uint8_t *)(Get_pNet_Buf()->Master_Device));
        
        p_data[RF_RoutLevelNumber] = device_rout->rout_main.rout_len;
#endif
        p_data[RF_RoutPathNumber] = Get_pDevAesInfo()->self_ID;
    }
	
#ifdef Use_Rout
    if(Get_pNet_Buf()->Pending_flag == NSelf_RoutFrame) //不是自己的帧，要自己路由的
    {
        if (direction)                                  //从站来的方向，每经过一级则递减
        {
            if(rout_len >= rout_level)
                rout_level =  rout_level - 1;
        }
        else
        {
            if(rout_len > rout_level)
                rout_level =  rout_level + 1;
            else if(rout_len == rout_level)             //到达最大路由级数，下一级是目的地
                rout_level = 0;
        }
        
        p_data[RF_RoutLevelNumber] = ((rout_level << 4) | com_mode);
    }
#endif
    memmove(&p_data[RF_PayloadNumber + rout_len], p_buf, send_len);
    //校验的长度为数据区长度+协议区长度（包括路由表长度）
    send_len = Frame_Check(p_data, send_len + RF_PayloadNumber + rout_len);
    
    return send_len;
}


/**
对上报数据的网络层封装
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
74解码的处理过程
**/
uint8_t Decode_74(uint8_t *wl_data)
{
    uint8_t wl_len = pRadioConfiguration->Radio_PacketLength;
    uint8_t wireless_RealData[APP_BUFF_MAX_LEN] = {0};
    
    if(wl_data[RF_LenNumber] <= (ENCODE_MAX_DATALEN * 2))			//长包未编码，不作解码动作
    {
        uint8_t Datacnt = RF_GroupAddrNumber;
        uint8_t Realcnt = 0;
        volatile uint16_t dest;
        uint8_t Datalength = (wl_len - RF_GroupAddrNumber - 2)/2;   //从网络号到校验区开始解码
        
        while(Datalength--)
        {
            dest = (wl_data[Datacnt]);
            dest = dest << 8;
            dest |= wl_data[Datacnt+1];                             //接收到的是单位1Byte的74码数据，处理成2Byte解码使用
            wireless_RealData[Realcnt++] = decode16_74bit(dest);    //解码成真实数据
            Datacnt+=2;
        }
        memset(&wl_data[RF_GroupAddrNumber], (uint8_t)0, wl_len);	//把接收到的编码帧清除，否则会有无用数据在帧的后缀出现
        memmove(&wl_data[RF_GroupAddrNumber], wireless_RealData, Realcnt);
        pRadioConfiguration->Radio_PacketLength = Realcnt + RF_GroupAddrNumber + 2; //数据长度+头部未编码长度+帧尾
        wl_data[RF_LenNumber] = Realcnt + RF_GroupAddrNumber - 2;   //-帧头长度
        memset(&wl_data[Realcnt + RF_GroupAddrNumber], RF_Frame_END, 2);
    }
    else
        return 0;
    
    return 1;
}


/**
74编码的处理过程
**/
uint8_t Code_74(uint8_t *addr, uint8_t *p_TxData, uint8_t length)
{
    if(addr[RF_LenNumber] <= ENCODE_MAX_DATALEN)    //长包编码后过长，不编码
    {
        uint8_t i;
        uint8_t code_cnt = 0;                       //编码后的数据长度
        volatile uint16_t dest = 0;
        uint8_t temp[2] = {0};
        uint8_t wireless_RealData[APP_BUFF_MAX_LEN] = {0};
        
        for(i = RF_GroupAddrNumber;i<length-2;i++)  //把CMD后面到CRC16校验码进行74编码
        {
            dest = code16_74bit(addr[i]);
            temp[0] = dest >> 8;
            temp[1] = dest & 0Xff;
            wireless_RealData[code_cnt] = temp[0];
            wireless_RealData[code_cnt+1] = temp[1];
            code_cnt+=2;
        }
        memset(&p_TxData[RF_GroupAddrNumber], (uint8_t)0, length*2);    //把要发送的原帧清除，否则会有无用数据在帧的后缀出现
        memmove(&p_TxData[RF_GroupAddrNumber], wireless_RealData, code_cnt);
        length = code_cnt + 2 + 1;                       //编码后数据长度+FL+AL+CMD
        p_TxData[RF_LenNumber] = length;
        memset(&p_TxData[2 + length], RF_Frame_END, 2);
    }
    return length + 2 + 2;
}


/**
无线编码前的校验处理
**/
uint8_t RealData_Detect(uint8_t *p_data,uint8_t Frame_len)
{
    if((p_data[RF_HeaderNumber] != RF_Frame_HEAD) || (p_data[RF_HeaderNumber + 1] != RF_Frame_HEAD))		//是路由帧的帧头
    {
        if ((p_data[RF_HeaderNumber] != RF_Mate_HEAD)
            || (p_data[RF_HeaderNumber + 1] != RF_Mate_HEAD))	//是配对帧的帧头
            return 1;
    }
    
    if((p_data[Frame_len - 2] != RF_Frame_END) || (p_data[Frame_len - 1] != RF_Frame_END))
        return 1;
    
    return 0;
}

#endif

