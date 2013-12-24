#include <stdint.h>
#include "aduc812.h"
#include "max.h"
#include "interrupt.h"
#include "keyboard.h"
#include "led.h"
#include "timer.h"
#include "sound.h"

#define KEYBUF_SIZE 64
#define PRESSED_TO_REPEAT_COUNT 10
#define REPEAT_WRITE_COUNT 3
#define DELAY_TIME_MS 10
#define SET_TIMER0_VALUE(t) (TH0 = ((t) & 0xFF00) >> 8,\
                            TL0 = (t) & 0x00FF)

typedef enum {
	KEY_PRESSED_STATE_RELEASED,
	KEY_PRESSED_STATE_BOUNCEDOWN,
	KEY_PRESSED_STATE_PRESSED,
	KEY_PRESSED_STATE_BOUNCEUP//,
	//KEY_PRESSED_STATE_REPEAT,
	//KEY_PRESSED_STATE_REPEAT_BOUNCEUP
} key_pressed_state;

typedef struct {
	key_pressed_state pressed;
	uint8_t repeat_count;
} key_state;

static key_state keys[4][4];

static uint8_t __xdata keybuf[KEYBUF_SIZE];
static uint8_t read_buf_index;
static uint8_t write_buf_index;
static __bit write_index_looped;

static const char __xdata keyboard_symbols[] = 
	"123A" 
	"456B"
	"789C"
	"*0#D";

static volatile __bit is_in_delay;

static void select_col(uint8_t col)
{
    write_max(KB, ~(0x01 << col));
}

static void update_key(uint8_t col, uint8_t row, uint8_t kb_reg)
{
    __bit key_down = 0;// = kb_reg & (0x10 << row) == 0;
    key_state *key_ptr;
    key_ptr = &(keys[row][col]);
	switch (kb_reg >> 4){
		case 0x0E:
			key_down = row == 0;
			break;
		case 0x0D:
			key_down = row == 1;
			break;
		case 0x0B:
			key_down = row == 2;
			break;
		case 0x07:
			key_down = row == 3;
			break;
	}
    switch (key_ptr->pressed)
    {
	    case KEY_PRESSED_STATE_RELEASED:
		    if (key_down) {
			    key_ptr->pressed = KEY_PRESSED_STATE_BOUNCEDOWN;
			}
		    break;

	    case KEY_PRESSED_STATE_BOUNCEDOWN:
		    if (key_down)
		    {
			    key_ptr->pressed = KEY_PRESSED_STATE_PRESSED;
			    key_ptr->repeat_count = 0;
			    write_kb(row*4 + col);
		    }
		    else
			    key_ptr->pressed = KEY_PRESSED_STATE_RELEASED;
		    break;

	    case KEY_PRESSED_STATE_PRESSED:
		    /*if (key_ptr->repeat_count == PRESSED_TO_REPEAT_COUNT)
		    {
			    key_ptr->pressed = KEY_PRESSED_STATE_REPEAT;
			    key_ptr->repeat_count = 0;
		    }
		    else
			    key_ptr->repeat_count++;*/
		
		    if (!key_down)
		    {
			    key_ptr->pressed = //(key_ptr->pressed == KEY_PRESSED_STATE_REPEAT) 
				    //? KEY_PRESSED_STATE_REPEAT_BOUNCEUP
				    /*:*/ KEY_PRESSED_STATE_BOUNCEUP;
		    }
		    break;

	    case KEY_PRESSED_STATE_BOUNCEUP:
		    if (key_down)
		    {
			    /*if (key_ptr->repeat_count == PRESSED_TO_REPEAT_COUNT)
			    {
				    key_ptr->pressed = KEY_PRESSED_STATE_REPEAT;
				    key_ptr->repeat_count = 0;
			    }
			    else*/
			    {
				    key_ptr->repeat_count++;
				    key_ptr->pressed = KEY_PRESSED_STATE_PRESSED;
			    }
		    }
		    else
			    key_ptr->pressed = KEY_PRESSED_STATE_RELEASED;
		    break;
	    /*case KEY_PRESSED_STATE_REPEAT:
		    if (key_ptr->repeat_count == REPEAT_WRITE_COUNT)
		    {
			    write_kb(row*4 + col);
			    key_ptr->repeat_count = 0;
		    }
		    else
			    key_ptr->repeat_count++;

		    if (!key_down)
			    key_ptr->pressed = KEY_PRESSED_STATE_REPEAT_BOUNCEUP;
		    break;

	    case KEY_PRESSED_STATE_REPEAT_BOUNCEUP:
		    if (key_down)
		    {
			    if (key_ptr->repeat_count == REPEAT_WRITE_COUNT)
			    {
				    write_kb(row*4 + col);
				    key_ptr->repeat_count = 0;
			    }
			    else
				    key_ptr->repeat_count++;
			    key_ptr->pressed = KEY_PRESSED_STATE_REPEAT;
		    }
		    else
			    key_ptr->pressed = KEY_PRESSED_STATE_RELEASED;
		    break;*/
    }
}

static void scan_col(uint8_t col)
{
    uint8_t row;
    uint8_t kb_reg;

    select_col(col);
    kb_reg = read_max(KB);
	write_max(KB, 0);

    for (row = 0; row < 4; row++)
	    update_key(col, row, kb_reg);
}

static void scan_all_cols()
{
    uint8_t col;
    for (col = 0; col < 4; col++) 
        scan_col(col);
}

/*static void delay_isr() __interrupt ( TF0_VECTOR )
{
	TR0 = 0;

    is_in_delay = 0;
	
	scan_all_cols();
}

static void delay(uint16_t delay_time)
{
	TR0 = 0;
	
    scan_all_cols();
	
	TMOD = (TMOD & T1_MASK) | T0_M0;
	TCON|=0x01;
    ET0 = 1;
    SET_TIMER0_VALUE((0x10000 - (0x10000 - 0xFC65) * delay_time));
    set_vector(TF0_ADDRESS, delay_isr);
    is_in_delay = 1;

    TR0 = 1;
}*/

static void kb_isr() __interrupt (IE0_VECTOR)
{
	uint16_t i;
	TCON&=0xFD;
	
	if (is_in_delay)
        return;
	
    scan_all_cols();
	
	for (i = 0; i < 100; i++)
		;

    //delay(10);
}

void init_kb()
{
	set_vector(IE0_ADDRESS, (void*)kb_isr);
	write_max(ENA, 0x60); // enable keyboard interrupt as INT0
    EX0 = 1; // enable INT0
	write_max(KB, 0);
	TMOD |= 0x01;
	TCON |= 0x11;

    init_sound();
}

static void write_kb(uint8_t keynum)
{
	if (write_index_looped && write_buf_index == read_buf_index)
		; //TODO: handle overflow here
	
	keybuf[write_buf_index] = keynum;
	
	if (write_buf_index == (KEYBUF_SIZE - 1)) {
		write_index_looped = 1;
		write_buf_index = 0;
	}
	else
		write_buf_index++;

    make_beep();
}

uint8_t read_kb_num()
{
    uint8_t num;
	while (!write_index_looped && write_buf_index == read_buf_index)
		;
	
	num = keybuf[read_buf_index];

	if (read_buf_index == (KEYBUF_SIZE - 1)) {
		write_index_looped = 0;
		read_buf_index = 0;
	}
	else
		read_buf_index++;

	return num;
}

char read_kb_char()
{
	return keyboard_symbols[read_kb_num()];
}

