#include <string.h>

#include "device_basic_info.h"
#include "stm32f0_eeprom.h"
#include "encrypt.h"
#include "uart_interface.h"
#include "dev_mate_interface.h"
#include "app_layer_api.h"
#include "hk_device.h"
#include "frame_process.h"
#include "m0_systick.h"
#include "carrier.h"
#include "stm32f0_gpio.h"



static uint8_t LogicAddr_SetFlag;
static uint8_t DEV_Action_Bits;     //设备可以操作的继电器位数

static DEVICE_INFO_T DeviceBasic_Info;





inline DEVIVE_MAC_T *Get_pSelfDevMacInfo(void)
{
    return &(DeviceBasic_Info.mac_Frame);
}

inline DEVICE_AES_T *Get_pDevAesInfo(void)
{
    return &(DeviceBasic_Info.aes_Field);
}

inline uint8_t Get_LogicAddr_SetFlag(void)
{
    return LogicAddr_SetFlag;
}

inline void Set_LogicAddr_SetFlag(uint8_t set_val)
{
    LogicAddr_SetFlag = set_val;
}

inline uint8_t Get_DEV_Action_Bits(void)
{
    return DEV_Action_Bits;
}

inline void Set_DEV_Action_Bits(uint8_t set_val)
{
    DEV_Action_Bits = set_val;
}


inline void Get_Mem_MacInfo(void)
{
    Get_pSystemFunc()->FLASH_Read(MAC_INFO_SAVE_ADDR, &DeviceBasic_Info, sizeof(DeviceBasic_Info));
}

void Set_Mem_MacInfo(void)
{
    Get_pSystemFunc()->FLASH_Erase(MAC_INFO_SAVE_ADDR);
    
    Get_pSystemFunc()->FLASH_Write(MAC_INFO_SAVE_ADDR, &DeviceBasic_Info, sizeof(DeviceBasic_Info));
}


static inline void Encrypt_ID(uint32_t *encryption_id)
{
    encryption_id[3] = UID_BASE - 4;
    
    encryption_id[0] = (uint32_t)(encryption_id[0] ^ encryption_id[3]);
    encryption_id[1] = (uint32_t)(encryption_id[1] ^ encryption_id[3]);
    encryption_id[2] = (uint32_t)(encryption_id[2] ^ encryption_id[3]);
    encryption_id[3] = (uint32_t)(encryption_id[0] | encryption_id[1] | encryption_id[2]);
}


/**
* @brief Return the unique device identifier (uid based on 96 bits)
* @param uid: pointer to 3 words array.
* @retval Device identifier
*/
void Get_ChipUniqueID(uint32_t *uid)
{
    uid[0] = (uint32_t)(READ_REG(*((uint32_t *)UID_BASE)));
    uid[1] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 4U))));
    uid[2] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 8U))));
}


void Mem_EncryptionID(void)
{
    uint32_t chip_uniqueID[4] = {0};
    uint32_t encryption_id[4] = {0};      //最后4Bytes用于加密、校验
    
    Get_ChipUniqueID(chip_uniqueID);
    memmove((uint8_t *)&encryption_id[0], (uint8_t *)chip_uniqueID, 3*4); //id只有96bit
    
    Encrypt_ID(encryption_id);
    
    Get_pSystemFunc()->FLASH_Write(INIT_UNIQUE_ID_ADDR, encryption_id, sizeof(encryption_id));
}


_Bool Verify_EncryptionID(void)
{
    uint32_t encryption_id[4] = {0};
    uint32_t verify_id[4] = {0};
    
    Get_ChipUniqueID(encryption_id);
    Encrypt_ID(encryption_id);
    Get_pSystemFunc()->FLASH_Read(INIT_UNIQUE_ID_ADDR, verify_id, sizeof(verify_id));
    
    if (memcmp(encryption_id, verify_id, sizeof(verify_id)) == 0)
    {
        return 1;
    }
    else
        return 0;
}


/* 获取传入数据的左移位数 */
uint8_t Get_DataIndex(uint8_t data)
{
    uint8_t data_index = 0;
    
    while ((data & 0X01) != 1)	//取某一位的下标
    {
        data >>= 1;
        data_index++;
    }
    return data_index;
}


