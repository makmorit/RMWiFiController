#include "main.h"
#include "device.h"

//
// �s���Ȃǂ̐ݒ���s��
//
void port_init()
{
	// Port A
	//   �A�i���O�͎g�p���Ȃ��i���ׂăf�W�^��I/O�Ɋ����Ă�j
	//   RA1,RA0(SENSOR)�͓���(�Z���T�[�𓥂ނ�Low�ɂȂ�܂�)
	//          76543210
	ANSELA  = 0b00000000;
	TRISA   = 0b00000011;
	PORTA   = 0b00000000;

	// Port B
	//   �A�i���O�͎g�p���Ȃ��i���ׂăf�W�^��I/O�Ɋ����Ă�j
	//   RB7,RB6(ROT_ENC)�͓���(10k pull up)
	//   RB5,RB4,RB3(SW)�͓���(10k pull down)
	//          76543210
	ANSELB  = 0b00000000;
	TRISB   = 0b11111000;
	PORTB   = 0b00000000;

	// Port C
	//   RC7(RX)�͓���
	//          76543210
	TRISC   = 0b10000000;
	PORTC   = 0b00000000;

	// Port E
	//   RE3(SW)�͓���(10k pull down)
	//          76543210
	TRISE   = 0b00001000;
	PORTE   = 0b00000000;
}

void ccp_init() 
{
	//
	// �^�C�}�[�Q�̐ݒ���s��
	//   PWM���� 0.05uS * (160+1) * 4 = 32.2us
	//   ���g��  31kHz
	//
	CCP1CON = 0b1100; // PWM
	CCPR1L  = 0;      // CCP1 �f���[�e�B�ݒ�
	PR2     = 160;    // �^�C�}�[�Q�������W�X�^�[�ɏ����l���Z�b�g
}

void set_ccp(unsigned char v) 
{
	// CCP �ɒl�i�f���[�e�B�[��j��ݒ�
	//   v = 0-100 �̒l
	CCPR1L = PR2 * v / 100;
}

//
// UART�̏����ݒ���s��
//
void uart_init()
{
	if (UART_NOT_USE) {
		return;
	}

	// ���M���ݒ�F�񓯊����[�h(SYNC=0),8bit(TX9=0),�m���p���e�B(TX9D=0)
	TXSTA = 0b00100100;
	// ��M���ݒ�
	RCSTA = 0b10010000;

	// 9600 �̏ꍇ�iFosc=8MHz�j
	//   SYNC = 0, BRGH = 1, BRG16 = 0
	//   SPBRG = 51 (8,000,000/(16 * 9,600) - 1)
	// 19200 �̏ꍇ�iFosc=20MHz�j
	//   SYNC = 0, BRGH = 0, BRG16 = 1
	//   SPBRG = 64 (20,000,000/(16 * 19,200) - 1)
	BRGH  = 0;
	BRG16 = 1;
	SPBRG = 64;

	// �t�r�`�q�s�����ݎ�M�t���O�̏�����
	RCIF  = 0;
}

//
// printf ���Ŏg�p���� putch �֐�
// (UART TX �ɂP�o�C�g�o�͂��܂�)
//
void putch(unsigned char byte)
{
	while (!TXIF) {
		continue;
	}
	TXREG = byte;
}

//
// ���샂�[�h��ݒ�
//
void init_device_mode()
{
	devide_type = 0;
	devide_type = DEVTYP_LAYOUT;
}
