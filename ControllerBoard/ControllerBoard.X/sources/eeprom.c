#include "common.h"
#include "eeprom.h"
#include "device.h"
#include "lcd.h"

//
// EEPROMデータと 初期値
//  8ビットずつ定義すること.
//
__EEPROM_DATA( 18,  24,  14, 255,  19,  25,  15, 255);
__EEPROM_DATA(  5,  12,   8,  30,   0,   5,  55, 255);
__EEPROM_DATA(255, 255, 255, 255, 255, 255, 255, 255);
__EEPROM_DATA(255, 255, 255, 255, 255, 255, 255, 255);

// パラメーターの書込位置
#define POS_stopping_duty          8
#define POS_accl_interval          9
#define POS_decl_interval         10
#define POS_starting_interval     11
#define POS_autodrive_type        12
#define POS_adrv_wait_for_reverse 13
#define POS_adrv_wait_for_restart 14

#define POS_starting_duty_fowd 0
#define POS_topspeed_duty_fowd 1
#define POS_reducing_duty_fowd 2
#define POS_starting_duty_back 4
#define POS_topspeed_duty_back 5
#define POS_reducing_duty_back 6

//
// アプリケーションで使用する変数のデフォルト値をロード
//
void restore_parameters()
{
	starting_duty_fowd = EEPROM_READ(POS_starting_duty_fowd);
	topspeed_duty_fowd = EEPROM_READ(POS_topspeed_duty_fowd);
	reducing_duty_fowd = EEPROM_READ(POS_reducing_duty_fowd);

	starting_duty_back = EEPROM_READ(POS_starting_duty_back);
	topspeed_duty_back = EEPROM_READ(POS_topspeed_duty_back);
	reducing_duty_back = EEPROM_READ(POS_reducing_duty_back);

	stopping_duty = EEPROM_READ(POS_stopping_duty);
	accl_interval = EEPROM_READ(POS_accl_interval);
	decl_interval = EEPROM_READ(POS_decl_interval);
	starting_interval = EEPROM_READ(POS_starting_interval);

	autodrive_type = EEPROM_READ(POS_autodrive_type);
	adrv_wait_for_reverse = EEPROM_READ(POS_adrv_wait_for_reverse);
	adrv_wait_for_restart = EEPROM_READ(POS_adrv_wait_for_restart);
}

void save_parameters()
{
	EEPROM_WRITE(POS_starting_duty_fowd, starting_duty_fowd);
	EEPROM_WRITE(POS_topspeed_duty_fowd, topspeed_duty_fowd);
	EEPROM_WRITE(POS_reducing_duty_fowd, reducing_duty_fowd);

	EEPROM_WRITE(POS_starting_duty_back, starting_duty_back);
	EEPROM_WRITE(POS_topspeed_duty_back, topspeed_duty_back);
	EEPROM_WRITE(POS_reducing_duty_back, reducing_duty_back);

	EEPROM_WRITE(POS_stopping_duty, stopping_duty);
	EEPROM_WRITE(POS_accl_interval, accl_interval);
	EEPROM_WRITE(POS_decl_interval, decl_interval);
	EEPROM_WRITE(POS_starting_interval, starting_interval);

	EEPROM_WRITE(POS_autodrive_type, autodrive_type);
	EEPROM_WRITE(POS_adrv_wait_for_reverse, adrv_wait_for_reverse);
	EEPROM_WRITE(POS_adrv_wait_for_restart, adrv_wait_for_restart);
}

void print_parameters()
{
	if (devide_type == DEVTYP_LAYOUT) {
		sprintf(lcd_upper_buf, "*LAYOUT CONTROL*");
		LCD_print_upper();
		__delay_ms(1000);
		__delay_ms(1000);
	}

	sprintf(lcd_upper_buf, "Fw:%3u->%3u->%3u", starting_duty_fowd, topspeed_duty_fowd, reducing_duty_fowd);
	sprintf(lcd_lower_buf, "Bk:%3u->%3u->%3u", starting_duty_back, topspeed_duty_back, reducing_duty_back);
	LCD_print_upper();
	LCD_print_lower();
	__delay_ms(1500);

	sprintf(lcd_upper_buf, "Stopping Duty:%2u", stopping_duty);
	sprintf(lcd_lower_buf, "Starting Intv:%2u", starting_interval);
	LCD_print_upper();
	LCD_print_lower();
	__delay_ms(1500);

	sprintf(lcd_upper_buf, "Accl Interval:%2u", accl_interval);
	sprintf(lcd_lower_buf, "Decl Interval:%2u", decl_interval);
	LCD_print_upper();
	LCD_print_lower();
	__delay_ms(1500);

	sprintf(lcd_upper_buf, "AdrvType:%1u Wait:", autodrive_type);
	sprintf(lcd_lower_buf, "Rev=%2u,Restr=%3u", adrv_wait_for_reverse, adrv_wait_for_restart);
	LCD_print_upper();
	LCD_print_lower();
	__delay_ms(1500);
}
