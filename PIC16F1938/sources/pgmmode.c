#include "main.h"
#include "device.h"
#include "lcd.h"
#include "rotenc.h"
#include "eeprom.h"
#include "process.h"

// �v���O�������[�h
//   0:�p�����[�^�[�\����
//   2:stopping_duty���͒�
//   3:starting_duty���͒�
//   4:topspeed_duty���͒�
//   5:reducing_duty���͒�
static unsigned char pgm_mode_status = 0;
static unsigned char pgm_mode_initdisp_cnt = 0;

// ���͒��̃p�����[�^�[��ޔ�
static unsigned char starting_duty_temp;
static unsigned char topspeed_duty_temp;
static unsigned char reducing_duty_temp;
static unsigned char stopping_duty_temp;

//
// �ύX�O�̃p�����[�^�[��\��
//
static void pgm_mode_initdsp()
{
	// ���]��
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
// �{�^���������̏���
//
void pgm_mode_init(int dir_relay_sw)
{
	// �v���O�������[�h�֑J��
	main_mode = 2;
	pgm_mode_status = 0;
	rsw_counter_value = 0;

	// �O�i�E��ނ̕ʂ�ݒ�
	DIR_RELAY_SW = dir_relay_sw;

	// �Q�b�قǁA�p�����[�^�[��\��
	pgm_mode_initdisp_cnt = 20; 
	pgm_mode_initdsp();
}

void pgm_mode_set_parameter()
{
	// �p�����[�^�[�����[�^���[�G���R�[�_�[�̌��ݒl�Őݒ�
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
		// Duty���~��ԂɕύX
		set_ccp(0);
	}
}
//
// 100ms���ƂɎ��s����鏈��
//
void process_on_pgm_mode()
{
	if (pgm_mode_status == 0) {
		// �p�����[�^�[�l��\��
		pgm_mode_initdisp_cnt--;
		if (pgm_mode_initdisp_cnt == 0) {
			// ���[�^���[�G���R�[�_�[
			rsw_counter_value = stopping_duty;
			// �X�e�[�^�X��i�߂�
			pgm_mode_status = 2;
		}

	} else if (pgm_mode_status == 2) {
		// �v�����v�g��\��
		sprintf(lcd_lower_buf, "stoppingDuty=%3u", rsw_counter_value);
		LCD_print_lower();
		set_ccp(rsw_counter_value);

	} else if (pgm_mode_status == 3) {
		// �v�����v�g��\��
		sprintf(lcd_lower_buf, "startingDuty=%3u", rsw_counter_value);
		LCD_print_lower();
		set_ccp(rsw_counter_value);

	} else if (pgm_mode_status == 4) {
		// �v�����v�g��\��
		sprintf(lcd_lower_buf, "topspeedDuty=%3u", rsw_counter_value);
		LCD_print_lower();
		set_ccp(rsw_counter_value);

	} else if (pgm_mode_status == 5) {
		// �v�����v�g��\��
		sprintf(lcd_lower_buf, "reducingDuty=%3u", rsw_counter_value);
		LCD_print_lower();
		set_ccp(rsw_counter_value);

	} else if (pgm_mode_status == 6) {
		// �v�����v�g��\��
		sprintf(lcd_lower_buf, "now saving param");
		LCD_print_lower();
		// �p�����[�^�[��K�p
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
		// EEPROM�Ƀp�����[�^�[����������
		save_parameters();
		// �p�����[�^�[��\������
		print_parameters();
		// �v���O�������[�h�I���i�蓮���[�h�ɑJ�ځj
		process_init();
	}
}
