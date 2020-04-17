#include <string.h>

#include "dev_mate_interface.h"
#include "hk_device.h"
#include "net_layer_api.h"
#include "rf_interface.h"
#include "device_basic_info.h"
#include "encrypt.h"
#include "app_layer_api.h"
#include "stm32f0_gpio.h"
#include "stm32f0_eeprom.h"
#include "m0_systick.h"



static MATE_INFO_T Dev_Mate_Buf;

static uint8_t MateDev_ReadyFlag;

static _Bool Mate_Frame_Flag = 0;   //标识当前无线是主站发来的通信还是从站的配对通信




inline MATE_INFO_T *Get_pDev_Mate_Buf(void)
{
    return &Dev_Mate_Buf;
}


inline uint8_t Get_MateDev_ReadyFlag(void)
{
    return MateDev_ReadyFlag;
}


inline void Set_MateDev_ReadyFlag(uint8_t set_val)
{
    MateDev_ReadyFlag = set_val;
}


inline WL_Status_TypDef Get_DevMate_Status(void)
{
    return Dev_Mate_Buf.Status;
}


inline void Set_DevMate_Status(WL_Status_TypDef cur_status)
{
    Dev_Mate_Buf.Status = cur_status;
}


inline _Bool Get_MateFrame_Flag(void)
{
    return Mate_Frame_Flag;
}

inline void Set_MateFrame_Flag(uint8_t val)
{
    Mate_Frame_Flag = val;
}



/**
把自己的开关位和配对的开关位下标筛选出来
**/
uint8_t PICK_Switch(uint8_t* self_num, uint8_t Mode)
{
    uint8_t self_which = 0;
    
    if (Mode)
        self_which = Get_DeviceState_MateChange_Flag(LOW_4BIT);
    else
        self_which = Get_DeviceState_StateChange_Flag(LOW_4BIT);
    
    if (self_which == 0)
        return 1;
    while ((self_which & 0X01) != 1)	//取某一位的下标
    {
        self_which >>= 1;
        (*self_num)++;
    }
    return 0;
}


static WL_Process_Status Data_ErrorInit(MATE_INFO_T* p_switch)
{
    Switch_Cmd_typ* p_cmd = &(p_switch->SW_CMD);
    
    p_cmd->mode = 0;
    p_cmd->boardcast = 0;
    p_cmd->respond = 0;
    p_cmd->Com_type = 0;
    
    return Data_error;
}


void MateSwitch_DataProcess(uint8_t *p_data, uint8_t *p_data_len)
{
    Encrypt_Convert(p_data, *p_data_len, p_data_len, DECRYPT);
    
    if (Mate_SynData_process(p_data))
        return;
    
    Mate_SwitchStatus_Syn(Get_pDev_Mate_Buf(), &(Get_pDeviceState_Buf()->Device_Val));
}


/*****************
 @Description: 确认配对设备的状态与本设备的状态是否需要同步
 @Parameter:p_payload_val：配对设备的负载数据指针
            p_mate_SrcAddr：配对设备的地址
            work_mode：是否工作在从站的数据格式兼容模式
 @Return: NULL
 @Note: 
******************/
static void Mate_Info_Confirm(uint8_t *p_payload_val, uint8_t* p_mate_SrcAddr, uint8_t work_mode)
{
    Switch_choice* p_self_info;
    uint8_t num = 0;
    uint8_t bit_status = 0;
    
    while (num < 1)
    {
        p_self_info = &(Get_pDev_Mate_Buf()->Self_SWITCH[num]);
        
        if (work_mode)  //NORMAL_MODE
        {
            bit_status = !!(p_payload_val[0] & (p_self_info->Mate_Which_num));  //开关位状态用DAC表示
            
            if(memcmp(p_self_info->Mate_Switch.Dev_MAC_Addr, p_mate_SrcAddr, MAC_Data_Len) == 0)
            {
            	if (bit_status != (!!Get_DeviceState_Device_Val(p_self_info->Self_Which_num)))
                {
                    Get_pDev_Mate_Buf()->Self_Cur_Switch |= p_self_info->Self_Which_num;
                    p_self_info->Mate_Switch.status = bit_status;
                }
            }
        }
        else            //COMPAT_MODE
        {
            bit_status = p_payload_val[3];  //开关位状态用DAC+状态量表示
            
            if((memcmp(p_self_info->Mate_Switch.Dev_MAC_Addr, p_mate_SrcAddr, MAC_Data_Len) == 0)
                && ((p_payload_val[0] & (p_self_info->Mate_Which_num)) == p_self_info->Mate_Which_num)) //地址和配对位都匹配
            {
                if (bit_status != (!!Get_DeviceState_Device_Val(p_self_info->Self_Which_num)))
                {
                    Get_pDev_Mate_Buf()->Self_Cur_Switch |= p_self_info->Self_Which_num;
                    p_self_info->Mate_Switch.status = bit_status;
                }
            }
        }
        num++;
    }
}


