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

// Change this to change the number of points that make up each line of a cube.
// Note that if you set this too high you will have a stack overflow!
// sizeof(float) * num_lines * (Num_Interpolated_Points + 2) = ?
#define Num_Interpolated_Points 3

#define delay_0_1_s     (1600000/3)
#define max_uint32_value (4294967295.0f)


/*************************************Defines***************************************/

/*********************************Global Variables**********************************/

Quat_t world_camera_pos = {0, 0, 0, 50};
Quat_t world_camera_frame_offset = {0, 0, 0, 50};
Quat_t world_camera_frame_rot_offset;
Quat_t world_view_rot = {1, 0, 0, 0};
Quat_t world_view_rot_inverse = {1, 0, 0, 0};

// How many cubes?
uint8_t num_cubes = 0;

// y-axis controls z or y
uint8_t joystick_y = 1;

// Kill a cube?
uint8_t kill_cube = 0;

// idle counter
uint8_t idle_count = 0;

// FIFO vals
uint32_t data = 0;

/*********************************Global Variables**********************************/

/*************************************Threads***************************************/
// Thread0, reads accel_x data, adjusts BLUE led duty cycle.
void Accel(void) {
    for(;;){
        // Share resources for I2C
        G8RTOS_WaitSemaphore(&sem_I2CA);
        int16_t accel_x_data = BMI160_AccelXGetResult();
        G8RTOS_SignalSemaphore(&sem_I2CA);

        //float accel_x_data_norm = (float)((accel_x_data/65535.0f));
        //if(accel_x_data_norm < 0){accel_x_data_norm *= -1;}
        //LaunchpadLED_PWMSetDuty(BLUE, accel_x_data_norm);

        G8RTOS_WaitSemaphore(&sem_UART);
        UARTprintf("Thread 0: Accelerometer X Data is %d\n\n", accel_x_data);
        G8RTOS_SignalSemaphore(&sem_UART);

        // bruh
        G8RTOS_KillSelf();

        sleep(500);
    }   
}
    

// Thread1, reads gyro_x data, adjusts RED led duty cycle.
void Gyro(void) {
    for(;;)
    {
        G8RTOS_WaitSemaphore(&sem_I2CA);
        int16_t gyro_x_data = BMI160_GyroXGetResult();
        G8RTOS_SignalSemaphore(&sem_I2CA);

        float gyro_x_data_norm = (float)(((gyro_x_data)/65535.0f));
        if(gyro_x_data_norm < 0){gyro_x_data_norm *= -1;}

        LaunchpadLED_PWMSetDuty(RED, gyro_x_data_norm);

        G8RTOS_WaitSemaphore(&sem_UART);
        UARTprintf("Thread 1: Gyroscope X Data is %d\n\n", gyro_x_data);
        G8RTOS_SignalSemaphore(&sem_UART);

        // SysCtlDelay(delay_0_1_s);
        sleep(700);

        
    }
}

// Thread2, reads optical sensor values, adjusts GREEN led duty cycle.
void Opto(void) {
    for(;;)
    {   
        G8RTOS_WaitSemaphore(&sem_I2CA);
        uint32_t opt_data = OPT3001_GetResult();
        G8RTOS_SignalSemaphore(&sem_I2CA);

        float opt_data_normalized = ((opt_data) / 20000.0f);
        uint32_t opt_data_get = (uint32_t)opt_data_normalized;
        LaunchpadLED_PWMSetDuty(GREEN, opt_data_normalized);

        G8RTOS_WaitSemaphore(&sem_UART);
        UARTprintf("Thread 2: Optometer Value is %d\n\n", opt_data);
        G8RTOS_SignalSemaphore(&sem_UART);

        // sleep for less time for fun and to test blocking and priority
        sleep(700);
    }
}

// Thread3, reads and output button 1 status using polling
void FIFOProducer(void) {
    for(;;)
    {
        data++;
        // uint32_t senData = 0x6769;
        int32_t bruh = G8RTOS_WriteFIFO(0, data);
        G8RTOS_WaitSemaphore(&sem_UART);
        //UARTprintf("FIFO 0: Put Data of value %u into FIFO\n\n", data);
        //UARTprintf("The write function returns %u\n\n", bruh);
        G8RTOS_SignalSemaphore(&sem_UART);

        sleep(700);
    }
}

