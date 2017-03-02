#ifndef __EEPROM_H
#define __EEPROM_H

//
// EEPROM �ŊǗ�����p�����[�^�[�̕ϐ�
//   ����o���^�ō����^��~�O���s�^��~���̊e�f���[�e�B�[��
//   0 - 100 �̒l���Ƃ�܂�
//   ���]���A�t�]���͂��ꂼ��ʂ̃p�����[�^�[�ŊǗ����܂�.
//
//   ���]��
unsigned char starting_duty_fowd;
unsigned char topspeed_duty_fowd;
unsigned char reducing_duty_fowd;
//   �t�]��
unsigned char starting_duty_back;
unsigned char topspeed_duty_back;
unsigned char reducing_duty_back;
//   ��~��
unsigned char stopping_duty;
//   �������x
unsigned char accl_interval;
unsigned char decl_interval;
//   ���i����������s���܂ł̎���
unsigned char starting_interval;
//   �����^�]�^�C�v
unsigned char autodrive_type;
//   �����^�]���F�t�]�܂ł̃E�F�C�g
unsigned char adrv_wait_for_reverse;
//   �����^�]���F�ăX�^�[�g�܂ł̃E�F�C�g
unsigned char adrv_wait_for_restart;

//
// �֐�
//
void restore_parameters();
void print_parameters();
void save_parameters();

#endif // __EEPROM_H