/**
开关设备位绑定状态记录
**/
static uint8_t Switch_status_save(MATE_INFO_T* pSwitch, uint8_t *p_data, uint8_t* SW_Addr)
{
    uint8_t self_num = 0;
    
    if(PICK_Switch(&self_num, 1))
        return 1;
    
    Switch_Info *self_info = &(pSwitch->Self_SWITCH[self_num].Mate_Switch);
    self_info->Dev_ID = p_data[0];
    pSwitch->Self_SWITCH[self_num].Mate_Which_num = p_data[1];
    pSwitch->Self_SWITCH[self_num].Self_Which_num = Get_DeviceState_MateChange_Flag(LOW_4BIT);
    pSwitch->Self_Cur_Switch |= Get_DeviceState_MateChange_Flag(LOW_4BIT);
    self_info->status = (!!p_data[2]);
    memmove(self_info->Dev_MAC_Addr, SW_Addr, MAC_Data_Len);
    
    Mate_SwitchStatus_Syn(Get_pDev_Mate_Buf(), &(Get_pDeviceState_Buf()->Device_Val));
    return 0;
}


/**
开关配对时网络层使用的无线协议解析
**/
WL_Process_Status WireLess_Mate_Process(uint8_t *p_mateData, MATE_INFO_T* pSwitch)
{
    Switch_Cmd_typ* p_cmd = &(pSwitch->SW_CMD);
    static uint8_t Temp_Mate_Addr[MAC_Data_Len] = {0};
    
    switch(pSwitch->Status)
    {
      case WL_RX_Cmd:
        {
            uint8_t Cmd = p_mateData[RF_CmdNumber];
            
            if((Cmd & 0X81) == 0X81)
            {
                p_cmd->Com_type = 1;
                p_cmd->boardcast = 0;
                p_cmd->respond = 0;
            }
            else
                p_cmd->Com_type = 0;
            
            if((Cmd & 0X08) == Mate_Mode)
                p_cmd->mode = 1;
            else
                p_cmd->mode = 0;
            
            if(!(p_cmd->Com_type))	//非通信帧判断配对帧类型
            {
                if((Cmd & 0X10) == Mate_Respond)
                {
                    p_cmd->respond = 1;
                    p_cmd->boardcast = 0;
                }
                else
                {
                    p_cmd->respond = 0;
                    p_cmd->boardcast = 1;
                }
            }
            
            pSwitch->Status = WL_RX_Addr;
        }
        break;
      case WL_RX_Addr:
        {
            if((p_cmd->mode) || (p_cmd->Com_type))		//配对帧或映射开关的通信帧，检查网络号是否相同
            {
                if (memcmp(&(Get_pDevAesInfo()->addr_GA[1]),&p_mateData[RF_GroupAddrNumber],2) != 0)
                    return Data_ErrorInit(pSwitch);
            }
            else
                return Data_ErrorInit(pSwitch);
            
            pSwitch->Status = WL_RX_Destin;
        }
        break;
      case WL_RX_Destin:
        {
            if(!(p_cmd->boardcast))		//不是广播帧都要检查目的地址
            {
                if(Get_LogicAddr_SetFlag())
                {
                    if(memcmp(&(p_mateData[RF_DestinNumber]), Get_pSelfDevMacInfo()->mac, MAC_Data_Len) != 0)
                        return Data_ErrorInit(pSwitch);
                }
                else
                {
                    if(memcmp(&(p_mateData[RF_DestinNumber]), Get_pSelfDevMacInfo()->mac, MAC_Data_Len) != 0)
                        return Data_ErrorInit(pSwitch);
                }
            }
            pSwitch->Status = WL_RX_Source;
        }
        break;
      case WL_RX_Source:
        {
            memmove(Temp_Mate_Addr, &p_mateData[RF_SourceNumber], MAC_Data_Len);
            
            pSwitch->Status = WL_RX_FrameData;
        }
        break;
      case WL_RX_FrameData:
        {
            if(p_cmd->boardcast)        //广播帧没有数据区跳过
            {
                uint8_t self_num = 0;
                if (PICK_Switch(&self_num, 1))
                    return Data_ErrorInit(pSwitch);
                
                pSwitch->Self_SWITCH[0].Mate_Switch.Dev_ID = p_mateData[RF_RoutPathNumber];
                memmove(pSwitch->Self_SWITCH[0].Mate_Switch.Dev_MAC_Addr, Temp_Mate_Addr, MAC_Data_Len);
                pSwitch->Self_SWITCH[0].Mate_Which_num = p_mateData[RF_PayloadNumber];
                pSwitch->Self_SWITCH[0].Self_Which_num = Get_DeviceState_MateChange_Flag(LOW_4BIT);
                
                pSwitch->Status = WL_RX_End;
            }
            else if(p_cmd->respond)     //回复帧有源地址的配对位和相应状态
            {
                if(Switch_status_save(pSwitch, &p_mateData[RF_RoutPathNumber], Temp_Mate_Addr))
                    return Data_ErrorInit(pSwitch);
                
                pSwitch->Status = WL_RX_Finish;
            }
            else if(p_cmd->Com_type)    //映射开关的通信帧
            {
                Mate_Info_Confirm(&p_mateData[RF_PayloadNumber], Temp_Mate_Addr, NORMAL_MODE);
                
                pSwitch->Status = WL_RX_Finish;
            }
        }
        break;
      default:
        return Data_ErrorInit(pSwitch);
        break;
    }
    return Data_Correct;
}


