#include "stm32f10x.h"
#include "lcd.h"
#include "snake.h"
#include "stdlib.h"
#include "time.h"

Snake  snake;
Food  food;

//=================================================================
//函数名称：Trun_On_Point(u8 x,u8 y)
//函数功能：点亮一个点 用于显示蛇的身体
//入口参数：蛇的起点坐标
//出口参数：无
//=================================================================
void Trun_On_Point (u16 x,u16 y)
{
	LCD_Fill(x*8, y*8, x*8+8, y*8+8, RED);
}


//=================================================================
//函数名称：Trun_Off_Point(u8 x,u8 y)
//函数功能：熄灭一个点 用于隐藏蛇的身体
//入口参数：
//出口参数：无
//=================================================================
void Trun_Off_Point (u16 x,u16 y)
{
	LCD_Fill(x*8, y*8, x*8+8, y*8+8, WHITE);
}


//蛇初始化函数
void Snake_Init(void)  
{
	u8 i;
	snake.longth = 3;	    			//定义初始化蛇的长度
	snake.islive = 1;	    			//1初始化蛇还活着
	snake.score = 0;                   //分数为0
	food.flag = 1;					    //
	
	for(i = 0; i < snake.longth; i++) 
	{
		snake.XPos[i] = 14 + i;
		snake.YPos[i] = 17;
	}
	
	for(i = 0; i < snake.longth ; i++)
	{
		Trun_On_Point(snake.XPos[i], snake.YPos[i]);
	}
}

//显示游戏界面
void Display_Dead()
{
		snake.islive = 0;
		LCD_Clear(BLACK);
		LCD_Fill(0,0,8,280,DARKBLUE);
		LCD_Fill(232,0,240,280,DARKBLUE);
		LCD_Fill(0,0,240,8,DARKBLUE);
		LCD_Fill(0,272,240,280,DARKBLUE);
		LCD_ShowString(60,100,240,24,24,"GAME OVER! "); 
	    LCD_ShowString(70,140,240,24,24,"score: ");
	    LCD_ShowNum (145,140,snake.score,3,24);
	    while(1){}
}	

//判断蛇是否死亡函数 死亡将snake.islive置0
void Is_Snake_Dead()
{
	u8 i;
	if(snake.XPos[snake.longth-1] <= 1 || snake.XPos[snake.longth-1] >= 28 
		|| snake.YPos[snake.longth-1] <= 1 || snake.YPos[snake.longth-1] >= 33  )
	{
		snake.islive = 0; //蛇死亡
	}
	
	//判断蛇是否咬到自己 从第三节开始判断
//	for(i = 0; i < snake.longth-2; i++)
//	if(snake.XPos[snake.longth-1] == snake.XPos[i] && snake.YPos[snake.longth-1] == snake.YPos[i])
//	{
//		snake.islive = 0;
//	}
	
	if(snake.islive == 0) //如果蛇死亡游戏结束
	{
		Display_Dead();
	}
}

//蛇运动函数
void Snake_Run(u8 dir)  
{
	u8 i;
	Trun_Off_Point(snake.XPos[0],snake.YPos[0]); //先熄灭蛇尾
	
    if(dir == right)
	{
		for(i = 0; i < snake.longth - 1 ; i++)
		{
			snake.XPos[i] = snake.XPos[i+1];
			snake.YPos[i] = snake.YPos[i+1];
		}
		snake.XPos[snake.longth-1] = snake.XPos[snake.longth-2] + 1; //蛇头移动到下一位置
		snake.YPos[snake.longth-1] = snake.YPos[snake.longth-2] - 0;
	}
	
	else if(dir == left)
	{
		for(i = 0; i < snake.longth - 1 ; i++)
		{
			snake.XPos[i] = snake.XPos[i+1];
			snake.YPos[i] = snake.YPos[i+1];
		}
		snake.XPos[snake.longth-1] = snake.XPos[snake.longth-2] - 1;
		snake.YPos[snake.longth-1] = snake.YPos[snake.longth-2] + 0;
	}
	
	else if(dir == down)
	{
		for(i = 0; i < snake.longth - 1 ; i++)
		{
			snake.XPos[i] = snake.XPos[i+1];
			snake.YPos[i] = snake.YPos[i+1];
		}
		snake.XPos[snake.longth-1] = snake.XPos[snake.longth-2] + 0;
		snake.YPos[snake.longth-1] = snake.YPos[snake.longth-2] + 1;
	}
	
	else if(dir == straight)
	{
		for(i = 0; i < snake.longth - 1 ; i++)
		{
			snake.XPos[i] = snake.XPos[i+1];
			snake.YPos[i] = snake.YPos[i+1];
		}
		snake.XPos[snake.longth-1] = snake.XPos[snake.longth-2] + 0;
		snake.YPos[snake.longth-1] = snake.YPos[snake.longth-2] - 1;
	}
	
	for(i=0; i<snake.longth; i++)			            //让蛇显示出来
    {
		Trun_On_Point(snake.XPos[i],snake.YPos[i]);
    }   
	
	Is_Snake_Dead();
	Eat_Food();
}

void Eat_Food()
{ 
	u8 length[20]; //用于保留后面score 和 蛇身长度的值
	u8 i;
	/*************************判蛇是否吃到食物**********************/	
	if(snake.XPos[snake.longth -1] == food.x && snake.YPos[snake.longth -1] == food.y)
	{
		snake.longth ++;
		snake.score += 5;
		snake.XPos[snake.longth -1] = food.x;
		snake.YPos[snake.longth -1] = food.y;
		sprintf((char*)length,"%d  length:%d",snake.score,snake.longth);//将score 和 蛇身长度的值打印到length数组
		LCD_ShowString(75,300,200,12,12,length);//Show Score
		food.flag = 1;
	}
	
	//放食物
	if(food.flag == 0)  //如果标志位为0 放置一个新的点
	{
		Trun_On_Point (food.x, food.y);
	}
	
	if(food.flag==1)  
	{
		srand(RTC_GetCounter());
		while(1)
		{  
			food.x=rand()%28+1;      			//获得随机数
			food.y=rand()%33+1; 

		    for(i=0;i<snake.longth ;i++)				//判断产生的食物坐标是否和蛇身重合
			{
				if((food.x==snake.XPos[i]) && (food.y==snake.YPos[i])) break;
			}
			if(i==snake.longth)    //当产生的新的点不与蛇身重合 将标志置为0
			{
				food.flag=0;
				LCD_ShowNum(150,290,food.x,3,12);  
				LCD_ShowNum(150,300,food.y,3,12);  
				break;								//产生有效的食物坐标
			}
		}
	}	
}

