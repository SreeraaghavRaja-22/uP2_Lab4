// G8RTOS_Threads.c
// Date Created: 2023-07-25
// Date Updated: 2025-07-09
// Defines for thread functions.

/************************************Includes***************************************/

#include "./threads.h"

#include "./MultimodDrivers/multimod.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/
// Assumes that the system clock is 16 MHz.
#define delay_0_1_s     (1600000/3)
#define max_uint32_value (4294967295.0f)
/*************************************Defines***************************************/

/********************************Public Functions***********************************/

// Thread0, reads accel_x data, adjusts BLUE led duty cycle.
void Thread0(void) {
    while(1)
    {
        // Share resources for I2C
        G8RTOS_WaitSemaphore(&sem_I2CA);
        int16_t accel_x_data = BMI160_AccelXGetResult();
        G8RTOS_SignalSemaphore(&sem_I2CA);

        float accel_x_data_norm = (float)((accel_x_data/65535.0f));
        if(accel_x_data_norm < 0){accel_x_data_norm *= -1;}
        LaunchpadLED_PWMSetDuty(BLUE, accel_x_data_norm);

        
        //LaunchpadLED_PWMSetDuty(GREEN, 1/PWM_Per);
        //LaunchpadLED_PWMSetDuty(RED, 1/PWM_Per);

        // Share resources for the UART
        //G8RTOS_WaitSemaphore(&sem_UART);
        //UARTprintf("Accelerometer X Data is %d\n\n", accel_x_data);
        //G8RTOS_SignalSemaphore(&sem_UART);

        SysCtlDelay(delay_0_1_s);
    }   
}
    

// Thread1, reads gyro_x data, adjusts RED led duty cycle.
void Thread1(void) {
    while(1)
    {
        G8RTOS_WaitSemaphore(&sem_I2CA);
        int16_t gyro_x_data = BMI160_GyroXGetResult();
        G8RTOS_SignalSemaphore(&sem_I2CA);

        float gyro_x_data_norm = (float)(((gyro_x_data)/65535.0f));
        if(gyro_x_data_norm < 0){gyro_x_data_norm *= -1;}

        LaunchpadLED_PWMSetDuty(RED, gyro_x_data_norm);
        //LaunchpadLED_PWMSetDuty(GREEN, 1/PWM_Per);
        //LaunchpadLED_PWMSetDuty(BLUE, 1/PWM_Per);

        // Share resources for the UART
        //G8RTOS_WaitSemaphore(&sem_UART);
        //UARTprintf("Gyroscope X Data is %d\n\n", gyro_x_data);
        //G8RTOS_SignalSemaphore(&sem_UART);
        SysCtlDelay(delay_0_1_s);
    }
    

}

// Thread2, reads optical sensor values, adjusts GREEN led duty cycle.
void Thread2(void) {
    while(1)
    {   
        G8RTOS_WaitSemaphore(&sem_I2CA);
        uint32_t opt_data = OPT3001_GetResult();
        G8RTOS_SignalSemaphore(&sem_I2CA);
        float opt_data_normalized = ((opt_data) / 20000.0f);
        // uint32_t opt_data_get = (uint32_t)opt_data_normalized;
        LaunchpadLED_PWMSetDuty(GREEN, opt_data_normalized);
        //LaunchpadLED_PWMSetDuty(RED, 1/PWM_Per);
        //LaunchpadLED_PWMSetDuty(BLUE, 1/PWM_Per);

        //G8RTOS_WaitSemaphore(&sem_UART);
        //UARTprintf("Optometer Value is %d\n\n", opt_data);
        //UARTprintf("Normalized Optometer Data is %d\n\n", opt_data_get);
        //G8RTOS_SignalSemaphore(&sem_UART);

        SysCtlDelay(delay_0_1_s);
    }
}

// Thread3, reads and output button 1 status using polling
void Thread3(void) {
    while(1)
    {
        G8RTOS_WaitSemaphore(&sem_UART);
        if(LaunchpadButtons_ReadSW1()){UARTprintf("Button 1 Pressed!\n\n");}
        G8RTOS_SignalSemaphore(&sem_UART);
        SysCtlDelay(delay_0_1_s);
    }
}

// Thread4, reads and output button 2 status using polling
void Thread4(void) {
    while(1)
    {
        G8RTOS_WaitSemaphore(&sem_UART);
        if(LaunchpadButtons_ReadSW2()){UARTprintf("Button 2 Pressed!\n\n");}
        G8RTOS_SignalSemaphore(&sem_UART);
        SysCtlDelay(delay_0_1_s);
    }
}

/********************************Public Functions***********************************/
