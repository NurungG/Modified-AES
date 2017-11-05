#ifndef _GF_H_
#define _GF_H_

#define IRR_POLY 0x4D

#include <stdint.h>

// Function Declaration
uint8_t GF_addition(uint8_t, uint8_t);
uint8_t GF_multiplication(uint8_t, uint8_t);

uint8_t GF_getAddInverse(uint8_t);
uint8_t GF_getMulInverse(uint8_t);


#endif