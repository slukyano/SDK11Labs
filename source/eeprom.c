//#define EEPROM_16   //Если EEPROM имеет 16-разрядное внутреннее адресное
                    //пространство

#include "eeprom.h" //Описание I2C-адреса EEPROM
#include "i2c.h"    //Функции модуля работы с I2C.

//Запись блока данных в EEPROM.
__bit write_block_eeprom( unsigned short address, unsigned char __xdata *buf, unsigned short length ) 
{
unsigned short try;
unsigned short pages, i, remainder;

    if( ( address + length ) > EEPROM_SIZE ) return 1;
    
    pages     = length >> 3;    // length / 8
    remainder = length &  0x7;  // length % 8

    for( i = 0; i < pages; ++i )
    {
        try = 0;

        while( !get_ack( EEPROM_ADDRESS ) )
        {
            if( ++try > 5000)
                return 1; //EEPROM failed to respond
        }
        if( transmit_block(EEPROM_ADDRESS, address + (i << 3), buf + (i << 3), 8) ) return 1; //Error writing
    }

    if( remainder )
    {
        try = 0;
        while( !get_ack(EEPROM_ADDRESS) )
        {
            if(++try > 5000)
            {
                return 1; //EEPROM failed to respond
            }
        }
        if( transmit_block( EEPROM_ADDRESS, address + (i << 3), buf + (i << 3), remainder ) ) return 1;                
    }

    return 0;
}


//Чтение блока данных из EEPROM.
__bit read_block_eeprom( unsigned short address, unsigned char __xdata *buf, unsigned short length )
{
	unsigned short try;

    if( ( address + length ) > EEPROM_SIZE) return 1;

    try = 0;

    while( !get_ack( EEPROM_ADDRESS ) )
    {
        if(++try > 5000) // >10 мс
            return 1; //EEPROM failed to respond
    }
    
    if( receive_block( EEPROM_ADDRESS, address, buf, length) ) return 1; //Error reading

    return 0;
}

