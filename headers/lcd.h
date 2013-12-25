#ifndef __LCD__H
#define __LCD__H

#define DATA_IND_ADDR 0x080001
#define C_IND_ADDR 0x080006

/*������� ���������� �������� ����� ��������� - BF */
unsigned char lcd_bfstate();
/* ������� ���������� �������� �������� ������ ��� DDRAM */
unsigned char lcd_acstate();
/* ��������� ������ � DDRAM */
void lcd_set_ddram_addr(const unsigned char addr);
/* ������� ��� ������� ������ ��� � ������� ������� � ������ ������. */
void lcd_clear();
/*������� ������� � ����� ������� ���� ������ (���������� X: 0 � Y: 0) */
void lcd_creturn();
/* ����� ������� �� ������� */
void lcd_putchar(const char c);
/* ����� �-������ �������� �� �������. ���� ������ "�� �������" � ���, �� ���������� ���� �� ����� */
void lcd_puts(const char * s);
/* ������� ��� �������� ������� � �������� ����������.���� ��� �����������, �� ������ �� ����������. */
void lcd_movcur(const unsigned char x, const unsigned char y);
#endif