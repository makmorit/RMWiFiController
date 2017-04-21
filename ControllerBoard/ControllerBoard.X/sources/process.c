#include "common.h"
#include "device.h"
#include "lcd.h"
#include "rotenc.h"
#include "eeprom.h"
#include "process.h"
#include "manualmode.h"
//#include "menumode.h"
//#include "pgmmode.h"
//#include "adrvmode.h"
//#include "parammode.h"

//
// 基板上のボタン押下時の処理
//
static void process_on_BUTTON_0()
{
#if 0
	switch (main_mode) {
	case 0:
		// 手動モードで、停止中の場合
		if (rsw_counter_value == 0) {
			// 自動運転モードに遷移
			adrv_mode_init();
		}
		break;
	case 1:
		// 自動運転モード時
		// ステータスを変更する
		adrv_mode_change_status();
		break;
	}
#endif
}

static void process_on_BUTTON_1()
{
#if 0
	switch (main_mode) {
	case 0:
		// 手動モードで、停止中の場合
		if (rsw_counter_value == 0) {
			// 設定メニューを表示
			menu_mode_init();
		}
		break;
	case 2:
		// プログラムモードでボタン押下時
		// パラメーターを設定する
		pgm_mode_set_parameter();
		break;
	case 3:
		// パラメーター変更モードでボタン押下時
		// パラメーターを設定する
		param_mode_set_parameter();
		break;
	case 9:
		// メニューモードでボタン押下時
		// メニュー項目を選択
		menu_mode_select_menu();
		break;
	}
#endif
}

static void process_on_BUTTON_2()
{
	// 手動モードに遷移
	process_init();
}

static void process_on_BUTTON_3()
{
	// 手動モード時
	if (main_mode == 0) {
		reverse_on_manual_mode();
		return;
	}
#if 0
	// 自動運転モード時
	if (main_mode == 1) {
		reverse_on_adrv_mode();
	}
#endif
}

static void send_uart_command(unsigned char c)
{
	// リモコンデバイスの場合
	if (devide_type == DEVTYP_REMCON) {
		// ボタン押下時に、UARTコマンドを送信する
		printf("/datasend/%c\r\n", c);
	}
}

static int process_on_button_press()
{
	int ret = 1;

	// スイッチOnに対する処理を実行
	if (BUTTON_0 == 1) {
		process_on_BUTTON_0();
		send_uart_command('A');

	} else if (BUTTON_1 == 1) {
		process_on_BUTTON_1();
		send_uart_command('M');

	} else if (BUTTON_2 == 1) {
		process_on_BUTTON_2();
		send_uart_command('E');

	} else if (BUTTON_3 == 1) {
		process_on_BUTTON_3();
		send_uart_command('C');

	} else {
		ret = 0;
	}
	return ret;
}

//
// UARTリモコンコマンド受信時の処理
//
void process_on_uart_command(unsigned char c)
{
	switch (c) {
	case 'A':
		// 自動運転実行
		process_on_BUTTON_0();
		break;
	case 'M':
		// モード変更実行
		process_on_BUTTON_1();
		break;
	case 'E':
		// キャンセル実行
		process_on_BUTTON_2();
		break;
	case 'C':
		// 正／逆転切替実行
		process_on_BUTTON_3();
		break;
	default:
		// 無効
		break;
	}
}

//
// 入力された文字列を数値に変換
//
static unsigned char atouc(char *value)
{
	char buff[8];
	int i, n;

	// 入力された文字列を作業領域にセット
	memset(buff, 0, sizeof(buff));
	for (i = 0; value[i] != 0; i++) {
		buff[i] = value[i];
	}

	// 数値変換
	n = atoi(buff);
	if (n < 0) {
		return 0;
	} else if (255 < n) {
		return 255;
	}
	return (unsigned char)n;
}
//
// UARTに入力された内容を解析する
//
void parse_uart_input(unsigned char *rc_buff)
{
	unsigned char c;

	// UART割り込みが設定されていなければ設定
	if (RCIE == 0) {
		RCIE = 1;
	}

	// UARTに入力された文字がなければ終了
	if (rs232c_flg == 0) {
		return;
	}

	// フラグをクリア
	rs232c_flg = 0;

	// 先頭が半角大文字以外の場合はスルー
	c = rc_buff[0];
	if (c < 64 || 90 < c) {
		return;
	}

	switch (c) {
	case 'D':
		// デューティー比入力
		c = atouc(&rc_buff[1]);
		if (rsw_counter_max < c) {
			c = rsw_counter_max;
		}
		rsw_counter_value = c;
		break;
	case 'A':
		// 自動運転実行(Auto drive)
	case 'M':
		// モード変更実行(Mode change)
	case 'E':
		// キャンセル実行(End)
	case 'C':
		// 正／逆転切替実行(direction Change)
		process_on_uart_command(c);
		break;
	default:
		// 無効
		break;
	}
}

