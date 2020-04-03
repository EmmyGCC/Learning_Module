/*
 * snake.c
 *
 *  Created on: 2020��3��27��
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
#include "main.h"
#include "delay.h"




SNAKE_T* SnakeList;
uint8_t last_dir = DIR_RIGHT;		//�����ϴ��ƶ��ķ���
uint16_t food_pos[2] = {0};
uint32_t RandomSeed = 0;

uint16_t ScorePool[MAX_EATEN_CNT] = {0};
volatile uint8_t EatenFoodCnt = 1;



static SNAKE_T* Node_Create(void)
{
	return malloc(sizeof(SNAKE_T));
}


void SnakeList_Init(void)
{
	SnakeList = Node_Create();
	uint8_t length = 0;
    DEBUG_Printf(" __LINE__:%d ,__FUNCTION__:%s",  __LINE__,__FUNCTION__);
	DEBUG_Printf("\r\n----SnakeList_Init----\r\n");	// ����ص�������־
	if (SnakeList == NULL)
	{
		DEBUG_Printf("SnakeList_Init error\r\n");
		return;
	}

	SnakeList->next = NULL;
	SnakeList->prev = SnakeList;
	SnakeList->x = 10;	//���еĳ�ʼλ��
	SnakeList->y = 10;

	for (; length < 3; length++)
	{
		SnakeList_LengthAppend(SnakeList);
	}
}


void Score_Init(uint16_t max_eaten_cnt)
{
	uint16_t cnt = 1;

	for (cnt = 1; cnt < max_eaten_cnt; cnt++)
	{
		ScorePool[cnt] = (cnt * 10);
		DEBUG_Printf("ScorePool[%d] = %d\r\n", cnt, ScorePool[cnt]);
	}
}


void SnakeList_LengthAppend(SNAKE_T* p_head)
{
	SNAKE_T *new_node = Node_Create();
	SNAKE_T *p_operate = p_head;

	if (p_head == NULL)	//����б�ͷ��ַ��Ч
	{
		DEBUG_Printf("SnakeList_LengthAppend error\r\n");
		free(new_node);
		return;
	}
	DEBUG_Printf("\r\n----new_node addr:%p----\r\n", new_node);	// ����ص�������־
	while (p_operate->next != NULL)		//�ҵ����һ����Ϊ�յĽڵ��ַ
	{
		p_operate = p_operate->next;
	}
DEBUG_Printf(" __LINE__:%d ,__FUNCTION__:%s",  __LINE__,__FUNCTION__);
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
	uint16_t adc_value[ADC_SAMPLE_TIMES] = {0};
    uint16_t adc_x = 0;
    uint16_t adc_y = 0;
    
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&adc_value[0], ADC_SAMPLE_TIMES);
    delay_ms(10);
    for (uint8_t cnt = 0; cnt < ADC_SAMPLE_TIMES/2; cnt++)
    {
        adc_y += adc_value[cnt*2+1];
    }
    adc_y /= (ADC_SAMPLE_TIMES/2);
    DEBUG_Printf("\r\n----adc_y: %d----\r\n", adc_y);
    
    for (uint8_t cnt = 0; cnt < ADC_SAMPLE_TIMES/2; cnt++)
    {
        adc_x += adc_value[cnt*2];
    }
    adc_x /= (ADC_SAMPLE_TIMES/2);
    DEBUG_Printf("\r\n----adc_x: %d----\r\n", adc_x);
    DEBUG_Printf(" __LINE__:%d ,__FUNCTION__:%s",  __LINE__,__FUNCTION__);
	if (adc_y >= 4000)
	{
		return DIR_DOWN;
	}
	else if(adc_y <= 1000)
	{
		return DIR_UP;
	}
    if (adc_x <= 1000)
	{
		return DIR_RIGHT;
	}
	else if(adc_x >= 4000)
	{
		return DIR_LEFT;
	}
	return DIR_NONE;
}

/*
uint8_t KEY_Direction_Input(void)
{
	if( HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) == 0 )			// ��ȡGPIO_0��ƽ,����Ϊ������
	{
		return DIR_LEFT;
	}
	if( HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) == 0 )			// ��ȡGPIO_5��ƽ,����Ϊ������
	{
		return DIR_UP;
	}
	if( HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) == 0 )			// ��ȡGPIO_12��ƽ,����Ϊѡ��Level1
	{
		return DIR_RIGHT;
	}
	if( HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) == 0 )			// ��ȡGPIO_13��ƽ,����Ϊѡ��Level1
	{
		return DIR_DOWN;
	}

	return DIR_NONE;
}
*/

