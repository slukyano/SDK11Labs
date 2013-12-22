#include <stdint.h>
#include "aduc812.h"
#include "max.h"
#include "interrupt.h"

void init_uart()
{
	set_vector(SI0_ADDRESS, 0);
    SCON = 0x52;
    TMOD = 0x20;
    TCON = 0x69;
    TH1 = 0xF4;
}

uint8_t read_uart()
{
	uint8_t databyte; 
	while (!RI)
		;
    databyte = SBUF;
    RI = 0;
    return databyte;
}

void write_uart(uint8_t databyte)
{
	uint16_t i;
    SBUF = databyte;
	//TI = 0;
	//while(TI != 1) 
    //    ;
	for (i = 0; i < 1000; i++)
		;
}


void writeln_uart(uint8_t * line)
{
	while( *line )
		write_uart(*line++);
}