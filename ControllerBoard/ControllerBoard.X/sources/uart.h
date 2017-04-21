#ifndef __UART_H
#define __UART_H

// 関数
void uart_intr();
void uart_init();
unsigned char *get_uart_recv_buff();
void putch(unsigned char byte);

#endif // __UART_H
