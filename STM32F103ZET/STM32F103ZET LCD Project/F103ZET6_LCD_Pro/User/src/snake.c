/*
 * snake.c
 *
 *  Created on: 2020��3��27��
 *      Author: Administrator
 */
#include "snake.h"
#include "stdlib.h"
#include "stdio.h"
#include "time.h"
#include "stm32f1xx_hal.h"
#include "lcd.h"




SNAKE_T* SnakeList;
uint8_t last_dir = DIR_RIGHT;		//�����ϴ��ƶ��ķ���
uint8_t food_pos[2] = {0};

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
	printf("\r\n----SnakeList_Init----\r\n");	// ����ص�������־
	if (SnakeList == NULL)
	{
		printf("SnakeList_Init error\r\n");
		return;
	}

	SnakeList->next = NULL;
	SnakeList->prev = SnakeList;
	SnakeList->x = 50;	//���еĳ�ʼλ��
	SnakeList->y = 50;

	for (; length < SNAKE_INIT_LENGTH; length++)
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
		printf("ScorePool[%d] = %d", cnt, ScorePool[cnt]);
	}
}


void SnakeList_LengthAppend(SNAKE_T* p_head)
{
	SNAKE_T *new_node = Node_Create();
	SNAKE_T *p_operate = p_head;

	if (p_head == NULL)	//����б�ͷ��ַ��Ч
	{
		printf("SnakeList_LengthAppend error\r\n");
		free(new_node);
		return;
	}
	/*printf("\r\n----new_node addr:%p----\r\n", new_node);	// ����ص�������־*/
	while (p_operate->next != NULL)		//�ҵ����һ����Ϊ�յĽڵ��ַ
	{
		p_operate = p_operate->next;
	}

	/*********************************************/
	new_node->next = p_operate->next;	//����while�����ж���������ʵ����NULL
	new_node->prev = p_operate;
	new_node->x = p_operate->x;
	new_node->y = p_operate->y;
	p_operate->next = new_node;			//β������һ�������½ڵ�
	/*********************�������������************************/
	p_head->prev = new_node;			//ͷ������һ����β���
}


uint8_t Snake_Direction_Input(void)
{
	uint16_t adc_addr[ADC_SAMPLE_TIMES] = {0};
	uint32_t cnt;
	uint16_t adc_value = 0;

	system_adc_read_fast(adc_addr, ADC_SAMPLE_TIMES, 8);
	for(cnt = 0; cnt < ADC_SAMPLE_TIMES; cnt++)
	{
		adc_value += adc_addr[cnt];
		/*printf("i = %d, adc_v = %d \r\n", i, adc_addr[i]);*/
	}
	adc_value /= ADC_SAMPLE_TIMES;

	if (adc_value <= 200)
	{
		return DIR_LEFT;
	}
	else if(adc_value >= 700)
	{
		return DIR_RIGHT;
	}
	if( GPIO_INPUT_GET(GPIO_ID_PIN(5)) == 0 )			// ��ȡGPIO_5��ƽ,����Ϊ������
	{
		return DIR_UP;
	}
	if( GPIO_INPUT_GET(GPIO_ID_PIN(13)) == 0 )			// ��ȡGPIO_13��ƽ,����Ϊѡ��Level1
	{
		return DIR_DOWN;
	}
	return DIR_NONE;
}


uint8_t KEY_Direction_Input(void)
{
	if( GPIO_INPUT_GET(GPIO_ID_PIN(0)) == 0 )			// ��ȡGPIO_0��ƽ,����Ϊ������
	{
		return DIR_LEFT;
	}
	if( GPIO_INPUT_GET(GPIO_ID_PIN(5)) == 0 )			// ��ȡGPIO_5��ƽ,����Ϊ������
	{
		return DIR_UP;
	}
	if( GPIO_INPUT_GET(GPIO_ID_PIN(12)) == 0 )			// ��ȡGPIO_12��ƽ,����Ϊѡ��Level1
	{
		return DIR_RIGHT;
	}
	if( GPIO_INPUT_GET(GPIO_ID_PIN(13)) == 0 )			// ��ȡGPIO_13��ƽ,����Ϊѡ��Level1
	{
		return DIR_DOWN;
	}

	return DIR_NONE;
}


