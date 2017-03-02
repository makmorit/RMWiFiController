#ifndef __ROTENC_H
#define __ROTENC_H

//
// ロータリーエンコーダーで使用する変数
//
unsigned char rsw_counter_value = 0;   // 初期値
unsigned char rsw_counter_min   = 0;   // 最小値
unsigned char rsw_counter_max   = 100; // 最大値
unsigned char rsw_status_value  = 0;   // ステータス
unsigned char rsw_curr_value;          // 入力された値を保持

// 関数
unsigned char rotenc_detection();

#endif // __ROTENC_H
