#include <string.h>

#include "rf_interface.h"
#include "radio.h"
#include "frame_process.h"
#include "net_layer_api.h"
#include "dev_mate_interface.h"
#include "app_layer_api.h"
#include "device_basic_info.h"
#include "stm32f0_gpio.h"
#include "m0_systick.h"
#include "si446x_api_lib.h"
#ifdef Use_74dcode                         //带上了74编码方式
#include "74.h"
#endif
#include "net_route_info.h"




static uint8_t Wireless_Channel;
static WLS Wireless_Buf;
volatile uint16_t SI4438_ReInit = 0;
volatile Wireless_Status_TypDef WIRELESS_STATUS;



inline WLS *Get_pWirelessBuf(void)
{
    return &Wireless_Buf;
}

inline uint8_t Get_RF_Channel(void)
{
    return Wireless_Channel;
}


inline void Set_SI4438_ReInit_Flag(uint8_t val)
{
    SI4438_ReInit = val;
}

inline void Clr_SI4438_ReInit_Flag(void)
{
    SI4438_ReInit = 0;
}

inline uint8_t Get_SI4438_ReInit_val(void)
{
    return SI4438_ReInit;
}


#if 0
//time函数使用
time_t __time32 (time_t * p) {
    return time_dat;
}
//a -- bms随机延时
static void Rand_delay(uint8_t a,uint8_t b)
{
    srand(time(0));
    uint8_t Time = rand() % (b - a + 1) + a;
    SysDelay_Xms(Time);                   
}
#endif


void Radio_Initfailed(void)
{
    uint8_t fail_Init_cnt = 0;
    
    while(SI4438_ReInit && (fail_Init_cnt <= 5))
    {
        fail_Init_cnt++;
        vRadio_Init();
    }
    Clr_SI4438_ReInit_Flag();
}


void Set_WireLessChannel(uint8_t *wire_chnel)
{
    uint8_t temp_addr[3] = {0};
    memmove(temp_addr, Get_pDevAesInfo()->addr_GA, GROUP_ADDR_LEN);
    
    uint32_t temp_val = temp_addr[0] + temp_addr[1] + temp_addr[2];
    
#ifdef Use_Rout
    Read_Rout(Get_pDevice_rout());
#endif
    
    if (temp_val == 0)                                    //未组网时用配对频道
    {
        temp_val = 0XFF + 0XFF + 0XFF;
        
        uint8_t Element_A = (uint8_t)temp_val >> 2;
        uint8_t Element_B = (uint8_t)temp_val & 0X0F;
        
        *wire_chnel = (uint8_t)(Element_A ^ Element_B);   //共32个信道组，每个信道组有两个信道
        
        if(*wire_chnel == DEFAULT_CHANNEL)
        {
            *wire_chnel = *wire_chnel + 2;
        }
    }
    else
    {
        uint8_t Element_A = (uint8_t)temp_val >> 2;
        uint8_t Element_B = (uint8_t)temp_val & 0X0F;
        
        *wire_chnel = (uint8_t)(Element_A ^ Element_B);
        
        if(*wire_chnel == DEFAULT_CHANNEL)
        {
            *wire_chnel = *wire_chnel + 2;              //跳到下一个信道组的第一个频道去
        }
    }
    //放入群组的主频道
    Radio_Configuration_Data_Array[361] = Channel_Frequency_Index[*wire_chnel];
}


void SI4438_Chip_Init(void)
{
    Set_WireLessChannel(&Wireless_Channel);
    vRadio_Init();
    Radio_Initfailed();
}


/*
Si4438_Receive_Start function
*/
Wireless_ErrorType Si4438_Receive_Start(uint8_t channel_number)
{
#ifndef NO_RF_DEBUG
    WL_TX_DIS();
    WL_RX_EN();
    
    if(channel_number > 65) channel_number = 65;
    
    vRadio_StartRX(Channel_Frequency_Index[channel_number], 220);
#endif
    Set_Not_RX_Cnt();
    WIRELESS_STATUS = Wireless_RX_Receiving;
    RECEIVE_TIMES_INIT();
    RECEIVE_REMAIN_INIT();
    return Wireless_NoError;
}


/*
Si4438_Transmit_Start function
*/
Wireless_ErrorType Si4438_Transmit_Start(WLS *pWL, uint8_t channel_number, uint8_t *addr, uint8_t length)
{
#ifndef NO_RF_DEBUG
    /*uint8_t Listen_failCnt = 0;*/
    if(length > 251) length = 251;
    if(channel_number > 65) channel_number = 65;
    /*
    while(RSSI_CSMA(channel_number))	//	CSMA/CD
    {
	if(Listen_failCnt >= 10)
    return Wireless_ExecuteError;
	Listen_failCnt++;
}
    */
    memmove(pWL->Wireless_TxData,addr,length);
    
#ifdef Use_74dcode
    length = Code_74(addr, pWL->Wireless_TxData, length);
#endif
    
    pWL->Wireless_FrameHeader[0] = length + 4;
    pWL->Wireless_FrameHeader[1] = channel_number;
    pWL->Wireless_FrameHeader[2] = 0x01;
    pWL->Wireless_FrameHeader[3] = (pWL->Wireless_FrameHeader[0]^pWL->Wireless_FrameHeader[1])^pWL->Wireless_FrameHeader[2];
    WL_TX_EN();
    WL_RX_DIS();
    
    vRadio_StartTx_Variable_Packet(Channel_Frequency_Index[channel_number], &pWL->Wireless_TxData[0], length+4);
#endif
    WIRELESS_STATUS = Wireless_TX_Sending;
    pWL->Sent_TimeOut_Cnt = 1;
    
    TRANSMIT_TIMES_INIT();
    return Wireless_NoError;
}


