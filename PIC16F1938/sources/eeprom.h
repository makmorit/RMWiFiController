#ifndef __EEPROM_H
#define __EEPROM_H

//
// EEPROM で管理するパラメーターの変数
//   走り出し／最高速／停止前徐行／停止時の各デューティー比
//   0 - 100 の値をとります
//   正転時、逆転時はそれぞれ別のパラメーターで管理します.
//
//   正転時
unsigned char starting_duty_fowd;
unsigned char topspeed_duty_fowd;
unsigned char reducing_duty_fowd;
//   逆転時
unsigned char starting_duty_back;
unsigned char topspeed_duty_back;
unsigned char reducing_duty_back;
//   停止時
unsigned char stopping_duty;
//   加減速度
unsigned char accl_interval;
unsigned char decl_interval;
//   発進から加速を行うまでの時間
unsigned char starting_interval;
//   自動運転タイプ
unsigned char autodrive_type;
//   自動運転時：逆転までのウェイト
unsigned char adrv_wait_for_reverse;
//   自動運転時：再スタートまでのウェイト
unsigned char adrv_wait_for_restart;

//
// 関数
//
void restore_parameters();
void print_parameters();
void save_parameters();

#endif // __EEPROM_H
