#include "crc16.h"

//CRC16校验在通讯中应用广泛，这里不对其理论进行讨论，只对常见的3种
//实现方法进行测试。方法1选用了一种常见的查表方法,类似的还有512字
//节、256字等查找表的，至于查找表的生成,这里也略过。
// ---------------- POPULAR POLYNOMIALS ----------------
// CCITT:      x^16 + x^12 + x^5 + x^0                 (0x1021)
// CRC-16:     x^16 + x^15 + x^2 + x^0                 (0x8005)
#define CRC_16_POLYNOMIALS 0x8005

// --------------------------------------------------------------
//      CRC16计算方法1:使用2个256长度的校验表
// --------------------------------------------------------------
const uint8_t chCRCHTalbe[] = // CRC 高位字节值表
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40
};
const uint8_t chCRCLTalbe[] = // CRC 低位字节值表
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
    0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
    0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
    0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
    0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
    0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
    0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
    0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
    0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
    0x41, 0x81, 0x80, 0x40
};

uint16_t CRC16_1(uint8_t *pchMsg, uint16_t wDataLen)
{
    uint8_t chCRCHi = 0xFF; // 高CRC字节初始化
    uint8_t chCRCLo = 0xFF; // 低CRC字节初始化
    uint16_t wIndex;        // CRC循环中的索引
    while (wDataLen--)
    {
        wIndex = chCRCLo ^ *pchMsg++;
        chCRCLo = chCRCHi ^ chCRCHTalbe[wIndex];
        chCRCHi = chCRCLTalbe[wIndex];
    }
    return ((chCRCHi << 8) | chCRCLo);
}

// --------------------------------------------------------------
//      CRC16计算方法2:使用简单的校验表
// --------------------------------------------------------------
const uint16_t wCRCTalbeAbs[] ={0x0000, 0xCC01, 0xD801, 0x1400, 
                            0xF001, 0x3C00, 0x2800, 0xE401, 
                            0xA001, 0x6C00, 0x7800, 0xB401, 
                            0x5000, 0x9C01, 0x8801, 0x4400,};

uint16_t CRC16_2(uint8_t *pchMsg, uint16_t wDataLen)
{
    uint16_t wCRC = 0xFFFF;
    uint16_t i;
    uint8_t chChar;
    for (i = 0; i < wDataLen; i++)
    {
        chChar = *pchMsg++;
        wCRC = wCRCTalbeAbs[(chChar ^ wCRC) & 15] ^ (wCRC >> 4);
        wCRC = wCRCTalbeAbs[((chChar >> 4) ^ wCRC) & 15] ^ (wCRC >> 4);
    }
    return wCRC;
}

// -----------------------------------------------------------------
//      CRC16计算方法3:使用直接结算的方法
// -----------------------------------------------------------------
uint16_t CRC16_3(uint8_t *pchMsg, uint16_t wDataLen)
{
    uint8_t i, chChar;
    uint16_t wCRC = 0xFFFF;
    while (wDataLen--)
    {
        chChar = *pchMsg++;
        chChar = (uint8_t)(~chChar);
        wCRC ^= (((uint8_t)chChar) << 8);
        for (i = 0; i < 8; i++)
        {
            if (wCRC & 0x8000)
                wCRC = (wCRC << 1) ^ CRC_16_POLYNOMIALS;
            else
                wCRC <<= 1;
        }
    }
    wCRC = (uint16_t)(~wCRC);
    return wCRC;
}

uint16_t CRC16_check(uint16_t *Data, uint16_t Data_length)
{
    uint16_t mid = 0;
    uint16_t times = 0, Data_index = 0;
    uint16_t sCRC = 0xFFFF;
    while (Data_length)
    {
        sCRC = Data[Data_index] ^ sCRC;
        for (times = 0; times < 8; times++)
        {
            mid = sCRC;
            sCRC = sCRC >> 1;
            if (mid & 0x0001)
            {
                sCRC = sCRC ^ 0xA001;
            }
        }
        Data_index++;
        Data_length--;
    }
    return sCRC;
}

//试验数据：
//      采用Metrowerks CodeWarrior在DSP56F80x平台上,对这3种方法
//进行了性能测试。
// ----------------------------------------------------------------
//                      代码大小(字)    额外存储空间(字)        执行时间(周期数)
// ----------------------------------------------------------------
//      方法1           32                      512                             540
//      方法2           57                      16                              1120
//      方法3           142*                    0                               4598
//
//说明：方法3的代码大小还包括字反转、字节反转程序(这里没有给出源码)
//
//结论：通常在存储空间没有限制的情况下，采用方法1是最好的，毕竟在
//通讯中，保障通讯速度是至关重要的。而方法2也不失为一种很好的方法，
//占用空间很少。而与方法2相比，方法3似乎不占有什么优势。

/*
//CRC16生成校验码

//生成多项式：X16+X15+X2+1

#include <stdio.h>

void main(void)

{

  unsigned char data[7] = { 0x7f };//{ 0x10,0x01,0xA2,0xE0,0x02,0x0A,0x03 };{ 0x7E,0x00,0x05,0x60,0x31,0x32,0x33 };

  unsigned int CRC_code = 0x0000;

  unsigned int CRC16 = 0x8005;

  //	unsigned char CRC1,CRC2;

  int i = 0,j = 0;

  for( i=0; i<1; i++ )

  {

  CRC_code = CRC_code ^ ( ((unsigned int)data[i]) << 8 );

  for( j=0; j<8; j++ )

  {

    if( CRC_code & 0x8000 )

    {

      CRC_code <<= 1;

      CRC_code = CRC_code ^ CRC16;

    }

    else

      CRC_code <<= 1;

  }

}

CRC_code = CRC_code & 0xFFFF;

printf( "生成校验码CRC（十六进制）：%X\n",CRC_code );

//	printf( "生成校验码CRC1（十六进制）：%X\n",CRC1 );

//	printf( "生成校验码CRC2（十六进制）：%X\n",CRC2 );

}
*/