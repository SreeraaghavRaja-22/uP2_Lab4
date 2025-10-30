// threads.h
// Date Created: 2023-07-26
// Date Updated: 2023-07-26
// Threads

#ifndef THREADS_H_
#define THREADS_H_

/************************************Includes***************************************/

#include "G8RTOS/G8RTOS_Semaphores.h"

/************************************Includes***************************************/

/***********************************Semaphores**************************************/

semaphore_t sem_UART;
semaphore_t sem_I2CA;
semaphore_t sem_SPI;
semaphore_t sem_SW1;
semaphore_t sem_MMB;
semaphore_t sem_JOY;



/***********************************Semaphores**************************************/


/********************************Thread Functions***********************************/

void Accel(void);
void Gyro(void);
void Opto(void);
void FIFOProducer(void);
void FIFOConsumer(void);
void FIFOConsumer2(void);
void Idle_Thread(void);
void CamMove_Thread(void);
void Cube_Thread(void);
void Read_Buttons();
void Read_JoystickPress();
void Print_WorldCoords(void);
void Get_Joystick(void);
void GPIOE_Handler();
void GPIOD_Handler();

// Testing Threads / ISRS
void SW1_ISR();
void SW2_ISR();
void SW1_Event_Handler(); 
void SW2_Event_Handler();
void ChildA_Thread(); 
void ChildB_Thread();
void PThread1();
void PThread2();
void LCDThread();







/********************************Thread Functions***********************************/


#endif /* THREADS_H_ */

