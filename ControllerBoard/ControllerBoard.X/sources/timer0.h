#ifndef __TIMER0_H
#define __TIMER0_H

//
// タイマー0で使用する変数
//
unsigned long tmr0_total_cnt_100m;
unsigned long tmr0_total_cnt_1s;
unsigned char tmr0_toggle;

// 関数
void timer0_intr();
void timer0_init();

#endif // __TIMER0_H
