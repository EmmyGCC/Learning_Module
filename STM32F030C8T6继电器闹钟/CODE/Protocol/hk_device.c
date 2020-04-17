#include <string.h>

#include "hk_device.h"
#include "net_layer_api.h"
#include "dev_mate_interface.h"
#include "app_layer_api.h"
#include "device_basic_info.h"
#include "carrier.h"
#include "stm32f0_eeprom.h"
#include "stm32f0_gpio.h"
#include "m0_systick.h"



static HK_Device_TypDef DeviceState_Buf;


//各个命令的数据标识
const uint8_t CMD_INIT[][2]= {
    {0x0F,0x0F},
    {0x0F,0x0E},
    {0x0F,0x0D},
    {0x0F,0x0C},
    {0x0F,0x0B},
    {0x0F,0x0A},
    {0x0F,0x02},
    {0x0F,0x01},
};

const uint8_t CMD_STATUE[][2]= {
    {0x00,0x01},
    {0x00,0x02},
    {0x00,0x03},
    {0x00,0x04},
    {0x00,0x05},
    {0x00,0x06},
    {0x00,0x07},
};



inline p_CMD Get_pCMD_INIT(uint8_t cmd_index)
{
    return &CMD_INIT[cmd_index];
}


inline p_CMD Get_pCMD_STATUE(uint8_t cmd_index)
{
    return &CMD_STATUE[cmd_index];
}


inline HK_Device_TypDef *Get_pDeviceState_Buf(void)
{
    return &DeviceState_Buf;
}


inline uint8_t Get_DeviceState_Device_Val(uint8_t switch_bit)
{
    return ((Get_pDeviceState_Buf()->Device_Val) & switch_bit);
}


void Set_DeviceState_Device_Val(ACTION_TYPE_E action_code, uint8_t set_val)
{
    switch(action_code)
    {
      case Reverse_DeVal:
        Get_pDeviceState_Buf()->Device_Val ^= (set_val);
        break;
        
      case SET_DeVal:
        Get_pDeviceState_Buf()->Device_Val |= (set_val);
        break;
        
      case Clr_DeVal:
        Get_pDeviceState_Buf()->Device_Val &= ~(set_val);
        break;
        
      default:
        break;
    }
}


inline uint8_t Get_DeviceState_Pending_Flag(uint8_t switch_bit)
{
    return ((Get_pDeviceState_Buf()->Pending_Flag) & switch_bit);
}


void Set_DeviceState_Pending_Flag(ACTION_TYPE_E action_code, uint8_t set_val)
{
    switch(action_code)
    {
      case SET_PendFlag:
        Get_pDeviceState_Buf()->Pending_Flag |= set_val;
        break;
        
      case Clr_PendFlag:
        Get_pDeviceState_Buf()->Pending_Flag &= ~(set_val);
        break;
        
      default:
        break;
    }
}


inline uint8_t Get_DeviceState_MateChange_Flag(uint8_t switch_bit)
{
    return ((Get_pDeviceState_Buf()->MateChange_Flag) & switch_bit);
}


void Set_DeviceState_MateChange_Flag(ACTION_TYPE_E action_code, uint8_t set_val)
{
    switch(action_code)
    {
      case SET_MtChangeFlag:
        Get_pDeviceState_Buf()->MateChange_Flag |= (set_val);
        break;
        
      case Clr_MtChangeFlag:
        Get_pDeviceState_Buf()->MateChange_Flag &= ~(set_val);
        break;
        
      default:
        break;
    }
}


inline uint8_t Get_DeviceState_StateChange_Flag(uint8_t switch_bit)
{
    return ((Get_pDeviceState_Buf()->StateChange_Flag) & switch_bit);
}


