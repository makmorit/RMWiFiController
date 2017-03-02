#include "main.h"
#include "lcd.h"
#include "rotenc.h"
#include "eeprom.h"
#include "process.h"
#include "parammode.h"
#include "pgmmode.h"

//
// メニューを表示
//
static unsigned char menu_mode_menu_number;
static unsigned char menu_mode_status;

static void menu_mode_headerdsp()
{
	sprintf(lcd_upper_buf, "**Select Menu** ");
	LCD_print_upper();
}

static void menu_mode_menudsp()
{
	switch (menu_mode_menu_number) {
	case 0:
		sprintf(lcd_lower_buf, " ProgramMode fwd");
		break;

	case 1:
		sprintf(lcd_lower_buf, " ProgramMode bak");
		break;

	case 2:
		sprintf(lcd_lower_buf, " ParamChange fwd");
		break;

	case 3:
		sprintf(lcd_lower_buf, " ParamChange bak");
		break;

	case 4:
		sprintf(lcd_lower_buf, " ParamChange cmn");
		break;

	case 5:
		sprintf(lcd_lower_buf, " ParamChange adr");
		break;

	case 6:
		sprintf(lcd_lower_buf, " Auto Drive Type");
		break;
	}
	LCD_print_lower();
}

static void menu_mode_menudsp_autodrv()
{
	sprintf(lcd_upper_buf,     "Select AutoDType");
	LCD_print_upper();

	switch (menu_mode_menu_number) {
	case 0:
		sprintf(lcd_lower_buf, "0:Run once      ");
		break;

	case 1:
		sprintf(lcd_lower_buf, "1:Auto reverse  ");
		break;

	case 2:
		sprintf(lcd_lower_buf, "2:Endless run   ");
		break;

	case 3:
		sprintf(lcd_lower_buf, "3:Restart by btn");
		break;
	}
	LCD_print_lower();
}

//
// ボタン押下時の処理
//
void menu_mode_init()
{
	// メニューモードへ遷移
	main_mode = 9;
	menu_mode_status = 0;
	menu_mode_menu_number = 0;
	rsw_counter_value = 0;

	// メニューを表示
	menu_mode_headerdsp();
}

//
// ボタン押下時の処理
//

static void apply_parameter()
{
	// プロンプトを表示
	sprintf(lcd_lower_buf, "now saving param");
	LCD_print_lower();

	// パラメーターを適用
	autodrive_type = rsw_counter_value;

	// EEPROMにパラメーターを書き込む
	save_parameters();
	// パラメーターを表示する
	print_parameters();
}

void menu_mode_select_menu()
{
	switch (menu_mode_status) {
	// パラメーター変更種別の選択実行
	case 0:
		switch (menu_mode_menu_number) {
		case 0:
		case 1:
			// プログラムモード（前進用・逆転用）に遷移
			pgm_mode_init(menu_mode_menu_number);
			break;

		case 2:
		case 3:
		case 4:
		case 5:
			// パラメーター変更モードに遷移
			param_mode_init(menu_mode_menu_number - 2);
			break;

		// 自動運転タイプ選択へ
		case 6:
			rsw_counter_value = autodrive_type;
			menu_mode_status = 1;
			break;
		}
		break;

	// 自動運転タイプ選択実行
	case 1:
		// パラメーターを適用
		apply_parameter();
		// プログラムモード終了（手動モードに遷移）
		process_init();
		break;
	}
}

//
// 100msごとに実行される処理
//
void process_on_menu_mode()
{
	unsigned char menu_max;

	switch (menu_mode_status) {
	// パラメーター変更種別の選択
	case 0:
		// ロータリーエンコーダーの値でメニュー切替
		menu_max = 6;
		if (menu_max < rsw_counter_value) {
			rsw_counter_value = menu_max;
		}
		menu_mode_menu_number = rsw_counter_value;
		menu_mode_menudsp();
		break;

	// 自動運転タイプの選択
	case 1:
		// ロータリーエンコーダーの値でメニュー切替
		menu_max = 3;
		if (menu_max < rsw_counter_value) {
			rsw_counter_value = menu_max;
		}
		menu_mode_menu_number = rsw_counter_value;
		menu_mode_menudsp_autodrv();
		break;
	}
}
