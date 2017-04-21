#include "common.h"
#include "device.h"

//
// ピンなどの設定を行う
//
void setup_port()
{
    // Port A
    //   アナログは使用しない（すべてデジタルI/Oに割当てる）
    //   RA1,RA0(SENSOR)は入力(センサーを踏むとLowになります)
    //          76543210
    ANSELA  = 0b00000000;
    TRISA   = 0b00000011;
    PORTA   = 0b00000000;

    // Port B
    //   アナログは使用しない（すべてデジタルI/Oに割当てる）
    //   RB7,RB6(ROT_ENC)は入力(10k pull up)
    //   RB5,RB4,RB3,RB2,RB1(SW)は入力(10k pull down)
    //   RB0は入力(10k pull up/downのいずれかで固定)
    //          76543210
    ANSELB  = 0b00000000;
    TRISB   = 0b11111111;
    PORTB   = 0b00000000;

    // Port C
    //   RC7(RX)は入力
    //          76543210
    TRISC   = 0b10000000;
    PORTC   = 0b00000000;

    // Port E
    //   RE3(SW)は入力(10k pull down)
    //          76543210
    TRISE   = 0b00001000;
    PORTE   = 0b00000000;
}

//
// TIMER0の設定
//
void setup_timer0()
{
    // TMR0ON: Enables Timer0
    // T08BIT: Timer0 is configured as an 8-bit timer/counter
    // プリスケーラー:64  − １カウント12.8μ秒(=1/20MHz*4*64)
    // OPTION_REG の先頭ビット:!WPUEN (1:内部プルアップ無)
    OPTION_REG = 0b10000101;

    // 256カウント（3.2768 ms）で割込み発生させる
    TMR0 = 0;

    // TMR0割り込み許可
    TMR0IE = 1;
}

//
// UARTの設定
//
void setup_uart()
{
    // 送信情報設定：非同期モード(SYNC=0),8bit(TX9=0),ノンパリティ(TX9D=0)
    TXSTA = 0b00100100;
    // 受信情報設定
    RCSTA = 0b10010000;

    // 9600 の場合（Fosc=8MHz）
    //   SYNC = 0, BRGH = 1, BRG16 = 0
    //   SPBRG = 51 (8,000,000/(16 * 9,600) - 1)
    // 19200 の場合（Fosc=20MHz）
    //   SYNC = 0, BRGH = 0, BRG16 = 1
    //   SPBRG = 64 (20,000,000/(16 * 19,200) - 1)
    BRGH  = 0;
    BRG16 = 1;
    SPBRG = 64;

    // ＵＳＡＲＴ割込み受信フラグの初期化
    RCIF  = 0;
}

//
// TIMER2 (CCP) の設定
//
void setup_timer2()
{
    //
    // タイマー２の設定を行う
    //   PWM周期 0.05uS * (160+1) * 4 = 32.2us
    //   周波数  31kHz
    //
    CCP1CON = 0b1100; // PWM
    CCPR1L  = 0;      // CCP1 デューティ設定
    PR2     = 160;    // タイマー２周期レジスターに初期値をセット
}

//
// 動作モードを設定
//
void set_device_mode()
{
    if (MODE_SELECT == 0) {
        devide_type = DEVTYP_LAYOUT; // 親機（パワーコントローラー）
    } else {
        devide_type = DEVTYP_REMCON; // 子機（リモコン）
    }
}
