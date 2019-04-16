
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <pic18f4550.h>
#include <stdio.h>
#include <math.h>
#include "ConfigurationPIC18F4550.h"

#define MINTHR              8000
#define RESOLUTION          488

#define InternalOsc_8MHz    8000000
#define InternalOsc_4MHz    4000000
#define InternalOsc_2MHz    2000000
#define InternalOsc_1MHz    1000000
#define InternalOsc_500KHz  500000
#define InternalOsc_250KHz  250000
#define InternalOsc_125KHz  125000
#define InternalOsc_31KHz   31000

#define Timer2Prescale_1    1
#define Timer2Prescale_4    4
#define Timer2Prescale_16   16

void PWM_Init();
int setPeriodTo(unsigned long FPWM);
void SetDutyCycleTo(float Duty_cycle, int Period);
void delay(unsigned int val);
void arrosage(float duty_cycle,int Period,int time);
void stream(int temperature, int air_humidity, int photoresistance) ;
void lcd_arrosage(float angle, int time);

//Initialization function
void PWM_Init()            /* Initialize PWM */
{
    TRISCbits.TRISC2 = 0;  /* Set CCP1 pin as output for PWM out */
    CCP1CON = 0x0C;        /* Set PWM mode */
}

int setPeriodTo(unsigned long FPWM)/* Set period */
{
    int clockSelectBits, TimerPrescaleBits;
    int TimerPrescaleValue;
    float period;
    unsigned long FOSC, _resolution = RESOLUTION;

    if (FPWM < MINTHR)    {TimerPrescaleBits = 2; TimerPrescaleValue = Timer2Prescale_16;}
    else                  {TimerPrescaleBits = 0; TimerPrescaleValue = Timer2Prescale_1;}

    if (FPWM > _resolution)               {clockSelectBits = 7; FOSC = InternalOsc_8MHz;}
    else if (FPWM > (_resolution >>= 1))  {clockSelectBits = 6; FOSC = InternalOsc_4MHz;}
    else if (FPWM > (_resolution >>= 1))  {clockSelectBits = 5; FOSC = InternalOsc_2MHz;}
    else if (FPWM > (_resolution >>= 1))  {clockSelectBits = 4; FOSC = InternalOsc_1MHz;}
    else if (FPWM > (_resolution >>= 1))  {clockSelectBits = 3; FOSC = InternalOsc_500KHz;}
    else if (FPWM > (_resolution >>= 1))  {clockSelectBits = 2; FOSC = InternalOsc_250KHz;}
    else if (FPWM > (_resolution >>= 1))  {clockSelectBits = 1; FOSC = InternalOsc_125KHz;}
    else                                  {clockSelectBits = 0; FOSC = InternalOsc_31KHz;}

    period = ((float)FOSC / (4.0 * (float)TimerPrescaleValue * (float)FPWM)) - 1.0;
    period = round(period);

    OSCCON = ((clockSelectBits & 0x07) << 4) | 0x02;
    PR2 = (int)period;
    T2CON = TimerPrescaleBits;
    TMR2 = 0;
    T2CONbits.TMR2ON = 1;  /* Turn ON Timer2 */
    return (int)period;
}

//This function sends the correct PWM to place correctly the servomoteur.
void SetDutyCycleTo(float Duty_cycle, int Period)
{
    int PWM10BitValue;

    PWM10BitValue = 4.0 * ((float)Period + 1.0) * (Duty_cycle/100.0);
    CCPR1L = (PWM10BitValue >> 2);
    CCP1CON = ((PWM10BitValue & 0x03) << 4) | 0x0C;
}


void delay(unsigned int val)
{
     unsigned int i,j;
        for(i=0;i<val;i++)
            for(j=0;j<10;j++);//Do one millisecond for 10 iterations
}

