#ifndef _LCD1602_H
#define _LCD1602_H

#include <reg52.h>

// bit	B_TX1_Busy;	//����æ��־
/********IO���Ŷ���***********************************************************/
sbit POWER = P2^4; //д������ǰ���ߵ�ѹ��Ȼ��͵�ѹ
sbit LCD_RS=P2^5;//��������
sbit LCD_RW=P2^6;
sbit LCD_E=P2^7;

/********�궨��***********************************************************/
#define LCD_Data P0
#define Busy    0x80 //���ڼ��LCD״̬���е�Busy��ʶ

/********���ݶ���*************************************************************/
//unsigned char code uctech[] = {"Happy every day"};

/*************************************************************/
void WriteDataLCD(unsigned char WDLCD);					//д����
void WriteCommandLCD(unsigned char WCLCD,BuysC);		//д����
unsigned char ReadDataLCD(void);						//������
unsigned char ReadStatusLCD(void);						//��״̬
void LCDInit();										//��ʼ��
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData);			//��Ӧ������ʾ�ֽ�����
void DisplayListChar(unsigned char X, unsigned char Y, unsigned char code *DData);	//��Ӧ���꿪ʼ��ʾһ������
void Delay5Ms();									//��ʱ
void Delay400Ms();									//��ʱ
void lcd_display(const char * up, const char * down);  //�ַ���˫����ʾ


#endif // 