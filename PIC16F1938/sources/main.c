#include "main.h"
#include "device.h"
#include "lcd.h"
#include "rotenc.h"
#include "eeprom.h"
#include "process.h"

__CONFIG(CLKOUTEN_OFF & FOSC_HS & FCMEN_OFF & IESO_OFF & BOREN_ON & PWRTE_ON & WDTE_OFF & MCLRE_OFF & CP_OFF & CPD_OFF);
__CONFIG(PLLEN_OFF & STVREN_ON & WRT_OFF & LVP_OFF);

//
// タイマーで使用する変数
//
static unsigned long total_tmr0_cnt_100m;
static unsigned long total_tmr0_cnt_1s;
static unsigned char tmr0_toggle;

//
// 初期化処理
//
static void initialize()
{
	// ピンなどの初期設定を行う
	port_init();

	//
	// タイマー０の設定を行う
	//
	// TMR0ON: Enables Timer0
	// T08BIT: Timer0 is configured as an 8-bit timer/counter
	// プリスケーラー:64  － １カウント12.8μ秒(=1/20MHz*4*64)
	// OPTION_REG の先頭ビット:!WPUEN (1:内部プルアップ無)
	OPTION_REG = 0b10000101;
	// 256カウント（3.2768 ms）で割込み発生させる
	TMR0 = 0;
	// TMR0割り込み許可
	TMR0IE = 1;

	// UART 初期化
	uart_init();

	// CCP1 初期化
	ccp_init();

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
	// パラメーターをEEPROMから読込
	restore_parameters();

	// ピンや機能等の初期化処理
	initialize();

	// do_events 処理回数カウンター
	//   処理時点での割込みカウンター
	total_tmr0_cnt_100m = 0;
	total_tmr0_cnt_1s = 0;

	// LCD 初期化処理
	LCD_init();

	// 動作モードを表示
	init_device_mode();

	// パラメーター初期表示
	print_parameters();

	// 手動モードで初期化
	process_init();

	while (1) {
		// UART処理
		parse_uart_input(rc_buff);
		// イベント処理
		do_events();
	}
}
