#include "main.h"
#include "device.h"
#include "lcd.h"
#include "rotenc.h"
#include "eeprom.h"
#include "process.h"

// プログラムモード
//   0:パラメーター表示中
//   2:stopping_duty入力中
//   3:starting_duty入力中
//   4:topspeed_duty入力中
//   5:reducing_duty入力中
static unsigned char pgm_mode_status = 0;
static unsigned char pgm_mode_initdisp_cnt = 0;

// 入力中のパラメーターを退避
static unsigned char starting_duty_temp;
static unsigned char topspeed_duty_temp;
static unsigned char reducing_duty_temp;
static unsigned char stopping_duty_temp;

//
// 変更前のパラメーターを表示
//
static void pgm_mode_initdsp()
{
	// 正転時
	if (DIR_RELAY_SW == 0) {
		sprintf(lcd_upper_buf, "*ProgramMode* fw");
		sprintf(lcd_lower_buf, "   %3u->%3u->%3u", starting_duty_fowd, topspeed_duty_fowd, reducing_duty_fowd);
	} else {
		sprintf(lcd_upper_buf, "*ProgramMode* bk");
		sprintf(lcd_lower_buf, "   %3u->%3u->%3u", starting_duty_back, topspeed_duty_back, reducing_duty_back);
	}
	LCD_print_upper();
	LCD_print_lower();
}

//
// ボタン押下時の処理
//
void pgm_mode_init(int dir_relay_sw)
{
	// プログラムモードへ遷移
	main_mode = 2;
	pgm_mode_status = 0;
	rsw_counter_value = 0;

	// 前進・後退の別を設定
	DIR_RELAY_SW = dir_relay_sw;

	// ２秒ほど、パラメーターを表示
	pgm_mode_initdisp_cnt = 20; 
	pgm_mode_initdsp();
}

void pgm_mode_set_parameter()
{
	// パラメーターをロータリーエンコーダーの現在値で設定
	if (pgm_mode_status == 2) {
		stopping_duty_temp = rsw_counter_value;
		pgm_mode_status = 3;

	} else if (pgm_mode_status == 3) {
		starting_duty_temp = rsw_counter_value;
		pgm_mode_status = 4;

	} else if (pgm_mode_status == 4) {
		topspeed_duty_temp = rsw_counter_value;
		pgm_mode_status = 5;

	} else if (pgm_mode_status == 5) {
		reducing_duty_temp = rsw_counter_value;
		pgm_mode_status = 6;
		// Dutyを停止状態に変更
		set_ccp(0);
	}
}
//
// 100msごとに実行される処理
//
void process_on_pgm_mode()
{
	if (pgm_mode_status == 0) {
		// パラメーター値を表示
		pgm_mode_initdisp_cnt--;
		if (pgm_mode_initdisp_cnt == 0) {
			// ロータリーエンコーダー
			rsw_counter_value = stopping_duty;
			// ステータスを進める
			pgm_mode_status = 2;
		}

	} else if (pgm_mode_status == 2) {
		// プロンプトを表示
		sprintf(lcd_lower_buf, "stoppingDuty=%3u", rsw_counter_value);
		LCD_print_lower();
		set_ccp(rsw_counter_value);

	} else if (pgm_mode_status == 3) {
		// プロンプトを表示
		sprintf(lcd_lower_buf, "startingDuty=%3u", rsw_counter_value);
		LCD_print_lower();
		set_ccp(rsw_counter_value);

	} else if (pgm_mode_status == 4) {
		// プロンプトを表示
		sprintf(lcd_lower_buf, "topspeedDuty=%3u", rsw_counter_value);
		LCD_print_lower();
		set_ccp(rsw_counter_value);

	} else if (pgm_mode_status == 5) {
		// プロンプトを表示
		sprintf(lcd_lower_buf, "reducingDuty=%3u", rsw_counter_value);
		LCD_print_lower();
		set_ccp(rsw_counter_value);

	} else if (pgm_mode_status == 6) {
		// プロンプトを表示
		sprintf(lcd_lower_buf, "now saving param");
		LCD_print_lower();
		// パラメーターを適用
		if (DIR_RELAY_SW == 0) {
			starting_duty_fowd = starting_duty_temp;
			topspeed_duty_fowd = topspeed_duty_temp;
			reducing_duty_fowd = reducing_duty_temp;
		} else {
			starting_duty_back = starting_duty_temp;
			topspeed_duty_back = topspeed_duty_temp;
			reducing_duty_back = reducing_duty_temp;
		}
		stopping_duty = stopping_duty_temp;
		// EEPROMにパラメーターを書き込む
		save_parameters();
		// パラメーターを表示する
		print_parameters();
		// プログラムモード終了（手動モードに遷移）
		process_init();
	}
}
