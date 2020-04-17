#include <string.h>

#include "rsa.h"
#include "stm32f0_iwdg.h"



uint8_t LargeNumber_Comp(uint8_t *p_first, uint8_t *p_second, uint8_t bit_size)
{
    uint8_t i;

    for (i = 0; i < bit_size; i++)
    {
        if (p_first[i] > p_second[i])
            return 1;
        else if (p_first[i] < p_second[i])
            return 2;
    }
    return 0;
}

uint8_t LargeNumber_Size(uint8_t *p_integer, uint8_t bit_size)
{
    uint8_t i;

    for (i = 0; i < bit_size; i++)
    {
        if (p_integer[i] > 0)
            return (bit_size - i);
    }
    return 0;
}

void LargeNumber_MoveHigh_1bit(uint8_t p_sourse[RsaByte_SizeMax])
{
    uint8_t i;

    for (i = 0; i < (RsaByte_SizeMax - 1); i++)
    {
        p_sourse[i] = p_sourse[i + 1];
    }
    p_sourse[RsaByte_SizeMax - 1] = 0;
}

void LargeNumber_SameMod(uint8_t *p_sourse, uint8_t *p_subtrahend, uint8_t bit_size)
{
    while (LargeNumber_Comp(p_sourse, p_subtrahend, bit_size) != 2)
    {
        uint8_t i, jiewei = 0;
        uint16_t temp_val = 0;

        for (i = bit_size; i > 0; i--)
        {
            uint8_t sub_val;

            temp_val = p_sourse[i - 1];
            if (temp_val == p_subtrahend[i - 1])
            {
                if (jiewei == 1)
                {
                    sub_val = (temp_val + 0xff) - p_subtrahend[i - 1];
                    //jiewei = 1;
                }
                else
                {
                    sub_val = 0;
                    //jiewei = 0;
                }
            }
            else if (temp_val > p_subtrahend[i - 1])
            {

                sub_val = (temp_val - jiewei) - p_subtrahend[i - 1];
                jiewei = 0;
            }
            else
            {
                sub_val = (temp_val + 0x0100 - jiewei) - p_subtrahend[i - 1];
                jiewei = 1;
            }
            p_sourse[i - 1] = sub_val;
        }
    }
}

void LargeNumber_Modulo(uint8_t dividend[RsaByte_SizeMax], uint8_t divisor[RsaByte_SizeMax])
{
    uint8_t i, move_weishu;

    move_weishu = RsaByte_SizeMax - LargeNumber_Size(divisor, RsaByte_SizeMax);
    //LargeNumber_MoveHigh(divisor, move_weishu);
    LargeNumber_SameMod(dividend, &divisor[move_weishu], RsaByte_SizeMax - move_weishu);
    for (i = 0; i < move_weishu; i++)
    {
        //LargeNumber_MoveLow_1bit(&divisor[i], RsaByte_SizeMax - move_weishu);
        LargeNumber_SameMod(&dividend[i], &divisor[move_weishu - 1], RsaByte_SizeMax - move_weishu + 1);
    }
}

void LargeNumber_Mul(uint8_t mul_factor1[RsaByte_Size], uint8_t mul_factor2, uint8_t mul_Product[RsaByte_SizeMax], uint8_t flag)
{

    uint8_t i, jinwei = 0;
    uint16_t chen_ji;
    //uint8_t *p = (uint8_t *)(&chen_ji);

    if (flag == 1)
    {
        for (i = RsaByte_Size; i > 0; i--)
        {
            chen_ji = mul_factor1[i - 1] * mul_factor2 + jinwei + mul_Product[i];
            //mul_Product[i] = p[0];
            //jinwei = p[1];
            mul_Product[i] = chen_ji & 0xff;
            jinwei = chen_ji >> 8;
        }
        mul_Product[0] = jinwei + mul_Product[0];
    }
    else
    {
        for (i = RsaByte_Size; i > 0; i--)
        {
            chen_ji = mul_factor1[i - 1] * mul_factor2 + jinwei;
            //mul_Product[i] = p[0];
            //jinwei = p[1];
            mul_Product[i] = chen_ji & 0xff;
            jinwei = chen_ji >> 8;
        }
        mul_Product[0] = jinwei;
    }
}


void LargeNumber_Mul_Mod(uint8_t mul_factor1[RsaByte_Size], uint8_t mul_factor2[RsaByte_Size], uint8_t mul_Product[RsaByte_SizeMax], uint8_t divisor[RsaByte_SizeMax])
{
    uint8_t i, bit_size;
    uint8_t *p;

    bit_size = LargeNumber_Size(mul_factor2, RsaByte_Size);
    p = &mul_factor2[RsaByte_Size - bit_size];

    for (i = 0; i < RsaByte_SizeMax; i++)
    {
        mul_Product[i] = 0;
    }
    ////////////////////////////

    for (i = 0; i < bit_size - 1; i++)
    {
        LargeNumber_Mul(mul_factor1, p[i], mul_Product, 1);
        LargeNumber_Modulo(mul_Product, divisor);
        //LargeNumber_Mul(mul_Product, 0xFF, mul_Product, 0);
        LargeNumber_MoveHigh_1bit(mul_Product);
        LargeNumber_Modulo(mul_Product, divisor);
    }
    LargeNumber_Mul(mul_factor1, p[i], mul_Product, 1);
    LargeNumber_Modulo(mul_Product, divisor);
}

uint8_t index_cnt;

void LargeNumber_Power_Mod(uint8_t model[RsaByte_Size], uint8_t radix[RsaByte_Size], uint8_t index[RsaByte_Size], uint8_t mul_Product[RsaByte_SizeMax], uint8_t divisor[RsaByte_SizeMax])
{
  uint8_t i,j, temp_val, bit_size, byte0_cnt = 8;
  uint8_t *p;
  
  bit_size = LargeNumber_Size(index, RsaByte_Size);  
  if(bit_size == 0)
  	return;
  
  memmove(model, radix, RsaByte_Size);

  ////////////////////////////////////////
  
  p = &index[RsaByte_Size - bit_size];
  
  temp_val = p[0];
  for(i = 0; i < 8; i++)
  {
    if(temp_val & 0x80)
      break;
    else
    {
      temp_val <<= 1;
      byte0_cnt--;
    }    
  }
  #if(1)
  temp_val <<= 1;
  byte0_cnt--;
  #endif

  for(i = 0; i < byte0_cnt; i++)
  {
    IWDG_FEED();
    LargeNumber_Mul_Mod(model, model, mul_Product, divisor);
    memmove(model, &mul_Product[1], RsaByte_Size);
    //while(1);
    if(temp_val & 0x80)
    {
      LargeNumber_Mul_Mod(radix, model, mul_Product, divisor);
      memmove(model, &mul_Product[1], RsaByte_Size);
    }
    temp_val <<= 1; 
  }//////////////
  
  p++;
  for(index_cnt = 0; index_cnt < (bit_size-1); index_cnt++)
  {
    temp_val = p[index_cnt];
    for(j = 0; j < 8; j++)
    {
      IWDG_FEED();
      LargeNumber_Mul_Mod(model, model, mul_Product, divisor);
      memmove(model, &mul_Product[1], RsaByte_Size);
      if(temp_val & 0x80)
      {
        LargeNumber_Mul_Mod(radix, model, mul_Product, divisor);
        memmove(model, &mul_Product[1], RsaByte_Size);
      }
      temp_val <<= 1;
    }
  }
}

