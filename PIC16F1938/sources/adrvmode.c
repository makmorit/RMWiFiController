#include "main.h"
#include "device.h"
#include "lcd.h"
#include "rotenc.h"
#include "eeprom.h"
#include "process.h"

// 自動運転モード
//   0:None     停車状態
//   1:starting 発車から徐行中
//   2:topspeed 最高速で走行中
//   3:reducing 最高速から減速中
//   4:stopping 停車状態
static unsigned char adrv_mode_status;

// 現在デューティー比
static unsigned char curr_duty;

// 走行パラメーター
static unsigned char starting_duty;
static unsigned char topspeed_duty;
static unsigned char reducing_duty;

// 加減速度パラメーター
static unsigned char accl_interval_cnt;
static unsigned char decl_interval_cnt;

// 加減開始までの時間
static unsigned char starting_interval_cnt;

// 自動運転終了までの時間
static unsigned char adrv_end_interval_cnt;

static unsigned char reverse_wait_cnt;
static unsigned char restart_wait_cnt;

// パラメーター初期化
static void param_init_adrv_mode()
{
	// 停止時Dutyを設定
	curr_duty = stopping_duty;
	set_ccp(curr_duty);

	// パラメーターを適用
	if (DIR_RELAY_SW == 0) {
		starting_duty = starting_duty_fowd;
		topspeed_duty = topspeed_duty_fowd;
		reducing_duty = reducing_duty_fowd;
	} else {
		starting_duty = starting_duty_back;
		topspeed_duty = topspeed_duty_back;
		reducing_duty = reducing_duty_back;
	}
}
//
// モード初期表示
//
static void print_adrv_mode()
{
	// モードを１行目に表示する
	if (DIR_RELAY_SW == 0) {
		sprintf(lcd_upper_buf, "*AutoDrvMode*   ");
	} else {
		sprintf(lcd_upper_buf, "*AutoDrvMode* bk");
	}
	LCD_print_upper();
}

//
// 現在Duty表示
//
static void adrv_mode_print_duty()
{
	switch (adrv_mode_status) {
	case 1:
		sprintf(lcd_lower_buf, "startingDuty=%3u", curr_duty);
		break;
	case 2:
		if (curr_duty == topspeed_duty) {
			sprintf(lcd_lower_buf, "topspeedDuty=%3u", curr_duty);
		} else {
			sprintf(lcd_lower_buf, "accelingDuty=%3u", curr_duty);
		}
		break;
	case 3:
		if (curr_duty == reducing_duty) {
			sprintf(lcd_lower_buf, "stoppingDuty=%3u", curr_duty);
		} else {
			sprintf(lcd_lower_buf, "reducingDuty=%3u", curr_duty);
		}
		break;
	case 0:
	case 4:
		sprintf(lcd_lower_buf, "stoppingDuty=%3u", curr_duty);
		break;
	case 7:
		sprintf(lcd_lower_buf, "Restart : %3usec", adrv_wait_for_restart - restart_wait_cnt);
		break;
	}
	LCD_print_lower();
}

//
// 停止中に自動運転ボタン押下時の処理
//
void adrv_mode_init()
{
	// 自動運転モードへ遷移
	main_mode = 1;
	adrv_mode_status = 0;

	// パラメーター初期化し、モードを表示
	param_init_adrv_mode();
	print_adrv_mode();

	// 現在Dutyを表示
	adrv_mode_print_duty();
}

//
// 自動運転中に自動運転ボタン押下時の処理
//
void adrv_mode_change_status()
{
	if (adrv_mode_status == 0) {
		// 発進
		adrv_mode_status = 1;
	} else if (adrv_mode_status == 2) {
		// 減速開始
		decl_interval_cnt = 0;
		adrv_mode_status = 3;
	} else if (adrv_mode_status == 3) {
		// 停止
		adrv_mode_status = 4;
	}
}

//
// 自動運転中に減速センサー検知時の処理
//
void adrv_mode_sense_reducing()
{
	if (adrv_mode_status == 2) {
		// 減速開始
		decl_interval_cnt = 0;
		adrv_mode_status = 3;
	}
}

//
// 自動運転中に停止センサー検知時の処理
//
void adrv_mode_sense_stopping()
{
	if (adrv_mode_status == 3) {
		// 停止
		adrv_mode_status = 4;
	}
}


