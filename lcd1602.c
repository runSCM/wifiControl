#include "lcd1602.h"                                                                          #include "lcd1602.h"   


/***********��ʼ��********************************************************/	
void LCDInit(void)
{
 	LCD_Data = 0;
 	WriteCommandLCD(0x38,0); 	//����ģʽ���ã������æ�ź�
 	Delay5Ms(); 
 	WriteCommandLCD(0x38,0);
 	Delay5Ms(); 
 	WriteCommandLCD(0x38,0);
 	Delay5Ms(); 

 	WriteCommandLCD(0x38,1); 	//��ʾģʽ����,��ʼҪ��ÿ�μ��æ�ź�
 	WriteCommandLCD(0x08,1); 	//�ر���ʾ
 	WriteCommandLCD(0x01,1); 	//��ʾ����
 	WriteCommandLCD(0x06,1); 	//��ʾ����ƶ�����
 	WriteCommandLCD(0x0C,1); 	//��ʾ�����������
}

/***********��ָ��λ����ʾһ���ַ�*******************************************/	
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData)
{
 	Y &= 0x1;
 	X &= 0xF; 				//����X���ܴ���15��Y���ܴ���1
 	if (Y) X |= 0x40; 		//��Ҫ��ʾ�ڶ���ʱ��ַ��+0x40;
 	X |= 0x80; 			//���ָ����
 	WriteCommandLCD(X, 0); //���ﲻ���æ�źţ����͵�ַ��
 	WriteDataLCD(DData);
}

/***********��ָ��λ����ʾһ���ַ�*****************************************/	
void DisplayListChar(unsigned char X, unsigned char Y, unsigned char code *DData)
{
 	unsigned char ListLength;

 	ListLength = 0;
 	Y &= 0x1;
 	X &= 0xF; 				//����X���ܴ���15��Y���ܴ���1
 	while (DData[ListLength]>=0x20){ //�������ִ�β���˳�
   		if (X <= 0xF){ 		//X����ӦС��0xF
     		DisplayOneChar(X, Y, DData[ListLength]); //��ʾ�����ַ�
     		ListLength++;
     		X++;
    	}
  	}
}


/***********����ʱ********************************************************/	
void Delay5Ms(void)
{
 	unsigned int TempCyc = 5552;
 	while(TempCyc--);
}

/***********����ʱ********************************************************/	
void Delay400Ms(void)
{
 	unsigned char TempCycA = 5;
 	unsigned int TempCycB;
 	while(TempCycA--){
  		TempCycB=7269;
  		while(TempCycB--);
 	}
}

/***********lcd��ʾ������ʼ********************************************************/	
void lcd_display(const char* up, const char* down)
{
   
	Delay400Ms(); 	//�����ȴ�����LCD���빤��״̬
	LCDInit(); 		//��ʼ��
	Delay5Ms(); 	//��ʱƬ��(�ɲ�Ҫ)                                         +


	DisplayListChar(0, 0, up);
	DisplayListChar(1, 5, down);
	ReadDataLCD();	//�����þ�������
    Delay400Ms();
    //POWER = 0;
}

/***********д����********************************************************/	
void WriteDataLCD(unsigned char WDLCD)
{
 	ReadStatusLCD(); //���æ
 	LCD_Data = WDLCD;
 	LCD_RS = 1;
 	LCD_RW = 0;
 	LCD_E = 0; 		//�������ٶ�̫�߿���������С����ʱ
 	LCD_E = 0; 		//��ʱ
 	LCD_E = 1;
}

/***********дָ��********************************************************/	
void WriteCommandLCD(unsigned char WCLCD,BuysC) //BuysCΪ0ʱ����æ���
{
 	if (BuysC) ReadStatusLCD(); //������Ҫ���æ
 	LCD_Data = WCLCD;
 	LCD_RS = 0;
 	LCD_RW = 0; 
 	LCD_E = 0;
 	LCD_E = 0;
	LCD_E = 1; 
}

/***********������********************************************************/	
unsigned char ReadDataLCD(void)
{
 	LCD_RS = 1; 
 	LCD_RW = 1;
 	LCD_E = 0;
 	LCD_E = 0;
 	LCD_E = 1;
 	return(LCD_Data);
}

/***********��״̬*******************************************************/	
unsigned char ReadStatusLCD(void)
{
 	LCD_Data = 0xFF; 
 	LCD_RS = 0;
 	LCD_RW = 1;
 	LCD_E = 0;
 	LCD_E = 0;
 	LCD_E = 1;
 	while (LCD_Data & Busy); //���æ�ź�
 	return(LCD_Data);
}
