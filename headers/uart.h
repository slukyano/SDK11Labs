#ifndef UART_H
#define UART_H

#include <stdint.h>

// Initialize UART.  
void init_uart();
// Read from UART
uint8_t read_uart();
// Write to UART
void write_uart(uint8_t data);
// Write line to UART
void writeln_uart(uint8_t * line);

#endif //UART_H
