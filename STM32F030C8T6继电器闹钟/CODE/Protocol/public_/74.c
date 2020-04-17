#include "74.h"


uint16_t code16_74bit(uint8_t InData);
uint8_t decode16_74bit(uint16_t InData);
static uint8_t decode74(uint8_t InData);
static uint8_t code74(uint8_t Indata);



uint8_t code74(uint8_t InData)
{
    uint8_t temp=0;

    temp = InData & 0xf0; //��������ݸ�4λ��Ч
    temp |= 0x01;         //����õ�1λ�̶�Ϊ1
    if (((temp >> 4) + (temp >> 5) + (temp >> 6)) & 0x01)
        temp |= 0x08; //P3
    if (((temp >> 4) + (temp >> 5) + (temp >> 7)) & 0x01)
        temp |= 0x04; //P2
    if (((temp >> 4) + (temp >> 6) + (temp >> 7)) & 0x01)
        temp |= 0x02; //P1
    return temp;
}


uint16_t code16_74bit(uint8_t InData)
{
    uint8_t temp_val=0;
    uint16_t verify_val = 0;

    temp_val = InData;                              //Ĭ��ȡ��4λ
    verify_val = (uint16_t)(code74(temp_val) << 8); //���ֽ�
    temp_val = InData << 4;                         //ȡ��4λ
    verify_val |= (uint16_t)code74(temp_val);       //���ϵ��ֽ�
    return verify_val;
}


uint8_t decode74(uint8_t InData)
{
    uint8_t temp=0;

    temp = InData;
    if (((temp >> 4) + (temp >> 5) + (temp >> 6) + (temp >> 3)) & 0x01) //D0,D1,D2,P3��һ����
    {
        if (((temp >> 4) + (temp >> 5) + (temp >> 7) + (temp >> 2)) & 0x01) //D0,D1��һ����
        {
            if (((temp >> 4) + (temp >> 6) + (temp >> 7) + (temp >> 1)) & 0x01) //D0��
            {
                temp ^= 0x10; //��D0ȡ��
            }
            else //D1��
            {
                temp ^= 0x20; //��D1ȡ��
            }
        }
        else //D2,P3��һ����
        {
            if (((temp >> 4) + (temp >> 6) + (temp >> 7) + (temp >> 1)) & 0x01) //D2��
            {
                temp ^= 0x40; //��D2ȡ��
            }
            else //P3��
            {
                temp ^= 0x08; //��P3ȡ��
            }
        }
    }
    else //D3,P2,P1�� �� ȫ����ȷ
    {
        if (((temp >> 4) + (temp >> 5) + (temp >> 7) + (temp >> 2)) & 0x01) //D3,P2��һ����
        {
            if (((temp >> 4) + (temp >> 6) + (temp >> 7) + (temp >> 1)) & 0x01) //D3��
            {
                temp ^= 0x80; //��D3ȡ��
            }
            else //P2��
            {
                temp ^= 0x04; //��P2ȡ��
            }
        }
        else //P1���ȫ����ȷ
        {
            if (((temp >> 4) + (temp >> 6) + (temp >> 7) + (temp >> 1)) & 0x01) //P1��
            {
                temp ^= 0x02; //��P1ȡ��
            }
        }
    }
    return temp;
}


uint8_t decode16_74bit(uint16_t InData)
{
    uint8_t temp_val=0;
    uint8_t right_val=0;

    temp_val = (uint8_t)(InData >> 8); //ȡ�����ֽ�
    right_val = decode74(temp_val) & 0xf0;
    temp_val = (uint8_t)InData; //ȡ�����ֽ�
    right_val |= decode74(temp_val) >> 4;
    return right_val;
}
