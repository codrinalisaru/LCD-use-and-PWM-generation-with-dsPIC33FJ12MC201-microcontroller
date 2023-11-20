#include <xc.h>
#include "newfile.h"
#include <time.h>
#include <p33Fxxxx.h>



void PWM_Init( void )
{
    
   
    P2TPER = 181; //pe 10 biti
    P2TCONbits.PTMOD = 0b00; 
    P2TCONbits.PTOPS = 0; // PWM timer post-scale
    P2TCONbits.PTCKPS = 0;// PWM timer pre-scale
    
 
    P2TMR = 0; // PWM counter value, start at 0
   
    PWM2CON1 = 0x11;
    PWM2CON1bits.PMOD1 = 0;
    PWM2CON1bits.PEN1H = 1; // PWM High pin is enabled
    PWM2CON1bits.PEN1L = 1; // PWM Low pin enabled (direction control later?)
 
   P2DTCON2bits.DTS1A = 0;   //DeadTime value
   P2DTCON2bits.DTS1I =1;
    
    P2OVDCONbits.POUT1H = 1;
    P2OVDCONbits.POUT1L = 0;
    P2OVDCONbits.POVD1H = 1;
    P2OVDCONbits.POVD1L = 0;
    
    P2DC1 = 0;
    
    P2TCONbits.PTEN = 1; // Enable PWM Timerbase!
    
    
}


int main() {
    AD1PCFGL = 0xFFFF;
    TRISA=0;
    TRISB = 0; 
    
    PWM_Init();

    while(1){  
        P2DC1 = 0.50*2* P2TPER;
    }
 
}