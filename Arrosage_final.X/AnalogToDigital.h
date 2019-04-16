
#ifndef ADC_H
#define	ADC_H

#include"ConfigurationPIC18F4550.h" //We include the PIC18F4550 properties
#include <stdio.h>                  //We include the standards libraries
#include <stdlib.h>
/*This file contains the function related to ADC. Weirdly the compiler did not want us to move the adc read function of the main in
 this file once we declared them in the main. Even after deleting all the codes about the adc_read function of the main and putting it here
 we still get the error "redefinition of 'adc_read_humid'" and so on.*/

    unsigned char l;            //--- To Store Lower 8 bit of ADC o/p
    unsigned int h,t;           //--- To Store Higher 2 bit of ADC o/p

//Analog to Digital Converter INITIALIZATION FUNCTION
    void adc_init()
    {
        TRISA = 0xFF;            //--- PORTA is consider as input
        ADCON0bits.ADON = 1;    //--- We enable the ADC
        ADCON1 = 0x09;          //--- Channel 0-5 are Analog and Vref=Vdd+Vss
        ADCON2 = 0xAD;          //--- Right Adjust and Clcok = F_CPU/16
    }
    
    //Convert the hexadecimal value to a decimal one
    unsigned int hex_converter(unsigned int x)
    {
        char convert[3];
        unsigned int result;
        convert[0]=((x)/1000)%10 | (0x30);
        convert[1]=((x)/100)%10 | (0x30);
        convert[2]=((x)/10)%10 | (0x30);
        convert[3]=((x))%10 | (0x30);
        result=atoi(convert);
        return(result);
    }
    


#endif	