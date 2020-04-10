#include "stm32f10x.h"
#include "lcd.h"
#include "snake.h"
#include "stdlib.h"
#include "time.h"

Snake  snake;
Food  food;

//=================================================================
//�������ƣ�Trun_On_Point(u8 x,u8 y)
//�������ܣ�����һ���� ������ʾ�ߵ�����
//��ڲ������ߵ��������
//���ڲ�������
//=================================================================
void Trun_On_Point (u16 x,u16 y)
{
	LCD_Fill(x*8, y*8, x*8+8, y*8+8, RED);
}


//=================================================================
//�������ƣ�Trun_Off_Point(u8 x,u8 y)
//�������ܣ�Ϩ��һ���� ���������ߵ�����
//��ڲ�����
//���ڲ�������
//=================================================================
void Trun_Off_Point (u16 x,u16 y)
{
	LCD_Fill(x*8, y*8, x*8+8, y*8+8, WHITE);
}


//�߳�ʼ������
void Snake_Init(void)  
{
	u8 i;
	snake.longth = 3;	    			//�����ʼ���ߵĳ���
	snake.islive = 1;	    			//1��ʼ���߻�����
	snake.score = 0;                   //����Ϊ0
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

//��ʾ��Ϸ����
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

//�ж����Ƿ��������� ������snake.islive��0
void Is_Snake_Dead()
{
	u8 i;
	if(snake.XPos[snake.longth-1] <= 1 || snake.XPos[snake.longth-1] >= 28 
		|| snake.YPos[snake.longth-1] <= 1 || snake.YPos[snake.longth-1] >= 33  )
	{
		snake.islive = 0; //������
	}
	
	//�ж����Ƿ�ҧ���Լ� �ӵ����ڿ�ʼ�ж�
//	for(i = 0; i < snake.longth-2; i++)
//	if(snake.XPos[snake.longth-1] == snake.XPos[i] && snake.YPos[snake.longth-1] == snake.YPos[i])
//	{
//		snake.islive = 0;
//	}
	
	if(snake.islive == 0) //�����������Ϸ����
	{
		Display_Dead();
	}
}

//���˶�����
void Snake_Run(u8 dir)  
{
	u8 i;
	Trun_Off_Point(snake.XPos[0],snake.YPos[0]); //��Ϩ����β
	
    if(dir == right)
	{
		for(i = 0; i < snake.longth - 1 ; i++)
		{
			snake.XPos[i] = snake.XPos[i+1];
			snake.YPos[i] = snake.YPos[i+1];
		}
		snake.XPos[snake.longth-1] = snake.XPos[snake.longth-2] + 1; //��ͷ�ƶ�����һλ��
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
	
	for(i=0; i<snake.longth; i++)			            //������ʾ����
    {
		Trun_On_Point(snake.XPos[i],snake.YPos[i]);
    }   
	
	Is_Snake_Dead();
	Eat_Food();
}

void Eat_Food()
{ 
	u8 length[20]; //���ڱ�������score �� �����ȵ�ֵ
	u8 i;
	/*************************�����Ƿ�Ե�ʳ��**********************/	
	if(snake.XPos[snake.longth -1] == food.x && snake.YPos[snake.longth -1] == food.y)
	{
		snake.longth ++;
		snake.score += 5;
		snake.XPos[snake.longth -1] = food.x;
		snake.YPos[snake.longth -1] = food.y;
		sprintf((char*)length,"%d  length:%d",snake.score,snake.longth);//��score �� �����ȵ�ֵ��ӡ��length����
		LCD_ShowString(75,300,200,12,12,length);//Show Score
		food.flag = 1;
	}
	
	//��ʳ��
	if(food.flag == 0)  //�����־λΪ0 ����һ���µĵ�
	{
		Trun_On_Point (food.x, food.y);
	}
	
	if(food.flag==1)  
	{
		srand(RTC_GetCounter());
		while(1)
		{  
			food.x=rand()%28+1;      			//��������
			food.y=rand()%33+1; 

		    for(i=0;i<snake.longth ;i++)				//�жϲ�����ʳ�������Ƿ�������غ�
			{
				if((food.x==snake.XPos[i]) && (food.y==snake.YPos[i])) break;
			}
			if(i==snake.longth)    //���������µĵ㲻�������غ� ����־��Ϊ0
			{
				food.flag=0;
				LCD_ShowNum(150,290,food.x,3,12);  
				LCD_ShowNum(150,300,food.y,3,12);  
				break;								//������Ч��ʳ������
			}
		}
	}	
}