//
// ボタン押下検知処理
//
static unsigned long btn_push_prevent_cnt;

// 割込みごとに処理（3.2768 ms）
void switch_prevent()
{
	// カウンターが０の時は終了
	if (0 == btn_push_prevent_cnt) {
		return;
	}

	// ボタン連続押下抑止カウンターを更新
	btn_push_prevent_cnt-- ;
}

// イベントループ内の最後のステップで処理
void switch_detection()
{
	// カウンターが０でない時は終了
	if (0 < btn_push_prevent_cnt) {
		return;
	}

	// スイッチ押下時の処理を実行
	if (process_on_button_press() != 0) {
		// 押下抑止カウンターを設定(約１秒に設定)
		btn_push_prevent_cnt = 300;
	} else {
		btn_push_prevent_cnt = 0;
	}
}

//
// センサー検知処理
//
unsigned char reducing_sensor_flg;
unsigned char stopping_sensor_flg;

static void process_on_REDUCING_SENSOR()
{
#if 0
	// 自動運転モード
	if (main_mode == 1) {
		adrv_mode_sense_reducing();
	}
#endif
}

static void process_on_STOPPING_SENSOR()
{
#if 0
	// 自動運転モード
	if (main_mode == 1) {
		adrv_mode_sense_stopping();
	}
#endif
}

// 割込みごとに処理（3.2768 ms）
void sensor_detection()
{
	// レイアウト制御デバイス以外の場合は処理しない
	if (devide_type != DEVTYP_LAYOUT) {
		return;
	}

	// センサーOnに対する処理を実行
	if (REDUCING_SENSOR == 0) {
		reducing_sensor_flg++;
		// n回連続して検知にかかったらセンサーを踏んだと判定
		if (5 == reducing_sensor_flg) {
			process_on_REDUCING_SENSOR();
		}

	} else if (STOPPING_SENSOR == 0) {
		stopping_sensor_flg++;
		// n回連続して検知にかかったらセンサーを踏んだと判定
		if (5 == stopping_sensor_flg) {
			process_on_STOPPING_SENSOR();
		}

	} else {
		reducing_sensor_flg = 0;
		stopping_sensor_flg = 0;
	}
}

// 約 0.1 秒ごとに処理（3.2768ms × 30回）
void process_on_100m_second()
{
	switch (main_mode) {
	case 0:
		// 手動運転モード時
		process_on_manual_mode();
		break;
#if 0
    case 1:
		// 自動運転モード時
		process_on_adrv_mode();
		break;
	case 2:
		// プログラムモード時
		process_on_pgm_mode();
		break;
	case 3:
		// パラメーター変更モード時
		process_on_param_mode();
		break;
	case 9:
		// メニューモード時
		process_on_menu_mode();
		break;
#endif
	}
}

// 約 1.0 秒ごとに処理（3.2768ms × 305回）
void process_on_one_second()
{
	switch (main_mode) {
	case 0:
		// 手動運転モード時
		process_on_one_second_manual_mode();
		break;
	case 1:
#if 0
		// 自動運転モード時
		process_on_one_second_adrv_mode();
#endif
		break;
	}
}

void process_init()
{
	// UART文字列受信フラグを初期化
	rs232c_flg = 0;

	// ローカル変数の初期化
	btn_push_prevent_cnt = 0;
	reducing_sensor_flg = 0;
	stopping_sensor_flg = 0;

	// モードを手動モードに設定
	manual_mode_init();
}

// ロータリーエンコーダーの現在値変更時の処理
void process_on_change_rotenc_val()
{
	// リモコンデバイスの場合
	if (devide_type == DEVTYP_REMCON) {
		// 値が変更時、UARTコマンドを送信する
		printf("/datasend/D%u\r\n", rsw_counter_value);
	}
}
