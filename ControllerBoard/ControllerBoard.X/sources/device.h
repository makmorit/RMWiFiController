#ifndef __DEVICE_H
#define __DEVICE_H

// デバイス種類を保持
unsigned char devide_type;
#define DEVTYP_LAYOUT 1
#define DEVTYP_REMCON 2

// LCD関連定義
//   LCD を使用するかどうかのフラグ
#define LCD_NOT_USE  0		// 1:使用しません

//   LCD を使用する場合の制御用ピンアサイン
//     LCD を使用しない場合、意味を持ちません
#define LCD_RS       RC0
#define LCD_EN       RC1

#define LCD_D4       RA5
#define LCD_D5       RA4
#define LCD_D6       RA3
#define LCD_D7       RA2

// UART 使用時設定
#define UART_NOT_USE  0		// 1:使用しません

// ロータリーエンコーダー関連定義
//   ロータリーエンコーダーが示す現在値を取得
//   A端子=RB7 B端子=RB6
//   (RB7/RB6の値を２ビットの値に変換)
#define ROT_ENC_CURVAL ((~PORTB >> 6) & 0b00000011)
//
//   ロータリーエンコーダーのパルス検知用
//   ピンアサインに応じ、UP/DN の値を入れ替えてください
//
#define ROT_ENC_SIGNAL_HI 0b11
#define ROT_ENC_SIGNAL_LO 0b00
#define ROT_ENC_SIGNAL_UP 0b01
#define ROT_ENC_SIGNAL_DN 0b10

// スイッチ入力
//   ボタン０
//     自動運転開始ボタン
//     自動運転中に押下されると、ステータスが順次変わります
//   ボタン１
//     プログラムモード時＝パラメーターを順次設定します
//     最後のパラメーター設定時に押下すると設定値がEEPROMにsaveされます
//   ボタン２
//     プログラムモード時＝キャンセルボタンとして機能
//     キャンセルボタン押下で手動モードに戻ります
//   ボタン３
//     正転・逆転切替用のボタン
// センサー入力（踏むとLowになります）
//    減速センサー
//    停止センサー
// 出力
//   反転スイッチ（リレー）出力
#define BUTTON_0    RE3
#define BUTTON_1    RB5
#define BUTTON_2    RB4
#define BUTTON_3    RB3
#define BUTTON_4    RB2
#define BUTTON_5    RB1
#define MODE_SELECT RB0
#define REDUCING_SENSOR RA1
#define STOPPING_SENSOR RA0
#define DIR_RELAY_SW RC5

// 関数
void setup_port();
void setup_timer0();

void setup_ccp();
void setup_uart();

void set_ccp(unsigned char v);
void putch(unsigned char byte);
void set_device_mode();


#endif // __DEVICE_H
