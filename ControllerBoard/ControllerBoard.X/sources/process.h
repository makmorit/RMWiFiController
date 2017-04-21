#ifndef __PROCESS_H
#define __PROCESS_H

// モード 
//   0:手動モード・・・ダイヤルを回し、デューティー比を手動変化させるモード
//   1:自動モード（未実装）・・・EEPROMにsaveした設定値をもとに、デューティー比が自動変化するモード
//   2:プログラムモード（未実装）・・・ダイヤルを回し、自動走行用パラメーターを設定するモード
unsigned char main_mode = 0;

//
// 関数
//
void switch_prevent();
void switch_detection();
void sensor_detection();
void parse_uart_input();
void process_on_uart_command(unsigned char c);
void process_on_100m_second();
void process_on_one_second();
void process_init();
void process_on_change_rotenc_val();

#endif // __PROCESS_H
