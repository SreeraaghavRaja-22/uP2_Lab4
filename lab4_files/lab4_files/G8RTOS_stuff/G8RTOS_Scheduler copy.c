// G8RTOS_Scheduler.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for scheduler functions

#include "../G8RTOS_Scheduler.h"
/************************************Includes***************************************/

#include <stdint.h>

#include "../G8RTOS_CriticalSection.h"

#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "MultimodDrivers/multimod_uart.h"

/************************************Includes***************************************/

/********************************Private Variables**********************************/

// Thread Control Blocks - array to hold information for each thread
// stuff goes here

// Thread Stacks - array of arrays for individual stacks of each thread
// stuff goes here

// Current Number of Threads currently in the scheduler
// stuff goes here

/********************************Private Variables**********************************/

/*******************************Private Functions***********************************/

static void InitSysTick(void) {
    // hint: use SysCtlClockGet() to get the clock speed without having to hardcode it!
    // use timer interrupt to overflow every 1 ms (aka context switch ever 1 ms)

    // disable the SysTick and system interrupt
    SysTickDisable();

    // set the period of the systick to 1ms
    // X MHz / 1ms = X/1000 - 1 ticks (because ticks start at 0)
    // sets the number of ticks which is 
    SysTickPeriodSet((SysCtlClockGet() / 1000));

    // clear the current value of the SysTick counter (instead of having to manually clear register)
    SysTickValueGet(); 

     // set the priority level of the interrupt -- Set to priority level 2
    IntPrioritySet(FAULT_SYSTICK, 0xB0); 
    
    // set the PenSV Priority to a lower level than the Systick -- just set to the lowest level
    IntPrioritySet(FAULT_PENDSV, 0xE0);

    
    // Register the SysTick Interrupt and SysTick Interrupt is Enabled
    SysTickIntRegister(&SysTick_Handler);
   

    // enable the SysTick Interrupt 
    // SysTickIntEnable();

    // finally start the SysTick
    SysTickEnable();

   //UARTprintf("InitSystick Finished\n\n");
}

/*******************************Private Functions***********************************/


/********************************Public Variables***********************************/

uint32_t SystemTime;



/********************************Public Variables***********************************/

/********************************Public Functions***********************************/

// G8RTOS_Init
// Initializes the RTOS by initializing system time.
// Return: void
void G8RTOS_Init() {
    // stuff goes here
    // disable interrupts 
    IntMasterDisable(); 

    NumThreads = 0;

    // set the system clock to desired speed 
    SystemTime = 0;

    IntRegister(FAULT_PENDSV, &PendSV_Handler);

   // UARTprintf("G8RTOS_Initialized\n");
    // initialize all hardware on the board that we need to use
    // initalize uart
    // initialize SPI
    // initialize I2C
}

// G8RTOS_Launch
// Launches the RTOS.
// Return: error codes, 0 if none
int32_t G8RTOS_Launch() {
    // stuff goes here
    // what could the possible error codes be here?
    IBit_State = StartCriticalSection();
    // set the currently running thread to the first thread value when the launch start 
    CurrentlyRunningThread = &tcbs[0];
    EndCriticalSection(IBit_State);

    //UARTprintf("The address of CRT is %X\n", CurrentlyRunningThread);


    // initialize SysTick and set priorities 
    InitSysTick(); 


    // Call G8RTOS_Start()
    G8RTOS_Start();

   //UARTprintf("RTOS Deployed!!\n");

    return 0;
}

// G8RTOS_Scheduler
// Chooses next thread in the TCB. Round-robin scheduling.
// Return: void
void G8RTOS_Scheduler() {
    // stuff goes here (alot of stuff) -- ok buddy
    CurrentlyRunningThread = CurrentlyRunningThread -> next;
}

