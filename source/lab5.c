#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "aduc812.h"
#include "max.h"
#include "led.h"
#include "interrupt.h"
#include "uart.h"
#include "keyboard.h"
#include "timer.h"
#include "lcd.h"

#define OPER_MAX_LGTH 7

void main(void)
{
	char[8] f_oper_buf, s_oper_buf;
	uint8_t oper_iter = 0;
	int32_t f_oper = 0 , s_oper = 0;
	int32_t result;
	__bit sign = 0;
	init_uart();
	init_kb();
	EA = 1;
	
	lcd_clear();
	
	while(1)
	{
		char input = 0;
		
		if(input=='#')
			lcd_clear();
		else{
			switch(input)
			{
				case '*': //compute here
					break;
				case 'A'://add
					
					break;
				case 'B'://sub
					break;
				case 'C'://mult
					break;
				case 'D'://div
					break;
				default://input
					if(sign == 0) //first operand
					{
						if(oper_iter < OPER_MAX_LGTH){
							f_oper_buf[oper_iter] = input;
							f_oper_buf[oper_iter+1] = '\n';
							lcd_putchar(input);
						}
					}
					else //second operand
					{
						if(oper_iter < OPER_MAX_LGTH){
							s_oper_buf[oper_iter] = input;
							s_oper_buf[oper_iter+1] = '\n';
							lcd_putchar(input);
						}
					}					
					break;
			}
		}
	}
}