//
// 逆転を実行し、パラメーターを再適用する
//
static void adrv_mode_do_reverse()
{
	// リレー反転
	DIR_RELAY_SW = ~DIR_RELAY_SW;

	// パラメーター初期化し、モードを表示
	param_init_adrv_mode();
	print_adrv_mode();
}

//
// 100msごとに実行される処理
//
void process_on_adrv_mode()
{
	switch (adrv_mode_status) {
	case 1:
		if (curr_duty < starting_duty) {
			// 徐行デューティーまで増加させる
			curr_duty++;
			set_ccp(curr_duty);
			// 現在Dutyを表示
			adrv_mode_print_duty();
			if (curr_duty == starting_duty) {
				starting_interval_cnt = 0;
			}
		} else {
			// 加速まで待つ
			starting_interval_cnt++;
			// 待ち時間に達したら
			if (starting_interval_cnt == starting_interval) {
				// 加速開始
				accl_interval_cnt = 0;
				// ステータスを１個進める
				adrv_mode_status++;
			}
		}
		break;
	case 2:
		// 最高速に達するまで処理を行う
		if (curr_duty < topspeed_duty) {
			// インターバルカウンターを進める
			accl_interval_cnt++;
			// カウンター上限まで来たら
			if (accl_interval_cnt == accl_interval) {
				accl_interval_cnt = 0;
				// 最高速Dutyまで増加させる
				curr_duty++;
				set_ccp(curr_duty);
				// 現在Dutyを表示
				adrv_mode_print_duty();
			}
		}
		break;
	case 3:
		// 停止前徐行Dutyに達するまで処理を行う
		if (reducing_duty < curr_duty) {
			// インターバルカウンターを進める
			decl_interval_cnt++;
			// カウンター上限まで来たら
			if (decl_interval_cnt == decl_interval) {
				decl_interval_cnt = 0;
				// 徐行Dutyまで減少させる
				curr_duty--;
				set_ccp(curr_duty);
				// 現在Dutyを表示
				adrv_mode_print_duty();
			}
		}
		break;
	case 4:
		if (stopping_duty < curr_duty) {
			// 停止中Dutyまで減少させる
			curr_duty--;
			set_ccp(curr_duty);
			// 現在Dutyを表示
			adrv_mode_print_duty();
			// 停止中Dutyに達したらステータスを進める
			if (stopping_duty == curr_duty) {
				adrv_mode_status = 5;
			}
		}
		break;
	case 5:
		// 単一走行モードの場合
		if (autodrive_type == 0) {
			// 1秒ほど wait した後、運転終了
			adrv_end_interval_cnt = 10;
			adrv_mode_status = 99;

		// 自動逆転モードの場合
		} else if (autodrive_type == 1) {
			// 逆転実行まで wait
			reverse_wait_cnt = 0;
			adrv_mode_status = 6;

		// 待機後再出発モードの場合
		} else if (autodrive_type == 2) {
			// 発進実行まで wait
			restart_wait_cnt = 0;
			adrv_mode_status = 7;

		// 連続走行モードであれば、最初のステータスに戻す
		} else {
			adrv_mode_status = 0;
		}
		break;
	case 6:
		// wait したら逆転を実行
		if (reverse_wait_cnt == adrv_wait_for_reverse) {
			adrv_mode_do_reverse();
			// 発進実行まで wait
			restart_wait_cnt = 0;
			adrv_mode_status = 7;
		}
		break;
	case 7:
		// 待機時間を表示
		adrv_mode_print_duty();
		// wait したら再度発進を実行
		if (restart_wait_cnt == adrv_wait_for_restart) {
			adrv_mode_status = 1;
		}
		break;
	case 99:
		// wait したらプログラムモード終了（手動モードに遷移）
		adrv_end_interval_cnt--;
		if (adrv_end_interval_cnt == 0) {
			process_init();
		}
		break;
	}
}

//
// 停止中に逆転ボタン押下時の処理
//
void reverse_on_adrv_mode()
{
	// 停止中でないときは無視
	if (adrv_mode_status != 0) {
		return;
	}
	// 逆転を実行し、パラメーターを再適用する
	adrv_mode_do_reverse();
}

//
// 1sごとに実行される処理
//
void process_on_one_second_adrv_mode()
{
	switch (adrv_mode_status) {
	case 6:
		reverse_wait_cnt++;
		break;
	case 7:
		restart_wait_cnt++;
		break;
	}
}
