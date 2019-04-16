/* 
 * File:   MainArossage.c
 * Author: Nicolas C
 *
 * Created on 7 novembre 2018, 09:21
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AnalogToDigital.h"
#include "ConfigurationPIC18F4550.h"
#include "LCD4BITS.h"
#include "servomoteur.h"

void lcd_lum(unsigned int l); /*Print the value of the luminosity on the LCD*/
void lcd_sol(unsigned int s); /*Print the value of the soil on the LCD*/
void lcd_humid(unsigned int h);/*Print the value of the humidity on the LCD*/
void lcd_temp(unsigned int t);/*Print the value of the temperature on the LCD*/
unsigned int adc_read_lum(); /*Read the value of the luminosity on the analog pin RA0*/
unsigned int adc_read_sol(); /*Read the value of the soil on the analog pin RA1*/
unsigned int adc_read_humid(); /*Read the value of the humidity on the analog pin RA2*/
unsigned int adc_read_temp(); /*Read the value of the temperature on the analog pin RA0*/
int humid_convert(unsigned int x); /*convert the analog value of humidity between 0 and 1023 to the % scale*/
int temp_convert(unsigned int x);/*convert the analog value of temperature between 0 and 1023 to the Celcius scale*/




int main(int argc, char** argv) {

    const int limit_humidity_soil=350;
    
    unsigned int x_lum;
    unsigned int lum;
    
    unsigned int x_sol;
    unsigned int sol;
    
    unsigned int x_humid;
    unsigned int humid;
    
    unsigned int x_temp;
    unsigned int temp;
   
    PWM_Init();
    int Period = setPeriodTo(50);
    SetDutyCycleTo(3.0,Period);
    
     
    
    while(1){
        OSCCON = 0x72;                        /*Use internal oscillator and set frequency to 8 MHz*/
        LCD_Init();
        adc_init();
    //On récupère les valeurs des pins du port A
        //MSdelay(2000);
        //LCD_String_xy(1,1,"debut");
       // MSdelay(2000);
        //LCD_Clear();
        LCD_String_xy(1,1,"Garden Infos");
        MSdelay(2000);
        x_lum=adc_read_lum();
        LCD_Clear();
        lum=hex_converter(x_lum);
        lcd_lum(lum);
        //MSdelay(4000);
        
        x_sol=adc_read_sol();
        LCD_Clear();
        sol=hex_converter(x_sol);
        lcd_sol(sol);
        
        x_humid=adc_read_humid();
        LCD_Clear();
        humid=hex_converter(x_humid);
        lcd_humid(humid);
        
        x_temp=adc_read_temp();
        LCD_Clear();
        temp=hex_converter(x_temp);
        lcd_temp(temp);
        
        if(x_sol<limit_humidity_soil)stream(temp, humid, lum);
	}
}


/*Print the value of the luminosity on the LCD*/
void lcd_lum(unsigned int l)
{
    //verif
    char value[3]="0000";
    sprintf(value,"%d",l);
    char lum_info[16]="NightNDay: ";
    strcat(lum_info, value);
    LCD_String_xy(1,1,lum_info);
    
    if(l<300)
    {
        LCD_String_xy(2,1,"Il fait nuit");
    }
    else 
    {
        LCD_String_xy(2,1,"Il fait jour");
    }
    
    MSdelay(1000);
    //LCD_Clear();
}

/*Print the value of the soil on the LCD*/
void lcd_sol(unsigned int s)

{
    char value[3]="0000";
    sprintf(value,"%d",s);
    char sol_info[16]="Soil State: ";
    strcat(sol_info,value);
    LCD_String_xy(1,1,sol_info);
    //MSdelay(1000);
    if(s<300)
    {
        LCD_String_xy(2,1,"Soil is dry");
    }
    else if (s>=300 && s<700)
    {
        LCD_String_xy(2,1,"Soil is wet");
    }
    
    else  
    {
        LCD_String_xy(2,1,"Soil is soaked");
    }
    
    MSdelay(1000);
    //LCD_Clear();
}

/*Print the value of the humidity on the LCD*/
void lcd_humid(unsigned int h)
{
    char value[3]="0000";
    //char pro[2]=" %";
    /*sprintf(value,"%d",h);
    /*char humid_info[16]="Humidity: ";
    
    strcat(humid_info,value);
    strcat(humid_info,pro);
    LCD_String_xy(1,1,value);
    MSdelay(1000);
    LCD_Clear();*/
    int humid=humid_convert(h);
    char humidity[3]="000";
    sprintf(humidity,"%d",humid);
    char humid_info[16]="Humidity %: ";
    strcat(humid_info,humidity);
    //strcat(humid_info,pro);
    LCD_String_xy(1,1,humid_info);
    //LCD_String_xy(1,1,humidity);
    if(h<500)
    {
        LCD_String_xy(2,1,"Il fait sec");
    }
    else 
    {
        LCD_String_xy(2,1,"Il fait humide");
    }
    
    MSdelay(1000);
    //LCD_Clear();
}