void lcd_arrosage(float angle, int time)
{
    char data[16]="Ag:";
    if(angle==7.6)strcat(data,"90");
    else if(angle==12.0)strcat(data,"180");
    strcat(data," Time:");
    char time2[2];
    sprintf(time2,"%d",time/1000);
    strcat(data,time2);
    strcat(data,"s");
    LCD_String_xy(1,1,"    Arrosage");
    LCD_String_xy(2,1,data);
}


//Cette fonction permet d'ouvrir le moteur pour une certaine durée
void arrosage(float duty_cycle,int Period,int time)
{
    SetDutyCycleTo(duty_cycle,Period);
    delay(time);
}


void stream(int temperature, int air_humidity, int photoresistance) 
{
    //Variables des valeurs des capteurs
    /*int temperature=0;//Temperature
    int air_humidity=0;//Humidité de l'air 
    int soil_humidity=0;//Humidité de la terre
    int photoresistance=0;//Quand il fait jour, on évite d'arroser*/
    
    //Constantes permettant de prendre les décisions pour le moteur
    const float servo_init=3.0;
    const float servo_mid=7.6;
    const float servo_max=12.0;
    const int time_mid=5000;
    const int time_max=10000;
    const int limit_temperature=500;
    const int limit_humidity_air=500;
    
    const int limit_time=300;
    
    int Period;
    //PWM_Init();                 /* Initialize PWM */
    Period = setPeriodTo(50);   /* 50Hz PWM frequency */
    /* Note that period step size will gradually increase with PWM frequency */

        //Si l'humidité du sol devient trop basse dans ce cas, 
        //on active l'arrosage selon plusieurs cas
        //Nos règles suivent trois lois
            
            /*
            1- A partir du moment où il fait jour, on arrose peu et pas longtemps
            Car c'est mauvais pour les plantes d'arroser sous le soleil
            
            2- Plus il fait chaud, plus on arrose
            
            3- Plus l'humidité de l'air est forte, moins on arrose
            */
        
        //if temperature est élevée et l'humidité est faible et il fait nuit
        if(temperature>limit_temperature && air_humidity<limit_humidity_air && photoresistance<limit_time)
            {lcd_arrosage(servo_max,time_max);arrosage(servo_max,Period,time_max);}
        if(temperature>limit_temperature && air_humidity>limit_humidity_air && photoresistance<limit_time)
            {lcd_arrosage(servo_max,time_mid);arrosage(servo_max,Period,time_mid);}
        if(temperature<limit_temperature && air_humidity<limit_humidity_air && photoresistance<limit_time)
            {lcd_arrosage(servo_mid,time_max);arrosage(servo_mid,Period,time_max);}
        if(temperature<limit_temperature && air_humidity>limit_humidity_air && photoresistance<limit_time)
            {lcd_arrosage(servo_mid,time_mid);arrosage(servo_mid,Period,time_mid);}

        //if temperature est élevée et l'humidité est faible et il fait jour
        if(temperature>limit_temperature && air_humidity<limit_humidity_air && photoresistance>limit_time)
            {lcd_arrosage(servo_mid,time_max);arrosage(servo_mid,Period,time_max);}
        if(temperature>limit_temperature && air_humidity>limit_humidity_air && photoresistance>limit_time)
            {lcd_arrosage(servo_mid,time_max);arrosage(servo_mid,Period,time_max);}
        if(temperature<limit_temperature && air_humidity<limit_humidity_air && photoresistance>limit_time)
            {lcd_arrosage(servo_mid,time_mid);arrosage(servo_mid,Period,time_mid);}
        if(temperature<limit_temperature && air_humidity>limit_humidity_air && photoresistance>limit_time)
            {lcd_arrosage(servo_mid,time_mid);arrosage(servo_mid,Period,time_mid);}    
            
        
    //On commence toujours avec le servo_moteur fermé
        SetDutyCycleTo(servo_init,Period);
        delay(5000);
 }


    //SetDutyCycleTo(3.0, Period);    /* 3% duty cycle -90° */
    //SetDutyCycleTo(7.0, Period);    /* 7% duty cycle 0° */
    //SetDutyCycleTo(12.0, Period);    /* 12% duty cycle +90° */




#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

