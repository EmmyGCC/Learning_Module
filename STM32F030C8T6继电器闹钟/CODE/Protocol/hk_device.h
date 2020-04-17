#ifndef HK_DEVICE_H
#define HK_DEVICE_H


#include <stdint.h>


#define ROUND_CNT           7       //7个动作
#define GROUP_CNT           4       //4组

#define START_TICK_CNT      100

#define GAP_TIME            8       // s
#define GAP_TICK_CNT        1000    // ms

#define STOP_TICK_CNT       100

typedef enum
{
    START_EXE = 0,
    STOP_EXE,
    GAP_EXE,
    
}SYSTEM_STATE_E;


/*************************************************************************************
**************************************************************************************/

#define Touch_FilterTime           20u    //滤波值
#define Touch_LongTime             625u   //长按值
#define Touch_DisTime              625u   //间隔值	5000ms

#define Key_NoVal                   0u
#define Key_ShortVal                1u
#define Key_LongVal                 2u
#define Key_CancelVal               3u

#define Switch_Change_Gap                   12  //开关切换间隔
#define Switch_UpReport_Gap                 (1000/8)

#define DEVCON_EVENT                        (1<<0)    //开关事件 


#define SWITCH_1        (1<<0)
#define SWITCH_2        (1<<1)
#define SWITCH_3        (1<<2)
#define SWITCH_4        (1<<3)

#define RF_SWITCH_1    (0X10<<0)
#define RF_SWITCH_2    (0X10<<1)
#define RF_SWITCH_3    (0X10<<2)
#define RF_SWITCH_4    (0X10<<3)

#define CLR_MATE_SW1    (0X10<<0)
#define CLR_MATE_SW2    (0X10<<1)
#define CLR_MATE_SW3    (0X10<<2)
#define CLR_MATE_SW4    (0X10<<3)

#define LOW_4BIT        (0x0f)
#define HIGH_4BIT       (0xf0)





typedef struct
{
  uint8_t Pending_Flag;
  uint8_t StateChange_Flag;            //低4位是触摸要求切换，高4位是控制帧要求切换
  uint8_t MateChange_Flag;				//在配对时的挂起标志，用低四位表示配对动作，高四位表示取消
  uint8_t Device_Val;
  
  uint8_t UpReport_cnt;            //上报状态计时，状态改变后多久能上报
  uint16_t ChangeDis_Cnt;              //防止频繁开关 
  uint8_t UpReport_Flag;            //上报标识
}HK_Device_TypDef;                  //智能电器的各状态


typedef enum
{
    //Pending_Flag
    SET_PendFlag = 0,
    Clr_PendFlag,
    
    //Switch_Val
    Reverse_DeVal,
    SET_DeVal,
    Clr_DeVal,
    
    //SwitchMate_Flag
    SET_MtChangeFlag,
    Clr_MtChangeFlag,
    
    //SwitchChange_Flag
    SET_StChangeFlag,
    Clr_StChangeFlag,
    
    //UpReport_Flag
    SET_UpReport_Flag,
    Clr_UpReport_Flag,
}ACTION_TYPE_E;

typedef const uint8_t (*p_CMD)[2];




p_CMD Get_pCMD_INIT(uint8_t cmd_index);
p_CMD Get_pCMD_STATUE(uint8_t cmd_index);

HK_Device_TypDef *Get_pDeviceState_Buf(void);
uint8_t Get_DeviceState_Device_Val(uint8_t switch_bit);
void Set_DeviceState_Device_Val(ACTION_TYPE_E action_code, uint8_t set_val);
uint8_t Get_DeviceState_Pending_Flag(uint8_t switch_bit);
void Set_DeviceState_Pending_Flag(ACTION_TYPE_E action_code, uint8_t set_val);
uint8_t Get_DeviceState_MateChange_Flag(uint8_t switch_bit);
void Set_DeviceState_MateChange_Flag(ACTION_TYPE_E action_code, uint8_t set_val);
uint8_t Get_DeviceState_StateChange_Flag(uint8_t switch_bit);
void Set_DeviceState_StateChange_Flag(ACTION_TYPE_E action_code, uint8_t set_val);
void Set_DeviceState_UpReport_Flag(ACTION_TYPE_E action_code, uint8_t event_val);
uint8_t Get_DeviceState_UpReport_Flag(uint8_t get_bit);

_Bool Device_Execute_Process(uint8_t *p_source, uint8_t *p_buf);
uint8_t UpReport_Process(uint8_t *p);
void Set_ChangeDis_Cnt(HK_Device_TypDef *p_hkdevice);
void Device_Process(void *p_srcdata);

void Gap_Tick(uint8_t gap_time);
void Exercise_Alarm(void);


#endif



