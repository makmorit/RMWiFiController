#include "main.h"
#include "device.h"
#include "lcd.h"
#include "rotenc.h"
#include "eeprom.h"
#include "process.h"

// �����^�]���[�h
//   0:None     ��ԏ��
//   1:starting ���Ԃ��珙�s��
//   2:topspeed �ō����ő��s��
//   3:reducing �ō������猸����
//   4:stopping ��ԏ��
static unsigned char adrv_mode_status;

// ���݃f���[�e�B�[��
static unsigned char curr_duty;

// ���s�p�����[�^�[
static unsigned char starting_duty;
static unsigned char topspeed_duty;
static unsigned char reducing_duty;

// �������x�p�����[�^�[
static unsigned char accl_interval_cnt;
static unsigned char decl_interval_cnt;

// �����J�n�܂ł̎���
static unsigned char starting_interval_cnt;

// �����^�]�I���܂ł̎���
static unsigned char adrv_end_interval_cnt;

static unsigned char reverse_wait_cnt;
static unsigned char restart_wait_cnt;

// �p�����[�^�[������
static void param_init_adrv_mode()
{
	// ��~��Duty��ݒ�
	curr_duty = stopping_duty;
	set_ccp(curr_duty);

	// �p�����[�^�[��K�p
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
// ���[�h�����\��
//
static void print_adrv_mode()
{
	// ���[�h���P�s�ڂɕ\������
	if (DIR_RELAY_SW == 0) {
		sprintf(lcd_upper_buf, "*AutoDrvMode*   ");
	} else {
		sprintf(lcd_upper_buf, "*AutoDrvMode* bk");
	}
	LCD_print_upper();
}

//
// ����Duty�\��
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
// ��~���Ɏ����^�]�{�^���������̏���
//
void adrv_mode_init()
{
	// �����^�]���[�h�֑J��
	main_mode = 1;
	adrv_mode_status = 0;

	// �p�����[�^�[���������A���[�h��\��
	param_init_adrv_mode();
	print_adrv_mode();

	// ����Duty��\��
	adrv_mode_print_duty();
}

//
// �����^�]���Ɏ����^�]�{�^���������̏���
//
void adrv_mode_change_status()
{
	if (adrv_mode_status == 0) {
		// ���i
		adrv_mode_status = 1;
	} else if (adrv_mode_status == 2) {
		// �����J�n
		decl_interval_cnt = 0;
		adrv_mode_status = 3;
	} else if (adrv_mode_status == 3) {
		// ��~
		adrv_mode_status = 4;
	}
}

//
// �����^�]���Ɍ����Z���T�[���m���̏���
//
void adrv_mode_sense_reducing()
{
	if (adrv_mode_status == 2) {
		// �����J�n
		decl_interval_cnt = 0;
		adrv_mode_status = 3;
	}
}

//
// �����^�]���ɒ�~�Z���T�[���m���̏���
//
void adrv_mode_sense_stopping()
{
	if (adrv_mode_status == 3) {
		// ��~
		adrv_mode_status = 4;
	}
}


//
// �t�]�����s���A�p�����[�^�[���ēK�p����
//
static void adrv_mode_do_reverse()
{
	// �����[���]
	DIR_RELAY_SW = ~DIR_RELAY_SW;

	// �p�����[�^�[���������A���[�h��\��
	param_init_adrv_mode();
	print_adrv_mode();
}

//
// 100ms���ƂɎ��s����鏈��
//
void process_on_adrv_mode()
{
	switch (adrv_mode_status) {
	case 1:
		if (curr_duty < starting_duty) {
			// ���s�f���[�e�B�[�܂ő���������
			curr_duty++;
			set_ccp(curr_duty);
			// ����Duty��\��
			adrv_mode_print_duty();
			if (curr_duty == starting_duty) {
				starting_interval_cnt = 0;
			}
		} else {
			// �����܂ő҂�
			starting_interval_cnt++;
			// �҂����ԂɒB������
			if (starting_interval_cnt == starting_interval) {
				// �����J�n
				accl_interval_cnt = 0;
				// �X�e�[�^�X���P�i�߂�
				adrv_mode_status++;
			}
		}
		break;
	case 2:
		// �ō����ɒB����܂ŏ������s��
		if (curr_duty < topspeed_duty) {
			// �C���^�[�o���J�E���^�[��i�߂�
			accl_interval_cnt++;
			// �J�E���^�[����܂ŗ�����
			if (accl_interval_cnt == accl_interval) {
				accl_interval_cnt = 0;
				// �ō���Duty�܂ő���������
				curr_duty++;
				set_ccp(curr_duty);
				// ����Duty��\��
				adrv_mode_print_duty();
			}
		}
		break;
	case 3:
		// ��~�O���sDuty�ɒB����܂ŏ������s��
		if (reducing_duty < curr_duty) {
			// �C���^�[�o���J�E���^�[��i�߂�
			decl_interval_cnt++;
			// �J�E���^�[����܂ŗ�����
			if (decl_interval_cnt == decl_interval) {
				decl_interval_cnt = 0;
				// ���sDuty�܂Ō���������
				curr_duty--;
				set_ccp(curr_duty);
				// ����Duty��\��
				adrv_mode_print_duty();
			}
		}
		break;
	case 4:
		if (stopping_duty < curr_duty) {
			// ��~��Duty�܂Ō���������
			curr_duty--;
			set_ccp(curr_duty);
			// ����Duty��\��
			adrv_mode_print_duty();
			// ��~��Duty�ɒB������X�e�[�^�X��i�߂�
			if (stopping_duty == curr_duty) {
				adrv_mode_status = 5;
			}
		}
		break;
	case 5:
		// �P�ꑖ�s���[�h�̏ꍇ
		if (autodrive_type == 0) {
			// 1�b�ق� wait ������A�^�]�I��
			adrv_end_interval_cnt = 10;
			adrv_mode_status = 99;

		// �����t�]���[�h�̏ꍇ
		} else if (autodrive_type == 1) {
			// �t�]���s�܂� wait
			reverse_wait_cnt = 0;
			adrv_mode_status = 6;

		// �ҋ@��ďo�����[�h�̏ꍇ
		} else if (autodrive_type == 2) {
			// ���i���s�܂� wait
			restart_wait_cnt = 0;
			adrv_mode_status = 7;

		// �A�����s���[�h�ł���΁A�ŏ��̃X�e�[�^�X�ɖ߂�
		} else {
			adrv_mode_status = 0;
		}
		break;
	case 6:
		// wait ������t�]�����s
		if (reverse_wait_cnt == adrv_wait_for_reverse) {
			adrv_mode_do_reverse();
			// ���i���s�܂� wait
			restart_wait_cnt = 0;
			adrv_mode_status = 7;
		}
		break;
	case 7:
		// �ҋ@���Ԃ�\��
		adrv_mode_print_duty();
		// wait ������ēx���i�����s
		if (restart_wait_cnt == adrv_wait_for_restart) {
			adrv_mode_status = 1;
		}
		break;
	case 99:
		// wait ������v���O�������[�h�I���i�蓮���[�h�ɑJ�ځj
		adrv_end_interval_cnt--;
		if (adrv_end_interval_cnt == 0) {
			process_init();
		}
		break;
	}
}

//
// ��~���ɋt�]�{�^���������̏���
//
void reverse_on_adrv_mode()
{
	// ��~���łȂ��Ƃ��͖���
	if (adrv_mode_status != 0) {
		return;
	}
	// �t�]�����s���A�p�����[�^�[���ēK�p����
	adrv_mode_do_reverse();
}

//
// 1s���ƂɎ��s����鏈��
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