/*Print the value of the temperature on the LCD*/
void lcd_temp(unsigned int t)
{
    char value[3]="0000";
   /* sprintf(value,"%d",t);
    LCD_String_xy(1,1,value);
    MSdelay(1000);
    LCD_Clear();*/
    int temp=temp_convert(t);
    char temperature[3]="000";
    sprintf(temperature,"%d",temp);
    char temp_info[16]="Temp C: ";
    strcat(temp_info,temperature);
    LCD_String_xy(1,1,temp_info);
    
    //MSdelay(1000);
if(t<500)
    {
        LCD_String_xy(2,1,"Il fait froid");
    }
    else 
    {
        LCD_String_xy(2,1,"Il fait chaud");
    }
    
    MSdelay(1000);
    //LCD_Clear();
}

/*Read the value of the luminosity on the analog pin RA0*/
unsigned int adc_read_lum()
    {
     
            ADCON0bits.CHS3 =0;
            ADCON0bits.CHS2 =0;
            ADCON0bits.CHS1 =0;
            ADCON0bits.CHS0 =0;
      
        
        //ADCON0 |= ((0x3C) & (0<<2));  //--- To select channel as desired
        ADCON0bits.GO = 1;            //--- Start of Conversion
        while(!ADCON0bits.DONE == 1); //--- End of Conversion Check
        ADCON0bits.GO = 1;            //--- Enable ADC Conversion once again
        MSdelay(3000);
        l = ADRESL;                   //--- Lower 8 bit Data of ADC
        h = ADRESH;                   //--- Higher 2 bit Data of ADC
        //ADRESL=0x00;
        //ADRESH=0X00;
        h = h << 8;                   //--- Shift 2 bit to higher 8 bit
        t = h | l;                    //--- Add 8 lower bit and 2 higher bit
        return(t);
        
    }
    
