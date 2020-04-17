#ifndef __DEV_MATE_INTERFACE_H
#define __DEV_MATE_INTERFACE_H


#include <stdint.h>
#include "protocol_data_def_struct.h"



#define Mate_Addr_Exist						0x99	//表示有映射开关

#define RF_Mate_HEAD    0X98

#define COMPAT_MODE     0X00
#define NORMAL_MODE     0X01






typedef struct
{
    uint8_t Dev_MAC_Addr[MAC_Data_Len];
    uint8_t Dev_ID;
    uint8_t status;                         //表示开关位状态 0：关	1：开
}Switch_Info;


typedef struct
{
    Switch_Info Mate_Switch;		//映射开关的信息
    uint8_t Mate_Which_num;         //对方开关的哪一位
    uint8_t Self_Which_num;         //配对自己的哪一位
}Switch_choice;


typedef struct
{
    uint8_t mode:           1;		//是否为配对模式
    uint8_t boardcast:		1;		//是否是广播帧
    uint8_t respond:        1;		//是否是回应帧
    uint8_t Com_type:       1;		//是否为配对开关的通信帧
    uint8_t temp:           4;
}Switch_Cmd_typ;


typedef struct
{
    WL_Status_TypDef Status;
    uint8_t Self_Cur_Switch;			//当前正在操作自己的哪些开关位
    Switch_choice Self_SWITCH[4];		//开关位本身支持最大四位
    Switch_Cmd_typ SW_CMD;
}MATE_INFO_T;





extern uint8_t Current_Mate_Addr[3];



MATE_INFO_T *Get_pDev_Mate_Buf(void);
uint8_t Get_MateDev_ReadyFlag(void);
void Set_MateDev_ReadyFlag(uint8_t set_val);
WL_Status_TypDef Get_DevMate_Status(void);
void Set_DevMate_Status(WL_Status_TypDef cur_status);

_Bool Get_MateFrame_Flag(void);
void Set_MateFrame_Flag(uint8_t val);

void MateSwitch_DataProcess(uint8_t *p_data, uint8_t *p_data_len);

uint8_t PICK_Switch(uint8_t* self_num, uint8_t Mode);
WL_Process_Status WireLess_Mate_Process(uint8_t *p_mateData, MATE_INFO_T* pSwitch);
void Mate_Process(void);
void Mate_Respond_process(void);
void Mate_SwitchStatus_Syn(MATE_INFO_T* pSwitch, uint8_t *switch_val);
void Mate_LED_Process(void);
void Mate_seal(uint8_t* p_desAddr, uint8_t* p_buf, uint8_t AFN_val);
uint8_t Mate_SynData_process(uint8_t* p_source);

void Package_SwitchStateInfo(uint8_t *mem_array, uint8_t switch_index);




#endif