void Snake_Move(SNAKE_T* p_head)
{
	SNAKE_T *p_operate = p_head;
	SNAKE_T *p_tail = p_head->prev;				//��ȡ��β���
	uint8_t move_dir = last_dir;						//��ȡ��ʵ�ƶ��ķ���
	uint8_t cur_dir = DIR_NONE;

	if (p_operate == NULL)
	{
		printf("SnakeList_Move error\r\n");
		return;
	}
	Map[p_tail->x][p_tail->y] = 0;				//oled�ж�Ӧβ�������ص�
	/********************��β�����ǰ���������޸�ֵ*********************/
	while (p_tail != p_head)
	{
		/*printf("\r\n----p_tail->prev addr = %p----\r\n", p_tail->prev);
		printf("\r\n----p_tail->data = %d,%d----\r\n", p_tail->x, p_tail->y);*/
		p_tail->x = p_tail->prev->x;			//β����ֵΪ��һ���ڵ��ֵ�������ƶ���ʱ�������ǰ�ƶ�
		p_tail->y = p_tail->prev->y;

		p_tail = p_tail->prev;
	}

	cur_dir = Snake_Direction_Input();			//KEY_Direction_Input();
	if (cur_dir != move_dir)					//�޸Ĳ������״̬
	{
		if (cur_dir != DIR_NONE)
		{
			last_dir = cur_dir;					//�˴λ�ȡ�ķ���ֵ�����´ν����ж�switch�Ż���Ч
			move_dir = last_dir;
		}
	}
	printf("\r\n----Snake_Move move_dir:%d ----\r\n", move_dir);
	switch(move_dir)							//�ж�ͷ����λ�øı䷽���޸�ͷ����ֵ
	{
		case DIR_UP:
			p_operate->y--;
			break;
		case DIR_DOWN:
			p_operate->y++;
			break;
		case DIR_LEFT:
			p_operate->x--;
			break;
		case DIR_RIGHT:
			p_operate->x++;
			break;
		default:
			break;
	}
}


static uint16_t random(void)
{
    uint16_t ret = 0;
    time_t t;

    /* ��ʼ������������� */
    srand((unsigned) time(&t));
    /* ����һ��0-30000������� */
    ret = rand() % 30000;
    printf("%d\n", ret);
    
    return ret;
}


void Create_NewFood(uint8_t *food_state)
{
	uint16_t random_num = 0;
	uint8_t x_value = 0;
	uint8_t y_value = 0;

	while(*food_state == FOOD_EMPTY)
	{
		random_num = random();
		x_value = random_num % MAX_ROW;
		y_value = random_num % MAX_COLUMN;

		if ((x_value > 1 && x_value < (MAX_ROW-2)) && (y_value < MAX_COLUMN-1 && y_value > (2*8+1)))		//����ʳ���λ�úϹ�
		{
			printf("\r\n----Create_NewFood----\r\n");	// ����ص�������־

			if (Map[x_value][y_value] == 0)
			{
				Map[x_value][y_value] = 1;
				food_pos[0] = x_value;
				food_pos[1] = y_value;
				*food_state = FOOD_READY;
			}
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
		Map[food_pos[0]][food_pos[1]] = 1;
		EatenFoodCnt++;

		return FOOD_GOT;
	}

	return SNAKE_MOVE;
}


void Show_AddScore(void)
{
	char src[5] = "";
	char dest[20] = "";

	os_sprintf(src, "%d", ScorePool[EatenFoodCnt]);
	printf("\r\n========src: %s, %d========\r\n", src, src[0]);//******************************
	os_memmove(dest, "Score: ", os_strlen("Score:"));

	strncat(dest, src, os_strlen(src));
	OLED_ShowString(0, 0, dest);
}
/*
 * �����Ϸ״̬
 * */
void Check_GameState(uint8_t *food_state)
{
	uint8_t game_state = SNAKE_MOVE;

	game_state = Snake_State(SnakeList, food_state);
	/*printf("\r\ngame_state: %d\r\n", game_state);	// ����ص�������־*/
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
	SNAKE_T *p_operate = p_head;

	static uint8_t food_state = FOOD_EMPTY;

	Snake_Move(p_head);

	while(p_operate != NULL)					//���������������Map��������ʾ
	{
		Map[p_operate->x][p_operate->y] = 1;

		p_operate = p_operate->next;
	}

	OLED_UpdateShow();
	Show_AddScore();

	Create_NewFood(&food_state);

	Check_GameState(&food_state);
}




