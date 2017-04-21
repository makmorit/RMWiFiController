#include "common.h"
#include "device.h"
#include "process.h"
#include "rotenc.h"

// CONFIG1
#pragma config FOSC = HS // 20MHz
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config MCLRE = OFF
#pragma config CP = OFF
#pragma config CPD = OFF
#pragma config BOREN = ON
#pragma config CLKOUTEN = OFF
#pragma config IESO = OFF
#pragma config FCMEN = OFF

// CONFIG2
#pragma config WRT = OFF
#pragma config PLLEN = OFF
#pragma config STVREN = ON
#pragma config BORV = HI
#pragma config LVP = OFF

//
// タイマーで使用する変数
//
static unsigned long total_tmr0_cnt_100m;
static unsigned long total_tmr0_cnt_1s;
static unsigned char tmr0_toggle;

//
// 各種設定／初期化処理
//
static void setup()
{
    // 各種設定処理
    //   ピン設定、タイマー０、UART、CCP
    //   
    setup_port();
    setup_timer0();
    setup_uart();
    setup_ccp();

    // 全割込み処理を許可する
    PEIE = 1;
    GIE  = 1;

	// TIMER2 on prescale=1
	// TIMER2スタート（onビットを１）
	T2CON = 0b100;
}

//
// 割込み処理
//
unsigned char rc_char;
unsigned char rc_buff[64];
unsigned char rc_cnt = 0;
static void interrupt intr(void)
{
	// RS232C受信割込みの場合
	if (RCIF == 1) {
		rc_char = RCREG;
		// 改行文字が入力されたら
		if (rc_char == '\n' || rc_char == '\r') {
			if (rc_cnt < 1) {
				return;
			}
			rc_buff[rc_cnt] = 0;
			rs232c_flg = 1;
			rc_cnt = 0;
		} else {
			rc_buff[rc_cnt++] = rc_char;
		}
	}
	// タイマー０割込み（1ミリ秒ごと）の場合
	if (TMR0IF == 1) {
		// 割込みカウンター
		total_tmr0_cnt_100m++;
		total_tmr0_cnt_1s++;
		tmr0_toggle = 1;
		// 256カウント（3.2768 ms）で割込み発生させる
		TMR0 = 0;
		// TMR0割り込みクリア
		TMR0IF = 0;
		// ロータリースイッチによるカウンター制御
		rsw_curr_value = ROT_ENC_CURVAL;
	}
}


//
// イベント処理
//
static void do_events()
{
	//
	// 割込みごとに処理（3.2768 ms）
	if (tmr0_toggle == 1) {
		// ロータリーエンコーダーによる検知処理
		if (rotenc_detection() != 0) {
			process_on_change_rotenc_val();
		}
		switch_prevent();
		tmr0_toggle = 0;
		// センサー検知処理
		sensor_detection();
	}

	//
	// 約 0.1 秒ごとに処理（3.2768ms × 30回）
	//
	if (total_tmr0_cnt_100m > 30) {
		// カウンターを初期化
		total_tmr0_cnt_100m = 0;
		// イベントごとの処理（モード／ステータス遷移）を行う
		process_on_100m_second();
	}

	//
	// 約 1.0 秒ごとに処理（3.2768ms × 305回）
	//
	if (total_tmr0_cnt_1s > 305) {
		// カウンターを初期化
		total_tmr0_cnt_1s = 0;
		// イベントごとの処理を行う
		process_on_one_second();
	}

	// スイッチ検知処理
	switch_detection();
}

//
// メインルーチン
//
void main() 
{
    // ピンや機能等の設定処理
    setup();

    // do_events 処理回数カウンター
    //   処理時点での割込みカウンター
    total_tmr0_cnt_1s = 0;

	// LCD 初期化処理
	LCD_init();

	// 動作モードを設定
	set_device_mode();

	// パラメーター初期表示
	print_parameters();

    // 各種初期化処理
    process_init();

    while (1) {
		// UART処理
		parse_uart_input(rc_buff);
		// イベント処理
		do_events();
    }
}
