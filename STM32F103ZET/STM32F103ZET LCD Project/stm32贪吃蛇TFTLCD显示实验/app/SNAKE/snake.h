#ifndef __SNAKE_H
#define __SNAKE_H
#include "stm32f10x.h"
#define MAX_Longth    100 //定义蛇的最大长度
#define straight  0
#define left   1
#define right  2
#define down   3

//定义一个贪吃蛇的结构体
typedef struct
{
	u16 XPos[MAX_Longth]; 
	u16 YPos[MAX_Longth];
	u8 longth;   
	u8 score;
	u8 islive;   //判断蛇是否活着1： 活  0：死
}Snake;

//定义一个食物的结构体
typedef struct
{
	u16 x;
	u16 y;
	u8 flag;  //1 放置食物 0 不放
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
