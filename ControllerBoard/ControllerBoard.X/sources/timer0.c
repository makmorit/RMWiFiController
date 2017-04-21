#include "common.h"
#include "device.h"
#include "timer0.h"

void timer0_intr()
{
    if (TMR0IF != 1) {
        return;
    }

    // 割込みカウンター
    tmr0_total_cnt_100m++;
    tmr0_total_cnt_1s++;
    tmr0_toggle = 1;
    // 256カウント（3.2768 ms）で割込み発生させる
    TMR0 = 0;
    // TMR0割り込みクリア
    TMR0IF = 0;
}

void timer0_init()
{
    // do_events 処理回数カウンター
    //   処理時点での割込みカウンター
    tmr0_total_cnt_1s = 0;
}
