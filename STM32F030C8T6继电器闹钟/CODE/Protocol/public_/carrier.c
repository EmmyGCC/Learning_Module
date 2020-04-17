#include <string.h>
#include <math.h>

#include "carrier.h"
#include "uart_interface.h"
#include "device_basic_info.h"


static CARRIER_T Carrier_Info;



CARRIER_T *Get_pCarrier_Info(void)
{
    return &Carrier_Info;
}



static uint8_t Cal_digits(uint64_t cal_num)
{
	uint8_t leng = 0;
	
	while(cal_num)
	{
		cal_num /= 10;
		leng++;
	}
	return leng;
}


/*
�Ѵ����ʮ������MAC��ַת����BCD���ز�ͨ�ŵ�ַ
*/
void HEXaddr_2_BCDaddr(uint8_t *hex_srcAddr, uint8_t *BCD_desAddr)
{
    uint32_t srcAddr_in_Dec = 0;
    uint64_t hex_val = 0;
    uint64_t dec_val = 0;
    uint64_t srcAddr_Dec_Sum = 0;
    uint8_t num_bit = 0;

    memmove(&srcAddr_in_Dec, hex_srcAddr, MAC_Data_Len);

    uint8_t num_cnt = Cal_digits(srcAddr_in_Dec);
    while(num_cnt)
    {
        num_bit = srcAddr_in_Dec / pow(10, num_cnt-1);
        hex_val = pow(16, num_cnt-1);
        dec_val = pow(10, num_cnt-1);

        srcAddr_Dec_Sum += (num_bit * hex_val);
        srcAddr_in_Dec -= (num_bit * dec_val);
        num_cnt--;
    }
    memmove(BCD_desAddr, &srcAddr_Dec_Sum, Carrier_MAC_LEN);
}


void Set_Carrier_Info(uint8_t *src_addr)
{
    uint8_t MAC_BCD_addr[Carrier_MAC_LEN] = {0};
    //ȡ��Դ��ַ����,ʮ������ת��BCD������ڵ��ַ
    HEXaddr_2_BCDaddr(src_addr, MAC_BCD_addr);
    memmove(Get_pCarrier_Info()->carrierSelfNode_Addr, MAC_BCD_addr, Carrier_MAC_LEN);
    
    USART1Com_EN();
}


/*
Ϊ�ز����͵��������ݴ�������ô��ڽ����ӽڵ�ȥ����
*/
void CarrierFramePackage_SendData(uint8_t *des_addr, uint8_t *src_data, uint16_t data_len)
{
    uint8_t temp_data[CARRIERDATA_LEN] = {0};
    
    CarrierFramePackage_Head1(temp_data);
    CarrierFramePackage_DesAddr(temp_data, des_addr);
    CarrierFramePackage_Head2(temp_data);
    CarrierFramePackage_Cmd(temp_data, 0X11);
    CarrierFramePackage_DataLen(temp_data, data_len);
    CarrierFramePackage_Data(temp_data, src_data, data_len);
    CarrierFramePackage_FormatVerify(temp_data);
    
    Get_pSystemFunc()->Com1TxData(Get_pCarrierUartTxBuf(), temp_data, data_len + Carrier_Data_Num + 2);
}



void Set_NodeAddr(uint8_t *des_addr)
{
    long long SrcAddr = 0;
    long long temp_plusval = 0X333333333333;
    uint8_t p_data[20] = {0};

    CarrierFramePackage_Head1(p_data);
    CarrierFramePackage_DesAddr(p_data, des_addr);
    CarrierFramePackage_Head2(p_data);
    CarrierFramePackage_Cmd(p_data, 0X93);
    CarrierFramePackage_DataLen(p_data, Carrier_MAC_LEN);

    memmove(&SrcAddr, des_addr, Carrier_MAC_LEN);   //����һ��Ҫ�ǹ涨��6�ֽڣ����뵽��ַ����֡ȥ����
    temp_plusval += SrcAddr;	//��ֹ����н�λ����long long���͵ı�������

    CarrierFramePackage_Data(p_data, (uint8_t *)&temp_plusval, Carrier_MAC_LEN);
    CarrierFramePackage_FormatVerify(p_data);

    while(Get_pCarrierUartRxBuf()->Frame_Data[5] != 0XAA);
    Get_pSystemFunc()->Com1TxData(Get_pCarrierUartTxBuf(), p_data, Carrier_MAC_LEN + Carrier_Data_Num + 2);
}






