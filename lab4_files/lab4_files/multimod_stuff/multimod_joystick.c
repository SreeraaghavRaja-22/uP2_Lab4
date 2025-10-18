// multimod_joystick.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for joystick functions

/************************************Includes***************************************/

#include "../multimod_joystick.h"

#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>
#include <driverlib/adc.h>
#include <driverlib/interrupt.h>

#include <inc/tm4c123gh6pm.h>
#include <inc/hw_types.h>
#include <inc/hw_memmap.h>
#include <inc/hw_i2c.h>
#include <inc/hw_gpio.h>

/************************************Includes***************************************/

/********************************Public Functions***********************************/

// JOYSTICK_Init
// Initializes ports & adc module for joystick
// Return: void
void JOYSTICK_Init(void) {
    // your code
}

// JOYSTICK_IntEnable
// Enables interrupts
// Return: void
void JOYSTICK_IntEnable() {
    // your code
}

// JOYSTICK_GetPress
// Gets button reading
// Return: bool
uint8_t JOYSTICK_GetPress() {
    // your code
}

// JOYSTICK_GetX
// Gets X adc reading from joystick
// Return: uint16_t
uint16_t JOYSTICK_GetX() {
    // your code
}

// JOYSTICK_GetY
// Gets Y adc reading from joystick
// Return: uint16_t
uint16_t JOYSTICK_GetY() {
    // your code
}


// JOYSTICK_GetXY
// Gets X and Y adc readings
// Return: uint32_t, 16-bit packed, upper 16-bit is X and lower 16-bit is Y.
uint32_t JOYSTICK_GetXY() {
    // your code
}

/********************************Public Functions***********************************/

