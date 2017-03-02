#include "main.h"
#include "device.h"
#include "lcd.h"
#include "rotenc.h"
#include "eeprom.h"
#include "process.h"
#include "pgmmode.h"
#include "adrvmode.h"
#include "parammode.h"
#include "manualmode.h"
#include "menumode.h"

//
// ���̃{�^���������̏���
//
static void process_on_BUTTON_0()
{
	switch (main_mode) {
	case 0:
		// �蓮���[�h�ŁA��~���̏ꍇ
		if (rsw_counter_value == 0) {
			// �����^�]���[�h�ɑJ��
			adrv_mode_init();
		}
		break;
	case 1:
		// �����^�]���[�h��
		// �X�e�[�^�X��ύX����
		adrv_mode_change_status();
		break;
	}
}

static void process_on_BUTTON_1()
{
	switch (main_mode) {
	case 0:
		// �蓮���[�h�ŁA��~���̏ꍇ
		if (rsw_counter_value == 0) {
			// �ݒ胁�j���[��\��
			menu_mode_init();
		}
		break;
	case 2:
		// �v���O�������[�h�Ń{�^��������
		// �p�����[�^�[��ݒ肷��
		pgm_mode_set_parameter();
		break;
	case 3:
		// �p�����[�^�[�ύX���[�h�Ń{�^��������
		// �p�����[�^�[��ݒ肷��
		param_mode_set_parameter();
		break;
	case 9:
		// ���j���[���[�h�Ń{�^��������
		// ���j���[���ڂ�I��
		menu_mode_select_menu();
		break;
	}
}

static void process_on_BUTTON_2()
{
	// �蓮���[�h�ɑJ��
	process_init();
}

static void process_on_BUTTON_3()
{
	// �蓮���[�h��
	if (main_mode == 0) {
		reverse_on_manual_mode();
		return;
	}
	// �����^�]���[�h��
	if (main_mode == 1) {
		reverse_on_adrv_mode();
	}
}

static void send_uart_command(unsigned char c)
{
	// �����R���f�o�C�X�̏ꍇ
	if (devide_type == DEVTYP_REMCON) {
		// �{�^���������ɁAUART�R�}���h�𑗐M����
		printf("/datasend/%c\r\n", c);
	}
}

