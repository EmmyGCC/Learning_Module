#include "74.h"


uint16_t code16_74bit(uint8_t InData);
uint8_t decode16_74bit(uint16_t InData);
static uint8_t decode74(uint8_t InData);
static uint8_t code74(uint8_t Indata);



uint8_t code74(uint8_t InData)
{
    uint8_t temp=0;

    temp = InData & 0xf0; //输入的数据高4位有效
    temp |= 0x01;         //最后不用的1位固定为1
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

    temp_val = InData;                              //默认取高4位
    verify_val = (uint16_t)(code74(temp_val) << 8); //高字节
    temp_val = InData << 4;                         //取低4位
    verify_val |= (uint16_t)code74(temp_val);       //或上低字节
    return verify_val;
}


uint8_t decode74(uint8_t InData)
{
    uint8_t temp=0;

    temp = InData;
    if (((temp >> 4) + (temp >> 5) + (temp >> 6) + (temp >> 3)) & 0x01) //D0,D1,D2,P3有一个错
    {
        if (((temp >> 4) + (temp >> 5) + (temp >> 7) + (temp >> 2)) & 0x01) //D0,D1有一个错
        {
            if (((temp >> 4) + (temp >> 6) + (temp >> 7) + (temp >> 1)) & 0x01) //D0错
            {
                temp ^= 0x10; //将D0取反
            }
            else //D1错
            {
                temp ^= 0x20; //将D1取反
            }
        }
        else //D2,P3有一个错
        {
            if (((temp >> 4) + (temp >> 6) + (temp >> 7) + (temp >> 1)) & 0x01) //D2错
            {
                temp ^= 0x40; //将D2取反
            }
            else //P3错
            {
                temp ^= 0x08; //将P3取反
            }
        }
    }
    else //D3,P2,P1错 或 全部正确
    {
        if (((temp >> 4) + (temp >> 5) + (temp >> 7) + (temp >> 2)) & 0x01) //D3,P2有一个错
        {
            if (((temp >> 4) + (temp >> 6) + (temp >> 7) + (temp >> 1)) & 0x01) //D3错
            {
                temp ^= 0x80; //将D3取反
            }
            else //P2错
            {
                temp ^= 0x04; //将P2取反
            }
        }
        else //P1错或全部正确
        {
            if (((temp >> 4) + (temp >> 6) + (temp >> 7) + (temp >> 1)) & 0x01) //P1错
            {
                temp ^= 0x02; //将P1取反
            }
        }
    }
    return temp;
}


uint8_t decode16_74bit(uint16_t InData)
{
    uint8_t temp_val=0;
    uint8_t right_val=0;

    temp_val = (uint8_t)(InData >> 8); //取出高字节
    right_val = decode74(temp_val) & 0xf0;
    temp_val = (uint8_t)InData; //取出低字节
    right_val |= decode74(temp_val) >> 4;
    return right_val;
}
