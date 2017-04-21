#ifndef __LCD_H
#define __LCD_H

// 文字列編集用
unsigned char lcd_upper_buf[17];
unsigned char lcd_lower_buf[17];

// 関数
void lcd_init();
void lcd_print_upper();
void lcd_print_lower();

#endif // __LCD_H
