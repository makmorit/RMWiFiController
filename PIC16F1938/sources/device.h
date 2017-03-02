#ifndef __DEVICE_H
#define __DEVICE_H

// �f�o�C�X��ނ�ێ�
unsigned char devide_type;
#define DEVTYP_LAYOUT 1
#define DEVTYP_REMCON 2

//
// �f�o�C�X��ނ��ƂɈقȂ�ݒ�^��`���W��
//
// LCD�֘A��`
//   LCD ���g�p���邩�ǂ����̃t���O
#define LCD_NOT_USE  0		// 1:�g�p���܂���

//   LCD ���g�p����ꍇ�̐���p�s���A�T�C��
//     LCD ���g�p���Ȃ��ꍇ�A�Ӗ��������܂���
#define LCD_RS       RC0
#define LCD_EN       RC1

#define LCD_D4       RA5
#define LCD_D5       RA4
#define LCD_D6       RA3
#define LCD_D7       RA2

// UART �g�p���ݒ�
#define UART_NOT_USE  0		// 1:�g�p���܂���

// ���[�^���[�G���R�[�_�[�֘A��`
//   ���[�^���[�G���R�[�_�[���������ݒl���擾
//   A�[�q=RB7 B�[�q=RB6
//   (RB7/RB6�̒l���Q�r�b�g�̒l�ɕϊ�)
#define ROT_ENC_CURVAL ((~PORTB >> 6) & 0b00000011)
//
//   ���[�^���[�G���R�[�_�[�̃p���X���m�p
//   �s���A�T�C���ɉ����AUP/DN �̒l�����ւ��Ă�������
//
#define ROT_ENC_SIGNAL_HI 0b11
#define ROT_ENC_SIGNAL_LO 0b00
#define ROT_ENC_SIGNAL_UP 0b01
#define ROT_ENC_SIGNAL_DN 0b10

// �X�C�b�`����
//   �{�^���O
//     �����^�]�J�n�{�^��
//     �����^�]���ɉ��������ƁA�X�e�[�^�X�������ς��܂�
//   �{�^���P
//     �v���O�������[�h�����p�����[�^�[�������ݒ肵�܂�
//     �Ō�̃p�����[�^�[�ݒ莞�ɉ�������Ɛݒ�l��EEPROM��save����܂�
//   �{�^���Q
//     �v���O�������[�h�����L�����Z���{�^���Ƃ��ċ@�\
//     �L�����Z���{�^�������Ŏ蓮���[�h�ɖ߂�܂�
//   �{�^���R
//     ���]�E�t�]�ؑ֗p�̃{�^��
// �Z���T�[���́i���ނ�Low�ɂȂ�܂��j
//    �����Z���T�[
//    ��~�Z���T�[
//    (��)16F1829�ɂ́A�Z���T�[���͂͂���܂���
// �o��
//   ���]�X�C�b�`�i�����[�j�o��
#ifdef DEVICE_LAYOUT
	#define BUTTON_0    RE3
	#define BUTTON_1    RB5
	#define BUTTON_2    RB4
	#define BUTTON_3    RB3
	#define BUTTON_4    1 // dummy
	#define BUTTON_5    1 // dummy
	#define REDUCING_SENSOR RA1
	#define STOPPING_SENSOR RA0
	#define DIR_RELAY_SW RC5
#endif // DEVICE_LAYOUT
#ifdef DEVICE_REMCON
	#define BUTTON_0    RE3
	#define BUTTON_1    RB5
	#define BUTTON_2    RB4
	#define BUTTON_3    RB3
	#define BUTTON_4    RB2
	#define BUTTON_5    RB1
	#define REDUCING_SENSOR 1 // dummy
	#define STOPPING_SENSOR 1 // dummy
	#define DIR_RELAY_SW RC5
#endif // DEVICE_REMCON

// �֐�
void port_init();
void ccp_init();
void set_ccp(unsigned char v);
void uart_init();
void putch(unsigned char byte);
void init_device_mode();

#endif // __DEVICE_H
