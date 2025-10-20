// multimod_OPT3001.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for OPT3001 functions

/************************************Includes***************************************/

#include "../multimod_PCA9555.h"

#include <stdint.h>
#include "../multimod_i2c.h"

/************************************Includes***************************************/

/********************************Public Functions***********************************/

uint16_t PCA9555_GetInput(uint32_t mod, uint8_t addr) {
    // your code
}

void PCA9555_SetPinDir(uint32_t mod, uint8_t addr, uint16_t pins) {
        // your code
}

void PCA9555_SetPinPol(uint32_t mod, uint8_t addr, uint16_t pins) {
    // your code
}
void PCA9555_SetOutput(uint32_t mod, uint8_t addr, uint16_t pins) {
    // your code
}



uint16_t PCA9555_ReadReg(uint32_t mod, uint8_t gpio_module_addr, uint8_t addr) {
    // your code
}

/********************************Public Functions***********************************/
