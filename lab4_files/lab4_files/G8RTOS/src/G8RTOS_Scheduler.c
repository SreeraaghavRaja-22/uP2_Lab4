// G8RTOS_Scheduler.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for scheduler functions

#include "../G8RTOS_Scheduler.h"
#include "../G8RTOS_Structures.h"

/************************************Includes***************************************/

#include <stdint.h>
#include <stdbool.h>

#include "../G8RTOS_CriticalSection.h"

#include <inc/hw_memmap.h>
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

/************************************Includes***************************************/

/********************************Private Variables**********************************/

// Thread Control Blocks - array to hold information for each thread
static tcb_t threadControlBlocks[MAX_THREADS];

// Thread threadStacks - array of arrays for individual stacks of each thread
static uint32_t threadStacks[MAX_THREADS][STACKSIZE];

// Periodic Event Threads - array to hold pertinent information for each thread
static ptcb_t pthreadControlBlocks[MAX_PTHREADS];

// Current Number of Threads currently in the scheduler
static uint32_t NumberOfThreads;

// Current Number of Periodic Threads currently in the scheduler
static uint32_t NumberOfPThreads;

static uint32_t threadCounter = 0;


/********************************Private Variables**********************************/

/*******************************Private Functions***********************************/

// Occurs every 1 ms.
static void InitSysTick(void)
{
    SysTickPeriodSet(SysCtlClockGet() / 1000);
    SysTickIntRegister(SysTick_Handler);
    IntRegister(FAULT_PENDSV, PendSV_Handler);
    SysTickIntEnable();
    SysTickEnable();
}

/*******************************Private Functions***********************************/


/********************************Public Variables***********************************/

uint32_t SystemTime;

tcb_t* CurrentlyRunningThread;

volatile uint8_t aliveCount = 0;

/********************************Public Variables***********************************/



/********************************Public Functions***********************************/

// SysTick_Handler
// Increments system time, sets PendSV flag to start scheduler.
// Return: void
void SysTick_Handler() {
    SystemTime++;

    tcb_t* currThread = CurrentlyRunningThread;
    ptcb_t* currentPeriodicThread = &pthreadControlBlocks[0];

    // loop through all the periodic threads and execute them appropriately (if their time is now)

    // Loop through the background threads: check sleeping threads and wake them up appropriately if their time is now
    do
    {
        currThread = currThread -> nextTCB;
        if((SystemTime >= currThread -> sleepCount) && (currThread -> sleepCount != 0)) // != or >= works but it's uint32_t so != should be chill
        {
            currThread -> asleep = false;
            currThread -> sleepCount = 0; 
        }
    } while (CurrentlyRunningThread != currThread);
    

    HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PEND_SV;
}

// G8RTOS_Init
// Initializes the RTOS by initializing system time.
// Return: void
void G8RTOS_Init() {
    uint32_t newVTORTable = 0x20000000;
    uint32_t* newTable = (uint32_t*) newVTORTable;
    uint32_t* oldTable = (uint32_t*) 0;

    for (int i = 0; i < 155; i++) {
        newTable[i] = oldTable[i];
    }

    HWREG(NVIC_VTABLE) = newVTORTable;

    SystemTime = 0;
    NumberOfThreads = 0;
    NumberOfPThreads = 0;
}

// G8RTOS_Launch
// Launches the RTOS.
// Return: error codes, 0 if none
int32_t G8RTOS_Launch() {
    InitSysTick();

    CurrentlyRunningThread = &threadControlBlocks[0];
    IntPrioritySet(FAULT_SYSTICK, 0xE0);
    IntPrioritySet(FAULT_PENDSV, 0xE0);
    G8RTOS_Start(); // call the assembly function
    return 0;
}

