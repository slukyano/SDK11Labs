#ifndef __LCD__H
#define __LCD__H

#define DATA_IND_ADDR 0x080001
#define C_IND_ADDR 0x080006

/*Функция возвращает значение флага занятости - BF */
unsigned char lcd_bfstate();
/* Функция возвращает значение счетчика адреса для DDRAM */
unsigned char lcd_acstate();
/* Установка адреса в DDRAM */
void lcd_set_ddram_addr(const unsigned char addr);
/* Функция для очистки экрана ЖКИ и возврат курсора в начало строки. */
void lcd_clear();
/*Возврат курсора в левый верхний угол экрана (координаты X: 0 и Y: 0) */
void lcd_creturn();
/* Вывод символа на дисплей */
void lcd_putchar(const char c);
/* вывод С-строки символов на дисплей. Если строка "не влезает" в ЖКИ, то печатается лишь ее часть */
void lcd_puts(const char * s);
/* Функция для перевода курсора в заданные координаты.Если они некорректны, то ничего не происходит. */
void lcd_movcur(const unsigned char x, const unsigned char y);
#endif