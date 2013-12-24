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

#define OPER_MAX_LGTH 2
#define STR_MAX_LGTH 3
#define RESULT_MAX_LGTH 5

void reverse(char s[])
{
    uint8_t i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void itoa(int32_t n, char s[])
{
     int i, sign;
 
     if ((sign = n) < 0)
         n = -n;        
     i = 0;
     do {       
         s[i++] = n % 10 + '0';
     } while ((n /= 10) > 0);
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
}
 
int32_t buf_to_int32(char buf[]){
	int s = 0, i;
	if (buf == NULL) return 0;    
    for (i = 0; buf[i] != '\0'; i++){
			s *= 10; s += (buf[i] - '0');        
    }
    return s;
}


int32_t compute(char f_oper[], char s_oper[], uint8_t sign){
	switch(sign)
	{
		case 1: return buf_to_int32(f_oper)+buf_to_int32(s_oper);
		case 2: return buf_to_int32(f_oper)-buf_to_int32(s_oper);
		case 3: return buf_to_int32(f_oper)*buf_to_int32(s_oper);
		case 4: return buf_to_int32(f_oper)/buf_to_int32(s_oper);
	}
	return 0;
}


void print_int(int32_t num){
	char result[RESULT_MAX_LGTH];
	char i;
	
	itoa(num, result);
	for(i=0; result[i] != '\0'; i++)		
		lcd_putchar(result[i]);
}


void main(void)
{
	char f_oper_buf[STR_MAX_LGTH];
	char s_oper_buf[STR_MAX_LGTH];
	uint8_t oper_iter = 0;
	uint8_t sign = 0;
	int32_t result=0;
	char input = 0;
	
	init_uart();
	init_kb();
	EA = 1;
	
	lcd_clear();
	
	while(1)
	{	
		input = read_kb_char();
		
		if(input=='#')
		{
			lcd_clear();
			oper_iter = 0;
			sign = 0;
		}
		else{
			switch(input)
			{
				case '*': //compute here
					if(sign != 0){
						lcd_putchar('=');
						
						result = compute(f_oper_buf,s_oper_buf,sign);
												
						print_int(result);
						leds(0xFF);
						
						input = read_kb_char();
						
						lcd_clear();
						oper_iter = 0;
						sign = 0;
						leds(0x00);						
					}
					continue;
				case 'A'://add
					if((sign == 0)&&(oper_iter!=0)){
						sign = 1;
						oper_iter = 0;
						lcd_putchar('+');
					}																
					break;
				case 'B'://sub
					if((sign == 0)&&(oper_iter!=0)){
						sign = 2;
						oper_iter = 0;
						lcd_putchar('-');
					}
					break;
				case 'C'://mult
					if((sign == 0)&&(oper_iter!=0)){
						sign = 3;
						oper_iter = 0;
						lcd_putchar('*');
					}
					break;
				case 'D'://div
					if((sign == 0)&&(oper_iter!=0)){
						sign = 4;
						oper_iter = 0;
						lcd_putchar('/');
					}
					break;
				default://input
					if(sign == 0) //first operand
					{
						if(oper_iter < OPER_MAX_LGTH){
							f_oper_buf[oper_iter] = input;
							oper_iter++;
							f_oper_buf[oper_iter] = '\0';
							lcd_putchar(input);
							
							leds(0xF0);
						}
					}
					else //second operand
					{
						if(oper_iter < OPER_MAX_LGTH){
							s_oper_buf[oper_iter] = input;
							oper_iter++;
							s_oper_buf[oper_iter] = '\0';
							lcd_putchar(input);
							
							leds(0x0F);
						}
					}					
					break;
			}
		}
	}
}