// Thread4, reads and output button 2 status using polling
void FIFOConsumer(void) {
    for(;;)
    {
        uint32_t recData = G8RTOS_ReadFIFO(0);
        G8RTOS_WaitSemaphore(&sem_UART);
        UARTprintf("FIFO 0 Consumer 1 Receives value: %u\n\n", recData);
        G8RTOS_SignalSemaphore(&sem_UART);
        sleep(700);
    }
}

void FIFOConsumer2(void)
{
    for(;;)
    {
        uint32_t recData2 = G8RTOS_ReadFIFO(0);
        G8RTOS_WaitSemaphore(&sem_UART);
        UARTprintf("FIFO 0 Consumer 2 Receives value: %u\n\n", recData2);
        G8RTOS_SignalSemaphore(&sem_UART);
        sleep(700);
    }
}

void Idle_Thread(void) {
    // need this to not get a deadlock or else you're cooked
    for(;;)
    {
        G8RTOS_WaitSemaphore(&sem_SPI);
        if(idle_count++ % 2 == 0){ST7789_Fill((ST7789_ORANGE));}
        else{ST7789_Fill(ST7789_BLUE);}
        G8RTOS_SignalSemaphore(&sem_SPI);

        G8RTOS_WaitSemaphore(&sem_UART);
        UARTprintf("I like Banana Milk\n\n");
        G8RTOS_SignalSemaphore(&sem_UART);
        // don't sleep idle thread
    }
}

/*
void CamMove_Thread(void) {
    uint32_t result;
    int16_t joystickX;
    int16_t joystickY;
    float joystickX_norm;
    float joystickY_norm;

    world_camera_pos.x = 0;
    world_camera_pos.y = 0;
    world_camera_pos.z = 60;

    
    Quat_t rot_quat, temp;

    float cr, sr, cp, sp, cy, sy;
    float mag;


    while(1) {
        result = G8RTOS_ReadFIFO(JOYSTICK_FIFO);

        // Normalize joystick input

        world_camera_pos.x -= joystickX_norm;

        if (joystick_y) {
            world_camera_pos.y += joystickY_norm;
        } else {
            world_camera_pos.z -= joystickY_norm;
        }

        sleep(10);

    }
}

*/

