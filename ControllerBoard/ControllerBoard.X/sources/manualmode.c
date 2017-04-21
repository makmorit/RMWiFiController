#include "common.h"
#include "device.h"
#include "lcd.h"
#include "rotenc.h"
#include "process.h"
#include "timert2.h"

// ヘルスチェックカウンター
static unsigned char hcheck_cnt;

static void print_duty_and_hchk(unsigned char c, unsigned char t)
{
    // モードを１行目に表示する
    if (DIR_RELAY_SW == 0) {
        sprintf(lcd_upper_buf, "*Manual Mode*   ");
    } else {
        sprintf(lcd_upper_buf, "*Manual Mode* bk");
    }
    lcd_print_upper();

    // デューティー比とヘルスチェックカウンターを表示
    sprintf(lcd_lower_buf, "Duty[%3u]Cnt[%2u]", c, t);
    lcd_print_lower();
}

//
// ボタン押下時の処理
//
void manual_mode_init()
{
    // モードを手動モードに設定
    main_mode = 0;

    // ヘルスチェックカウンター
    hcheck_cnt = 0;

    // ロータリーエンコーダーのカウンター
    rsw_counter_value = 0;

    // CCPの値を０とする
    timer2_set_ccpr1(rsw_counter_value);
}

//
// 逆転ボタン押下時
//
void reverse_on_manual_mode()
{
    // リレー反転
    DIR_RELAY_SW = ~DIR_RELAY_SW;
}

//
// 100msごとに実行される処理
//
void process_on_manual_mode()
{
    // ロータリーエンコーダーの現在値をそのまま設定
    timer2_set_ccpr1(rsw_counter_value);

    // 現在のDutyとヘルスチェックカウンターを表示
    print_duty_and_hchk(rsw_counter_value, hcheck_cnt);
}

//
// 1sごとに実行される処理
//
void process_on_one_second_manual_mode()
{
    // ヘルスチェックカウンターを増加
    hcheck_cnt++;
    if (hcheck_cnt == 60) {
        hcheck_cnt = 0;
    }
}
