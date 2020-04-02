/*
 * snake.c
 *
 *  Created on: 2020年3月27日
 *      Author: Administrator
 */
#include "snake.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "time.h"
#include "stm32f1xx_hal.h"
#include "lcd.h"
#include "adc.h"




SNAKE_T* SnakeList;
uint8_t last_dir = DIR_RIGHT;		//蛇身上次移动的方向
uint16_t food_pos[2] = {0};
uint32_t RandomSeed = 0;

uint16_t ScorePool[MAX_EATEN_CNT] = {0};
volatile uint8_t EatenFoodCnt = 0;



static SNAKE_T* Node_Create(void)
{
	return malloc(sizeof(SNAKE_T));
}


void SnakeList_Init(void)
{
	SnakeList = Node_Create();
	uint8_t length = 0;
	printf("\r\n----SnakeList_Init----\r\n");	// 进入回调函数标志
	if (SnakeList == NULL)
	{
		printf("SnakeList_Init error\r\n");
		return;
	}

	SnakeList->next = NULL;
	SnakeList->prev = SnakeList;
	SnakeList->x = 10;	//蛇有的初始位置
	SnakeList->y = 10;

	for (; length < 3; length++)
	{
		SnakeList_LengthAppend(SnakeList);
	}
}


void Score_Init(uint16_t max_eaten_cnt)
{
	uint16_t cnt = 0;

	for (cnt = 0; cnt < max_eaten_cnt; cnt++)
	{
		ScorePool[cnt] = (cnt * 10);
		//printf("ScorePool[%d] = %d\r\n", cnt, ScorePool[cnt]);
	}
}


void SnakeList_LengthAppend(SNAKE_T* p_head)
{
	SNAKE_T *new_node = Node_Create();
	SNAKE_T *p_operate = p_head;

	if (p_head == NULL)	//检查列表头地址有效
	{
		printf("SnakeList_LengthAppend error\r\n");
		free(new_node);
		return;
	}
	/*printf("\r\n----new_node addr:%p----\r\n", new_node);	// 进入回调函数标志*/
	while (p_operate->next != NULL)		//找到最后一个不为空的节点地址
	{
		p_operate = p_operate->next;
	}

	/*********************************************/
	new_node->next = p_operate->next;	//根据while条件判断下来，其实就是NULL
	new_node->prev = p_operate;
	new_node->x = p_operate->x;
	new_node->y = p_operate->y;
	p_operate->next = new_node;			//尾结点的下一个接上新节点
	/*********************不能忘记了这个************************/
	p_head->prev = new_node;			//头结点的上一个是尾结点
}


uint8_t Snake_Direction_Input(void)
{
	uint16_t adc_value[ADC_SAMPLE_TIMES] = {0};
    uint16_t adc_x = 0;
    uint16_t adc_y = 0;
    
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&adc_value[0], ADC_SAMPLE_TIMES);
    
    for (uint8_t cnt = 0; cnt < ADC_SAMPLE_TIMES/2; cnt++)
    {
        adc_y += adc_value[cnt*2];
    }
    adc_y /= (ADC_SAMPLE_TIMES/2);
    printf("\r\n----adc_y: %d----\r\n", adc_y);
    
    for (uint8_t cnt = 0; cnt < ADC_SAMPLE_TIMES/2; cnt++)
    {
        adc_x += adc_value[cnt*2+1];
    }
    adc_x /= (ADC_SAMPLE_TIMES/2);
    printf("\r\n----adc_x: %d----\r\n", adc_x);
    
	if (adc_x >= 4000)
	{
		return DIR_LEFT;
	}
	else if(adc_x <= 1000)
	{
		return DIR_RIGHT;
	}
    if (adc_y <= 300)
	{
		return DIR_UP;
	}
	else if(adc_y >= 800)
	{
		return DIR_DOWN;
	}
	return DIR_NONE;
}

/*
uint8_t KEY_Direction_Input(void)
{
	if( HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) == 0 )			// 读取GPIO_0电平,按下为方向上
	{
		return DIR_LEFT;
	}
	if( HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) == 0 )			// 读取GPIO_5电平,按下为方向上
	{
		return DIR_UP;
	}
	if( HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) == 0 )			// 读取GPIO_12电平,按下为选择Level1
	{
		return DIR_RIGHT;
	}
	if( HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) == 0 )			// 读取GPIO_13电平,按下为选择Level1
	{
		return DIR_DOWN;
	}

	return DIR_NONE;
}
*/