// G8RTOS_Scheduler
// Chooses next thread in the TCB. This time uses priority scheduling.
// Return: void
void G8RTOS_Scheduler() {

    // define the max priority 
    uint8_t max = 255;
    
    // define two threads that are the thread and current thread that we're looking at
    tcb_t *pt; 
    tcb_t *bestPt;

    // set the thread we're looking at to the currently running thread
    pt = CurrentlyRunningThread; 

    // check if the current thread is alive
    if(!CurrentlyRunningThread->isAlive)
    {
        // this could work but what if the next thread is linked to itself
        pt = CurrentlyRunningThread; //temporary thread;
    }

    // have a do while loop so that we can at least go through the loop once 
    // this loops through all the threads and finds the next one highest priority one that is neither asleep or blocked
    do
    {
        pt = pt -> nextTCB; 

        // if it's strictly less than, then if the only available thread is the same priority level then this fails 
        if((pt->priority <= max) && ((pt->blocked) == 0) && (pt->sleepCount == 0) && (pt->isAlive))
        {
            max = pt->priority;
            bestPt = pt;
        }

    }while(CurrentlyRunningThread != pt); 

    // update the value of the CRT to the bestPT
    CurrentlyRunningThread = bestPt;
}

// G8RTOS_AddThread
// Adds a thread. This is now in a critical section to support dynamic threads.
// It also now should initalize priority and account for live or dead threads.
// Param void* "threadToAdd": pointer to thread function address
// Param uint8_t "priority": priority from [0, 255].
// Param char* "name": character array containing the thread name.
// Return: sched_ErrCode_t
sched_ErrCode_t G8RTOS_AddThread(void (*threadToAdd)(void), uint8_t priority, char *name, threadID_t ID) {
    IBit_State = StartCriticalSection();
    
    // check that num_threads == max_threads {return error code }
    if(NumberOfThreads >= MAX_THREADS){
        
        // end critical section early and exist function
        EndCriticalSection(IBit_State);

        // return error code
        return THREAD_LIMIT_REACHED;
    } 

    // added always points to the first thread (next)
    threadControlBlocks[NumberOfThreads].nextTCB = &threadControlBlocks[0];
    // first always points to the added (prev)
    threadControlBlocks[0].previousTCB = &threadControlBlocks[NumberOfThreads];

    if(NumberOfThreads != 0){
        // case when you're adding threads
        // previously / intermediate thread points to most recently added thread
    threadControlBlocks[NumberOfThreads - 1].nextTCB = &threadControlBlocks[NumberOfThreads];        
        // most recently added thread points to previously added thread 
    threadControlBlocks[NumberOfThreads].previousTCB = &threadControlBlocks[NumberOfThreads-1];
    }

    // initialize the stack of the current thread
    SetInitialStack(NumberOfThreads);

    // set the program counter value to the value of the thread that we need
    threadStacks[NumberOfThreads][STACKSIZE-2] = (int32_t)threadToAdd;
    
    threadControlBlocks[NumberOfThreads].priority = priority;
    threadControlBlocks[NumberOfThreads].blocked = 0; 
    threadControlBlocks[NumberOfThreads].asleep = false; 
    threadControlBlocks[NumberOfThreads].ThreadID = ID;


    threadControlBlocks[NumberOfThreads].isAlive = true;

    // increment the aliveCount value -- useful for KillThread
    aliveCount++;

    // threadControlBlocks[NumberOfThreads].sleepCount = 0;

    // set the thread name 
    strcpy(threadControlBlocks[NumberOfThreads].threadName, name);

    // increment the number of threads at the end
    NumberOfThreads++; 
    
    EndCriticalSection(IBit_State);

    return NO_ERROR;
}

// G8RTOS_Add_APeriodicEvent
// Param void* "AthreadToAdd": pointer to thread function address
// Param uint8_t "priority": Priority of aperiodic event, [1..6]
// Param int32_t "IRQn": Interrupt request number that references the vector table. [0..155].
// Return: sched_ErrCode_t
sched_ErrCode_t G8RTOS_Add_APeriodicEvent(void (*AthreadToAdd)(void), uint8_t priority, int32_t IRQn) {
  // your code
    return NO_ERROR;
}

// G8RTOS_Add_PeriodicEvent
// Adds periodic threads to G8RTOS Scheduler
// Function will initialize a periodic event struct to represent event.
// The struct will be added to a linked list of periodic events
// Param void* "PThreadToAdd": void-void function for P thread handler
// Param uint32_t "period": period of P thread to add
// Param uint32_t "execution": When to execute the periodic thread
// Return: sched_ErrCode_t
sched_ErrCode_t G8RTOS_Add_PeriodicEvent(void (*PThreadToAdd)(void), uint32_t period, uint32_t execution) {
    // your code
    return NO_ERROR;
}

