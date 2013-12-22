#include <stdint.h>
#include <math.h>
#include "aduc812.h"
#include "timer.h"
#include "interrupt.h"

#include "led.h"

#define CPU_SPEED 12000000
#define MACHINE_CYCLE_LENGTH 12

// Shortcuts for using timer registers  
#define GET_TIMER0_VALUE() ((TH0 << 8) | TL0)
#define SET_TIMER0_VALUE(t) (TH0 = ((t) & 0xFF00) >> 8,\
                            TL0 = (t) & 0x00FF)

#define GET_TIMER1_VALUE() ((TH1 << 8) | TL1)
#define SET_TIMER1_VALUE(t) (TH1 = ((t) & 0xFF00) >> 8,\
                            TL1 = (t) & 0x00FF)
                            
#define GET_TIMER2_VALUE() ((TH2 << 8) | TL2)
#define SET_TIMER2_VALUE(t) (TH2 = ((t) & 0xFF00) >> 8,\
                            TL2 = (t) & 0x00FF)

// Converters ms <-> cycles.  
uint16_t machine_cycles_to_ms(uint16_t cycles)
{
    return ceilf((cycles / ((float)CPU_SPEED / MACHINE_CYCLE_LENGTH) * 1000));
}

uint16_t ms_to_machine_cycles(uint16_t ms)
{
    return ceilf((ms / 1000.0f) * ((float)CPU_SPEED / MACHINE_CYCLE_LENGTH));
}
                            
// Timer part  
// Timer/counter 0 is used as timer  

static void timer_isr(void) //__interrupt ( TF0_VECTOR )
{ 
    
}

void init_timer()
{
    TR0 = 0;
    TMOD = (TMOD & T1_MASK) | T0_M0;
    TL0 = 0;
    TH0 = 0;
    //set_vector(TF0_ADDRESS, timer_isr);
	ET0 = 1;
    TR0 = 1;
}

uint16_t get_time_stamp()
{
    return machine_cycles_to_ms(GET_TIMER0_VALUE());
}

uint16_t time_since_time_stamp(uint16_t time_stamp)
{
    return machine_cycles_to_ms(GET_TIMER0_VALUE()) - time_stamp;
}

// Counter part  
// Timer/counter 1 is used as counter  

static void counter_isr() __interrupt ( TF1_VECTOR )
{
    
}

void init_counter()
{
    TR1 = 0;
    TMOD = (TMOD & T0_MASK) | T1_CT | T1_M0;
    TL1 = 0;
    TH1 = 0;
    set_vector(TF1_ADDRESS, counter_isr);
	ET1 = 1;
    TR1 = 1;
}

uint16_t get_counter_value()
{
    return GET_TIMER1_VALUE();
}

// Delay part
// Timer/counter 0 is used as timer for delay

static volatile __bit is_in_delay;

static void delay_isr() __interrupt ( TF0_VECTOR )
{
    is_in_delay = 0;
}

void delay(uint16_t delay_time)
{
	int i;

    TMOD = (TMOD & T1_MASK) | T0_M0;
    ET0 = 1;

	for (i = 0; i < delay_time; i++)
	{
        TR0 = 0;
        SET_TIMER0_VALUE(0xFC65);
        set_vector(TF0_ADDRESS, delay_isr);
        is_in_delay = 1;

        TR0 = 1;
        
        while(is_in_delay)
            ;
	}
} 

