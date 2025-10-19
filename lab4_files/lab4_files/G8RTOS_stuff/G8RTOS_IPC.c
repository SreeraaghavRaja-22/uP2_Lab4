// G8RTOS_IPC.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for FIFO functions for interprocess communication

#include "G8RTOS_IPC.h"

/************************************Includes***************************************/

#include "G8RTOS_Semaphores.h"

/************************************Includes***************************************/

/******************************Data Type Definitions********************************/

#define FIFO_SIZE 16
#define MAX_NUMBER_OF_FIFOS 4

/******************************Data Type Definitions********************************/

/****************************Data Structure Definitions*****************************/

typedef struct G8RTOS_FIFO_t {
    int32_t buffer[FIFO_SIZE];
    int32_t* head;
    int32_t* tail; 
    uint32_t lostData; // used for debugging
    semaphore_t currentSize;
    semaphore_t mutex;

} G8RTOS_FIFO_t;

/****************************Data Structure Definitions*****************************/

/***********************************Externs*****************************************/
/***********************************Externs*****************************************/

/********************************Public Variables***********************************/

static G8RTOS_FIFO_t FIFOs[MAX_NUMBER_OF_FIFOS];

uint32_t volatile *putPt;
uint32_t volatile *getPt;
/********************************Public Variables***********************************/

/********************************Public Functions***********************************/

// G8RTOS_InitFIFO
// Initializes FIFO, points head & tail to relevant buffer
// memory addresses.
// Param "FIFO_index": Index of FIFO block
// Return: int32_t, -1 if error (i.e. FIFO full), 0 if okay
int32_t G8RTOS_InitFIFO(uint32_t FIFO_index) {

    if(FIFOs[FIFO_index].currentSize = FIFO_SIZE)
    {
        return -1;
    }
    FIFOs[FIFO_index].head = FIFOs[FIFO_index].tail = &FIFOs[FIFO_index];
    // need to update the size of this semaphore
    G8RTOS_InitSemaphore(&FIFOs[FIFO_index].currentSize, 0);
    // in case multiple threads read the same FIFO
    G8RTOS_InitSemaphore(&FIFOs[FIFO_index].mutex, 1);
}

// G8RTOS_ReadFIFO
// Reads data from head pointer of FIFO.
// Param "FIFO_index": Index of FIFO block
// Return: int32_t, data at head pointer
int32_t G8RTOS_ReadFIFO(uint32_t FIFO_index) {


}

// G8RTOS_WriteFIFO
// Writes data to tail of buffer.
// Param "FIFO_index": Index of FIFO block
// Return: int32_t, data at head pointer
int32_t G8RTOS_WriteFIFO(uint32_t FIFO_index, uint32_t data) {


}

/********************************Public Functions***********************************/