/*Read the value of the soil on the analog pin RA1*/
    unsigned int adc_read_sol()
    {
     
            ADCON0bits.CHS3 =0;
            ADCON0bits.CHS2 =0;
            ADCON0bits.CHS1 =0;
            ADCON0bits.CHS0 =1;
      
        
        //ADCON0 |= ((0x3C) & (0<<2));  //--- To select channel as desired
        ADCON0bits.GO = 1;            //--- Start of Conversion
        while(!ADCON0bits.DONE == 1); //--- End of Conversion Check
        ADCON0bits.GO = 1;            //--- Enable ADC Conversion once again
        MSdelay(3000);
        
        l = ADRESL;                   //--- Lower 8 bit Data of ADC
        h = ADRESH;                   //--- Higher 2 bit Data of ADC
        ADRESL=0x00;
        ADRESH=0X00;
        h = h << 8;                   //--- Shift 2 bit to higher 8 bit
        t = h | l;                    //--- Add 8 lower bit and 2 higher bit
        return(t);
        
    }
    
    /*Read the value of the humidity on the analog pin RA2*/
    unsigned int adc_read_humid()
    {
     
            ADCON0bits.CHS3 =0;
            ADCON0bits.CHS2 =0;
            ADCON0bits.CHS1 =1;
            ADCON0bits.CHS0 =0;
      
        
        //ADCON0 |= ((0x3C) & (0<<2));  //--- To select channel as desired
        ADCON0bits.GO = 1;            //--- Start of Conversion
        while(!ADCON0bits.DONE == 1); //--- End of Conversion Check
        ADCON0bits.GO = 1;            //--- Enable ADC Conversion once again
        MSdelay(3000);
        
        l = ADRESL;                   //--- Lower 8 bit Data of ADC
        h = ADRESH;                   //--- Higher 2 bit Data of ADC
        ADRESL=0x00;
        ADRESH=0X00;
        h = h << 8;                   //--- Shift 2 bit to higher 8 bit
        t = h | l;                    //--- Add 8 lower bit and 2 higher bit
        return(t);
        
    }
    
    /*Read the value of the temperature on the analog pin RA0*/
    unsigned int adc_read_temp()
    {
     
            ADCON0bits.CHS3 =0;
            ADCON0bits.CHS2 =0;
            ADCON0bits.CHS1 =1;
            ADCON0bits.CHS0 =1;
      
        
        //ADCON0 |= ((0x3C) & (0<<2));  //--- To select channel as desired
        ADCON0bits.GO = 1;            //--- Start of Conversion
        while(!ADCON0bits.DONE == 1); //--- End of Conversion Check
        ADCON0bits.GO = 1;            //--- Enable ADC Conversion once again
        MSdelay(3000);
        
        l = ADRESL;                   //--- Lower 8 bit Data of ADC
        h = ADRESH;                   //--- Higher 2 bit Data of ADC
        ADRESL=0x00;
        ADRESH=0X00;
        h = h << 8;                   //--- Shift 2 bit to higher 8 bit
        t = h | l;                    //--- Add 8 lower bit and 2 higher bit
        return(t);
        
    }

     /*convert the analog value of humidity between 0 and 1023 to the % scale*/
    int humid_convert(unsigned int x)
    {
        
        int humidity;
        
        if (x<50){humidity=5;}
        else if (x>=50 && x<100){humidity=10;}
        else if (x>=100 && x<150){humidity=15;}
        else if (x>=150 && x<200){humidity=20;}
        else if (x>=200 && x<250){humidity=25;}
        else if (x>=250 && x<300){humidity=30;}
        else if (x>=300 && x<350){humidity=35;}
        else if (x>=350 && x<400){humidity=40;}
        else if (x>=400 && x<450){humidity=45;}
        else if (x>=450 && x<500){humidity=50;}
        else if (x>=500 && x<550){humidity=55;}
        else if (x>=550 && x<600){humidity=60;}
        else if (x>=600 && x<650){humidity=65;} 
        else if (x>=650 && x<700){humidity=70;}
        else if (x>=700 && x<750){humidity=75;}
        else if (x>=750 && x<800){humidity=80;}
        else if (x>=800 && x<850){humidity=85;}
        else if (x>=850 && x<900){humidity=90;}
        else if (x>=900 && x<950){ humidity=95;}
        else {humidity=100;}
        
        return humidity;
        
        
    }
    
    /*convert the analog value of temperature between 0 and 1023 to the Celcius scale*/
    int temp_convert(unsigned int x)
    {
        
        int temperature;
        
        if (x<25){temperature=0;}
        else if (x>=25 && x<50){temperature=1;}
        else if (x>=50 && x<75){temperature=2;}
        else if (x>=75 && x<100){temperature=3;}
        else if (x>=100 && x<125){temperature=4;}
        else if (x>=125 && x<150){temperature=5;}
        else if (x>=150 && x<175){temperature=6;}
        else if (x>=175 && x<200){temperature=7;}
        else if (x>=200 && x<225){temperature=8;}
        else if (x>=225 && x<250){temperature=9;}
        else if (x>=250 && x<275){temperature=10;}
        else if (x>=275 && x<300){temperature=11;}
        else if (x>=300 && x<325){temperature=12;} 
        else if (x>=325 && x<350){temperature=13;}
        else if (x>=350 && x<375){temperature=14;}
        else if (x>=375 && x<400){temperature=15;}
        else if (x>=400 && x<425){temperature=16;}
        else if (x>=425 && x<450){temperature=17;}
        else if (x>=450 && x<475){temperature=18;}
        else if (x>=475 && x<500){temperature=19;}
        else if (x>=500 && x<525){temperature=20;}
        else if (x>=525 && x<550){temperature=21;}
        else if (x>=550 && x<575){temperature=22;}
        else if (x>=575 && x<600){temperature=23;}
        else if (x>=600 && x<625){temperature=24;}
        else if (x>=625 && x<650){temperature=25;}
        else if (x>=650 && x<675){temperature=26;}
        else if (x>=675 && x<700){temperature=27;}
        else if (x>=700 && x<725){temperature=28;}
        else if (x>=725 && x<750){temperature=29;}
        else if (x>=750 && x<775){temperature=30;}
        else if (x>=775 && x<800){temperature=31;}
        else if (x>=800 && x<825){temperature=32;}
        else if (x>=825 && x<850){temperature=33;}
        else if (x>=850 && x<875){temperature=34;}
        else if (x>=875 && x<900){temperature=35;}
        else if (x>=900 && x<925){temperature=36;}
        else if (x>=925 && x<950){temperature=37;}
        else if (x>=950 && x<975){temperature=38;}
        else if (x>=975 && x<1000){temperature=39;}
        else {temperature=40;}
        
        return temperature;
        
        
    }