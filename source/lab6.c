#include <stdint.h>
#include <string.h>
#include "aduc812.h"
#include "uart.h"
#include "eeprom.h"
#include "timer.h"
#include "led.h"
#include "lcd.h"
#include "keyboard.h"

#define SIZE_BUF    128
#define CRC_POLYNOM 0x0131
#define KEYBOARD_LCD

static uint8_t __xdata buf[SIZE_BUF];

// using timers as entropy source
static uint8_t randomize()
{
	uint8_t rand;
	uint16_t i;
	rand = ((TL0 >> 4) ^ (TL1 << 4) ^ TL2);
	leds(rand);
	for(i=0;i<=10000;i++)
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
	#ifdef KEYBOARD_LCD
	lcd_clear();	
	lcd_puts("Write: [");
	#endif

    if (write_block_eeprom(0, buf, SIZE_BUF) == 1)
	{
        writeln_uart("ERR_I2C]\r\n");
		#ifdef KEYBOARD_LCD
		lcd_puts("ERR_I2C]");
		if(verbose)
			read_kb_char();
		#endif
	}
    else
	{
        writeln_uart("OK]\r\n");
		#ifdef KEYBOARD_LCD
		lcd_puts("OK]");
		if(verbose)
			read_kb_char();
		#endif
	}
    #ifndef KEYBOARD_LCD
    if(verbose)        
        show_buf();
	#endif		
}

static void read_eeprom(__bit verbose)
{
    writeln_uart("Read test: [");
	#ifdef KEYBOARD_LCD
	if(verbose){		
		lcd_clear();
	}
	else{
		lcd_movcur(0,1);
	}
	lcd_puts("Read: [");
	#endif

    if (read_block_eeprom(0, buf, SIZE_BUF) == 1) {
        writeln_uart("ERR_I2C]\r\n");
		#ifdef KEYBOARD_LCD
		lcd_puts("ERR_I2C]");
		read_kb_char();
		#endif
        return;
    }

    if (buf[SIZE_BUF-1] != crc8(buf, SIZE_BUF-1, CRC_POLYNOM)) // check crc8
	{
        writeln_uart("ERR_CRC]\r\n");
		#ifdef KEYBOARD_LCD
		lcd_puts("ERR_CRC]");
		read_kb_char();
		#endif
	}
    else
	{
		writeln_uart("OK]\r\n");
		#ifdef KEYBOARD_LCD
		lcd_puts("OK]");
		read_kb_char();
		#endif
	}
    #ifndef KEYBOARD_LCD
    if(verbose)        
        show_buf();
	#endif
}

static void clear_eeprom()
{
    uint8_t i;
    for(i = 0; i <= SIZE_BUF-2; i++)
        buf[i] = 0xff;
	buf[SIZE_BUF-1] = crc8(buf, SIZE_BUF-1, CRC_POLYNOM); // write crc8	
	
    writeln_uart("Clear: [");
	#ifdef KEYBOARD_LCD
	lcd_clear();
	lcd_puts("Clear: [");
	#endif

    if (write_block_eeprom(0, buf, SIZE_BUF) == 1)
	{
		writeln_uart("ERR_I2C]\r\n");
		#ifdef KEYBOARD_LCD
		lcd_puts("ERR_I2C]");
		read_kb_char();
		#endif
	}
    else
	{	
		writeln_uart("OK]\r\n");
		#ifdef KEYBOARD_LCD
		lcd_puts("OK]");
		read_kb_char();
		#endif
	}
}

static void print_menu()
{
    writeln_uart( "    EEPROM TEST\r\n" );
    writeln_uart( "===================================\r\n" );
    writeln_uart( "1 - Write\r\n" );
    writeln_uart( "2 - Read\r\n" );
    writeln_uart( "3 - Clear EEPROM\r\n" );
    writeln_uart( "4 - Autotest\r\n" );
    writeln_uart( "===================================\r\n" );
}

static void print_menu_lcd()
{
	lcd_clear();
	lcd_puts("1-Write 2-Read  3-Clear 4-Auto");
}

void main( void )
{
	char choice;
    init_uart();	
	init_kb();
	#ifdef KEYBOARD_LCD
	init_lcd();
	#endif
	EA=1;
	
    while(1)
    {	
		#ifdef KEYBOARD_LCD
		print_menu_lcd();
		choice = read_kb_char();
		#else
		print_menu();
		choice = read_uart();
		#endif
        switch(choice)
        {
            case '1':    write_eeprom(1); break;
            case '2':    read_eeprom(1); break;
            case '3':    clear_eeprom(); break;
            case '4':    write_eeprom(0); read_eeprom(0); break;

            default: writeln_uart("wrong input\r\n"); break;
        }
    }
}