void Set_DeviceState_StateChange_Flag(ACTION_TYPE_E action_code, uint8_t set_val)
{
    switch(action_code)
    {
      case SET_StChangeFlag:
        Get_pDeviceState_Buf()->StateChange_Flag |= (set_val);
        break;
        
      case Clr_StChangeFlag:
        Get_pDeviceState_Buf()->StateChange_Flag &= ~(set_val);
        break;
        
      default:
        break;
    }
}


void Set_DeviceState_UpReport_Flag(ACTION_TYPE_E action_code, uint8_t event_val)
{
    switch(action_code)
    {
      case SET_UpReport_Flag:
    	Get_pDeviceState_Buf()->UpReport_Flag |= event_val;
        break;
      case Clr_UpReport_Flag:
        Get_pDeviceState_Buf()->UpReport_Flag &= ~event_val;
        break;
      default:
        break;
    }
}

uint8_t Get_DeviceState_UpReport_Flag(uint8_t get_bit)
{
    return ((Get_pDeviceState_Buf()->UpReport_Flag) & get_bit);
}
/****** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ****/
/******               以上为DeviceStatus_Buf相关操作               *****/
/****** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ****/


uint8_t Touch1_Detect(void)
{
    uint8_t Touch1_this_state = 1;
    static uint8_t Touch1_LastState = 1, Touch1Dis_Flag = 0, Cancel_MateFlag1 = 0;
    static uint16_t Touch1_FilterCnt, Cancelmate_Cnt;
    
    Touch1_this_state = Touch1_RDPIN();
    
    if(Touch1_this_state)  //没有按下时
    {
        if(Touch1_this_state != Touch1_LastState) 
        {
            if(++Touch1_FilterCnt > Touch_FilterTime) 
            {
                Touch1_FilterCnt = 0;
                Touch1Dis_Flag = 0;
                Touch1_LastState = Touch1_this_state;   
            }
        }
        else
        {
            if(Get_DeviceState_MateChange_Flag(SWITCH_1))	//有配对请求挂起
            {
                if(++Cancelmate_Cnt > Touch_DisTime)	//长按后抬起时间大于5s
                {
                    Cancel_MateFlag1 = 1;
                }
            }
            
            Touch1_FilterCnt = 0;
        }
    }
    else
    {
        if(Touch1_this_state != Touch1_LastState)
        {
            if(++Touch1_FilterCnt > Touch_FilterTime)
            {
                Touch1_FilterCnt = 0;
                Touch1_LastState = Touch1_this_state;  
                return Key_ShortVal;
            }
        }
        else    //长按时
        {
            if(!Touch1Dis_Flag)    //第一次记录下长按
            {
                if(++Touch1_FilterCnt > Touch_LongTime)
                {
                    Touch1Dis_Flag = 1;
                    Touch1_FilterCnt = Touch_LongTime;
                    Cancelmate_Cnt = 0;
                    
                    if(Get_DeviceState_MateChange_Flag(SWITCH_1))//长按过的
                    {
                        if(Cancel_MateFlag1)
                        {
                            Cancel_MateFlag1 = 0;
                            Set_DeviceState_MateChange_Flag(SET_MtChangeFlag, CLR_MATE_SW1);
                        }
                        else    //第二次记录下长按
                        {
                            Switch_choice* p_sw = &(Get_pDev_Mate_Buf()->Self_SWITCH[0]);
                            
                            memset(p_sw, 0 ,sizeof(*p_sw));
                            Set_DeviceState_MateChange_Flag(SET_MtChangeFlag, CLR_MATE_SW1);
                        }
						return Key_CancelVal;
                    }
                    
                    return Key_LongVal;
                }
            }
        }
    }     
    return Key_NoVal;
}


//*****************************************************************************************//