/**
长按开关位时灯的闪烁控制
**/
void Mate_LED_Process(void)
{
    uint8_t which_mate = Get_DeviceState_MateChange_Flag(LOW_4BIT);
    uint8_t which_cancel = Get_DeviceState_MateChange_Flag(HIGH_4BIT);
    static uint16_t LED_cnt = 0;
    
    if(which_mate)	//有配对动作时
    {
        LED_cnt++;
        if(LED_cnt <= Touch_LongTime)
        {
            if((LED_cnt & 0x05) == 0x01)
            {
                switch(which_mate)
                {
                  case 0X01:
                    {
                        LEDG_INVERSE();
                    }
                    break;
                  default:
                    Set_DeviceState_MateChange_Flag(Clr_MtChangeFlag, LOW_4BIT);
                    break;
                }
            }
        }
        else if((LED_cnt & 0x0F) == 0x01)//超过长按值闪烁更慢
        {
            switch(which_mate)
            {
              case 0X01:
                {
                    LEDG_INVERSE();
                }
                break;
              default:
                Set_DeviceState_MateChange_Flag(Clr_MtChangeFlag, LOW_4BIT);
                break;
            }
        }
        
        if(LED_cnt > 3750)
        {
            LED_cnt = 0;
            Set_DeviceState_MateChange_Flag(Clr_MtChangeFlag, LOW_4BIT);
        }
    }
    else
        LED_cnt = 0;
    
    if(which_cancel)		//有取消动作时
    {
        Switch_choice* p_self_info;
        uint8_t default_addr[3] = {0X00,0X00,0X00};
        
        Set_DeviceState_MateChange_Flag(Clr_MtChangeFlag, HIGH_4BIT | LOW_4BIT);
        
        p_self_info = &(Dev_Mate_Buf.Self_SWITCH[0]);
        
        if(memcmp(default_addr, p_self_info->Mate_Switch.Dev_MAC_Addr, MAC_Data_Len) == 0)
        {
            Set_MateDev_ReadyFlag(0);
            Get_pSelfDevMacInfo()->mate_dev_exist = 0;
            
            Set_Mem_MacInfo();
        }
    }
    
}