void Snake_Move(SNAKE_T* p_head)
{
	SNAKE_T *p_operate = p_head;
	SNAKE_T *p_tail = p_head->prev;				//获取到尾结点
	uint8_t move_dir = last_dir;                //获取真实移动的方向
	static uint8_t cur_dir = DIR_NONE;

	if (p_operate == NULL)
	{
		printf("SnakeList_Move error\r\n");
		return;
	}
    
    Trun_Off_Point(p_tail->x, p_tail->y);       //熄灭蛇尾的点
	/********************从尾结点向前遍历链表并修改值*********************/
	while (p_tail != p_head)
	{
		/*printf("\r\n----p_tail->prev addr = %p----\r\n", p_tail->prev);
		printf("\r\n----p_tail->data = %d,%d----\r\n", p_tail->x, p_tail->y);*/
		p_tail->x = p_tail->prev->x;			//尾结点的值为上一个节点的值，这样移动的时候就在向前移动
		p_tail->y = p_tail->prev->y;

		p_tail = p_tail->prev;
	}

	cur_dir = Snake_Direction_Input();			//KEY_Direction_Input();
	if (cur_dir != move_dir)					//修改操作后的状态
	{
		if (cur_dir != DIR_NONE)
		{
			move_dir = cur_dir;					//此次获取的方向值，在下次进来判断switch才会生效
            last_dir = move_dir;
            RandomSeed = HAL_GetTick();
		}
	}
	printf("\r\n----Snake_Move move_dir:%d ----\r\n", move_dir);
	switch(move_dir)							//判断头结点的位置改变方向修改头结点的值
	{
		case DIR_UP:
			p_operate->y--;
			break;
		case DIR_DOWN:
			p_operate->y++;
			break;
		case DIR_RIGHT:
			p_operate->x++;
			break;
		case DIR_LEFT:
			p_operate->x--;
			break;
		default:
			break;
	}
    
    Snake_Show(p_head);
}



static uint16_t random(void)
{
    uint16_t ret = 0;
    
    /* 初始化随机数发生器 */
    srand(RandomSeed);
    /* 产生一个0-30000的随机数 */
    ret = rand() % 30000;
    printf("%d\n", ret);
    
    return ret;
}


void Create_NewFood(uint8_t *food_state)
{
	uint16_t random_num1 = 0;
    uint16_t random_num2 = 0;
	uint16_t x_value = 0;
	uint16_t y_value = 0;

	while(*food_state == FOOD_EMPTY)
	{
		random_num1 = HAL_GetTick();//random();
        random_num2 = HAL_GetTick();//random();
		x_value = random_num1 % 240;
		y_value = random_num2 % 320;

		if ((x_value > BORDER_LEFT_POS && x_value < BORDER_RIGHT_POS) 
            && (y_value < ((280-BORDER_WIDTH)/BORDER_WIDTH) && y_value > BORDER_UP_POS))		//放置食物的位置合规
		{
			printf("\r\n----Create_NewFood----\r\n");
            printf("\r\n----x_value: %d----\r\n", x_value);
        printf("\r\n----Y_value: %d----\r\n", y_value);
            Trun_On_Point(x_value, y_value);
            
            food_pos[0] = x_value;
            food_pos[1] = y_value;
            *food_state = FOOD_READY;
            break;
		}
	}
}


static uint8_t Snake_State(SNAKE_T* p_head, uint8_t *food_state)
{
	if (p_head->x == BORDER_LEFT_POS || p_head->x == BORDER_RIGHT_POS || p_head->y == BORDER_UP_POS || p_head->y == BORDER_DOWN_POS)
		return SNAKE_HIT;

	if (p_head->x == food_pos[0] && p_head->y == food_pos[1])
	{
		*food_state = FOOD_EMPTY;
        Trun_On_Point(food_pos[0], food_pos[1]);
		EatenFoodCnt++;

		return FOOD_GOT;
	}

	return SNAKE_MOVE;
}


void Show_AddScore(void)
{
	char src[5] = "";
	char dest[20] = "";

	sprintf(src, "%d", ScorePool[EatenFoodCnt]);
	printf("\r\n========src: %s, %d========\r\n", src, src[0]);//******************************
	memmove(dest, "Score: ", strlen("Score:"));

	strncat(dest, src, strlen(src));
    
    LCD_ShowString((MAX_ROW - strlen((char *)dest)*(FONT_SIZE_24/2))/2, 300,
                    200, FONT_SIZE_24, (uint8_t *)dest);
}
/*
 * 检查游戏状态
 * */
void Check_GameState(uint8_t *food_state)
{
	uint8_t game_state = SNAKE_MOVE;

	game_state = Snake_State(SnakeList, food_state);
	/*printf("\r\ngame_state: %d\r\n", game_state);	// 进入回调函数标志*/
	switch(game_state)
	{
		case SNAKE_MOVE:
			break;
		case FOOD_GOT:
			Show_AddScore();
			SnakeList_LengthAppend(SnakeList);
			break;
		case SNAKE_HIT:
			printf("your snake hit the wall, game over!\r\n");
			game_state = GAME_OVER;
			LCD_GameOverShow();
            
			HAL_NVIC_SystemReset();
			break;
		default:
			break;
	}
}


void Game_Running(SNAKE_T* p_head)
{
	static uint8_t food_state = FOOD_EMPTY;
    
	Snake_Move(p_head);

	Create_NewFood(&food_state);

	Check_GameState(&food_state);
}


/*LCD绘制出蛇身*/
void Snake_Show(void *p_head)
{
    SNAKE_T *p_operate = (SNAKE_T *)p_head;
    
    while(p_operate != NULL)
    {
        Trun_On_Point(p_operate->x, p_operate->y);
        
        p_operate = p_operate->next;
    }
}