static void Switch_SetStateCmd_Process(uint8_t *p_source, uint8_t *p_buf)
{
    uint8_t *p_Payload_data = AppFrameGet_pDataPayload(p_source);
    uint8_t switch_val = p_Payload_data[3] & LOW_4BIT;
    uint8_t reliable_transport_temp[3 + RELIABLE_TRANSPORT_LAYER_LEN] = {0};
	_Bool ret_err = false;
    
	if ((p_Payload_data[0] & 0x7F) > (0XFF >> (8 - Get_DEV_Action_Bits())))
        ret_err = true;
	
	AppFramePackage_DesAddr(p_buf, AppFrameGet_pDataSrcAddr(p_source));
    AppFramePackage_Cmd(p_buf, EVT_Reserve, E_SET_crl_funcCode);
    AppFramePackage_Cmd(p_buf, 1, E_SET_transmissionDir);
    //因需要扩展板添加AES信息到负载数据区，对可靠传输层缓存
    memmove(reliable_transport_temp, AppFrameGet_pReliable_Transport_Layer(p_source), 3 + RELIABLE_TRANSPORT_LAYER_LEN);
	
	if (ret_err)
	{
        uint8_t user_data[MAC_Data_Len + 8 + 1] = {0};
		AppFramePackage_Cmd(p_buf, 1, E_SET_errorFlag);
        AppFramePackage_DataLen(p_buf, MAC_Data_Len + 1);
        
        memmove(user_data, Get_pSelfDevMacInfo()->mac, MAC_Data_Len);
        memmove(&user_data[MAC_Data_Len], &reliable_transport_temp[3 + 3], 8);
        user_data[sizeof(user_data) - 1] = ERR_Invaild_FuncMark;
        memmove(AppFrameGet_pDataDAC(p_buf), user_data, sizeof(user_data));
        
        return;
	}
    //以下判断支持了批量控制时多位同时需要动作的情况
    if (p_Payload_data[0] & SWITCH_1)   //DAC有效
    {
    	if ((switch_val & SWITCH_1) != (Get_DeviceState_Device_Val(SWITCH_1)))   //电器状态发生了改变
        {
        	Set_DeviceState_StateChange_Flag(SET_StChangeFlag, RF_SWITCH_1);
        }
    }
	if ((Get_DEV_Action_Bits() > 1) && (p_Payload_data[0] & SWITCH_2))
	{
        if ((switch_val & SWITCH_2) != (Get_DeviceState_Device_Val(SWITCH_2)))
        {
            Set_DeviceState_StateChange_Flag(SET_StChangeFlag, RF_SWITCH_2);
        }
	}
    if ((Get_DEV_Action_Bits() > 2) && (p_Payload_data[0] & SWITCH_3))
    {
        if ((switch_val & SWITCH_3) != (Get_DeviceState_Device_Val(SWITCH_3)))
        {
            Set_DeviceState_StateChange_Flag(SET_StChangeFlag, RF_SWITCH_3);
        }
    }
    
    
    uint8_t data_index = AppFramePackage_DataHead(p_buf, 0, 0, No_datamark);
    memmove(&p_buf[data_index - 3], reliable_transport_temp, 3 + RELIABLE_TRANSPORT_LAYER_LEN); //放置好数据头后放入协议层标识再打包
    data_index += AppFramePackage_DataProtocolMark(&p_buf[data_index], E_SET_compression_layer, ACK);
    data_index += AppFramePackage_DataProtocolMark(&p_buf[data_index], E_SET_fragment_layer, ACK);
    data_index += AppFramePackage_DataProtocolMark(&p_buf[data_index], E_SET_transport_layer, ACK);
    
    AppFramePackage_DataLen(p_buf, data_index - Region_DataDACNumber);
    return;
}


_Bool Device_Execute_Process(uint8_t *p_source, uint8_t *p_buf)
{
    uint8_t *pPayload_data = AppFrameGet_pDataPayload(p_source);
    //改写设备的状态
    if((*pPayload_data & FDAC_RDWR_BIT) == 0)
    {
        Switch_SetStateCmd_Process(p_source, p_buf);
        
        return true;
    }
    return false;
}


