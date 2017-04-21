#include "common.h"
#include "device.h"
#include "rotenc.h"

//
// ロータリーエンコーダー検知処理
//
unsigned char rotenc_detection()
{
    switch (rsw_status_value) {
    case 0 :
        if (rsw_curr_value == ROT_ENC_SIGNAL_UP) {
            rsw_status_value++;
        } else if (rsw_curr_value == ROT_ENC_SIGNAL_DN) {
            rsw_status_value = 4;
        } else if (rsw_curr_value == ROT_ENC_SIGNAL_HI) {
            rsw_status_value = 2;
        }
        break;
    case 1 :
        if (rsw_curr_value == ROT_ENC_SIGNAL_LO) {
            rsw_status_value = 0;
        } else if (rsw_curr_value == ROT_ENC_SIGNAL_HI) {
            rsw_status_value++;
        } else if (rsw_curr_value == ROT_ENC_SIGNAL_DN) {
            rsw_status_value = 3;
        }
        break;
    case 2 :
        if (rsw_curr_value == ROT_ENC_SIGNAL_LO) {
            rsw_status_value = 0;
        } else if (rsw_curr_value == ROT_ENC_SIGNAL_UP) {
            rsw_status_value--;
        } else if (rsw_curr_value == ROT_ENC_SIGNAL_DN) {
            rsw_status_value++;
        }
        break;
    case 3 :
        if (rsw_curr_value == ROT_ENC_SIGNAL_LO) {
            rsw_status_value = 0; 
            // 値を増加
            if (rsw_counter_value < rsw_counter_max) {
                rsw_counter_value = rsw_counter_value + 1;
                return 1;
            }
        } else if (rsw_curr_value == ROT_ENC_SIGNAL_HI) {
            rsw_status_value--;
        } else if (rsw_curr_value == ROT_ENC_SIGNAL_UP) {
            rsw_status_value = 1;
        }
        break;
    case 4 :
        if (rsw_curr_value == ROT_ENC_SIGNAL_LO) {
            rsw_status_value = 0;
        } else if (rsw_curr_value == ROT_ENC_SIGNAL_HI) {
            rsw_status_value++;
        } else if (rsw_curr_value == ROT_ENC_SIGNAL_UP) {
            rsw_status_value = 6;
        }
        break;
    case 5 :
        if (rsw_curr_value == ROT_ENC_SIGNAL_LO) {
            rsw_status_value = 0;
        } else if (rsw_curr_value == ROT_ENC_SIGNAL_UP) {
            rsw_status_value++;
        } else if (rsw_curr_value == ROT_ENC_SIGNAL_DN) {
            rsw_status_value--;
        }
        break;
    case 6 :
        if (rsw_curr_value == ROT_ENC_SIGNAL_LO) {
            rsw_status_value = 0; 
            // 値を減少
            if (rsw_counter_min < rsw_counter_value) {
                rsw_counter_value = rsw_counter_value - 1;
                return 1;
            }
        } else if (rsw_curr_value == ROT_ENC_SIGNAL_HI) {
            rsw_status_value--;
        } else if (rsw_curr_value == ROT_ENC_SIGNAL_DN) {
            rsw_status_value = 4;
        }
        break;
    }
    return 0;
}
