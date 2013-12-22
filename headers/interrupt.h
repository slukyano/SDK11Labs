#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "aduc812.h" // this file has some vector numbers  
#include <stdint.h>

#define TF2_VECTOR 5 // not defined in aduc812.h  

#define IE0_ADDRESS (0x2003) // addresses of interrupt vectors  
#define TF0_ADDRESS (0x200B) //
#define IE1_ADDRESS (0x2013) //
#define TF1_ADDRESS (0x201B) //
#define SI0_ADDRESS (0x2023) //
#define TF2_ADDRESS (0x202B) //

void set_vector(uint8_t __xdata *address, void (*vector)());

#endif //INTERRUPT_H