static void UserData_Package(uint8_t *user_data, uint8_t *bit_val)
{
    memmove(&user_data[1], Get_pCMD_STATUE(4), sizeof(*Get_pCMD_STATUE(4)));
    
    if (Get_DeviceState_StateChange_Flag(SWITCH_1 | RF_SWITCH_1))
    {
        user_data[0] |= SWITCH_1;
    }
    if (Get_DeviceState_Device_Val(SWITCH_1))
    {
        *bit_val |= SWITCH_1;
    }
    
    if (Get_DEV_Action_Bits() > 1)
    {
        if (Get_DeviceState_StateChange_Flag(SWITCH_2 | RF_SWITCH_2))
        {
            user_data[0] |= SWITCH_2;
        }
        if (Get_DeviceState_Device_Val(SWITCH_2))
        {
            *bit_val |= SWITCH_2;
        }
    }
    if (Get_DEV_Action_Bits() > 2)
    {
        if (Get_DeviceState_StateChange_Flag(SWITCH_3 | RF_SWITCH_3))
        {
            user_data[0] |= SWITCH_3;
        }
        if (Get_DeviceState_Device_Val(SWITCH_3))
        {
            *bit_val |= SWITCH_3;
        }
    }
}

uint8_t UpReport_Process(uint8_t *p_data)
{
    uint8_t user_data[1 + 2 + 1] = {0,0,0,0};	//DAC+ID+状态量
    uint8_t bit_val = 0;
    uint8_t default_Addr[3] = {0X00,0X00,0X00};
    
    if (Get_DeviceState_UpReport_Flag(DEVCON_EVENT) == DEVCON_EVENT)   //状态信息改变了才会执行上报
    {
        
        UserData_Package(user_data, &user_data[3]);
        
        AppFramePackage_DesAddr(p_data, (uint8_t *)&(Get_pNet_Buf()->Master_Device[0]));
        AppFramePackage_Cmd(p_data, 1, E_SET_transmissionDir);
        AppFramePackage_Cmd(p_data, EVT_Event, E_SET_crl_funcCode);
        
        uint8_t data_len = AppFramePackage_DataHead(p_data, user_data, sizeof(user_data), Have_datamark);
        AppFramePackage_DataLen(p_data, data_len - Region_DataDACNumber - 3);   //没有协议层标识头
        data_len = AppFramePackage_FormatVerify(p_data);
        
        Set_DeviceState_StateChange_Flag(Clr_StChangeFlag, 0XFF);
        Set_DeviceState_UpReport_Flag(Clr_UpReport_Flag, 0XFF);
        return data_len;
    }
    else if(Get_DeviceState_UpReport_Flag(0x10<<DEVCON_EVENT) == (0x10<<DEVCON_EVENT))	//无分配设备ID时上报
    {
        uint8_t self_num = 0;
        
        Set_DeviceState_UpReport_Flag(Clr_UpReport_Flag, 0XFF);
        if(PICK_Switch(&self_num, 0))
            return 1;
        
        uint8_t* pDev_addr = &(Get_pDev_Mate_Buf()->Self_SWITCH[self_num].Mate_Switch.Dev_MAC_Addr[0]);
        
        if(memcmp(pDev_addr, default_Addr, MAC_Data_Len) == 0)	//没有绑定过  不上报
        {
            Set_DeviceState_StateChange_Flag(Clr_StChangeFlag, 0XFF);
            return 1;
        }
        
        if(Get_DeviceState_StateChange_Flag(SWITCH_1))
        {
            if(Get_DeviceState_Device_Val(SWITCH_1))
            {
                bit_val |= 1<<0;
            }
        }
        else if ((Get_DEV_Action_Bits() > 1) && (Get_DeviceState_StateChange_Flag(SWITCH_2)))
        {
            if(Get_DeviceState_Device_Val(SWITCH_2))
            {
                bit_val |= 1<<1;
            }
        }
        else if ((Get_DEV_Action_Bits() > 2) && (Get_DeviceState_StateChange_Flag(SWITCH_3)))
        {
            if(Get_DeviceState_Device_Val(SWITCH_3))
            {
                bit_val |= 1<<2;
            }
        }
        
        Mate_seal(pDev_addr, p_data, bit_val);
        
        Set_DeviceState_StateChange_Flag(Clr_StChangeFlag, 0XFF);
    }
    return 0;
}


