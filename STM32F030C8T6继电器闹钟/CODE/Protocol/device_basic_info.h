#ifndef __DEVICE_BASIC_INFO_H
#define __DEVICE_BASIC_INFO_H


#include <stdint.h>
#include "protocol_data_def_struct.h"
#include "rsa.h"
#include "system_func.h"


/****************************** 版本管理与记录 ****************************************/
/*
Rev_0100  继电器闹钟初版		2020/04/17

*/

#define Version_Number                      (uint32_t)0x010100


#define NO_RF_DEBUG
/************************************************************************************/
#define UID_BASE              0x1FFFF7AC    /*!< Unique device ID register base address */
#define Use_74dcode                     //带74编码功能
#define Use_Rout                        //路由功能


#define MAC_FRAME_ERROE         0XAA
#define MAC_FRAME_EXIST         0XBB
#define MAC_FRAME_CORRECT       0XCC



//MAC
typedef struct
{
    uint8_t mac_h;				//MAC协议头
    uint8_t mac_frame_len;		//MAC协议数据长度
    uint8_t mac_len;			//MAC数据长度
    uint8_t dev_action_bits;    //设备可操作位数
    uint8_t mac[MAC_Data_Len];	//设备MAC地址 MAC_Data_Len
    uint8_t mac_crc_h;     		//MAC协议CRC
    uint8_t mac_crc_l;     		//MAC协议CRC
    uint8_t mate_dev_exist;     //设备是否有配对
    uint8_t nc;                 //对齐使用
} DEVIVE_MAC_T;

//密钥和群组
typedef struct
{
    uint8_t aes[RsaByte_Size];          //密钥
    uint8_t addr_GA[GROUP_ADDR_LEN];    //家庭组地址
    uint8_t self_ID;                    //设备自身ID
} DEVICE_AES_T;


typedef struct
{
    DEVIVE_MAC_T mac_Frame;	//mac协议帧
    DEVICE_AES_T aes_Field; //aes+家庭组
} DEVICE_INFO_T;




DEVIVE_MAC_T *Get_pSelfDevMacInfo(void);
DEVICE_AES_T *Get_pDevAesInfo(void);

void Set_LogicAddr_SetFlag(uint8_t set_val);
uint8_t Get_LogicAddr_SetFlag(void);

void Set_DEV_Action_Bits(uint8_t set_val);
uint8_t Get_DEV_Action_Bits(void);

void Get_Mem_MacInfo(void);
void Set_Mem_MacInfo(void);

void Get_ChipUniqueID(uint32_t *uid);
void Mem_EncryptionID(void);
_Bool Verify_EncryptionID(void);
uint8_t Get_DataIndex(uint8_t data);

void Read_Device_State(void);
void MacAddr_Read(void);
void Aes_Key_Init(void);

void AesFrame_Process(uint8_t *p_source);


#endif

