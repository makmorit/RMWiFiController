#include "main.h"
#include "device.h"
#include "lcd.h"
#include "rotenc.h"
#include "eeprom.h"
#include "process.h"

__CONFIG(CLKOUTEN_OFF & FOSC_HS & FCMEN_OFF & IESO_OFF & BOREN_ON & PWRTE_ON & WDTE_OFF & MCLRE_OFF & CP_OFF & CPD_OFF);
__CONFIG(PLLEN_OFF & STVREN_ON & WRT_OFF & LVP_OFF);

//
// �^�C�}�[�Ŏg�p����ϐ�
//
static unsigned long total_tmr0_cnt_100m;
static unsigned long total_tmr0_cnt_1s;
static unsigned char tmr0_toggle;

//
// ����������
//
static void initialize()
{
	// �s���Ȃǂ̏����ݒ���s��
	port_init();

	//
	// �^�C�}�[�O�̐ݒ���s��
	//
	// TMR0ON: Enables Timer0
	// T08BIT: Timer0 is configured as an 8-bit timer/counter
	// �v���X�P�[���[:64  �| �P�J�E���g12.8�ʕb(=1/20MHz*4*64)
	// OPTION_REG �̐擪�r�b�g:!WPUEN (1:�����v���A�b�v��)
	OPTION_REG = 0b10000101;
	// 256�J�E���g�i3.2768 ms�j�Ŋ����ݔ���������
	TMR0 = 0;
	// TMR0���荞�݋���
	TMR0IE = 1;

	// UART ������
	uart_init();

	// CCP1 ������
	ccp_init();

	// �S�����ݏ�����������
	PEIE = 1;
	GIE  = 1;

	// TIMER2 on prescale=1
	// TIMER2�X�^�[�g�ion�r�b�g���P�j
	T2CON = 0b100;
}

//
// �����ݏ���
//
unsigned char rc_char;
unsigned char rc_buff[64];
unsigned char rc_cnt = 0;
static void interrupt intr(void)
{
	// RS232C��M�����݂̏ꍇ
	if (RCIF == 1) {
		rc_char = RCREG;
		// ���s���������͂��ꂽ��
		if (rc_char == '\n' || rc_char == '\r') {
			if (rc_cnt < 1) {
				return;
			}
			rc_buff[rc_cnt] = 0;
			rs232c_flg = 1;
			rc_cnt = 0;
		} else {
			rc_buff[rc_cnt++] = rc_char;
		}
	}
	// �^�C�}�[�O�����݁i1�~���b���Ɓj�̏ꍇ
	if (TMR0IF == 1) {
		// �����݃J�E���^�[
		total_tmr0_cnt_100m++;
		total_tmr0_cnt_1s++;
		tmr0_toggle = 1;
		// 256�J�E���g�i3.2768 ms�j�Ŋ����ݔ���������
		TMR0 = 0;
		// TMR0���荞�݃N���A
		TMR0IF = 0;
		// ���[�^���[�X�C�b�`�ɂ��J�E���^�[����
		rsw_curr_value = ROT_ENC_CURVAL;
	}
}

//
// �C�x���g����
//
static void do_events()
{
	//
	// �����݂��Ƃɏ����i3.2768 ms�j
	if (tmr0_toggle == 1) {
		// ���[�^���[�G���R�[�_�[�ɂ�錟�m����
		if (rotenc_detection() != 0) {
			process_on_change_rotenc_val();
		}
		switch_prevent();
		tmr0_toggle = 0;
		// �Z���T�[���m����
		sensor_detection();
	}

	//
	// �� 0.1 �b���Ƃɏ����i3.2768ms �~ 30��j
	//
	if (total_tmr0_cnt_100m > 30) {
		// �J�E���^�[��������
		total_tmr0_cnt_100m = 0;
		// �C�x���g���Ƃ̏����i���[�h�^�X�e�[�^�X�J�ځj���s��
		process_on_100m_second();
	}

	//
	// �� 1.0 �b���Ƃɏ����i3.2768ms �~ 305��j
	//
	if (total_tmr0_cnt_1s > 305) {
		// �J�E���^�[��������
		total_tmr0_cnt_1s = 0;
		// �C�x���g���Ƃ̏������s��
		process_on_one_second();
	}

	// �X�C�b�`���m����
	switch_detection();
}

//
// ���C�����[�`��
//
void main() 
{
	// �p�����[�^�[��EEPROM����Ǎ�
	restore_parameters();

	// �s����@�\���̏���������
	initialize();

	// do_events �����񐔃J�E���^�[
	//   �������_�ł̊����݃J�E���^�[
	total_tmr0_cnt_100m = 0;
	total_tmr0_cnt_1s = 0;

	// LCD ����������
	LCD_init();

	// ���샂�[�h��\��
	init_device_mode();

	// �p�����[�^�[�����\��
	print_parameters();

	// �蓮���[�h�ŏ�����
	process_init();

	while (1) {
		// UART����
		parse_uart_input(rc_buff);
		// �C�x���g����
		do_events();
	}
}