/*
对触摸得到的结果做相应处理
*/
static void Touch_Response(uint8_t switch_bit)
{
    uint8_t touch_val = Key_NoVal;
    
    switch(switch_bit)
    {
      case SWITCH_1:
        touch_val = Touch1_Detect();
        break;
      default:
        return;
        break;
    }
#ifdef NO_RF_DEBUG
    if (touch_val == Key_ShortVal)    //短按时，发送载波测试数据
    {
        uint8_t test[3] = {0XAA,0XBB,0XCC};
        uint8_t des_addr[] = {0x00,0x00,0x01,0x03};
        
        HEXaddr_2_BCDaddr(des_addr, Get_pCarrier_Info()->comSrcNode_Addr);
        CarrierFramePackage_SendData(Get_pCarrier_Info()->comSrcNode_Addr, test, sizeof(test));
    }
#endif
	if(touch_val == Key_LongVal)	//长按
    {
        Set_DeviceState_MateChange_Flag(SET_MtChangeFlag, switch_bit);
        Mate_Process();
    }
    
}


void Touch_Process(HK_Device_TypDef *p_hkdevice)
{
    //还有要上报的动作，触摸不响应
    if(Get_DeviceState_UpReport_Flag(DEVCON_EVENT))
        return;
    
    if(p_hkdevice->ChangeDis_Cnt == 0)
    {
        Touch_Response(SWITCH_1);
        
        if (Get_DEV_Action_Bits() <= 1)
            return;
        Touch_Response(SWITCH_2);
        
        if (Get_DEV_Action_Bits() <= 2)
            return;
        Touch_Response(SWITCH_3);
    }
}


static void SetDevice_State(uint8_t which_bit, uint8_t device_val)
{
    switch (which_bit)
    {
      case SWITCH_1:
        if (device_val)
        {
            Switch1_ON();
        }
        else
        {
            Switch1_OFF();
        }
        break;
      default:
        break;
    }
}


void Set_Device_StateInfo(HK_Device_TypDef *p_hkdevice, uint8_t which_bit)
{//批量控制帧，回应ACK扩展板收不到
    uint8_t temp_val[10] = {0};
    
    if ((!Get_DeviceState_StateChange_Flag(which_bit)) && !(Get_DeviceState_StateChange_Flag(0x10 << Get_DataIndex(which_bit))))
        return;
    
    if (Get_DeviceState_StateChange_Flag(0x10 << Get_DataIndex(which_bit)))   //无线要求开关状态切换
    {
        if (Get_LogicAddr_SetFlag())    //已经组过网
            p_hkdevice->UpReport_cnt = 1;
    }
    Set_DeviceState_Device_Val(Reverse_DeVal, which_bit);
    
    temp_val[0] = Get_DeviceState_Device_Val(which_bit);
    Package_SwitchStateInfo(temp_val, which_bit);
    
    p_hkdevice->ChangeDis_Cnt = 1;
    switch(which_bit)
    {
      case SWITCH_1:
        SetDevice_State(which_bit, temp_val[0]);
        
        Get_pSystemFunc()->FLASH_Erase(Device_State_Addr1);
        Get_pSystemFunc()->FLASH_Write(Device_State_Addr1, temp_val, sizeof(temp_val));
        break;
      case SWITCH_2:
        SetDevice_State(which_bit, temp_val[0]);
        
        Get_pSystemFunc()->FLASH_Erase(Device_State_Addr2);
        Get_pSystemFunc()->FLASH_Write(Device_State_Addr2, temp_val, sizeof(temp_val));
        break;
      case SWITCH_3:
        SetDevice_State(which_bit, temp_val[0]);
        
        Get_pSystemFunc()->FLASH_Erase(Device_State_Addr3);
        Get_pSystemFunc()->FLASH_Write(Device_State_Addr3, temp_val, sizeof(temp_val));
        break;
      default:
        break;
    }
}