/**
对映射开关作状态同步
**/
void Mate_SwitchStatus_Syn(MATE_INFO_T* pSwitch, uint8_t *switch_val)
{
    Switch_choice* p_self_info;
    uint8_t Self_Switch = pSwitch->Self_Cur_Switch;
    uint8_t temp_val[10] = {0};
    
    if (!Self_Switch)
        return;
    
    if (Self_Switch & SWITCH_1)
    {
        p_self_info = &(pSwitch->Self_SWITCH[0]);
        if(p_self_info->Mate_Switch.status)
        {
            Switch1_ON();
            
            *switch_val |= Self_Switch;
        }
        else
        {
            Switch1_OFF();
            
            *switch_val &= ~Self_Switch;
        }
        
        memmove(&temp_val[1], p_self_info, sizeof(*p_self_info));
        temp_val[0] = *switch_val & SWITCH_1;
    	Get_pSystemFunc()->FLASH_Erase(Device_State_Addr1);
    	Get_pSystemFunc()->FLASH_Write(Device_State_Addr1, temp_val, sizeof(temp_val));
    }
    if (Get_MateDev_ReadyFlag())	//有过配对且没记录过才要写入FLASH
    {
        if (Get_pSelfDevMacInfo()->mate_dev_exist != Mate_Addr_Exist)
        {
            Get_pSelfDevMacInfo()->mate_dev_exist = Mate_Addr_Exist;
            Set_Mem_MacInfo();
        }
    }
    else	//之前没有配对现在配对了
    {
        Set_MateDev_ReadyFlag(1);
    }
    pSwitch->Self_Cur_Switch = 0;
    Set_DeviceState_MateChange_Flag(Clr_MtChangeFlag, LOW_4BIT);
    //Data_ErrorInit(pSwitch);	//清除标志
}


/**
收到映射开关要同步的数据
**/
uint8_t Mate_SynData_process(uint8_t* p_source)
{
    if (((AppFrameGet_Cmd(p_source, E_GET_crl_funcCode) == EVT_Event)
         && (AppFrameGet_Cmd(p_source, E_GET_transmissionDir) == FCMD_DIR_BIT))
        || ((AppFrameGet_Cmd(p_source, E_GET_crl_funcCode) == EVT_Reserve)
            && (AppFrameGet_Cmd(p_source, E_GET_transmissionDir) == 0x00)))    //从站的事件帧或主站控制才处理
    {
        if (Get_pNet_Buf()->Mate_Flag == 1)
        {
            if (AppFrameGet_DataPayloadLen(p_source) > 3)   //除了数据标识还有1Byte状态量
                Mate_Info_Confirm(AppFrameGet_pDataPayload(p_source), AppFrameGet_pDataSrcAddr(p_source), COMPAT_MODE);
            else
                Mate_Info_Confirm(AppFrameGet_pDataPayload(p_source), AppFrameGet_pDataSrcAddr(p_source), NORMAL_MODE);
        }
        else if (Get_pNet_Buf()->Mate_Flag == 2)
        {
            Mate_Info_Confirm(AppFrameGet_pDataPayload(p_source), AppFrameGet_pDesAddr(p_source), NORMAL_MODE);
        }
        
        return 0;
	}
	else
        return 1;
}


/**
用于发送开关配对广播帧的组包,新框架改
**/
void Mate_Process(void)
{
    uint8_t *p_buf = Get_NetBuf_pWL_Data_Buf();
    uint8_t send_len = 0;
    
    if(Get_DeviceState_MateChange_Flag(LOW_4BIT))	//有配对要求
    {
        memset(&p_buf[RF_HeaderNumber], RF_Mate_HEAD, 2);
        p_buf[RF_CmdNumber] = 0X88;
        
        if(Get_LogicAddr_SetFlag())
        {
            memmove(&p_buf[RF_GroupAddrNumber], &(Get_pDevAesInfo()->addr_GA[1]), 2);
        }
        else
        {
            memset(&p_buf[RF_GroupAddrNumber], 0x00, 2);
        }
        memset(&p_buf[RF_DestinNumber], 0x00, MAC_Data_Len);
        memmove(&p_buf[RF_SourceNumber], Get_pSelfDevMacInfo()->mac, MAC_Data_Len);
        p_buf[RF_RoutLevelNumber] = 0X0F;
        p_buf[RF_RoutTabLenNumber] = 0;
        p_buf[RF_RoutPathNumber] = Get_pDevAesInfo()->self_ID;
        p_buf[RF_PayloadNumber] = Get_DeviceState_MateChange_Flag(LOW_4BIT);
        
        send_len = Frame_Check(p_buf, RF_PayloadNumber + 1);
        
        Si4438_Transmit_Start(Get_pWirelessBuf(), DEFAULT_CHANNEL, p_buf, send_len);
    }
}


