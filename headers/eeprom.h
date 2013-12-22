#ifndef __EEPROM__H
#define __EEPROM__H

#define EEPROM_ADDRESS  0xA2
#define EEPROM_SIZE     32768

extern __bit write_block_eeprom( unsigned short address, unsigned char __xdata *buf, unsigned short length );
extern __bit read_block_eeprom( unsigned short address, unsigned char __xdata *buf, unsigned short length );

#endif //__EEPROM__H