Wireless_ErrorType Si4438_Transmit(WLS *pWL, uint8_t channel_number, uint8_t *addr, uint8_t length)
{
    if(length > 251) length = 251;
    if(channel_number > 65) channel_number = 65;
    
    memmove(pWL->Wireless_TxData,addr,length);
    
    pWL->Wireless_FrameHeader[0] = length + 4;
    pWL->Wireless_FrameHeader[1] = channel_number;
    pWL->Wireless_FrameHeader[2] = 0x01;
    pWL->Wireless_FrameHeader[3] = (pWL->Wireless_FrameHeader[0]^pWL->Wireless_FrameHeader[1])^pWL->Wireless_FrameHeader[2];
    
    WL_TX_EN();
    WL_RX_DIS();
    
    vRadio_StartTx_Variable_Packet(Channel_Frequency_Index[channel_number], &pWL->Wireless_TxData[0], length+4);
    
    WIRELESS_STATUS = Wireless_TX_Sending;
    TRANSMIT_TIMES_INIT();
    return Wireless_NoError;
}


/*
确认应用层的数据是什么命令类型的
*/
void AppFrameData_TypeConfirm(uint8_t *p_data)
{
    if (0 == AppFrameGet_Cmd(p_data, E_GET_transmissionDir))  //主站发出
    {
        Set_DeviceState_Pending_Flag(SET_PendFlag, 0x10);
    }
    else
    {
        Set_DeviceState_Pending_Flag(SET_PendFlag, 0x20);
    }
}


/*
关于无线接收不同状态的处理
*/
void WireLess_Process(void *p_srcdata)
{
    WLS *p_wl = (WLS *)p_srcdata;
    static uint16_t Wireless_ErrCnt = 0;
#ifndef NO_RF_DEBUG
    if(++Wireless_ErrCnt > Two_Min_TxTime)			//2分钟没改变状态发送内容
    {
        static uint8_t Send_cnt = 0;
        uint8_t TwoMin_Txbuf[2] = {0XAB,0XCD};
        Wireless_ErrCnt = 0;
        
        if(Send_cnt >= 2)   //4分钟重启一次无线模块
        {
            Send_cnt = 0;
            
            Set_SI4438_ReInit_Flag(1);
            SI4438_PWR_OFF();
            return;
        }
        Send_cnt++;
        
        if(WIRELESS_STATUS != Wireless_TX_Sending)
            Si4438_Transmit(p_wl, Get_RF_Channel(), TwoMin_Txbuf, 2);
    }
#endif
    
    switch(WIRELESS_STATUS)
    {
      case Wireless_RX_Failure:
        {
            /* Reset FIFO */
            si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_FIFO_RX_BIT);
            Clr_Not_RX_Cnt();
            WIRELESS_STATUS = Wireless_RX_Receiving;
        }
        break;
      case Wireless_TX_Finish:
        {
            WIRELESS_STATUS = Wireless_Free;
            Clr_Not_RX_Cnt();
            Wireless_ErrCnt = 0;
            Si4438_Receive_Start(DEFAULT_CHANNEL);   //Start Receive
        }
        break;
      case Wireless_RX_Finish:
        {
            WIRELESS_STATUS = Wireless_Free;
            Set_LedIndicate_Cnt(1);
            Clr_Not_RX_Cnt();
            Wireless_ErrCnt = 0;
            
#ifdef Use_74dcode
            uint8_t ret;
            ret = RealData_Detect(p_wl->Wireless_RxData, pRadioConfiguration->Radio_PacketLength);
            if(ret == 1)	//帧格式错误，丢弃
            {
                Set_LedIndicate_Cnt(0);
                Si4438_Receive_Start(DEFAULT_CHANNEL);   //Start Receive
                return;
            }
            else			//解码
            {
                Decode_74(p_wl->Wireless_RxData);
            }
#endif
            
            WL_Process_Status result = Data_Correct;
            
            result = Net_Protocol_Process(p_wl->Wireless_RxData);
            if(result != Data_Correct)					//网络层数据处理结果
            {
                if ((result == Slave_Mate_Data) || (result == Master_Mate_Data))
                {
                    Set_DeviceState_Pending_Flag(SET_PendFlag, 0x02);
                    return;			//配对开关数据需要同步
                }
                else
                {
                    Si4438_Receive_Start(DEFAULT_CHANNEL);	//Start Receive
                    return;
                }
            }
#ifdef Use_Rout
            if(Get_pNet_Buf()->WL_CMD.Alive)
            {
                Set_LedIndicate_Cnt(0);
                Alive_Upreport(Get_pNet_Buf()->Pending_flag, (uint8_t *)&(Get_pNet_Buf()->WL_CMD));
                return;
            }
#endif
            
            //对接收到的应用层数据校验
            if(0 == AppFrameData_FormatVerify(Get_NetBuf_pWL_Data_Buf(), Get_pNet_Buf()->Frame_Data_Len))
            {
                AppFrameData_TypeConfirm(Get_NetBuf_pWL_Data_Buf());
            }
            else
                Si4438_Receive_Start(DEFAULT_CHANNEL);	//Start Receive
        }
        
        break;
      default:
        break;
    }
}