/*
void Cube_Thread(void) {
    cube_t cube;

    // Get spawn position
    uint32_t packet = G8RTOS_ReadFIFO(SPAWNCOOR_FIFO);

    cube.x_pos = (packet >> 20 & 0xFFF) - 100;
    cube.y_pos = (packet >> 8 & 0xFFF) - 100;
    cube.z_pos = -(packet & 0xFF) - 20;

    cube.width = 50;
    cube.height = 50;
    cube.length = 50;

    Quat_t v[8];
    Quat_t v_relative[8];

    Cube_Generate(v, &cube);

    // Declare a 2d array to store interpolated points
    // This is faster and more robust at the cost of vastly increased space.
    uint32_t m = Num_Interpolated_Points + 1;
    Vect3D_t interpolated_points[12][Num_Interpolated_Points + 2];
    Vect3D_t projected_point;

    Quat_t camera_pos;
    Quat_t camera_frame_offset;

    // Quat_t view_rot;
    Quat_t view_rot_inverse;
    //Quat_t camera_frame_rot_offset;


    uint8_t kill = 0;

    while(1) {

        G8RTOS_WaitSemaphore(&sem_KillCube);
        if (kill_cube) {
            kill = 1;
            kill_cube = 0;
        }
        G8RTOS_SignalSemaphore(&sem_KillCube);

        // set so that the positions are static during viewpoint calculations
        camera_pos.x = world_camera_pos.x;
        camera_pos.y = world_camera_pos.y;
        camera_pos.z = world_camera_pos.z;

        camera_frame_offset.x = world_camera_frame_offset.x;
        camera_frame_offset.y = world_camera_frame_offset.y;
        camera_frame_offset.z = world_camera_frame_offset.z;

        
        view_rot.w = world_view_rot.w;
        view_rot.x = world_view_rot.x;
        view_rot.y = world_view_rot.y;
        view_rot.z = world_view_rot.z;
        

        view_rot_inverse.w = world_view_rot_inverse.w;
        view_rot_inverse.x = world_view_rot_inverse.x;
        view_rot_inverse.y = world_view_rot_inverse.y;
        view_rot_inverse.z = world_view_rot_inverse.z;

        
        camera_frame_rot_offset.w = world_camera_frame_rot_offset.w;
        camera_frame_rot_offset.x = world_camera_frame_rot_offset.x;
        camera_frame_rot_offset.y = world_camera_frame_rot_offset.y;
        camera_frame_rot_offset.z = world_camera_frame_rot_offset.z;
        

        for (int i = 0; i < 12; i++) {
            for (int j = 0; j < m+1; j++) {
                getViewOnScreen(&projected_point, &camera_frame_offset, &(interpolated_points[i][j]));
                // NEEDS A SEMAPHORE
                ST7789_DrawPixel(projected_point.x, projected_point.y, ST7789_BLACK);
            }
        }

        // If kill is set, killself after clearing the cube from the screen.

        // Get relative view points (for perspective calculations)
        for (int i = 0; i < 8; i++) {
            getViewRelative(&(v_relative[i]), &camera_pos, &(v[i]), &view_rot_inverse);
        }

        // Interpolate all pixels between vertices
        interpolatePoints(interpolated_points[0], &v_relative[0], &v_relative[1], m);
        interpolatePoints(interpolated_points[1], &v_relative[1], &v_relative[2], m);
        interpolatePoints(interpolated_points[2], &v_relative[2], &v_relative[3], m);
        interpolatePoints(interpolated_points[3], &v_relative[3], &v_relative[0], m);
        interpolatePoints(interpolated_points[4], &v_relative[0], &v_relative[4], m);
        interpolatePoints(interpolated_points[5], &v_relative[1], &v_relative[5], m);
        interpolatePoints(interpolated_points[6], &v_relative[2], &v_relative[6], m);
        interpolatePoints(interpolated_points[7], &v_relative[3], &v_relative[7], m);
        interpolatePoints(interpolated_points[8], &v_relative[4], &v_relative[5], m);
        interpolatePoints(interpolated_points[9], &v_relative[5], &v_relative[6], m);
        interpolatePoints(interpolated_points[10], &v_relative[6], &v_relative[7], m);
        interpolatePoints(interpolated_points[11], &v_relative[7], &v_relative[4], m);

        // Draw all points by projecting them to the screen.
        for (int i = 0; i < 12; i++) {
            for (int j = 0; j < m+1; j++) {
                getViewOnScreen(&projected_point, &camera_frame_offset, &(interpolated_points[i][j]));

                if (interpolated_points[i][j].z < 0) {
                    // NEEDS A SEMAPHORE
                    ST7789_DrawPixel(projected_point.x, projected_point.y, ST7789_BLUE);
                    
                }
            }
        }

        sleep(20);
    }
}

*/

/*
void Read_Buttons() {

    while(1) {
        // your code goes here
    }
}

void Read_JoystickPress() {

    while(1) {
        // your code goes here
    }
}

*/

/********************************Periodic Threads***********************************/

/*
void Print_WorldCoords(void) {
    UARTprintf("Cam Pos, X: %d, Y: %d, Z: %d\n", (int32_t) world_camera_pos.x, (int32_t)world_camera_pos.y, (int32_t)world_camera_pos.z);
}

void Get_Joystick(void) {
	// your code}
}

*/
/********************************Periodic Threads***********************************/


/*******************************Aperiodic Threads***********************************/

/*
void GPIOE_Handler() {
    // your code}

void GPIOD_Handler() {
   	// your code}
*/

/*******************************Aperiodic Threads***********************************/
