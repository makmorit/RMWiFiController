#include "common.h"
#include "device.h"
#include "uart.h"

//
// UARTで使用する変数
//
unsigned char uart_recv_char;
unsigned char uart_recv_buff[64];
unsigned char uart_recv_cnt = 0;

// UART文字列受信フラグ（1:受信済 0:受信まち）
unsigned char uart_char_recv_flg = 0;

void uart_intr()
{
    // RS232C受信割込みの場合
    if (RCIF != 1) {
        return;
    }

    uart_recv_char = RCREG;
    // 改行文字が入力されたら
    if (uart_recv_char == '\n' || uart_recv_char == '\r') {
        if (uart_recv_cnt < 1) {
            return;
        }
        uart_recv_buff[uart_recv_cnt] = 0;
        uart_char_recv_flg = 1;
        uart_recv_cnt = 0;
    } else {
        uart_recv_buff[uart_recv_cnt++] = uart_recv_char;
    }
}


void uart_init()
{
    // UART文字列受信フラグを初期化
    uart_char_recv_flg = 0;
    // バッファを初期化
    memset(uart_recv_buff, 0, sizeof(uart_recv_buff));
}

unsigned char *get_uart_recv_buff()
{
    // UART割り込みが設定されていなければ設定
    if (RCIE == 0) {
        RCIE = 1;
    }

    // UARTに入力された文字がなければ終了
    if (uart_char_recv_flg == 0) {
        return NULL;
    }

    // フラグをクリア
    uart_char_recv_flg = 0;
    
    // 文字列バッファーの先頭アドレスを戻す
    return (unsigned char *)uart_recv_buff;
}

//
// printf 内で使用する putch 関数
// (UART TX に１バイト出力します)
//
void putch(unsigned char byte)
{
    while (!TXIF) {
        continue;
    }
    TXREG = byte;
}
