#include "main.h"
#include "device.h"
#include "lcd.h"
#include "rotenc.h"
#include "eeprom.h"
#include "process.h"

// �w���X�`�F�b�N�J�E���^�[
static unsigned char hcheck_cnt;

static void print_duty_and_hchk(unsigned char c, unsigned char t)
{
	// ���[�h���P�s�ڂɕ\������
	if (DIR_RELAY_SW == 0) {
		sprintf(lcd_upper_buf, "*Manual Mode*   ");
	} else {
		sprintf(lcd_upper_buf, "*Manual Mode* bk");
	}
	LCD_print_upper();

	// �f���[�e�B�[��ƃw���X�`�F�b�N�J�E���^�[��\��
	sprintf(lcd_lower_buf, "Duty[%3u]Cnt[%2u]", c, t);
	LCD_print_lower();
}

//
// �{�^���������̏���
//
void manual_mode_init()
{
	// ���[�h���蓮���[�h�ɐݒ�
	main_mode = 0;

	// �w���X�`�F�b�N�J�E���^�[
	hcheck_cnt = 0;

	// ���[�^���[�G���R�[�_�[�̃J�E���^�[
	rsw_counter_value = 0;

	// CCP�̒l���O�Ƃ���
	set_ccp(rsw_counter_value);
}

//
// �t�]�{�^��������
//
void reverse_on_manual_mode()
{
	// ��~Duty�����傫���ꍇ�͖���
	if (stopping_duty < rsw_counter_value) {
		return;
	}
	// �����[���]
	DIR_RELAY_SW = ~DIR_RELAY_SW;
}

//
// 100ms���ƂɎ��s����鏈��
//
void process_on_manual_mode()
{
	// ���[�^���[�G���R�[�_�[�̌��ݒl�����̂܂ܐݒ�
	set_ccp(rsw_counter_value);

	// ���݂�Duty�ƃw���X�`�F�b�N�J�E���^�[��\��
	print_duty_and_hchk(rsw_counter_value, hcheck_cnt);
}

//
// 1s���ƂɎ��s����鏈��
//
void process_on_one_second_manual_mode()
{
	// �w���X�`�F�b�N�J�E���^�[�𑝉�
	hcheck_cnt++;
	if (hcheck_cnt == 60) {
		hcheck_cnt = 0;
	}
}
