#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

// Initialize keyboard.  
void init_kb();
// Read key
uint8_t read_kb_num();
char read_kb_char();
//Write to KB buffer
static void write_kb(uint8_t keynum);

#endif //KEYBOARD_H

