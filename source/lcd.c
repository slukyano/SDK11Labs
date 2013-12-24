#include "max.h"
#include "aduc812.h"
#include <stdint.h>

#define DATA_IND_ADDR 0x080001
#define C_IND_ADDR 0x080006



/*������� ���������� �������� ����� ��������� - BF */
unsigned char lcd_bfstate() {
    unsigned char retval = 0x00;

    write_max(C_IND_ADDR, 0x03); /* RS=0 RW=1 E=1 */
    retval = read_max(DATA_IND_ADDR);
    write_max(C_IND_ADDR, 0x02); /* RS=0 RW=1 E=0 */
    retval = retval >> 7;

    return retval;
}

/* ������� ���������� �������� �������� ������ ��� DDRAM */
unsigned char lcd_acstate() {
    unsigned char retval = 0x00;

    write_max(C_IND_ADDR, 0x03); /* RS=0 RW=1 E=1 */
    retval = read_max(DATA_IND_ADDR);
    write_max(C_IND_ADDR, 0x02); /* RS=0 RW=1 E=0 */
    retval = retval & 0x7f;

    return retval;
}

/* ��������� ������ � DDRAM */
void lcd_set_ddram_addr(const unsigned char addr) {
    while(lcd_bfstate()) {}
    write_max(DATA_IND_ADDR, addr | 0x80);
    write_max(C_IND_ADDR, 0x01);
    write_max(C_IND_ADDR, 0x00);
    return;
}

/* ������� ��� ������� ������ ��� � ������� ������� � ������ ������. */
void lcd_clear() {
    while(lcd_bfstate()) {}
    write_max(DATA_IND_ADDR, 0x01);
    write_max(C_IND_ADDR, 0x01);
    write_max(C_IND_ADDR, 0x00);

    while(lcd_bfstate()) {}
    write_max(DATA_IND_ADDR, 0x06);
    write_max(C_IND_ADDR, 0x01);
    write_max(C_IND_ADDR, 0x00);

    while(lcd_bfstate()) {}
    write_max(DATA_IND_ADDR, 0x0f);
    write_max(C_IND_ADDR, 0x01);
    write_max(C_IND_ADDR, 0x00);
    return;
}

/*������� ������� � ����� ������� ���� ������ (���������� X: 0 � Y: 0) */
void lcd_creturn() {
    while(lcd_bfstate()) {}
    write_max(DATA_IND_ADDR, 0x02);
    write_max(C_IND_ADDR, 0x01);
    write_max(C_IND_ADDR, 0x00);
    return;
}

/* ����� ������� �� ������� */
void lcd_putchar(const char c) {
    unsigned char ddram_addr = lcd_acstate();

    if ((ddram_addr > 0x0f) && (ddram_addr < 0x40)) {
        lcd_set_ddram_addr(0x40);
    }
    while(lcd_bfstate()) {}
    write_max(DATA_IND_ADDR, c);
    write_max(C_IND_ADDR, 0x05);
    write_max(C_IND_ADDR, 0x04);
    return;
}

/* ����� �-������ �������� �� �������. ���� ������ "�� �������" � ���, �� ���������� ���� �� ����� */
void lcd_puts(const char * s) {
    unsigned char wr_chars = 0;

    while ((s[wr_chars] != '\0') && (wr_chars <= 32)) {
        lcd_putchar(s[wr_chars++]);
    }

    return;
}

/* ������� ��� �������� ������� � �������� ����������.���� ��� �����������, �� ������ �� ����������. */
void lcd_movcur(const unsigned char x, const unsigned char y) {
    if ((x > 15) || (y > 1)) {
        return;
    }
    lcd_set_ddram_addr(x + 0x40 * y);

    return;
}
