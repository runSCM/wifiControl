#include "lcd1602.h"  


#define uchar unsigned char
#define uint  unsigned int
#define RX_LENGTH 30
#define BUFFER_LENGTH 30

/*************	���ر�������	**************/
bit status = 0;//����һ����־λ�����ڳ������ָ�����ִ�������̬���˵�ָ�����ַ�����1
uchar data_string[RX_LENGTH] ; //�����û����ı�
bit	B_TX1_Busy;	//����æ��־
uchar ceng = 0;//����ƥ���ַ������α�
uchar RSV_MODE = 0;
uchar RSV_PID = 1;   //�����û����͵���Ϣ
uchar RSV_OK  = 2;
uchar RSV_ERROR = 3;
uchar userId = 0; //�ӵ�ģ����û�id
uchar text_length = 0;//�û��������Ĵ��ı��ĳ���
uchar text_num; //��¼�û��ı����ַ�������
uchar flags = 0; //�ַ���־λ
uchar vvalue = 0;

//Led�������
sbit DIO = P2^0;				//������������
sbit LRCLK = P2^1;				//ʱ�������źš�����������Ч
sbit LSCLK = P2^2;				//�����źš���������������Ч


unsigned char code LED_0F[] = 
{// 0	 1	  2	   3	4	 5	  6	   7	8	 9	  A	   b	C    d	  E    F    -
	0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x8C,0xBF,0xC6,0xA1,0x86,0xFF,0xbf
};


const uchar * MODE_STRING = "AT+MODE=2\r\n";
const uchar * MUX_STRING = "AT+CIPMUX=1\r\n";
uchar const * SERVER_STRING = "AT+CIPSERVER=1,5000\r\n";
uchar string_length(char* sub_string);  //�����ַ�������
const uchar *FB_PID = "+IPD,";
const uchar *FB_OK = "OK";
const uchar *FB_ERROR = "ERROR";


uchar scan_keyboard(void);   // ɨ�����
void delay_ms(int n);       //���뼶��ʱ����
void get_LED(uchar key);    //���LED��Ӧ������ֵ
void LED_ON(uchar x);      //����led�����
void uart_init();
void send_string(const char* c_string);     //�����ַ���
void receive_string();               //�����ַ���
void clear_buffer(uchar *s);      //�����������
uchar match_string(const char * f_string, const char * s_string);      //ƥ���յ����ַ���
void clone_string(char* s, char* sc, uchar len); //��ֵ��Ч�ַ���
/********IO���Ŷ���***********************************************************/
//sbit POWER = P2^4; //д������ǰ���ߵ�ѹ��Ȼ��͵�ѹ
//sbit LCD_RS=P2^5;//��������
//sbit LCD_RW=P2^6;
//sbit LCD_E=P2^7;
//������Ŷ���
sbit motor1 = P2^3;
sbit motor2 = P2^4;

/********�궨��***********************************************************/
//#define LCD_Data P0
//#define Busy    0x80 //���ڼ��LCD״̬���е�Busy��ʶ

/********���ݶ���*************************************************************/
unsigned char code uctech[] = {"Happy every day"};
/*************************************************************/
void motor_forward();//�����ת
void motor_reverse();//�����ת
void motor_stop();//���ֹͣ
//�����ת
void motor_forward()
{
    motor2 = 0;
    motor1 = 1;
}
//�����ת
void motor_reverse()
{
    motor1 = 0;
    motor2 = 1;
}
//���ֹͣ
void motor_stop()
{
    motor1 = 0;
    motor2 = 0;
}

/*������*/
void main()
{   
    
    uchar key = 0X00;  //�˴�Ҫ����ֵ
    uchar x = 0x01;
    uchar y = 0x05;
    uchar k = 0;
    //P1 = 0xf0;              //�������ȫΪ0,�������ȫΪ1 
    uart_init();
    //clear_buffer(data_string);
    while(1){

       
       if(flags == 1)
       {
        //��������LCD��ʾ
          x = 0x01;
          Delay400Ms(); 	//�����ȴ�����LCD���빤��״̬
	      LCDInit(); 		//��ʼ��
	      Delay5Ms(); 	//��ʱƬ��(�ɲ�Ҫ) 
          DisplayListChar(0, 0, uctech);
          y &= 0x01;
 	      x &= 0xF;
          send_string(&vvalue);
          while(k < vvalue) {
   		     if (x <= 0xF){ 		//X����ӦС��0xF
     	        DisplayOneChar(x, y, data_string[k]); //��ʾ�����ַ�
     	        k++;
                x++;
               }            
             } 
          vvalue = 0;   
          flags = 0;
          k = 0;
       }
       key = scan_keyboard();   
       get_LED(key);
      
    }
}


