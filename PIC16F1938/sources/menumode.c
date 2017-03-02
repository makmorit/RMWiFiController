#include "main.h"
#include "lcd.h"
#include "rotenc.h"
#include "eeprom.h"
#include "process.h"
#include "parammode.h"
#include "pgmmode.h"

//
// ���j���[��\��
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
// �{�^���������̏���
//
void menu_mode_init()
{
	// ���j���[���[�h�֑J��
	main_mode = 9;
	menu_mode_status = 0;
	menu_mode_menu_number = 0;
	rsw_counter_value = 0;

	// ���j���[��\��
	menu_mode_headerdsp();
}

//
// �{�^���������̏���
//

static void apply_parameter()
{
	// �v�����v�g��\��
	sprintf(lcd_lower_buf, "now saving param");
	LCD_print_lower();

	// �p�����[�^�[��K�p
	autodrive_type = rsw_counter_value;

	// EEPROM�Ƀp�����[�^�[����������
	save_parameters();
	// �p�����[�^�[��\������
	print_parameters();
}

void menu_mode_select_menu()
{
	switch (menu_mode_status) {
	// �p�����[�^�[�ύX��ʂ̑I�����s
	case 0:
		switch (menu_mode_menu_number) {
		case 0:
		case 1:
			// �v���O�������[�h�i�O�i�p�E�t�]�p�j�ɑJ��
			pgm_mode_init(menu_mode_menu_number);
			break;

		case 2:
		case 3:
		case 4:
		case 5:
			// �p�����[�^�[�ύX���[�h�ɑJ��
			param_mode_init(menu_mode_menu_number - 2);
			break;

		// �����^�]�^�C�v�I����
		case 6:
			rsw_counter_value = autodrive_type;
			menu_mode_status = 1;
			break;
		}
		break;

	// �����^�]�^�C�v�I�����s
	case 1:
		// �p�����[�^�[��K�p
		apply_parameter();
		// �v���O�������[�h�I���i�蓮���[�h�ɑJ�ځj
		process_init();
		break;
	}
}

//
// 100ms���ƂɎ��s����鏈��
//
void process_on_menu_mode()
{
	unsigned char menu_max;

	switch (menu_mode_status) {
	// �p�����[�^�[�ύX��ʂ̑I��
	case 0:
		// ���[�^���[�G���R�[�_�[�̒l�Ń��j���[�ؑ�
		menu_max = 6;
		if (menu_max < rsw_counter_value) {
			rsw_counter_value = menu_max;
		}
		menu_mode_menu_number = rsw_counter_value;
		menu_mode_menudsp();
		break;

	// �����^�]�^�C�v�̑I��
	case 1:
		// ���[�^���[�G���R�[�_�[�̒l�Ń��j���[�ؑ�
		menu_max = 3;
		if (menu_max < rsw_counter_value) {
			rsw_counter_value = menu_max;
		}
		menu_mode_menu_number = rsw_counter_value;
		menu_mode_menudsp_autodrv();
		break;
	}
}