void DeviceState_Process(HK_Device_TypDef *p_hkdevice)
{
    if(Get_DeviceState_UpReport_Flag(DEVCON_EVENT))
        return;
    
    Set_Device_StateInfo(p_hkdevice, SWITCH_1);
    
    if (Get_DEV_Action_Bits() <= 1)
        return;
    Set_Device_StateInfo(p_hkdevice, SWITCH_2);
    
    if (Get_DEV_Action_Bits() <= 2)
        return;
    Set_Device_StateInfo(p_hkdevice, SWITCH_3);
}


void Set_ChangeDis_Cnt(HK_Device_TypDef *p_hkdevice)
{
    if (p_hkdevice->ChangeDis_Cnt)
    {
        if(++p_hkdevice->ChangeDis_Cnt > Switch_Change_Gap)
            p_hkdevice->ChangeDis_Cnt = 0;
    }
    if (p_hkdevice->UpReport_cnt)
    {
        if (++p_hkdevice->UpReport_cnt > Switch_UpReport_Gap)
        {
            p_hkdevice->UpReport_cnt = 0;
            
            if (Get_LogicAddr_SetFlag())     //已经组过网的状态改变，上报
                Set_DeviceState_UpReport_Flag(SET_UpReport_Flag, DEVCON_EVENT);
        }
    }
}


void Device_Process(void *p_srcdata)
{
    HK_Device_TypDef *p_hkdevice = (HK_Device_TypDef *)p_srcdata;
    Touch_Process(p_hkdevice);
    DeviceState_Process(p_hkdevice);
}


void Switch_Reverse_Freq(uint32_t delay_time)
{
    Switch1_ON();
    SysDelay_Xms(delay_time);
    Switch1_OFF();
    SysDelay_Xms(delay_time);
}

void Switch_Reverse_Settle(void)
{
    Switch1_ON();
    SysDelay_Xms(50);
    Switch1_OFF();
    SysDelay_Xms(50);
}


_Bool Switch_Reverse(uint8_t res_cnt, uint32_t delay, _Bool run_mode)
{
    while(res_cnt--)
    {
        if (run_mode)
        {
            Switch_Reverse_Freq(delay);
        }
        else
        {
            Switch_Reverse_Settle();
        }
        
        if (!res_cnt)
        {
            return true;
        }
    }
    
    return true;
}


void Start_Alarm(void)
{
    if (Switch_Reverse(4, START_TICK_CNT, 0))
    {
        return;
    }
}


void Stop_Alarm(void)
{
    if (Switch_Reverse(2, STOP_TICK_CNT, 0))
    {
        return;
    }
}


void Gap_Tick(uint8_t gap_time)
{
    if (Switch_Reverse(gap_time/2, GAP_TICK_CNT, 1))
    {
        return;
    }
}


void Exercise_Alarm(void)
{
    SYSTEM_STATE_E state = START_EXE;
    
    for (uint8_t group_cnt = GROUP_CNT; group_cnt > 0; group_cnt--)
    {
        for (uint8_t round_cnt = ROUND_CNT; round_cnt > 0; round_cnt--)
        {
            switch(state)
            {
              case START_EXE:
                Start_Alarm();
                SysDelay_Xms(16000);
                state = STOP_EXE;
                break;
              case STOP_EXE:
                Stop_Alarm();
                SysDelay_Xms(1000);
                state = GAP_EXE;
                break;
              case GAP_EXE:
                Gap_Tick(GAP_TIME);
                SysDelay_Xms(1000);
                state = START_EXE;
                break;
              default:
                break;
            }
        }
    }
}


