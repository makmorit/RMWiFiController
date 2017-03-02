#include "main.h"
#include "device.h"

//
// ピンなどの設定を行う
//
void port_init()
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
	//   RB5,RB4,RB3(SW)は入力(10k pull down)
	//          76543210
	ANSELB  = 0b00000000;
	TRISB   = 0b11111000;
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

void ccp_init() 
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

void set_ccp(unsigned char v) 
{
	// CCP に値（デューティー比）を設定
	//   v = 0-100 の値
	CCPR1L = PR2 * v / 100;
}

//
// UARTの初期設定を行う
//
void uart_init()
{
	if (UART_NOT_USE) {
		return;
	}

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
// printf 内で使用する putch 関数
// (UART TX に１バイト出力します)
//
void putch(unsigned char byte)
{
	while (!TXIF) {
		continue;
	}
	TXREG = byte;
}

//
// 動作モードを設定
//
void init_device_mode()
{
	devide_type = 0;
	devide_type = DEVTYP_LAYOUT;
}
