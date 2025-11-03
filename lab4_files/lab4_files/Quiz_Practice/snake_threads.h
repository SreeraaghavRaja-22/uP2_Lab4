// threads.h
// Date Created: 2023-07-26
// Date Updated: 2023-07-26
// Threads

#ifndef SNAKE_THREADS_H_
#define SNAKE_THREADS_H_

/************************************Includes***************************************/

#include "G8RTOS/G8RTOS_Semaphores.h"
#include "G8RTOS/G8RTOS_CriticalSection.h"

/************************************Includes***************************************/

/***********************************Semaphores**************************************/
semaphore_t sem_UART;
semaphore_t sem_I2CA;
semaphore_t sem_SPI;
semaphore_t sem_SW1;
semaphore_t sem_PCA9555;
semaphore_t sem_KillCube;
/***********************************Semaphores**************************************/

// thread definitions
void Idle_Thread(void);



#endif /* SNAKE_THREADS_H */
