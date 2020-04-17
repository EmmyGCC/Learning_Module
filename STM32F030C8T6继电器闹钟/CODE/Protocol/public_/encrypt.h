#ifndef __ENCRYPT_H
#define __ENCRYPT_H

#include "aes.h"



typedef enum
{
    DECRYPT = 0,
    ENCRYPT
}ENCRYPY_TYPE_E;

#define RoutPath_Size                  3               //路由的最大路径数
#define RoutSeries_Size                3               //路由的最大级数


void Rsa_Decode(uint8_t *p_minw);
void Encrypt_Convert(uint8_t *p_source, uint8_t input_len, uint8_t *output_len, ENCRYPY_TYPE_E Convert_Mode) ;

#endif

