/*
 * snake.h
 *
 *  Created on: 2020年3月27日
 *      Author: Administrator
 */

#ifndef APP_DRIVER_SNAKE_H_
#define APP_DRIVER_SNAKE_H_

#include "stdint.h"			// 数据类型声明


#define SNAKE_WIDTH         8
#define BORDER_WIDTH        8

#define MAX_EATEN_CNT		50
#define ADC_SAMPLE_TIMES	2

#define BORDER_LEFT_POS     (0)
#define BORDER_RIGHT_POS    (MAX_ROW-BORDER_WIDTH)/8
#define BORDER_UP_POS       (0)
#define BORDER_DOWN_POS     (280-BORDER_WIDTH)/8



typedef enum
{
	FOOD_EMPTY = 0,
	FOOD_READY,
	FOOD_GOT,
	SNAKE_MOVE,
	SNAKE_HIT,
	GAME_OVER,

}GAME_STATE_E;

typedef enum
{
	DIR_NONE = 0,
	DIR_UP,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
}MOVE_DIR_E;

typedef struct SNAKE SNAKE_T;
struct SNAKE
{
	uint16_t x;
	uint16_t y;
    uint16_t color;

	SNAKE_T* prev;
	SNAKE_T* next;
};


extern SNAKE_T* SnakeList;

void SnakeList_Init(void);
void Score_Init(uint16_t max_eaten_cnt);
void SnakeList_LengthAppend(SNAKE_T* p_head);
void Game_Running(SNAKE_T* p_head);
void Snake_Show(void *p_head);
uint8_t Snake_Direction_Input(void);


#endif /* APP_DRIVER_SNAKE_H_ */
