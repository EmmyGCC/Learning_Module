#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

typedef unsigned long long uint64_t;
typedef unsigned int	uint32_t;
typedef unsigned short	uint16_t;
typedef unsigned char 	uint8_t;

#if 0
//0x50462720
uint64_t BCD2HEX(uint64_t bcd_data)
{
	int i = 0, j = 0, cnt = 0;
	uint64_t val = 0;
	uint8_t res = 0x0f;

	for (i; i< 6; i++)
	{
		cnt = 0;
		for (j=i; j<(2 + i); j++)
		{
			val += (((bcd_data & res)>>(4*cnt)) * pow(10, i+j));
			res ^= 0xff;
			cnt++;
		}
		bcd_data >>= 8;
	}
	return val;
	
}
#else
uint64_t BCD2HEX(uint8_t *hex_data, uint8_t *bcd_data, uint8_t data_len)
{
	uint8_t index = 0, pow_num = 0, move_cnt = 0;
    uint8_t res = 0x0f;
	uint64_t hexindec_val = 0;

	if (!data_len || (data_len > sizeof(uint64_t)))
	{
		return hexindec_val; 
	}
	for (index = 0; index < data_len; index++)
	{
		move_cnt = 0;
        
		for (pow_num = index; pow_num < (2 + index); pow_num++)
		{
			hexindec_val += (((bcd_data[index] & res) >> (4 * move_cnt)) * pow(10, index + pow_num));
            
			res ^= 0xff;
			move_cnt++;
		}
	}
    memmove(hex_data, &hexindec_val, data_len);

	return hexindec_val;
}
#endif


uint64_t BCD_2_HEX(uint8_t *bcd_data)
{
	uint64_t dec_num = 0;

	for (uint8_t cnt = 0; cnt<6; cnt++)
	{
		if (bcd_data[cnt])
		{
			dec_num <<= 8;
		}
		dec_num |= (bcd_data[cnt] & 0xff);
		
		printf("0x%x \n", dec_num);
	}

	return dec_num;
}

int main()
{
	//input 0XFFFFFFFF		BCD	0X84, 0X71, 0x39, 0x50, 0x00, 0x00
	uint8_t input_data[] = {0};
	//output 0XFFFFFFFF		HEX
	uint8_t output_data[6] = {0};
	uint64_t output_dec_num = 0;
	
	output_dec_num = BCD2HEX(output_data, input_data, sizeof(input_data));
	
	printf("output_dec_num in DEC: %d \n", output_dec_num);
	printf("output_dec_num in HEX: 0x%x \n", output_dec_num);
	
	printf("output_data in HEX: \n", output_dec_num);
	for (uint8_t i = 0; i < sizeof(output_data); i++)
		printf("0x%x ", output_data[i]);
	
	printf("\n\n");
	system("pause");
	return 0;
}
