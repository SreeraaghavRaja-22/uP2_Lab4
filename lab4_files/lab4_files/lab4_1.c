// Lab 3, uP2 Fall 2025
// Created: 2023-07-31
// Updated: 2025-07-09
// Lab 3 is intended to introduce you to RTOS concepts. In this, you will
// - configure the systick function
// - write asm functions for context switching
// - write semaphore functions
// - write scheduler functions to add threads / run scheduling algorithms
// - write critical section assembly functions

/************************************Includes***************************************/

#include "G8RTOS/G8RTOS.h"
#include "./MultimodDrivers/multimod.h"

#include "./threads.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/
/*************************************Defines***************************************/

/********************************Public Variables***********************************/
//extern uint32_t SystemTime;

/********************************Public Variables***********************************/
/********************************Public Functions***********************************/


/********************************Public Functions***********************************/

/************************************MAIN*******************************************/

// Be sure to add in your source files from previous labs into "MultimodDrivers/src/"!
// If you made any modifications to the corresponding header files, be sure to update
// those, too.
int main(void) {

    // sysclock
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Initializes the necessary peripherals.
    Multimod_Init();

    ST7789_Fill(ST7789_WHITE);
   
    // Add threads, initialize semaphores here!
    G8RTOS_InitSemaphore(&sem_UART, UART_Resources);
    G8RTOS_InitSemaphore(&sem_I2CA, I2C_Resources);
    G8RTOS_InitSemaphore(&sem_SPI, SPI_Resources);

    // initialize the FIFOs
    G8RTOS_InitFIFO(0);
    G8RTOS_Init();
    G8RTOS_AddThread(Accel, 0, "Thread 0", 32);
    G8RTOS_AddThread(Gyro, 1, "Thread 1", 64);
    // G8RTOS_AddThread(Opto, 2, "Thread 2", 96);
    // G8RTOS_AddThread(FIFOProducer, 3, "FIFPROD", 100);
    // G8RTOS_AddThread(FIFOConsumer, 5, "FIFCONS", 101);
    // G8RTOS_AddThread(FIFOConsumer2, 5, "FIFOCONS2", 133);
    G8RTOS_AddThread(Idle_Thread, MIN_PRIORITY, "IDLE", 200);
    G8RTOS_Launch();

    // spin - the RTOS will take over now
    while(1);
}

/************************************MAIN*******************************************/
