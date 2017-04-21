#include "common.h"
#include "device.h"
#include "lcd.h"

//
// ＬＣＤにデータを実際に送信する処理
//
static void lcd_write(unsigned char c) 
{
    // 送信データのバイト列上位４ビットを処理
    LCD_D4 = ((c >> 4) & 0x01);
    LCD_D5 = ((c >> 5) & 0x01);
    LCD_D6 = ((c >> 6) & 0x01);
    LCD_D7 = ((c >> 7) & 0x01);

    __delay_us(10);
    LCD_EN = 1;

    __delay_us(10);
    LCD_EN = 0;

    // 送信データのバイト列下位４ビットを処理
    LCD_D4 = ((c) & 0x01);
    LCD_D5 = ((c >> 1) & 0x01);
    LCD_D6 = ((c >> 2) & 0x01);
    LCD_D7 = ((c >> 3) & 0x01);

    __delay_us(10);
    LCD_EN = 1;

    __delay_us(10);
    LCD_EN = 0;
}
//
// ＬＣＤにコマンドを発行する処理
//
static void lcd_command(unsigned char c) 
{
    LCD_RS = 0;
    LCD_D4 = ((c) & 0x01);
    LCD_D5 = ((c >> 1 ) & 0x01);
    LCD_D6 = ((c >> 2 ) & 0x01);
    LCD_D7 = ((c >> 3 ) & 0x01);

    __delay_us(10);
    LCD_EN = 1;

    __delay_us(10);
    LCD_EN = 0;
}
//
// lcd_clear - ＬＣＤモジュールの画面を消す処理
//
static void lcd_clear(void)
{
    LCD_RS = 0;
    lcd_write(0x01); // Clear Display : 画面全体に20Hのｽﾍﾟｰｽで表示、ｶｰｿﾙはcol=0,row=0に移動
    __delay_ms(2);   // LCDが処理(1.53ms)するのを待ちます
}
//
// lcd_setCursor - ＬＣＤモジュール画面内のカーソル位置を移動する処理
// col : 横(列)方向のカーソル位置(0-15)
// row : 縦(行)方向のカーソル位置(0-1)
//
static void lcd_setCursor(int col, int row) 
{
    int row_offsets[] = {0x00, 0x40};

    LCD_RS = 0;
    lcd_write(0x80 | (col + row_offsets[row])); // Set DDRAM Adddress : 00H-0FH,40H-4FH
}
//
// lcd_puts - LCDに文字列データを出力する処理(文字列をNULL(0x00)まで繰返し出力)
//
static void lcd_puts(const char * s) 
{
    // RSの制御信号線をセットします
    LCD_RS = 1;
    while (*s) {
        lcd_write(*s++);
    }
}
//
// lcd_init - ＬＣＤの初期化処理
//
void lcd_init() 
{
    if (LCD_NOT_USE) return;

    LCD_RS = 0;
    LCD_EN = 0;

    __delay_ms(150);    // 電源ＯＮ後150msまで待ってから初期化

    // LCDの立上げ時のチェックデータ(イニシャライズ処理用)を設定
    lcd_command(0x03);
    __delay_ms(5);
    lcd_command(0x02);

    // LCDにコマンドを発行します
    lcd_write(0x28);   // function set   : データ線は4本・表示は２行・フォントは5x8ドット
    lcd_write(0x0c);   // display control: 画面表示はON・カーソル表示はOFF・カーソル点滅はOFF
    lcd_clear();       // Clear Display  : 画面をクリアし、カーソル位置はcol=0,row=0
    lcd_write(0x06);   // entry mode set : 文字を表示した次にカーソルを移動するを指示
}
//
// ＬＣＤに文字列を表示する処理
//
void lcd_print_upper()
{
    if (LCD_NOT_USE) return;

    // カーソルを1行目の先頭に移動しＬＣＤ表示
    lcd_setCursor(0, 0);
    lcd_puts(lcd_upper_buf);
}
void lcd_print_lower()
{
    if (LCD_NOT_USE) return;

    // カーソルを2行目の先頭に移動しＬＣＤ表示
    lcd_setCursor(0, 1);
    lcd_puts(lcd_lower_buf);
}
