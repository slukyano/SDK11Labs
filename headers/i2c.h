#ifndef __I2C__H
#define __I2C__H

#include <stdint.h>

__bit get_ack(uint8_t address); 
__bit receive_block(uint8_t address, uint8_t addr, uint8_t __xdata * block, uint8_t len);
__bit transmit_block(uint8_t address, uint8_t addr, uint8_t __xdata * block, uint8_t len);

__bit receive_block16(uint8_t address, uint16_t addr, uint8_t __xdata * block, uint16_t len);
__bit transmit_block16(uint8_t address, uint16_t addr, uint8_t __xdata * block, uint16_t len);


#endif //_I2C_H