/**
* ���ڳ�ʼ��
*/
void uart_init()
{
    SCON  = 0x50;		        /* SCON: ģʽ 1, 8-bit UART, ʹ�ܽ���         */
    TMOD |= 0x20;               /* TMOD: timer 1, mode 2, 8-bit reload        */
    TH1   = 0xFD;               /* TH1:  reload value for 9600 baud @ 11.0592MHz   */
    TL1 = 0XFD;
    EA    = 1;                  /*�����ж�*/
    ES    = 1;                  /*�򿪴����ж�*/
    TR1   = 1;                  /* TR1:  timer 1 run   ���������ʷ���     */
}

/**
*   ��ʱ����
*/
void delay_ms(int n)
{
    int i = 0, j = 0;
    for(i = 0; i < n; i++)
        for(j = 0; j < 100; j++);
}

//����
void LED_ON(uchar lcode)
{
    uint i;
	for(i = 8;i >= 1; i--)
	{
        if (lcode & 0x80) 
            DIO = 1; 
        else 
            DIO = 0;
        lcode <<= 1;
	    LSCLK = 0;
	    LSCLK = 1;
	}
}
                 
void LED_code(uchar value)
{
    uchar code *led_table;          // ���ָ��
	uchar lcode;               //Ҫ�õ�����ʾ��
	//��ʾ��1λ
/*	led_table = LED_0F + value;
	lcode = *led_table;

	LED_ON(lcode);			
	LED_ON(0x0f);
    
    LRCLK = 0;
	LRCLK = 1;
    //��ʾ��2λ
	led_table = LED_0F + value;
	lcode = *led_table;

	LED_ON(lcode);			
	LED_ON(0x0f);
    
    LRCLK = 0;
	LRCLK = 1;
    //��ʾ��3λ
	led_table = LED_0F + value;
	lcode = *led_table;

	LED_ON(lcode);			
	LED_ON(0x0f);
    
    LRCLK = 0;
	LRCLK = 1; */
    //��ʾ��4λ
	led_table = LED_0F + value;
	lcode = *led_table;

	LED_ON(lcode);			
	LED_ON(0x0f);
    
    LRCLK = 0;
	LRCLK = 1;     
}

void get_LED(uchar key)
{
    if(key == 0x00)
        return;   //���Ϊ0x00��˵��û��ɨ�赽ֵ������
    switch(key)
    {
        case 0xee: 
            LED_code(0); 
            send_string(MODE_STRING);
            lcd_display(uctech, "MODE ok");
            break;//0������Ӧ�ļ���ʾ���Ӧ����ֵ
		case 0xde: 
            LED_code(1);   
            send_string(MUX_STRING); 
            lcd_display(uctech, "CIP ok");
            //send_string(MUX_STRING);
            break;//1 ������Ӧ�ļ���ʾ���Ӧ����ֵ 
		case 0xbe: 
            LED_code(2);
            send_string(SERVER_STRING);
            lcd_display(uctech, "Server ok");
            //send_string(SERVER_STRING); 
            break;//2
		case 0x7e: 
            LED_code(3);
            break;//3
		case 0xed: 
            LED_code(4); 
            break;//4
		case 0xdd: 
            LED_code(5); 
            break;//5
		case 0xbd: 
            LED_code(6);
            lcd_display(uctech, "runhang");
            break;//6
		case 0x7d: 
            LED_code(7); 
            break;//7
		case 0xeb: 
            LED_code(8); 
            motor_forward(); 
            break;//8
		case 0xdb: 
            LED_code(9); 
            motor_reverse();
            break;//9
		case 0xbb: 
            LED_code(10); 
            motor_stop();
            break;//a
		case 0x7b: 
            LED_code(11); 
            break;//b
		case 0xe7: 
            LED_code(12); 
            break;//c
		case 0xd7: 
            LED_code(13);  
            break;//d
		case 0xb7: 
            LED_code(14); 
            break;//e
		case 0x77: 
            LED_code(15); 
            break;//f
    }
}

//  ɨ�����
uchar scan_keyboard()
{
    uchar row_wire, col_wire;    //�������ߺ����ߣ�����0-3������4-7
    //uchar flags;
    P1 = 0xf0;              //�������ȫΪ0,�������ȫΪ1 
    col_wire = P1 & 0xf0;   //��������ֵ
    //if(flag != '1')
      //  return 0x00;
    if(col_wire != 0xf0)
    {
        delay_ms(1);  //ȥ����ʱ
        if(col_wire != 0xf0)  
        {
            col_wire = P1 & 0xf0;  //��������ֵ
            P1 = col_wire | 0x0f;  //�����ǰ����ֵ
            row_wire = P1 & 0x0f;  //��������ֵ
            P1 = 0xf0;
            while((P1&0xf0) != 0xf0);
            return(col_wire + row_wire);//������������ֵ
        }
    }
    return 0x00;
}