/**
开关设备配对回复帧的组包
**/
void Mate_Respond_process(void)
{
    uint8_t *p_buf = Get_NetBuf_pWL_Data_Buf();
    uint8_t sw_which = Get_DeviceState_MateChange_Flag(LOW_4BIT);
    uint8_t self_num = 0;
    uint8_t send_len = 0;
    Switch_choice *p_self_switch = &Dev_Mate_Buf.Self_SWITCH[0];
    
    if (PICK_Switch(&self_num, 1))
        return;
    
    memset(&p_buf[RF_HeaderNumber], RF_Mate_HEAD, 2);
    p_buf[RF_CmdNumber] = 0X98;
    
    if (Get_LogicAddr_SetFlag())			//设备组过网，有家庭组和逻辑地址则在回应时放入
    {
        memmove(&p_buf[RF_GroupAddrNumber], &(Get_pDevAesInfo()->addr_GA[1]), 2);
    }
    else
    {
        memset(&p_buf[RF_GroupAddrNumber], 0x00, 2);
    }
    memmove(&p_buf[RF_DestinNumber], p_self_switch[self_num].Mate_Switch.Dev_MAC_Addr, MAC_Data_Len);
    memmove(&p_buf[RF_SourceNumber], Get_pSelfDevMacInfo()->mac, MAC_Data_Len);
    p_buf[RF_RoutLevelNumber] = 0X0F;
    p_buf[RF_RoutTabLenNumber] = 0;
    p_buf[RF_RoutPathNumber] = Get_pDevAesInfo()->self_ID;
    p_buf[RF_PayloadNumber] = sw_which;
    p_buf[RF_PayloadNumber + 1] = Get_DeviceState_Device_Val(sw_which);
    
    send_len = Frame_Check(p_buf, RF_PayloadNumber + 2);
    
    Set_MateDev_ReadyFlag(1);
    Set_DeviceState_MateChange_Flag(Clr_MtChangeFlag, LOW_4BIT);
    
    Si4438_Transmit_Start(Get_pWirelessBuf(), DEFAULT_CHANNEL, p_buf, send_len);
    SysDelay_Xms(50);
}


/**
上报配对开关的组包
**/
void Mate_seal(uint8_t* p_desAddr, uint8_t* p_buf, uint8_t AFN_val)
{
    uint8_t send_len = 0;
    uint8_t self_num = 0;
    
    if(PICK_Switch(&self_num, 0))
        return;
    
    memset(&p_buf[RF_HeaderNumber], RF_Mate_HEAD, 2);
    p_buf[RF_CmdNumber] = 0X81;
    memset(&p_buf[RF_GroupAddrNumber], 0x00, 2);
    memmove(&p_buf[RF_DestinNumber], p_desAddr, MAC_Data_Len);
    memmove(&p_buf[RF_SourceNumber], Get_pSelfDevMacInfo()->mac, MAC_Data_Len);
    p_buf[RF_RoutLevelNumber] = 0X0F;
    p_buf[RF_RoutTabLenNumber] = 0;
    p_buf[RF_RoutPathNumber] = Get_pDevAesInfo()->self_ID;
    p_buf[RF_PayloadNumber] = AFN_val;
    
    send_len = Frame_Check(p_buf, RF_PayloadNumber + 1);
    
    Si4438_Transmit_Start(Get_pWirelessBuf(), Get_RF_Channel(), p_buf, send_len);
}


void Package_SwitchStateInfo(uint8_t *mem_array, uint8_t switch_index)
{
    if(Get_MateDev_ReadyFlag())	//配对过
    {
        Switch_choice* p_self_info = &(Dev_Mate_Buf.Self_SWITCH[Get_DataIndex(switch_index)]);
        
        memmove(&mem_array[1], p_self_info, sizeof(*p_self_info));
    }
}

