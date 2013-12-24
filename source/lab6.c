#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "aduc812.h"
#include "uart.h"
#include "eeprom.h"
#include "timer.h"
#include "led.h"
#include "lcd.h"

#define SIZE_BUF    128
#define CRC_POLYNOM 0x0131

static uint8_t __xdata buf[SIZE_BUF];

// using timers as entropy source
static uint8_t randomize()
{
	uint8_t rand;
	uint16_t i;
	rand = ((TL0 >> 4) ^ (TL1 << 4) ^ TL2);
	leds(rand);
	for(i = 0;i<10000;i++)
		{}
	return rand;
}

static uint8_t crc8(uint8_t *dataptr, uint32_t len, uint16_t polynom)
{
    uint8_t *dataa = dataptr;
    uint16_t crc = 0;
    uint8_t i;
    uint32_t j;
	
    for (j = 0; j < len; j++, dataa++) {
        crc ^= (*dataa << 8);
        for(i = 0; i < 8; i++) {
            if (crc & 0x8000)
                crc ^= (polynom << 7);
            crc <<= 1;
        }
    }

    return (uint8_t)(crc >> 8);
}

// hex <= 0x0F
static char hex_to_ascii(uint8_t hex)
{
    return (hex < 0x0A) ? ('0' + hex) : ('A' + hex - 0x0A);
}

static void show_buf()
{
    uint32_t i;
    for(i = 0; i < SIZE_BUF; i++)
    {
        //write_uart(hex_to_ascii(buf[i] & 0xF0) >> 4);
        //write_uart(hex_to_ascii(buf[i] & 0x0F));
		write_uart(buf[i]);
        write_uart(i%16==0 ? '\n' : ' ');
				
        //write_uart(buf[i] & 0x0F);
    }
	 write_uart('\n');
}

static void write_eeprom(__bit verbose)
{
    uint32_t i;
    for(i = 0; i <= SIZE_BUF-2; i++)
        buf[i] = randomize();        
    
    buf[SIZE_BUF-1] = crc8(buf, SIZE_BUF-1, CRC_POLYNOM); // write crc8

    writeln_uart("Write test: [");

    if (write_block_eeprom(0, buf, SIZE_BUF) == 1)
        writeln_uart("ERR_I2C]\r\n");
    else
        writeln_uart("OK]\r\n");
    
    if(verbose)        
        show_buf();    
}

static void read_eeprom(__bit verbose)
{
    writeln_uart("Read test: [");

    if (read_block_eeprom(0, buf, SIZE_BUF) == 1) {
        writeln_uart("ERR_I2C]\r\n");
        return;
    }

    if (buf[SIZE_BUF-1] != crc8(buf, SIZE_BUF-1, CRC_POLYNOM)) // check crc8
        writeln_uart("ERR_CRC]\r\n");
    else
        writeln_uart("OK]\r\n");
    
    if(verbose)        
        show_buf();
}

static void clear_eeprom()
{
    uint8_t i;
    for(i = 0; i <= SIZE_BUF-2; i++)
        buf[i] = 0xff;
	buf[SIZE_BUF-1] = crc8(buf, SIZE_BUF-1, CRC_POLYNOM); // write crc8	
	
    writeln_uart("Clear: [");

    if (write_block_eeprom(0, buf, SIZE_BUF) == 1)
        writeln_uart("ERR_I2C]\r\n");
    else
        writeln_uart("OK]\r\n");
}

static void print_menu()
{
    writeln_uart( "EEPROM TEST\r\n" );
    writeln_uart( "===================================\r\n" );
    writeln_uart( "1 - Write\r\n" );
    writeln_uart( "2 - Read\r\n" );
    writeln_uart( "3 - Clear EEPROM\r\n" );
    writeln_uart( "4 - Autotest\r\n" );
    writeln_uart( "===================================\r\n" );
}

void main( void )
{
    init_uart();
	init_timer();
	
    while(1)
    {
        print_menu();

        switch(read_uart())
        {
            case '1':    write_eeprom(1); break;
            case '2':    read_eeprom(1); break;
            case '3':    clear_eeprom(); break;
            case '4':    write_eeprom(0); read_eeprom(0); break;

            default: writeln_uart("wrong input\r\n"); break;
        }
    }
}