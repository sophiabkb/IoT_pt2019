
#include <pic18f4550.h>
#include "ConfigurationPIC18F4550.h"
#include <xc.h>

/*********************Definition of Ports********************************/

#define RS LATB0                    /*PIN 0 of PORTB is assigned for register select Pin of LCD*/
#define EN LATB1                    /*PIN 1 of PORTB is assigned for enable Pin of LCD */
#define ldata LATB                  /*PORTB(PB4-PB7) is assigned for LCD Data Output*/ 
#define LCD_Port TRISB              /*define macros for PORTB Direction Register*/
#define lcd PORTB
#define rs PORTBbits.RB0
#define en PORTBbits.RB1
/*********************Proto-Type Declaration*****************************/

void MSdelay(unsigned int );        /*Generate delay in ms*/
void LCD_Init();                    /*Initialize LCD*/
void LCD_Command(unsigned char );   /*Send command to LCD*/
void LCD_Char(unsigned char x);     /*Send data to LCD*/
void LCD_String(const char *);      /*Display data string on LCD*/
void LCD_String_xy(char, char , const char *);
void LCD_Clear();                   /*Clear LCD Screen*/




//****************************Functions********************************

//initialize the LCD
void LCD_Init()
{
    LCD_Port = 0;                   /*PORT as Output Port*/
    MSdelay(15);                    /*15ms,16x2 LCD Power on delay*/
    LCD_Command(0x02);              /*send for initialization of LCD 
                                     *for nibble (4-bit) mode */
    LCD_Command(0x28);              /*use 2 line and 
                                     *initialize 5*8 matrix in (4-bit mode)*/
	LCD_Command(0x01);              /*clear display screen*/
    LCD_Command(0x0c);              /*display on cursor off*/
	LCD_Command(0x06);              /*increment cursor (shift cursor to right)*/	   
}

//send commands to the LCD
void LCD_Command(unsigned char cmd )
{
	ldata = (ldata & 0x0f) |(0xF0 & cmd);   /*Send higher nibble of command first to PORT*/ 
	RS = 0;                                 /*Command Register is selected i.e.RS=0*/ 
	EN = 1;                                 /*High-to-low pulse on Enable pin to latch data*/ 
	NOP();
	EN = 0;
	MSdelay(1);
    ldata = (ldata & 0x0f) | (cmd<<4);      /*Send lower nibble of command to PORT */
	EN = 1;
	NOP();
	EN = 0;
	MSdelay(3);
}

//print a char in the lcd 16x2
void LCD_Char(unsigned char dat)
{
	ldata = (ldata & 0x0f) | (0xF0 & dat);   /*Send higher nibble of data first to PORT*/
	RS = 1;                                  /*Data Register is selected*/
	EN = 1;                                  /*High-to-low pulse on Enable pin to latch data*/
	NOP();
	EN = 0;
	MSdelay(1);
    ldata = (ldata & 0x0f) | (dat<<4);               /*Send lower nibble of data to PORT*/
	EN = 1;                         /*High-to-low pulse on Enable pin to latch data*/
	NOP();
	EN = 0;
	MSdelay(3);
}

//print a string in the lcd 16x2
void LCD_String(const char *msg)
{
	while((*msg)!=0)
	{		
	  LCD_Char(*msg);
	  msg++;	
    }
}


//Print a string at the choosen position on the lcd 16x2
void LCD_String_xy(char row,char pos,const char *msg)
{
    char location=0;
    if(row<=1)
    {
        location=(0x80) | ((pos) & 0x0f);      /*Print message on 1st row and desired location*/
        LCD_Command(location);
    }
    else
    {
        location=(0xC0) | ((pos) & 0x0f);      /*Print message on 2nd row and desired location*/
        LCD_Command(location);    
    }  
    

    LCD_String(msg);

}
void LCD_Clear()
{
   	LCD_Command(0x01);     /*clear display screen*/
}

//Make a delay in milisecond with 8MHz clock input
void MSdelay(unsigned int val)
{
 unsigned int i,j;
 for(i=0;i<=val;i++)
     for(j=0;j<81;j++);             /*This count Provide delay of 1 ms for 8MHz Frequency */
 }

