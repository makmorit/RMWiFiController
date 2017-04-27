#include "common.h"
#include "device.h"
#include "process.h"
#include "uart.h"
#include "timer0.h"
#include "timert2.h"
#include "lcd.h"

// CONFIG1
#pragma config FCMEN = OFF
#pragma config CSWEN = OFF
#pragma config CLKOUTEN = OFF
#pragma config RSTOSC = EXT1X // External Oscillator no PLL
#pragma config FEXTOSC = HS   // External Oscillator 20MHz
// CONFIG2
#pragma config STVREN = ON
#pragma config BORV = 1
#pragma config BOREN = ON
#pragma config PWRTE = ON
#pragma config MCLRE = OFF
// CONFIG3
#pragma config WDTE = OFF
// CONFIG4
#pragma config LVP = OFF
#pragma config WRT = OFF
// CONFIG5
#pragma config CPD = OFF
#pragma config CP = OFF

//
// 各種設定
//   ピン設定, TIME 0, UART, CCP
//
static void setup()
{
    setup_port();
    setup_timer0();
    setup_timer2();
    setup_uart();

    // 全割込み処理を許可
    PEIE = 1;
    GIE  = 1;
}

//
// 割込み処理
//   UART, TIMER 0
//
static void interrupt intr(void)
{
    uart_intr();
    timer0_intr();
}

//
// メインルーチン
//
void main() 
{
    // ピンや機能等の設定処理
    setup();

    // 初期化処理
    //   TIMER 0, CCP, UART, LCD
    timer0_init();
    timer2_init();
    uart_init();
    lcd_init();
    
    // 主処理ループで使用される変数の初期化
    process_init();

    // 主処理ループ
    while (1) {
        process();
    }
}