void Snake_Move(SNAKE_T* p_head)
{
	SNAKE_T *p_operate = p_head;
	SNAKE_T *p_tail = p_head->prev;				//��ȡ��β���
	uint8_t move_dir = last_dir;                //��ȡ��ʵ�ƶ��ķ���
	static uint8_t cur_dir = DIR_NONE;

	if (p_operate == NULL)
	{
		DEBUG_Printf("SnakeList_Move error\r\n");
		return;
	}
    
    Trun_Off_Point(p_tail->x, p_tail->y);       //Ϩ����β�ĵ�
	/********************��β�����ǰ���������޸�ֵ*********************/
	while (p_tail != p_head)
	{
		p_tail->x = p_tail->prev->x;			//β����ֵΪ��һ���ڵ��ֵ�������ƶ���ʱ�������ǰ�ƶ�
		p_tail->y = p_tail->prev->y;

		p_tail = p_tail->prev;
	}

	cur_dir = Snake_Direction_Input();			//KEY_Direction_Input();
	if (cur_dir != move_dir)					//�޸Ĳ������״̬
	{
		if (cur_dir != DIR_NONE)
		{
			move_dir = cur_dir;					//�˴λ�ȡ�ķ���ֵ�����´ν����ж�switch�Ż���Ч
            last_dir = move_dir;
            RandomSeed = HAL_GetTick();
		}
	}
    DEBUG_Printf("\r\n----Snake_Move move_dir:%d ----\r\n", move_dir);
	switch(move_dir)							//�ж�ͷ����λ�øı䷽���޸�ͷ����ֵ
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
    DEBUG_Printf(" __LINE__:%d ,__FUNCTION__:%s",  __LINE__,__FUNCTION__);
    Snake_Show(p_operate);
}



static uint16_t random(void)
{
    uint16_t ret = 0;
    
    /* ��ʼ������������� */
    srand(RandomSeed);
    /* ����һ��0-300������� */
    ret = rand() % 300;
    
    return ret;
}


void Create_NewFood(GAME_STATE_E *food_state)
{
	uint32_t random_num1 = 0;
    uint32_t random_num2 = 0;
	uint16_t x_value = 0;
	uint16_t y_value = 0;

	while(*food_state == FOOD_EMPTY)
	{
		random_num1 = HAL_GetTick() + random();
        random_num2 = HAL_GetTick() + random();
		x_value = random_num1 % (240 - BORDER_WIDTH);
		y_value = random_num2 % (280 - BORDER_WIDTH);
        
        DEBUG_Printf(" __LINE__:%d ,__FUNCTION__:%s",  __LINE__,__FUNCTION__);
		if (x_value > (BORDER_LEFT_POS+BORDER_WIDTH) && y_value > (BORDER_UP_POS+BORDER_WIDTH))		//����ʳ���λ�úϹ�
		{
			DEBUG_Printf("\r\n----Create_NewFood----\r\n");
            DEBUG_Printf("\r\n----x_value: %d----\r\n", x_value);
            DEBUG_Printf("\r\n----Y_value: %d----\r\n", y_value);
            
            Trun_On_Point(x_value/8, y_value/8);
            
            food_pos[0] = x_value/8;
            food_pos[1] = y_value/8;
            *food_state = FOOD_READY;
            break;
		}
	}
}



void Show_AddScore(void)
{
	char src[5] = "";
	char dest[20] = "";

	sprintf(src, "%d", ScorePool[EatenFoodCnt++]);
	DEBUG_Printf("\r\n========src: %s, %d========\r\n", src, src[0]);//******************************
	memmove(dest, "Score: ", strlen("Score: "));
    
	strncat(dest, src, strlen(src));
    DEBUG_Printf(" __LINE__:%d ,__FUNCTION__:%s",  __LINE__,__FUNCTION__);

    LCD_ShowString(30/*(MAX_ROW - strlen((char *)dest)*(FONT_SIZE_16/2))/2*/, 285,
                    200, FONT_SIZE_24, (uint8_t *)dest);
}


static uint8_t Snake_State(SNAKE_T* p_head, GAME_STATE_E *food_state)
{
	if (p_head->x == BORDER_LEFT_POS || p_head->x == BORDER_RIGHT_POS || p_head->y == BORDER_UP_POS || p_head->y == BORDER_DOWN_POS)
		return SNAKE_HIT;

	if (p_head->x == food_pos[0] && p_head->y == food_pos[1])
	{
		*food_state = FOOD_EMPTY;
        //Trun_On_Point(food_pos[0], food_pos[1]);
        
        DEBUG_Printf(" __LINE__:%d ,__FUNCTION__:%s",  __LINE__,__FUNCTION__);        
        DEBUG_Printf("FOOD_GOT!\r\n");
        
		return FOOD_GOT;
	}

	return SNAKE_MOVE;
}


/*
 * �����Ϸ״̬
 * */
void Check_GameState(GAME_STATE_E *food_state)
{
	uint8_t game_state = SNAKE_MOVE;

	game_state = Snake_State(SnakeList, food_state);
    DEBUG_Printf(" __LINE__:%d ,__FUNCTION__:%s",  __LINE__,__FUNCTION__);
	DEBUG_Printf("\r\ngame_state: %d\r\n", game_state);	// ����ص�������־
	switch(game_state)
	{
		case SNAKE_MOVE:
			break;
		case FOOD_GOT:
            Show_AddScore();
            SnakeList_LengthAppend(SnakeList);
			break;
		case SNAKE_HIT:
            DEBUG_Printf(" __LINE__:%d ,__FUNCTION__:%s",  __LINE__,__FUNCTION__);
			DEBUG_Printf("your snake hit the wall, game over!\r\n");
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
	static GAME_STATE_E food_state = FOOD_EMPTY;
    
	Snake_Move(p_head);

	Create_NewFood(&food_state);

	Check_GameState(&food_state);
}


/*LCD���Ƴ�����*/
void Snake_Show(void *p_head)
{
    SNAKE_T *p_operate = (SNAKE_T *)p_head;
    
    while(p_operate != NULL)
    {
        Trun_On_Point(p_operate->x, p_operate->y);
        
        p_operate = p_operate->next;
    }
}


