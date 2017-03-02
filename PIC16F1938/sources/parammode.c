#include "main.h"
#include "lcd.h"
#include "rotenc.h"
#include "eeprom.h"
#include "process.h"

// パラメーター変更モード
//   0:パラメーター表示中
//   2:accl_interval入力中
//   3:decl_interval入力中
//   4:starting_interval入力中
static unsigned char param_mode_status;
static unsigned char param_mode_initdisp_cnt;

// パラメーター変更種別
//   0:個別パラメーター（前進用）
//   1:個別パラメーター（逆転用）
//   2:共通パラメーター
//   3:自動運転用パラメーター
static unsigned char param_mode_type;

// 入力中のパラメーターを退避
static unsigned char stopping_duty_temp;
static unsigned char starting_duty_temp;
static unsigned char topspeed_duty_temp;
static unsigned char reducing_duty_temp;
static unsigned char accl_interval_temp;
static unsigned char decl_interval_temp;
static unsigned char starting_interval_temp;
static unsigned char adrv_wait_for_reverse_temp;
static unsigned char adrv_wait_for_restart_temp;

//
// 表示用
//
static void param_mode_promptdsp()
{
	switch (param_mode_status) {
	case 2:
		sprintf(lcd_lower_buf, "startingDuty=%3u", rsw_counter_value);
		break;

	case 3:
		sprintf(lcd_lower_buf, "topspeedDuty=%3u", rsw_counter_value);
		break;

	case 4:
		sprintf(lcd_lower_buf, "reducingDuty=%3u", rsw_counter_value);
		break;

	case 5:
		sprintf(lcd_lower_buf, "stoppingDuty=%3u", rsw_counter_value);
		break;

	case 6:
		sprintf(lcd_lower_buf, "Starting Intv=%2u", rsw_counter_value);
		break;

	case 7:
		sprintf(lcd_lower_buf, "Accl Interval=%2u", rsw_counter_value);
		break;

	case 8:
		sprintf(lcd_lower_buf, "Decl Interval=%2u", rsw_counter_value);
		break;

	case 9:
		sprintf(lcd_lower_buf, "Wait Reverse=%3u", rsw_counter_value);
		break;

	case 10:
		sprintf(lcd_lower_buf, "Wait Restart=%3u", rsw_counter_value);
		break;
	}
	LCD_print_lower();
}

static void param_mode_initdsp()
{
	switch (param_mode_type) {
	case 0:
		sprintf(lcd_upper_buf, "*Parameter   fwd");
		break;
	case 1:
		sprintf(lcd_upper_buf, "*Parameter   bak");
		break;
	case 2:
		sprintf(lcd_upper_buf, "*Parameter   cmn");
		break;
	case 3:
		sprintf(lcd_upper_buf, "*Parameter  adrv");
		break;
	}
	LCD_print_upper();
	sprintf(lcd_lower_buf, "    Change Mode*");
	LCD_print_lower();
}

//
// ボタン押下時の処理
//
void param_mode_init(unsigned char type)
{
	// パラメーター変更モードへ遷移
	main_mode = 3;
	param_mode_status = 0;
	rsw_counter_value = 0;

	// 変更種別をセット
	param_mode_type = type;

	// メニューを表示
	param_mode_initdisp_cnt = 20;
	param_mode_initdsp();
}

void param_mode_set_parameter()
{
	switch (param_mode_status) {
	// パラメーターをロータリーエンコーダーの現在値で設定
	//   前進・逆転用個別パラメーター
	case 2:
		starting_duty_temp = rsw_counter_value;
		if (param_mode_type == 0) {
			rsw_counter_value = topspeed_duty_fowd;
		} else {
			rsw_counter_value = topspeed_duty_back;
		}
		param_mode_status = 3;
		break;
	case 3:
		topspeed_duty_temp = rsw_counter_value;
		if (param_mode_type == 1) {
			rsw_counter_value = reducing_duty_fowd;
		} else {
			rsw_counter_value = reducing_duty_back;
		}
		param_mode_status = 4;
		break;
	case 4:
		reducing_duty_temp = rsw_counter_value;
		param_mode_status = 99;
		break;
	//   共通パラメーター
	case 5:
		stopping_duty_temp = rsw_counter_value;
		rsw_counter_value = starting_interval;
		param_mode_status = 6;
		break;
	case 6:
		starting_interval_temp = rsw_counter_value;
		rsw_counter_value = accl_interval;
		param_mode_status = 7;
		break;
	case 7:
		accl_interval_temp = rsw_counter_value;
		rsw_counter_value = decl_interval;
		param_mode_status = 8;
		break;
	case 8:
		decl_interval_temp = rsw_counter_value;
		param_mode_status = 99;
		break;
	//   自動運転用パラメーター
	case 9:
		adrv_wait_for_reverse_temp = rsw_counter_value;
		rsw_counter_value = adrv_wait_for_restart;
		param_mode_status = 10;
		break;
	case 10:
		adrv_wait_for_restart_temp = rsw_counter_value;
		param_mode_status = 99;
		break;
	}
}

static void apply_parameter()
{
	// プロンプトを表示
	sprintf(lcd_lower_buf, "now saving param");
	LCD_print_lower();

	// パラメーターを適用
	switch (param_mode_type) {
	case 0:
		starting_duty_fowd = starting_duty_temp;
		topspeed_duty_fowd = topspeed_duty_temp;
		reducing_duty_fowd = reducing_duty_temp;
		break;
	case 1:
		starting_duty_back = starting_duty_temp;
		topspeed_duty_back = topspeed_duty_temp;
		reducing_duty_back = reducing_duty_temp;
		break;
	case 2:
		stopping_duty     = stopping_duty_temp;
		starting_interval = starting_interval_temp;
		accl_interval     = accl_interval_temp;
		decl_interval     = decl_interval_temp;
		break;
	case 3:
		adrv_wait_for_reverse = adrv_wait_for_reverse_temp;
		adrv_wait_for_restart = adrv_wait_for_restart_temp;
		break;
	}

	// EEPROMにパラメーターを書き込む
	save_parameters();
	// パラメーターを表示する
	print_parameters();
}

//
// 100msごとに実行される処理
//
void process_on_param_mode()
{
	switch (param_mode_status) {
	case 0:
		// メニューを表示
		param_mode_initdisp_cnt--;
		if (param_mode_initdisp_cnt == 0) {
			// 個別パラメーター変更の場合
			if (param_mode_type == 0 || param_mode_type == 1) {
				// カウンターの値を初期化し、ステータスを進める
				if (param_mode_type == 0) {
					rsw_counter_value = starting_duty_fowd;
				} else {
					rsw_counter_value = starting_duty_back;
				}
				param_mode_status = 2;

			// 共通パラメーター変更の場合
			} else if (param_mode_type == 2) {
				// カウンターの値を初期化し、ステータスを進める
				rsw_counter_value = stopping_duty;
				param_mode_status = 5;

			// 自動運転用パラメーター変更の場合
			} else if (param_mode_type == 3) {
				// カウンターの値を初期化し、ステータスを進める
				rsw_counter_value = adrv_wait_for_reverse;
				param_mode_status = 9;
			}
		}
		break;
	// 個別パラメーター
	case 2:
	case 3:
	case 4:
	// 共通パラメーター
	case 5:
	case 6:
	case 7:
	case 8:
	// 自動運転用パラメーター
	case 9:
	case 10:
		// プロンプトを表示
		param_mode_promptdsp();
		break;
	case 99:
		// パラメーターを適用
		apply_parameter();
		// プログラムモード終了（手動モードに遷移）
		process_init();
		break;
	}
}
