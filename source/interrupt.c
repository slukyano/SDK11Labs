#include <stdint.h>

void set_vector(uint8_t __xdata *address, void (*vector)()) 
{ 
    address[0] = 0x02; // ljmp
    
    // Big endian: higher byte at lower address  
    address[1] = (uint8_t)((uint16_t)vector >> 8);
    address[2] = (uint8_t)vector;
}
