#include "max.h"
#include "lcd.h"
#include "aduc812.h"
#include <stdint.h>

//Функция возвращает значение флага занятости - BF
uint8_t lcd_bfstate() {
    uint8_t retval = 0x00;

    write_max(C_IND_ADDR, 0x03); /* RS=0 RW=1 E=1 */
    retval = read_max(DATA_IND_ADDR);
    write_max(C_IND_ADDR, 0x02); /* RS=0 RW=1 E=0 */
    retval = retval >> 7;

    return retval;
}

//Настройка экрана (курсор, очистка)
void init_lcd(){
	while(lcd_bfstate()) {}
    write_max(DATA_IND_ADDR, 0x06);
    write_max(C_IND_ADDR, 0x01);
    write_max(C_IND_ADDR, 0x00);

    while(lcd_bfstate()) {}
    write_max(DATA_IND_ADDR, 0x0f);
    write_max(C_IND_ADDR, 0x01);
    write_max(C_IND_ADDR, 0x00);
	
	lcd_clear();
	
    return;
}
//Функция возвращает значение счетчика адреса для DDRAM
uint8_t lcd_acstate() {
    uint8_t retval = 0x00;

    write_max(C_IND_ADDR, 0x03); /* RS=0 RW=1 E=1 */
    retval = read_max(DATA_IND_ADDR);
    write_max(C_IND_ADDR, 0x02); /* RS=0 RW=1 E=0 */
    retval = retval & 0x7f;

    return retval;
}

//Установка адреса в DDRAM
void lcd_set_ddram_addr(const uint8_t addr) {
    while(lcd_bfstate()) {}
    write_max(DATA_IND_ADDR, addr | 0x80);
    write_max(C_IND_ADDR, 0x01);
    write_max(C_IND_ADDR, 0x00);
    return;
}

//Функция для очистки экрана ЖКИ и возврат курсора в начало строки
void lcd_clear() {
    while(lcd_bfstate()) {}
    write_max(DATA_IND_ADDR, 0x01);
    write_max(C_IND_ADDR, 0x01);
    write_max(C_IND_ADDR, 0x00);
	
	return;	
}

//Возврат курсора в левый верхний угол экрана (координаты X: 0 и Y: 0)
void lcd_creturn() {
    while(lcd_bfstate()) {}
    write_max(DATA_IND_ADDR, 0x02);
    write_max(C_IND_ADDR, 0x01);
    write_max(C_IND_ADDR, 0x00);
    return;
}

//Вывод символа на дисплей
void lcd_putchar(const char c) {
    uint8_t ddram_addr = lcd_acstate();

    if ((ddram_addr > 0x0f) && (ddram_addr < 0x40)) {
        lcd_set_ddram_addr(0x40);
    }
    while(lcd_bfstate()) {}
    write_max(DATA_IND_ADDR, c);
    write_max(C_IND_ADDR, 0x05);
    write_max(C_IND_ADDR, 0x04);
    return;
}

//Вывод нуль-терминированной строки символов на дисплей. Если строка "не влезает" в ЖКИ, то печатается лишь ее часть
void lcd_puts(const char * s) {
    uint8_t wr_chars = 0;

    while ((s[wr_chars] != '\0') && (wr_chars <= 32)) {
        lcd_putchar(s[wr_chars++]);
    }

    return;
}

//Функция для перевода курсора в заданные координаты.Если они некорректны, то ничего не происходит
void lcd_movcur(const uint8_t x, const uint8_t y) {
    if ((x > 15) || (y > 1)) {
        return;
    }
    lcd_set_ddram_addr(x + 0x40 * y);

    return;
}
