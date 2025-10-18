// G8RTOS_Semaphores.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for semaphore functions

#include "../G8RTOS_Semaphores.h"

/************************************Includes***************************************/

#include "../G8RTOS_CriticalSection.h"

/************************************Includes***************************************/

/******************************Data Type Definitions********************************/
/******************************Data Type Definitions********************************/

/****************************Data Structure Definitions*****************************/
/****************************Data Structure Definitions*****************************/

/***********************************Externs*****************************************/
/***********************************Externs*****************************************/

/********************************Public Variables***********************************/

int32_t IBit_State;

/********************************Public Variables***********************************/

/********************************Public Functions***********************************/

// G8RTOS_InitSemaphore
// Initializes semaphore to a value. This is a critical section!
// Param "s": Pointer to semaphore
// Param "value": Value to initialize semaphore to
// Return: void
void G8RTOS_InitSemaphore(semaphore_t* s, int32_t value) {
   // your stuff goes here
    (*s) = value;
}

// G8RTOS_WaitSemaphore
// Waits on the semaphore to become available, decrements value by 1. This is a
// critical section!
// Currently spin-locks.
// Param "s": Pointer to semaphore
// Return: void
void G8RTOS_WaitSemaphore(semaphore_t* s) {
    IBit_State = StartCriticalSection(); 
    while((*s) == 0)
    {
        // Enable interrupts do nothing?
        // this is a waste of CPU resources but whatever
        // Disable Interrupts 
        EndCriticalSection(IBit_State);
        IBit_State = StartCriticalSection();
    }
    // shows that the thread has ownership by decrementing pointer by 1
    (*s)--;
    EndCriticalSection(IBit_State);
}

// G8RTOS_SignalSemaphore
// Signals that the semaphore has been released by incrementing the value by 1.
// This is a critical section!
// Param "s": Pointer to semaphore
// Return: void
void G8RTOS_SignalSemaphore(semaphore_t* s) {
    IBit_State = StartCriticalSection();
    // just increment the value of s by 1
    (*s)++;
    EndCriticalSection(IBit_State);
}

/********************************Public Functions***********************************/
