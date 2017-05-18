#include "common.h"
#include "device.h"
#include "timert2.h"

void timer2_init()
{
    // TIMER2開始
    TMR2IF = 0;
    T2CONbits.ON = 1;
}

void timer2_set_ccpr1(unsigned char v) 
{
    // CCP1 に値（デューティー比）を設定
    //   v = 0-100 の値
    //   CCPR1H = 0-160(PR2) の値
    CCPR1H = PR2 * v / 100;
}
