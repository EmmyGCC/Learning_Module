#ifndef __SNAKE_H
#define __SNAKE_H
#include "stm32f10x.h"
#define MAX_Longth    100 //�����ߵ���󳤶�
#define straight  0
#define left   1
#define right  2
#define down   3

//����һ��̰���ߵĽṹ��
typedef struct
{
	u16 XPos[MAX_Longth]; 
	u16 YPos[MAX_Longth];
	u8 longth;   
	u8 score;
	u8 islive;   //�ж����Ƿ����1�� ��  0����
}Snake;

//����һ��ʳ��Ľṹ��
typedef struct
{
	u16 x;
	u16 y;
	u8 flag;  //1 ����ʳ�� 0 ����
}Food;

void Trun_On_Point (u16 x,u16 y);	
void Trun_Off_Point (u16 x,u16 y);
void Snake_Run(u8 dir) ;
void Snake_Init(void) ;
void Is_Snake_Dead(void);
void Display_Dead(void);
void Eat_Food(void);
u8 Is_Alive(Snake snake);
#endif
