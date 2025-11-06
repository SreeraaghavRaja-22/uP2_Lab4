
/************************************Includes***************************************/

#include "./Quiz_Practice/snake_threads.h"
#include "G8RTOS/G8RTOS_Scheduler.h"
#include "G8RTOS/G8RTOS_IPC.h"

#include "./MultimodDrivers/multimod.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/************************************Includes***************************************/

/*************************************Defines***************************************/
#define S_R_MAX 50
#define S_C_MAX 50
#define JOY_U_BOUND (2048+250)
#define JOY_L_BOUND (2048-250)
#define PIX_OFFSET 5
#define GRID_W 24
#define GRID_L 28
#define PIX_SQU 10
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
    RIGHT = 3, 
    NONE = 4
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
static Block square;
static Block orange;
static bool game_begin = true;
static bool game_over = false;
static uint16_t joy_data_x;
static uint16_t joy_data_y;


/*********************************** FUNCTIONS ********************************/

void draw_block(Block* block, uint16_t color);
void spawn_orange(void);
static void check_edge(void);
static void check_dir(void);

// just a function (don't care about prototypes)
void draw_block(Block* block, uint16_t color){
    G8RTOS_WaitSemaphore(&sem_SPI);
    ST7789_DrawRectangle((*block).current_point.col, (*block).current_point.row, 10, 10, color);
    G8RTOS_SignalSemaphore(&sem_SPI);
}

void spawn_orange(void){
        orange.current_point.col = rand() % (X_MAX / PIX_SQU) * PIX_SQU + 5;
        orange.current_point.row = rand() % (Y_MAX / PIX_SQU) * PIX_SQU + 5;
        draw_block(&orange, ST7789_ORANGE);
}

void check_edge(){
    // edge cases for the blocks
    if(square.current_point.col + 5 == X_MAX){
        square.current_point.col = 5;
    }
    else if(square.current_point.col - 5 == 0){
        square.current_point.col = X_MAX - 5;
    }
    else if(square.current_point.row + 5 == Y_MAX){
        square.current_point.row = 5;
    }
    else if(square.current_point.row - 5 == 0){
        square.current_point.row = Y_MAX-5;
    }
}

static void check_dir(){
    // check for square's direction
    if(square.snk_dir == UP){
        square.current_point.row += 10;
    }
    else if(square.snk_dir == DOWN){
        square.current_point.row -= 10;
    }
    else if(square.snk_dir == LEFT){
        square.current_point.col -= 10;
    }
    else if(square.snk_dir == RIGHT){
        square.current_point.col += 10;
    }
}

void check_collision(){
    //
}
/*************************************Threads***************************************/




// Working Threads 
void Idle_Thread_Snake(void) {
    for(;;){
    }
}

void Block_Init(void){
    for(;;){
        if(game_begin){
            square.current_point.row = 55; 
            square.current_point.col = 55; 
            square.snk_dir = RIGHT;

            spawn_orange();

            draw_block(&square, ST7789_WHITE);

            game_begin = false;
        }
        
        if(game_over){
            G8RTOS_WaitSemaphore(&sem_SPI);
            ST7789_Fill(ST7789_ORANGE);
            G8RTOS_SignalSemaphore(&sem_SPI);
        }
    }
}


void Game_Update(void){

    draw_block(&square, ST7789_BLACK);
    check_dir();
    check_edge();

    if((square.current_point.row + PIX_OFFSET == orange.current_point.row - PIX_OFFSET)){
        draw_block(&orange, ST7789_BLACK);
        spawn_orange();
    }
    else if((square.current_point.row - PIX_OFFSET == orange.current_point.row + PIX_OFFSET)){
        draw_block(&orange, ST7789_BLACK);
        spawn_orange();
    }

    draw_block(&square, ST7789_WHITE);
}

// periodic thread
void Get_Joystick_Snake(void) {

    joy_data_x = JOYSTICK_GetX();
    joy_data_y = JOYSTICK_GetY();

    if(joy_data_x <= JOY_L_BOUND){
        square.snk_dir = RIGHT;
    }
    else if(joy_data_x >= JOY_U_BOUND){
        square.snk_dir = LEFT;
    }
    else if(joy_data_y <= JOY_L_BOUND){
        square.snk_dir = DOWN;
    }
    else if(joy_data_y >= JOY_U_BOUND){
        square.snk_dir = UP;
    }
}


