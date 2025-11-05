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
    int32_t x; 
    int32_t y;
} Point; 

typedef struct Snake{
    loc snake_array[S_R_MAX][S_C_MAX];
    Point current_point;
    loc snk_val;
    dir snk_dir; 
    bool alive;
    loc orange_count;
} Snake; 


/*********************************Global Variables**********************************/

/*************************************Threads***************************************/

// Working Threads 
void Idle_Thread(void) {
    for(;;){
    }
}

// 