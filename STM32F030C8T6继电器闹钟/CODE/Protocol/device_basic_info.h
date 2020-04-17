#ifndef __DEVICE_BASIC_INFO_H
#define __DEVICE_BASIC_INFO_H


#include <stdint.h>
#include "protocol_data_def_struct.h"
#include "rsa.h"
#include "system_func.h"


/****************************** �汾�������¼ ****************************************/
/*
Rev_0100  �̵������ӳ���		2020/04/17

*/

#define Version_Number                      (uint32_t)0x010100


#define NO_RF_DEBUG
/************************************************************************************/
#define UID_BASE              0x1FFFF7AC    /*!< Unique device ID register base address */
#define Use_74dcode                     //��74���빦��
#define Use_Rout                        //·�ɹ���


#define MAC_FRAME_ERROE         0XAA
#define MAC_FRAME_EXIST         0XBB
#define MAC_FRAME_CORRECT       0XCC



//MAC
typedef struct
{
    uint8_t mac_h;				//MACЭ��ͷ
    uint8_t mac_frame_len;		//MACЭ�����ݳ���
    uint8_t mac_len;			//MAC���ݳ���
    uint8_t dev_action_bits;    //�豸�ɲ���λ��
    uint8_t mac[MAC_Data_Len];	//�豸MAC��ַ MAC_Data_Len
    uint8_t mac_crc_h;     		//MACЭ��CRC
    uint8_t mac_crc_l;     		//MACЭ��CRC
    uint8_t mate_dev_exist;     //�豸�Ƿ������
    uint8_t nc;                 //����ʹ��
} DEVIVE_MAC_T;

//��Կ��Ⱥ��
typedef struct
{
    uint8_t aes[RsaByte_Size];          //��Կ
    uint8_t addr_GA[GROUP_ADDR_LEN];    //��ͥ���ַ
    uint8_t self_ID;                    //�豸����ID
} DEVICE_AES_T;


typedef struct
{
    DEVIVE_MAC_T mac_Frame;	//macЭ��֡
    DEVICE_AES_T aes_Field; //aes+��ͥ��
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

