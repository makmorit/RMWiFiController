#ifndef __ROTENC_H
#define __ROTENC_H

//
// ���[�^���[�G���R�[�_�[�Ŏg�p����ϐ�
//
unsigned char rsw_counter_value = 0;   // �����l
unsigned char rsw_counter_min   = 0;   // �ŏ��l
unsigned char rsw_counter_max   = 100; // �ő�l
unsigned char rsw_status_value  = 0;   // �X�e�[�^�X
unsigned char rsw_curr_value;          // ���͂��ꂽ�l��ێ�

// �֐�
unsigned char rotenc_detection();

#endif // __ROTENC_H