/*
// �����ַ���
void send_string(char  * c_string)
{
    unsigned char i = 0;
    ES = 0;
    while(c_string[i] != '\0')
    {
        SBUF = c_string[i];
        while(!TI);		  //�ȴ������������
        i++;
		TI=0;  
    }
    delay_ms(50);		  //��ʱһ���ٷ�
    ES = 1;

} */


// �����ַ���
void send_string(const uchar * c_string)
{
    ES = 0;
    while(*c_string != '\0')
    {
        SBUF = *c_string;
        while(!TI);        //�ȴ������������
        TI = 0;
        c_string++;
    }
   ES = 1;
}



//�Ƚ��û����͹������ַ����ж��Ķ���ģʽ
void fb_pid(uchar stmp)
{   
    if(ceng <= 4)
    {
        if(stmp == FB_PID[ceng])
        {
            ceng++;    
        }else
        {
            ceng = 0;
            RSV_MODE = 0;
        }
    }else if(ceng == 5)  
    {
        userId = stmp;
        ceng++;
    }/*else if(stmp == ',' && ceng == 6)
    {
        ceng++;    
    }*/else if(ceng == 7)
    {   
        //text_length = stmp - 1;//#/r/n��β����ʽ
        text_length = stmp - 0x30;//��ȥ0->��Ӧascii��Ϊ0x30
        ceng++;
    }else if(ceng == 6)
    {
        ceng++;
    }else if(ceng == 8)//(stmp == ':' && ceng == 8)
    {  // send_string(&stmp);
        if(stmp == ':')  //˵���ַ�С��9
        {
           //send_string(&stmp);
           ceng = 0;
           status = 1; 
        }else
        {
            text_length = stmp - 0x26;// ��20��
            ceng++;
        }
        //ceng = 0;
        //status = 1;
        //RSV_MODE = 0;       
    }else if(ceng == 9)
    {
        if(stmp == ':')
        {
            ceng = 0;
            status = 1;
        }else{       //������Ϊ��չ
            ceng = 0;
            RSV_MODE = 0;
        }
    }else{
        send_string("errorpid");
            ES = 0;
            RI = 0;
            SBUF = ceng;
            while(!TI);
            TI = 0;
            ES = 1;
        ceng = 0;
        RSV_MODE = 0;
    }
    
/*
    if(stmp == '+' && ceng == 0)
    {
        buffer_string[ceng] = stmp;
        ceng++;   
    }else if(stmp == 'I' && ceng == 1)
    {
        buffer_string[ceng] = stmp;
        ceng++;
    }else if(stmp == 'P' && ceng == 2)
    {
        buffer_string[ceng] = stmp;
        ceng++;
    }else if(stmp == 'D' && ceng == 3)
    {
        buffer_string[ceng] = stmp;
        ceng++;
    }else if(stmp == ',' && (ceng == 4 ||ceng == 6))
    {
        buffer_string[ceng] = stmp;
        ceng++;
    }else if(stmp <= 0x39 && stmp >= 0x30 && (ceng == 5 ||ceng == 7) ) */ /*ascii��*/
     /*{
        buffer_string[ceng] = stmp;
        ceng++;
    }else if(stmp == ':' && ceng == 8)
    {
        buffer_string[ceng] = stmp;
        ceng++;
        status = 1;        
    } else{
        ceng = 0;
    }*/
   }

//ƥ�䷴����ERROR�ַ�
void fb_error(uchar stmp)
{
    if(ceng <= 4)
    {
        if(stmp == FB_ERROR[ceng])
        {
            //buffer_string[ceng] = stmp;
            ceng++;    
        }else{
            ceng = 0;
            RSV_MODE = 0;    
        }
    }
    if(ceng > 4)
    {
        send_string(FB_ERROR);// ����buffer_string ����ʹ��
        ceng = 0;
        RSV_MODE = 0;
    }
    /*
     if(stmp == 'E' && ceng == 0)
    {
        buffer_string[ceng] = stmp;
        ceng++;        
    }else if(stmp == 'R' && (ceng == 1 || ceng == 2 ||ceng == 4))
    {
        buffer_string[ceng] = stmp;
        if(ceng == 4)
            status = 1;
        else 
            ceng++;
    }else if(stmp == 'O' && ceng == 3)
    {
        ceng++;      
    }else{
        ceng = 0;
    } */        
}


//�Ƚ�ģ��OK���ַ�
void fb_oK(uchar stmp)
{
    if(ceng <= 1)
    {
        if(stmp == FB_OK[ceng])
        {
           // buffer_string[ceng] = stmp;
            ceng++;     
        }else{
            ceng = 0;
            RSV_MODE = 0;
        }
    }
    if(ceng > 1){//���ݽ������
        send_string(FB_OK);// ����buffer_string ����ʹ��
        ceng = 0;
        RSV_MODE = 0;
    }

  /*  if(stmp == 'O' && ceng == 0)
    {
        buffer_string[ceng] = stmp;
        ceng++;        
    }else if(stmp == 'K' && ceng == 1)
    {
        buffer_string[ceng] = stmp;
        status = 1;
    }else{
        ceng = 0;
    }   */    
}



 // �����ַ�
