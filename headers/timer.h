#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

// Initialize and run timer (using timer 0).  
void init_timer();
// Returns current timer value in ms.  
uint16_t get_time_stamp();
// Returns time since time_stamp.  
uint16_t time_since_time_stamp(uint16_t time_stamp);

// Initialize and run counter (using timer 1).  
void init_counter();
// Returns current counter value.  
uint16_t get_counter_value();

// Wait delayTime using timer 2.  
void delay(uint16_t delay_time);

#endif //TIMER_H
