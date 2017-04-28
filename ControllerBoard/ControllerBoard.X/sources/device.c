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
    WPUA    = 0b00000000;

    // Port B
    //   アナログは使用しない（すべてデジタルI/Oに割当てる）
    //   RB7,RB6(ROT_ENC)は入力(10k pull up)
    //   RB5,RB4,RB3,RB2,RB1(SW)は入力(10k pull down)
    //   RB0は入力(10k pull up/downのいずれかで固定)
    //          76543210
    ANSELB  = 0b00000000;
    TRISB   = 0b11111111;
    PORTB   = 0b00000000;
    WPUB    = 0b00000000;

    // Port C
    //   RC7(RX)は入力
    //          76543210
    ANSELC  = 0b00000000;
    TRISC   = 0b10000000;
    PORTC   = 0b00000000;
    WPUC    = 0b00000000;

    // Port E
    //   RE3(SW)は入力(10k pull down)
    //          76543210
    PORTE   = 0b00000000;
    WPUE    = 0b00000000;
}

//
// TIMER0の設定
//
void setup_timer0()
{
    // Enables Timer0
    // Timer0 is configured as an 8-bit timer/counter
    T0CON0bits.T0EN = 1;
    T0CON0bits.T016BIT = 0;

    // T0CS<2:0>: 010 = FOSC/4
    // T0ASYNC: 0 = The input to the TMR0 counter is synchronized to FOSC/4
    // T0CKPS<3:0>: 0110 = 1:64 (Prescaler Rate Select bit)
    //   1 count = 12.8us(=1/20MHz*4*64)
    T0CON1bits.T0CS = 0b010;
    T0CON1bits.T0ASYNC = 0;
    T0CON1bits.T0CKPS = 0b0110;

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
    // Peripheral Pin Select (PPS) module settings
    //   RC6 = TX/CK(0x10) for output
    RC6PPS = 0x10;
    //   RX = RC7(0x17) for input
    RXPPS = 0x17;

    // 送信情報設定
    // 非同期モード(SYNC=0),8bit(TX9=0),ノンパリティ(TX9D=0)
    TX1STAbits.TXEN = 1;
    TX1STAbits.SYNC = 0;
    TX1STAbits.TX9 = 0;
    TX1STAbits.TX9D = 0;

    // 受信情報設定
    RC1STAbits.SPEN = 1;
    RC1STAbits.CREN = 1;
    RC1STAbits.RX9 = 0;
    RC1STAbits.RX9D = 0;

    // 9600 の場合（Fosc=8MHz）
    //   SYNC = 0, BRGH = 1, BRG16 = 0
    //   SPBRG = 51 (8,000,000/(16 * 9,600) - 1)
    // 19200 の場合（Fosc=20MHz）
    //   SYNC = 0, BRGH = 0, BRG16 = 1
    //   SPBRG = 64 (20,000,000/(16 * 19,200) - 1)
    TX1STAbits.BRGH = 0;
    BAUD1CONbits.BRG16 = 1;
    SP1BRG = 64;

    // ＵＳＡＲＴ割込み受信フラグの初期化
    RCIF  = 0;
}

//
// TIMER2 (CCP) の設定
//
void setup_timer2()
{
    // Peripheral Pin Select (PPS) module settings
    //   RC2 = CCP1(0x09) for output
    RC2PPS = 0x09;

    // Timer2 Clock Select bits
    T2CLKbits.T2CS = 0b0001; // 0001 = FOSC/4

    // Timer2-type Clock Prescale Select bits
    T2CONbits.CKPS = 0; // 000 = 1:1 Prescaler
    
    //
    // タイマー２の設定を行う
    //   PWM周期 0.05uS * (160+1) * 4 = 32.2us
    //   周波数  31kHz
    //
    CCP1CONbits.EN = 1; // 1 = CCPx is enabled
    CCP1CONbits.FMT = 1; // 0 = Right-aligned format
    CCP1CONbits.MODE = 0b1111; // 1111 = PWM mode

    // CCP1 デューティ設定
    //   CCPR1L に8ビット分の値が設定される
    //   PR2(タイマー２周期レジスター)初期値<--PWM周期
    CCPR1H = 0;
    PR2    = 160;
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
