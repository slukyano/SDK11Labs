#include <stdint.h>
#include <string.h>
#include "aduc812.h"
#include "max.h"
#include "led.h"
#include "interrupt.h"
#include "uart.h"
#include "keyboard.h"
#include "timer.h"

#define REQUEST_MAX_LENGTH 4
#define RESPONSE_MAX_LENGTH 5

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

void itoa(uint8_t n, char s[])
{
    int8_t i;
    i = 0;
    do {
        s[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    s[i] = '\0';
    reverse(s);
}

uint8_t hex_char_to_integer(char hexchar)
{
    if (hexchar >= '0' && hexchar <= '9')
        return hexchar - '0';
    else if (hexchar >= 'A' && hexchar <= 'F')
        return 10 + hexchar - 'A';
    else if (hexchar >= 'a' && hexchar <= 'f')
        return 10 + hexchar - 'a';
    else 
        return 0;
}

void hex_string_to_dec_string(char *hex, char *dec)
{
    char hex_char = hex[0];
    uint8_t hex_value = 0;
    uint8_t i = 0;
    while (hex_char != '\0') {
        hex_value = hex_value * 16 + hex_char_to_integer(hex_char);
        hex_char = hex[++i];
    }
    itoa(hex_value, dec);
}

void main(void)
{
    init_uart(); 
	init_kb();
	EA = 1;
	
	for (;;) 
    {
		char recieved_string[REQUEST_MAX_LENGTH];
		char response_string[RESPONSE_MAX_LENGTH];
		uint8_t i;
		__bit char_is_valid, sent_response;
		
		i = 0;
		sent_response = 0;
		do {
			char curr_char;
			
			curr_char = read_kb_char();
				
			write_uart(curr_char);
			
			if (curr_char == '#' ) {
				uint8_t response_length;
				uint8_t j;
				
				recieved_string[i] = '\0';
				hex_string_to_dec_string(recieved_string, response_string);
				
				response_length = strlen(response_string);
				response_string[response_length] = ' ';
				response_string[response_length + 1] = '\n';
				
				for (j = 0; j <= response_length + 1; j++)
					write_uart(response_string[j]);
				
				sent_response = 1;
			}
			else {
				char_is_valid = (curr_char >= '0' && curr_char <= '9')
						|| (curr_char >= 'A' && curr_char <= 'F')
						|| (curr_char >= 'a' && curr_char <= 'f');
				if (!char_is_valid)
					break;
				
				recieved_string[i] = curr_char;
			}
			
			i++;
		} while (!sent_response && i < REQUEST_MAX_LENGTH);
		
		if (!sent_response) {
			const char *error_message = "ERROR\n";
			uint8_t err_i;
			
			for (err_i = 0; err_i < 6; err_i++)
				write_uart(error_message[err_i]);
		}
	}
}

