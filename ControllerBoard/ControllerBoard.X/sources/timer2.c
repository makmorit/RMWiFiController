#include "common.h"
#include "device.h"
#include "timert2.h"

void timer2_init()
{
    // TIMER2 on prescale=1
    // TIMER2スタート（onビットを１）
    T2CON = 0b100;
}

void timer2_set_ccpr1(unsigned char v) 
{
    // CCP に値（デューティー比）を設定
    //   v = 0-100 の値
    CCPR1L = PR2 * v / 100;
}
