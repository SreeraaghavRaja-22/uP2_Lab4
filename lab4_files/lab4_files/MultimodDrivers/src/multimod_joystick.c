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
    // enable ADC module (Idk if it's 0 or 1) -- shouldn't matter (might have to use both)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0)){}

    // would I need to enable both ADCs for each axis?? 
    // configure the ADC Module pins
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_2 | GPIO_PIN_3);

    // set the input pins as ADC inputs 
    GPIOPINCOnfigure(GPIO_PORTE_BASE, (GPIO_PCTL_PE2_AIN1 | GPIO_PCTL_PE3_AIN0));


}

// JOYSTICK_IntEnable
// Enables interrupts
// Return: void
void JOYSTICK_IntEnable() {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD)){}

    // configure the interrupt pin for the switch on joystick 
    GPIOPinTypeGPIOInput(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);

    GPIOIntClear(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
    GPIOIntTypeSet(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN, GPIO_FALLING_EDGE);
    GPIOIntEnable(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN);
}

// JOYSTICK_GetPress
// Gets button reading
// Return: bool
uint8_t JOYSTICK_GetPress() {
    return ((uint8_t)GPIOPinRead(JOYSTICK_INT_GPIO_BASE, JOYSTICK_INT_PIN));
}

// JOYSTICK_GetX
// Gets X adc reading from joystick
// Return: uint16_t
uint16_t JOYSTICK_GetX() {
    // do some ADC stuff
}

// JOYSTICK_GetY
// Gets Y adc reading from joystick
// Return: uint16_t
uint16_t JOYSTICK_GetY() {
    // do some ADC stuff
}


// JOYSTICK_GetXY
// Gets X and Y adc readings
// Return: uint32_t, 16-bit packed, upper 16-bit is X and lower 16-bit is Y.
uint32_t JOYSTICK_GetXY() {
    // Do some ADC stuff
}

/********************************Public Functions***********************************/