static void Dev_StateInfo_Init(uint8_t switch_bit)
{
    uint8_t Device_info[10] = {0};
    
    switch(switch_bit)
    {
      case SWITCH_1:
        Get_pSystemFunc()->FLASH_Read(Device_State_Addr1, Device_info, sizeof(Device_info));
        if(Device_info[0] & switch_bit)
        {
            Switch1_ON();

            Set_DeviceState_Device_Val(SET_DeVal, switch_bit);
        }
        else
            Set_DeviceState_Device_Val(Clr_DeVal, switch_bit);
        break;
      default:
        return;
        break;
    }
    if(Get_MateDev_ReadyFlag())
    {
        Switch_choice* p_self_info = &(Get_pDev_Mate_Buf()->Self_SWITCH[Get_DataIndex(switch_bit)]);
        
        memmove(p_self_info, &Device_info[1], sizeof(*p_self_info));
    }
}



void Read_Device_State(void)
{
    Set_Carrier_Info(Get_pSelfDevMacInfo()->mac);
#if 1
    //设置从节点的设备地址
    Set_NodeAddr(&(Get_pCarrier_Info()->carrierSelfNode_Addr[0]));
#endif
    Dev_StateInfo_Init(SWITCH_1);
}


/*
验证MAC地址是否拥有并填充设备初始信息
*/
void MacAddr_Read(void)
{
    uint8_t TxEnd_Cnt = 0;
    uint8_t const no_addr_GA[GROUP_ADDR_LEN] = {0XFF,0XFF,0XFF};
    do
    {
        if(Get_pDevUartTxBuf()->Tx_Status == UartTx_End)
        {
            if(++TxEnd_Cnt > Tx_GapTime_Size) //调整帧与帧发送的间隙
            {
                TxEnd_Cnt = 0;
                Get_pDevUartTxBuf()->Tx_Status = UartTx_Finished;
            }
        }
        
        Get_pSystemFunc()->Mac_RxProcess(Get_pDevUartTxBuf());
        RxData_Process();
        
        Get_Mem_MacInfo();
    }
    while (Get_pSelfDevMacInfo()->mac_h != AESFreamHeader);
    
    if (Verify_EncryptionID() == 0) for(;;);
	
    USART2Com_DIS();
    if (memcmp(Get_pDevAesInfo()->addr_GA, no_addr_GA, GROUP_ADDR_LEN) != 0)
    {
        Set_LogicAddr_SetFlag(1);
        Get_pHKFrame_Buf()->OnLine_Staus = 1;
    }
    else
    {
        Set_LogicAddr_SetFlag(0);
    }
    Set_DEV_Action_Bits(Get_pSelfDevMacInfo()->dev_action_bits);
    
    if (Get_pSelfDevMacInfo()->mate_dev_exist == Mate_Addr_Exist)
        Set_MateDev_ReadyFlag(1);
    else
        Set_MateDev_ReadyFlag(0);
}


void Aes_Key_Init(void)
{
    //计算出密文，存放在aes_w，供加解密用
    memmove(&aes_out[2*RsaByte_Size], Get_pDevAesInfo()->aes, RsaByte_Size);
    memmove(&aes_out[3*RsaByte_Size], Get_pDevAesInfo()->addr_GA, GROUP_ADDR_LEN);
    
    Rsa_Decode(aes_out);
    key_expansion(aes_out, aes_w);
}


void AesFrame_Process(uint8_t *p_source)
{
    uint8_t reply_array[MAC_Data_Len] = {0};
    
    if(p_source[1] == MACWrite_Cmd_Request)
    {
        if(memcmp((uint8_t *)Get_pSelfDevMacInfo(), p_source, MAC_Data_Len + 6) != 0) //只能烧一次
        {
            Get_pSystemFunc()->FLASH_Erase(MAC_INFO_SAVE_ADDR);
            Get_pSystemFunc()->FLASH_Write(MAC_INFO_SAVE_ADDR, p_source, MAC_Data_Len + 6);
            Get_pSystemFunc()->FLASH_Read(MAC_INFO_SAVE_ADDR, Get_pSelfDevMacInfo(), MAC_Data_Len + 6);
            
            if(memcmp(Get_pSelfDevMacInfo(), p_source, MAC_Data_Len + 6) == 0)
            {
                memset(reply_array, MAC_FRAME_CORRECT, MAC_Data_Len);
            }
            else
            {
                memset(reply_array, MAC_FRAME_ERROE, MAC_Data_Len);
            }
        }
        else
        {
            memset(reply_array, MAC_FRAME_EXIST, MAC_Data_Len);
        }

        Get_pSystemFunc()->Com2TxData(Get_pDevUartTxBuf(), reply_array, MAC_Data_Len);
        
        Mem_EncryptionID();
    }
}