void receive_char() interrupt 4
{  /*
    uchar stmp = 0;
    stmp = SBUF;
    RI = 0;
    //BUFFER_STRING[i] = stmp;
    SBUF = stmp;
    while(!TI);		  //�ȴ������������
	    TI=0;			  //���������ɱ�־λ
   */ 
   if(status == 1)
    {   
        RI = 0;
       /* if(SBUF != '#')
        {   
            data_string[vvalue] = SBUF;
            vvalue++;
            RI = 0;
        }
        if(SBUF == '#')
        {   
            uchar i;
            flags = 1; /*
            for(i = 0; i <= vvalue; i++)
            {
                ES = 0;
                SBUF = data_string[i];
                while(!TI);
                    TI = 0;
                ES = 1;
            }  */
            //send_string(&vvalue);   
            //data_string[ceng] ='\0';
            //vvalue = ceng;
            //ceng = 0;
         /*   status = 0;
            RSV_MODE = 0;     
            //�������
            if(data_string[0] == 'M' && vvalue > 1)
            {
             if(data_string[1] == '1')  //�����ת
                {      
                    LED_code(8);
                    motor_forward();//
                }else if(data_string[1] == '2'){ //�����ת
                    motor_reverse();
                    LED_code(9);
                }else{ //��������3  ���ֹͣ
                    motor_stop();
                    LED_code(10);
                }
                
            }else{
                LED_code(11);
            }                     
        } 
          */

        
        
        
        //uchar i =0;
        if(ceng < text_length)
        {   
            data_string[ceng] = SBUF;
            //ES = 0;
            RI = 0;
            //SBUF = data_string[ceng]; 
           // while(!TI);
            ceng++;
            //TI = 0;
           // ES = 1;
        }
        if(ceng >= text_length)
        {   
            uchar i = 0;
            flags = 1;/*
            for(i = 0; i < text_length; i++)
            {
                ES = 0;
                RI = 0;
                SBUF = data_string[i]; 
                while(!TI);
                TI = 0;
                ES = 1;
            }*/
            vvalue = text_length;
            if(data_string[0] == 'M' && text_length > 1)
            {
             if(data_string[1] == '1')  //�����ת
                {      
                    LED_code(8);
                    motor_forward();//
                }else if(data_string[1] == '2'){ //�����ת
                    motor_reverse();
                    LED_code(9);
                }else{ //��������3  ���ֹͣ
                    motor_stop();
                    LED_code(10);
                }
                
            }else{
                LED_code(11);
            }
           // clear_buffer(data_string);
            status = 0;
            RSV_MODE = 0;
            ceng = 0;
       }  
    }else{

        if(RSV_MODE == 0) //�Ե�һ���ַ�������ģʽ
        {
            if(SBUF == 'O')
            {
                RSV_MODE = RSV_OK;
                fb_oK(SBUF);

            }else if(SBUF == '+')
            {
                RSV_MODE = RSV_PID;
                fb_pid(SBUF);
            }else if(SBUF == 'E')
            {
                RSV_MODE = RSV_ERROR;
                fb_error(SBUF);
            }
        }else if(RSV_MODE == RSV_OK)
        {
            fb_oK(SBUF);       
        }else if(RSV_MODE == RSV_ERROR)
        {
            fb_error(SBUF);
        }else if(RSV_MODE == RSV_PID)
        {
            fb_pid(SBUF);    
        }

    }
    RI = 0;  
   
}
  
  
  
 //uchar buf;  
    /*if(status)
    {
        if(ceng < buffer_string[7])
            data_string[7] = SBUF;
        else
            send_string(data_string);
        
       
    }else{

        if(RSV_MODE == 0) //�Ե�һ���ַ�������ģʽ
        {
            if(SBUF == 'O')
            {
                RSV_MODE = RSV_OK;
                fb_oK(SBUF);

            }else if(SBUF == '+')
            {
                RSV_MODE = RSV_PID;
                fb_pid(SBUF);
            }else if(SBUF == 'E')
            {
                RSV_MODE = RSV_ERROR;
                fb_error(SBUF);
            }
        }else if(RSV_MODE == RSV_OK)
        {
            fb_oK(SBUF);       
        }else if(RSV_MODE == RSV_ERROR)
        {
            fb_error(SBUF);
        }else if(RSV_MODE == RSV_PID)
        {
            fb_pid(SBUF);    
        }

    }
    RI = 0;  
    */