// G8RTOS_KillThread
// Param uint32_t "threadID": ID of thread to kill
// Return: sched_ErrCode_t
sched_ErrCode_t G8RTOS_KillThread(threadID_t threadID) {
    IBit_State = StartCriticalSection();
    // Loop through tcb. If not found, return thread does not exist error. If there is only one thread running, don't kill it.
    tcb_t* pt = CurrentlyRunningThread; 
   
    do{
        pt = pt->nextTCB;
        if(pt->ThreadID == threadID){
            if(aliveCount > 1){

                // kill the currently running thread
                pt->isAlive = false; 

                // decrement the count for the number of alive Threads 
                aliveCount--;

                // fix the linked list logic here 
                tcb_t* prev = pt->previousTCB; 
                tcb_t* next = pt->nextTCB; 
                prev->nextTCB = next; 
                next->previousTCB = prev; 

                // exit function as soon as the thread is killed
                if(pt = CurrentlyRunningThread){
                    EndCriticalSection(IBit_State);
                }
                else{
                    EndCriticalSection(IBit_State);
                    return NO_ERROR; 
                }
            } 
            else if(aliveCount == 1){
                EndCriticalSection(IBit_State);
                return CANNOT_KILL_LAST_THREAD;
            }
        }
    }while(pt != CurrentlyRunningThread);

    // thread not found if the loop finishes without exiting the function in some way
   	EndCriticalSection(IBit_State);
   	return THREAD_DOES_NOT_EXIST;
}

// G8RTOS_KillSelf
// Kills currently running thread.
// Return: sched_ErrCode_t
// Get this to work, the issue is that changing the CRT's values will prevent it from updating in the sche
sched_ErrCode_t G8RTOS_KillSelf() {
    G8RTOS_KillThread(CurrentlyRunningThread->ThreadID);
    HWREG(NVIC_INT_CTRL) |= (NVIC_INT_CTRL_PEND_SV);
    return NO_ERROR;
}

// sleep
// Puts current thread to sleep
// Param uint32_t "durationMS": how many systicks to sleep for
void sleep(uint32_t durationMS) {
    CurrentlyRunningThread -> sleepCount = durationMS + SystemTime; 
    CurrentlyRunningThread -> asleep = true;
    HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PEND_SV;
}

// G8RTOS_GetThreadID
// Gets current thread ID.
// Return: threadID_t
threadID_t G8RTOS_GetThreadID(void) {
    return CurrentlyRunningThread->ThreadID;        //Returns the thread ID
}

// G8RTOS_GetNumberOfThreads
// Gets number of threads.
// Return: uint32_t
uint32_t G8RTOS_GetNumberOfThreads(void) {
    return NumberOfThreads;         //Returns the number of threads
}


void SetInitialStack(int i){
    // stack builds down, so stack pointer initialized to "top of stack" which is at the bottom since the stack builds down 
    // just sets the stack for each thread with fake news
    threadControlBlocks[i].stackPointer = &threadStacks[i][STACKSIZE-16]; // thread stack pointer
    threadStacks[i][STACKSIZE-1] = 0x01000000; // Thumb bit -- could also use the Group Configuration PSR

    // LR = R14 and R12
    threadStacks[i][STACKSIZE-3] = 0x14141414; // R14
    threadStacks[i][STACKSIZE-4] = 0x12121212; // R12

    // Hardware Stack Registers
    threadStacks[i][STACKSIZE-5] = 0x03030303; // R3
    threadStacks[i][STACKSIZE-6] = 0x02020202; // R2
    threadStacks[i][STACKSIZE-7] = 0x01010101; // R1
    threadStacks[i][STACKSIZE-8] = 0x00000000; // R0

    // Scratch registers???
    threadStacks[i][STACKSIZE-9] = 0x11111111; // R11
    threadStacks[i][STACKSIZE-10] = 0x10101010; // R10
    threadStacks[i][STACKSIZE-11] = 0x09090909; // R9
    threadStacks[i][STACKSIZE-12] = 0x08080808; // R8
    threadStacks[i][STACKSIZE-13] = 0x07070707; // R7
    threadStacks[i][STACKSIZE-14] = 0x06060606; // R6
    threadStacks[i][STACKSIZE-15] = 0x05050505; // R5
    threadStacks[i][STACKSIZE-16] = 0x04040404; // R4
}
/********************************Public Functions***********************************/
