/*
 * snake.h
 *
 *  Created on: 2020年3月27日
 *      Author: Administrator
 */

#ifndef APP_DRIVER_SNAKE_H_
#define APP_DRIVER_SNAKE_H_

#include "stdint.h"			// 数据类型声明


#define SNAKE_INIT_LENGTH	10

#define MAX_EATEN_CNT		50
#define ADC_SAMPLE_TIMES	10



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
	uint8_t x;
	uint8_t y;

	SNAKE_T* prev;
	SNAKE_T* next;
};


extern SNAKE_T* SnakeList;

void SnakeList_Init(void);
void Score_Init(uint16_t max_eaten_cnt);
void SnakeList_LengthAppend(SNAKE_T* p_head);
void Game_Running(SNAKE_T* p_head);


#endif /* APP_DRIVER_SNAKE_H_ */