static int process_on_button_press()
{
	int ret = 1;

	// �X�C�b�`On�ɑ΂��鏈�������s
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
// UART�����R���R�}���h��M���̏���
//
void process_on_uart_command(unsigned char c)
{
	switch (c) {
	case 'A':
		// �����^�]���s
		process_on_BUTTON_0();
		break;
	case 'M':
		// ���[�h�ύX���s
		process_on_BUTTON_1();
		break;
	case 'E':
		// �L�����Z�����s
		process_on_BUTTON_2();
		break;
	case 'C':
		// ���^�t�]�ؑ֎��s
		process_on_BUTTON_3();
		break;
	default:
		// ����
		break;
	}
}

//
// ���͂��ꂽ������𐔒l�ɕϊ�
//
static unsigned char atouc(char *value)
{
	char buff[8];
	int i, n;

	// ���͂��ꂽ���������Ɨ̈�ɃZ�b�g
	memset(buff, 0, sizeof(buff));
	for (i = 0; value[i] != 0; i++) {
		buff[i] = value[i];
	}

	// ���l�ϊ�
	n = atoi(buff);
	if (n < 0) {
		return 0;
	} else if (255 < n) {
		return 255;
	}
	return (unsigned char)n;
}
//
// UART�ɓ��͂��ꂽ���e����͂���
//
void parse_uart_input(unsigned char *rc_buff)
{
	unsigned char c;

	// UART���荞�݂��ݒ肳��Ă��Ȃ���ΐݒ�
	if (RCIE == 0) {
		RCIE = 1;
	}

	// UART�ɓ��͂��ꂽ�������Ȃ���ΏI��
	if (rs232c_flg == 0) {
		return;
	}

	// �t���O���N���A
	rs232c_flg = 0;

	// �擪�����p�啶���ȊO�̏ꍇ�̓X���[
	c = rc_buff[0];
	if (c < 64 || 90 < c) {
		return;
	}

	switch (c) {
	case 'D':
		// �f���[�e�B�[�����
		c = atouc(&rc_buff[1]);
		if (rsw_counter_max < c) {
			c = rsw_counter_max;
		}
		rsw_counter_value = c;
		break;
	case 'A':
		// �����^�]���s(Auto drive)
	case 'M':
		// ���[�h�ύX���s(Mode change)
	case 'E':
		// �L�����Z�����s(End)
	case 'C':
		// ���^�t�]�ؑ֎��s(direction Change)
		process_on_uart_command(c);
		break;
	default:
		// ����
		break;
	}
}

//
// �{�^���������m����
//
static unsigned long btn_push_prevent_cnt;

// �����݂��Ƃɏ����i3.2768 ms�j
void switch_prevent()
{
	// �J�E���^�[���O�̎��͏I��
	if (0 == btn_push_prevent_cnt) {
		return;
	}

	// �{�^���A�������}�~�J�E���^�[���X�V
	btn_push_prevent_cnt-- ;
}

// �C�x���g���[�v���̍Ō�̃X�e�b�v�ŏ���
void switch_detection()
{
	// �J�E���^�[���O�łȂ����͏I��
	if (0 < btn_push_prevent_cnt) {
		return;
	}

	// �X�C�b�`�������̏��������s
	if (process_on_button_press() != 0) {
		// �����}�~�J�E���^�[��ݒ�(��P�b�ɐݒ�)
		btn_push_prevent_cnt = 300;
	} else {
		btn_push_prevent_cnt = 0;
	}
}

//
// �Z���T�[���m����
//
unsigned char reducing_sensor_flg;
unsigned char stopping_sensor_flg;

static void process_on_REDUCING_SENSOR()
{
	// �����^�]���[�h
	if (main_mode == 1) {
		adrv_mode_sense_reducing();
	}
}

static void process_on_STOPPING_SENSOR()
{
	// �����^�]���[�h
	if (main_mode == 1) {
		adrv_mode_sense_stopping();
	}
}

// �����݂��Ƃɏ����i3.2768 ms�j
void sensor_detection()
{
	// ���C�A�E�g����f�o�C�X�ȊO�̏ꍇ�͏������Ȃ�
	if (devide_type != DEVTYP_LAYOUT) {
		return;
	}

	// �Z���T�[On�ɑ΂��鏈�������s
	if (REDUCING_SENSOR == 0) {
		reducing_sensor_flg++;
		// n��A�����Č��m�ɂ���������Z���T�[�𓥂񂾂Ɣ���
		if (5 == reducing_sensor_flg) {
			process_on_REDUCING_SENSOR();
		}

	} else if (STOPPING_SENSOR == 0) {
		stopping_sensor_flg++;
		// n��A�����Č��m�ɂ���������Z���T�[�𓥂񂾂Ɣ���
		if (5 == stopping_sensor_flg) {
			process_on_STOPPING_SENSOR();
		}

	} else {
		reducing_sensor_flg = 0;
		stopping_sensor_flg = 0;
	}
}

// �� 0.1 �b���Ƃɏ����i3.2768ms �~ 30��j
void process_on_100m_second()
{
	switch (main_mode) {
	case 0:
		// �蓮�^�]���[�h��
		process_on_manual_mode();
		break;
	case 1:
		// �����^�]���[�h��
		process_on_adrv_mode();
		break;
	case 2:
		// �v���O�������[�h��
		process_on_pgm_mode();
		break;
	case 3:
		// �p�����[�^�[�ύX���[�h��
		process_on_param_mode();
		break;
	case 9:
		// ���j���[���[�h��
		process_on_menu_mode();
		break;
	}
}

// �� 1.0 �b���Ƃɏ����i3.2768ms �~ 305��j
void process_on_one_second()
{
	switch (main_mode) {
	case 0:
		// �蓮�^�]���[�h��
		process_on_one_second_manual_mode();
		break;
	case 1:
		// �����^�]���[�h��
		process_on_one_second_adrv_mode();
		break;
	}
}

void process_init()
{
	// UART�������M�t���O��������
	rs232c_flg = 0;

	// ���[�J���ϐ��̏�����
	btn_push_prevent_cnt = 0;
	reducing_sensor_flg = 0;
	stopping_sensor_flg = 0;

	// ���[�h���蓮���[�h�ɐݒ�
	manual_mode_init();
}

// ���[�^���[�G���R�[�_�[�̌��ݒl�ύX���̏���
void process_on_change_rotenc_val()
{
	// �����R���f�o�C�X�̏ꍇ
	if (devide_type == DEVTYP_REMCON) {
		// �l���ύX���AUART�R�}���h�𑗐M����
		printf("/datasend/D%u\r\n", rsw_counter_value);
	}
}
