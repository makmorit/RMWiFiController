#ifndef __LCD_H
#define __LCD_H

// ������ҏW�p
unsigned char lcd_upper_buf[17];
unsigned char lcd_lower_buf[17];

// �֐�
void LCD_init();
void LCD_print_upper();
void LCD_print_lower();

#endif // __LCD_H
