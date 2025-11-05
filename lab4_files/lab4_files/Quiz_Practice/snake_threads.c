// G8RTOS_Threads.c
// Date Created: 2023-07-25
// Date Updated: 2025-07-27
// Defines for thread functions.

/************************************Includes***************************************/

#include "threads.h"
#include "G8RTOS/G8RTOS_Scheduler.h"
#include "G8RTOS/G8RTOS_IPC.h"

#include "./MultimodDrivers/multimod.h"
#include "./MiscFunctions/Shapes/inc/cube.h"
#include "./MiscFunctions/LinAlg/inc/linalg.h"
#include "./MiscFunctions/LinAlg/inc/quaternions.h"
#include "./MiscFunctions/LinAlg/inc/vect3d.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/************************************Includes***************************************/

/*************************************Defines***************************************/
#define S_R_MAX 50
#define S_C_MAX 50
/*************************************Defines***************************************/

/*********************************Global Variables**********************************/
// define location of snake with enum
typedef enum loc{
    SNAKE = 0, 
    EMPTY = 1, 
    ORANGE = 2
} loc; 

// define direction of snake with enum
typedef enum dir{
    UP = 0, 
    DOWN = 1, 
    LEFT = 2, 
    RIGHT = 3
} dir;

// Create a 2-D array for the snake size 
loc snake_array[S_R_MAX][S_C_MAX];

// struct for the point a snake is at
typedef struct Point{
    int16_t row; 
    int16_t col;
} Point; 

typedef struct Snake{
    loc snake_array[S_R_MAX][S_C_MAX];
    Point current_point;
    loc snk_val;
    dir snk_dir; 
    bool alive;
    loc orange_count;
} Snake; 

typedef struct Block{
    Point current_point;
    dir snk_dir; 
} Block;




/*********************************Global Variables**********************************/
// define global square entity based on block struct
Block square;
/*************************************Threads***************************************/

// Working Threads 
void Idle_Thread(void) {
    for(;;){
    }
}

void Block_Init(void){
    square.current_point.row= 5; 
    square.current_point.col = 5; 
    square.snk_dir = RIGHT;
    ST7789_DrawRectangle(square.current_point.col, square.current_point.row, 10, 10, ST7789_WHITE);
}

void Game_Update(void){

    ST7789_Fill(ST7789_BLACK);

    //check for square's direction
    if(square.snk_dir == UP){
        square.current_point.row++;
    }
    else if(square.snk_dir == DOWN){
        square.current_point.row--;
    }
    else if(square.snk_dir == LEFT){
        square.current_point.col--;
    }
    else{
        square.current_point.col++;
    }

    ST7789_DrawRectangle(square.current_point.row, square.current_point.col, 10, 10, ST7789_WHITE);
}