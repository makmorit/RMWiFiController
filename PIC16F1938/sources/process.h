#ifndef __PROCESS_H
#define __PROCESS_H

// ���[�h 
//   0:�蓮���[�h�E�E�E�_�C�������񂵁A�f���[�e�B�[����蓮�ω������郂�[�h
//   1:�������[�h�E�E�EEEPROM��save�����ݒ�l�����ƂɁA�f���[�e�B�[�䂪�����ω����郂�[�h
//   2:�v���O�������[�h�E�E�E�_�C�������񂵁A�������s�p�p�����[�^�[��ݒ肷�郂�[�h
unsigned char main_mode = 0;

// UART�������M�t���O�i1:��M�� 0:��M�܂��j
unsigned char rs232c_flg;

//
// �֐�
//
void switch_prevent();
void switch_detection();
void sensor_detection();
void parse_uart_input(unsigned char *rc_buff);
void process_on_uart_command(unsigned char c);
void process_on_100m_second();
void process_on_one_second();
void process_init();
void process_on_change_rotenc_val();

#endif // __PROCESS_H
