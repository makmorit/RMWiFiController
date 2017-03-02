#include "main.h"
#include "lcd.h"
#include "rotenc.h"
#include "eeprom.h"
#include "process.h"

// �p�����[�^�[�ύX���[�h
//   0:�p�����[�^�[�\����
//   2:accl_interval���͒�
//   3:decl_interval���͒�
//   4:starting_interval���͒�
static unsigned char param_mode_status;
static unsigned char param_mode_initdisp_cnt;

// �p�����[�^�[�ύX���
//   0:�ʃp�����[�^�[�i�O�i�p�j
//   1:�ʃp�����[�^�[�i�t�]�p�j
//   2:���ʃp�����[�^�[
//   3:�����^�]�p�p�����[�^�[
static unsigned char param_mode_type;

// ���͒��̃p�����[�^�[��ޔ�
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
// �\���p
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
// �{�^���������̏���
//
void param_mode_init(unsigned char type)
{
	// �p�����[�^�[�ύX���[�h�֑J��
	main_mode = 3;
	param_mode_status = 0;
	rsw_counter_value = 0;

	// �ύX��ʂ��Z�b�g
	param_mode_type = type;

	// ���j���[��\��
	param_mode_initdisp_cnt = 20;
	param_mode_initdsp();
}

void param_mode_set_parameter()
{
	switch (param_mode_status) {
	// �p�����[�^�[�����[�^���[�G���R�[�_�[�̌��ݒl�Őݒ�
	//   �O�i�E�t�]�p�ʃp�����[�^�[
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
	//   ���ʃp�����[�^�[
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
	//   �����^�]�p�p�����[�^�[
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
	// �v�����v�g��\��
	sprintf(lcd_lower_buf, "now saving param");
	LCD_print_lower();

	// �p�����[�^�[��K�p
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

	// EEPROM�Ƀp�����[�^�[����������
	save_parameters();
	// �p�����[�^�[��\������
	print_parameters();
}

//
// 100ms���ƂɎ��s����鏈��
//
void process_on_param_mode()
{
	switch (param_mode_status) {
	case 0:
		// ���j���[��\��
		param_mode_initdisp_cnt--;
		if (param_mode_initdisp_cnt == 0) {
			// �ʃp�����[�^�[�ύX�̏ꍇ
			if (param_mode_type == 0 || param_mode_type == 1) {
				// �J�E���^�[�̒l�����������A�X�e�[�^�X��i�߂�
				if (param_mode_type == 0) {
					rsw_counter_value = starting_duty_fowd;
				} else {
					rsw_counter_value = starting_duty_back;
				}
				param_mode_status = 2;

			// ���ʃp�����[�^�[�ύX�̏ꍇ
			} else if (param_mode_type == 2) {
				// �J�E���^�[�̒l�����������A�X�e�[�^�X��i�߂�
				rsw_counter_value = stopping_duty;
				param_mode_status = 5;

			// �����^�]�p�p�����[�^�[�ύX�̏ꍇ
			} else if (param_mode_type == 3) {
				// �J�E���^�[�̒l�����������A�X�e�[�^�X��i�߂�
				rsw_counter_value = adrv_wait_for_reverse;
				param_mode_status = 9;
			}
		}
		break;
	// �ʃp�����[�^�[
	case 2:
	case 3:
	case 4:
	// ���ʃp�����[�^�[
	case 5:
	case 6:
	case 7:
	case 8:
	// �����^�]�p�p�����[�^�[
	case 9:
	case 10:
		// �v�����v�g��\��
		param_mode_promptdsp();
		break;
	case 99:
		// �p�����[�^�[��K�p
		apply_parameter();
		// �v���O�������[�h�I���i�蓮���[�h�ɑJ�ځj
		process_init();
		break;
	}
}
