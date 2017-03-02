#include "main.h"
#include "device.h"
#include "lcd.h"

//
// �k�b�c�Ƀf�[�^�����ۂɑ��M���鏈��
//
static void lcd_write(unsigned char c) 
{
	// ���M�f�[�^�̃o�C�g���ʂS�r�b�g������
	LCD_D4 = ((c >> 4) & 0x01);
	LCD_D5 = ((c >> 5) & 0x01);
	LCD_D6 = ((c >> 6) & 0x01);
	LCD_D7 = ((c >> 7) & 0x01);

	__delay_us(10);
	LCD_EN = 1;

	__delay_us(10);
	LCD_EN = 0;

	// ���M�f�[�^�̃o�C�g�񉺈ʂS�r�b�g������
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
// �k�b�c�ɃR�}���h�𔭍s���鏈��
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
// lcd_clear - �k�b�c���W���[���̉�ʂ���������
//
static void lcd_clear(void)
{
	LCD_RS = 0;
	lcd_write(0x01); // Clear Display : ��ʑS�̂�20H�̽�߰��ŕ\���A���ق�col=0,row=0�Ɉړ�
	__delay_ms(2);   // LCD������(1.53ms)����̂�҂��܂�
}
//
// lcd_setCursor - �k�b�c���W���[����ʓ��̃J�[�\���ʒu���ړ����鏈��
// col : ��(��)�����̃J�[�\���ʒu(0-15)
// row : �c(�s)�����̃J�[�\���ʒu(0-1)
//
static void lcd_setCursor(int col, int row) 
{
	int row_offsets[] = {0x00, 0x40};

	LCD_RS = 0;
	lcd_write(0x80 | (col + row_offsets[row])); // Set DDRAM Adddress : 00H-0FH,40H-4FH
}
//
// lcd_puts - LCD�ɕ�����f�[�^���o�͂��鏈��(�������NULL(0x00)�܂ŌJ�Ԃ��o��)
//
static void lcd_puts(const char * s) 
{
	// RS�̐���M�������Z�b�g���܂�
	LCD_RS = 1;
	while (*s) {
		lcd_write(*s++);
	}
}
//
// lcd_init - �k�b�c�̏���������
//
void LCD_init() 
{
	if (LCD_NOT_USE) return;

	LCD_RS = 0;
	LCD_EN = 0;

	__delay_ms(150);    // �d���n�m��150ms�܂ő҂��Ă��珉����

	// LCD�̗��グ���̃`�F�b�N�f�[�^(�C�j�V�����C�Y�����p)��ݒ�
	lcd_command(0x03);
	__delay_ms(5);
	lcd_command(0x02);

	// LCD�ɃR�}���h�𔭍s���܂�
	lcd_write(0x28);   // function set   : �f�[�^����4�{�E�\���͂Q�s�E�t�H���g��5x8�h�b�g
	lcd_write(0x0c);   // display control: ��ʕ\����ON�E�J�[�\���\����OFF�E�J�[�\���_�ł�OFF
	lcd_clear();       // Clear Display  : ��ʂ��N���A���A�J�[�\���ʒu��col=0,row=0
	lcd_write(0x06);   // entry mode set : ������\���������ɃJ�[�\�����ړ�������w��
}
//
// �k�b�c�ɕ������\�����鏈��
//
void LCD_print_upper()
{
	if (LCD_NOT_USE) return;

	// �J�[�\����1�s�ڂ̐擪�Ɉړ����k�b�c�\��
	lcd_setCursor(0, 0);
	lcd_puts(lcd_upper_buf);
}
void LCD_print_lower()
{
	if (LCD_NOT_USE) return;

	// �J�[�\����2�s�ڂ̐擪�Ɉړ����k�b�c�\��
	lcd_setCursor(0, 1);
	lcd_puts(lcd_lower_buf);
}