// G8RTOS_AddThread
// - Adds threads to G8RTOS Scheduler
// - Checks if there are still available threads to insert to scheduler
// - Initializes the thread control block for the provided thread
// - Initializes the stack for the provided thread to hold a "fake context"
// - Sets stack thread control block stack pointer to top of thread stack
// - Sets up the next and previous thread control block pointers in a round robin fashion
// Param void* "threadToAdd": pointer to thread function address
// Return: scheduler error code
sched_ErrCode_t G8RTOS_AddThread(void (*threadToAdd)(void)) {

    // when adding threads, start critical section 
    IBit_State = StartCriticalSection();
    // check that num_threads == max_threads {return error code }
    if(NumThreads >= MAX_THREADS){
        
        // end critical section early and exist function
        EndCriticalSection(IBit_State);

        // return error code
        return 1;
    } 

    
 
    // added always points to the first thread (next)
    tcbs[NumThreads].next = &tcbs[0];
    // first always points to the added (prev)
    tcbs[0].prev = &tcbs[NumThreads];

    if(NumThreads != 0){
        // case when you're adding threads
        // previously / intermediate thread points to most recently added thread
        tcbs[NumThreads - 1].next = &tcbs[NumThreads];        
        // most recently added thread points to previously added thread 
        tcbs[NumThreads].prev = &tcbs[NumThreads-1];
    }

    // initialize the stack of the current thread
    SetInitialStack(NumThreads);

    // set the program counter value to the value of the thread that we need
    Stacks[NumThreads][STACKSIZE-2] = (int32_t)threadToAdd;
    
    // increment the number of threads at the end
    NumThreads++; 
    
    EndCriticalSection(IBit_State);

    //UARTprintf("This is the IBIt_Value, %d\n", IBit_State);
    //UARTprintf("Thread %d added!\n", NumThreads);

    return 0;
}

// SysTick_Handler
// Increments system time, sets PendSV flag to start scheduler.
// Return: void
void SysTick_Handler() {
    // increment SystemTime to get a marker of how much time has passed 
    SystemTime++;

    // UARTprintf("System Time: %d", SystemTime); DO NOT UNCOMMENT THIS OMG IT'S SO MUCH

    // set the PendSV flag to start scheduler 
    // use the HWREG command to access the register directly without having to include TM4123... file
    // set interrupt (make sure to use |= to not have to rewrite the entire register)
    HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PEND_SV; // could have used IntPendSet(FAULT_PENDSV)
 }

void SetInitialStack(int i){
    // stack builds down, so stack pointer initialized to "top of stack" which is at the bottom since the stack builds down 
    // just sets the stack for each thread with fake news
    tcbs[i].sp = &Stacks[i][STACKSIZE-16]; // thread stack pointer
    Stacks[i][STACKSIZE-1] = 0x01000000; // Thumb bit -- could also use the Group Configuration PSR

    // LR = R14 and R12
    Stacks[i][STACKSIZE-3] = 0x14141414; // R14
    Stacks[i][STACKSIZE-4] = 0x12121212; // R12

    // Hardware Stack Registers
    Stacks[i][STACKSIZE-5] = 0x03030303; // R3
    Stacks[i][STACKSIZE-6] = 0x02020202; // R2
    Stacks[i][STACKSIZE-7] = 0x01010101; // R1
    Stacks[i][STACKSIZE-8] = 0x00000000; // R0

    // Scratch registers???
    Stacks[i][STACKSIZE-9] = 0x11111111; // R11
    Stacks[i][STACKSIZE-10] = 0x10101010; // R10
    Stacks[i][STACKSIZE-11] = 0x09090909; // R9
    Stacks[i][STACKSIZE-12] = 0x08080808; // R8
    Stacks[i][STACKSIZE-13] = 0x07070707; // R7
    Stacks[i][STACKSIZE-14] = 0x06060606; // R6
    Stacks[i][STACKSIZE-15] = 0x05050505; // R5
    Stacks[i][STACKSIZE-16] = 0x04040404; // R4
}

