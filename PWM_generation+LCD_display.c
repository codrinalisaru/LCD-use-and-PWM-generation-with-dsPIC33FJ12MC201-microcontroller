#include <xc.h>
#include "newfile.h"
#include <time.h>
#include <p33Fxxxx.h>
#include <stdlib.h>

#define RS PORTBbits.RB0
#define EN PORTBbits.RB1
#define D4 PORTBbits.RB4
#define D5 PORTBbits.RB7
#define D6 PORTBbits.RB12
#define D7 PORTBbits.RB13

char Buffer1[1];
char Buffer2[1];
char Buffer3[1];

void delay(int number_of_seconds) {
    int milli_seconds = 1000 * number_of_seconds;
    clock_t start_time = clock();
    while (clock() < start_time + milli_seconds)
        ;

}

void Lcd_SetBit(char data_bit) {
    if (data_bit & 1)
        D4 = 1;
    else
        D4 = 0;

    if (data_bit & 2)
        D5 = 1;
    else
        D5 = 0;

    if (data_bit & 4)
        D6 = 1;
    else
        D6 = 0;

    if (data_bit & 8)
        D7 = 1;
    else
        D7 = 0;
}

void Lcd_Cmd(char a) {
    RS = 0;
    Lcd_SetBit(a); //Incoming Hex value
    EN = 1;
    delay(4);
    EN = 0;
}

void Lcd_Clear() {
    Lcd_Cmd(0); //Clear the LCD
    Lcd_Cmd(1); //Move the curser to first position
}

void Lcd_Set_Cursor(char a, char b) {
    char temp;
    char z = 0;
    char y = 0;
    if (a == 1) {
        temp = 0x80 + b - 1; //80H is used to move the cursor
        z = temp >> 4; //Lower 8-bits
        y = temp & 0x0F; //Upper 8-bits
        Lcd_Cmd(z); //Set Row
        Lcd_Cmd(y); //Set Column
    } else if (a == 2) {
        temp = 0xC0 + b - 1;
        z = temp >> 4; //Lower 8-bits
        y = temp & 0x0F; //Upper 8-bits
        Lcd_Cmd(z); //Set Row
        Lcd_Cmd(y); //Set Column
    }
}

void Lcd_Start() {
    Lcd_SetBit(0x00);
    int i;
    for (i = 32767; i <= 0; i--) Nop();
    Lcd_Cmd(0x03);
    delay(5);
    Lcd_Cmd(0x03);
    delay(11);
    Lcd_Cmd(0x03);
    Lcd_Cmd(0x02); //02H is used for Return home -> Clears the RAM and initializes the LCD
    Lcd_Cmd(0x02); //02H is used for Return home -> Clears the RAM and initializes the LCD
    Lcd_Cmd(0x08); //Select Row 1
    Lcd_Cmd(0x00); //Clear Row 1 Display
    Lcd_Cmd(0x0C); //Select Row 2
    Lcd_Cmd(0x00); //Clear Row 2 Display
    Lcd_Cmd(0x06);
}

void Lcd_Print_Char(char data) //Send 8-bits through 4-bit mode
{
    char Lower_Nibble;
    char Upper_Nibble;
    Lower_Nibble = data & 0x0F;
    Upper_Nibble = data & 0xF0;
    RS = 1; //
    Lcd_SetBit(Upper_Nibble >> 4); //Send upper half by shifting by 4
    EN = 1;
    int i;
    for (i = 65535; i <= 0; i--) Nop();
    EN = 0;
    Lcd_SetBit(Lower_Nibble); //Send Lower half
    EN = 1;
    for (i = 65535; i <= 0; i--) Nop();
    EN = 0;
}

void Lcd_Print_String(char *a) {
    int i;
    for (i = 0; a[i] != '\0'; i++)
        Lcd_Print_Char(a[i]); //Split the string using pointers and call the Char function
}

void PWM_Init(void) {
    P2TPER = 183;
    P2TCONbits.PTMOD = 0b00;
    P2TCONbits.PTOPS = 0; // PWM timer post-scale
    P2TCONbits.PTCKPS = 0; // PWM timer pre-scale


    P2TMR = 0; // PWM counter value, start at 0

    PWM2CON1 = 0x11;
    PWM2CON1bits.PMOD1 = 0;
    PWM2CON1bits.PEN1H = 1; // PWM High pin is enabled
    PWM2CON1bits.PEN1L = 1; // PWM Low pin enabled (direction control later?)

    P2DTCON2bits.DTS1A = 0; //DeadTime value
    P2DTCON2bits.DTS1I = 1;

    P2OVDCONbits.POUT1H = 1;
    P2OVDCONbits.POUT1L = 0;
    P2OVDCONbits.POVD1H = 1;
    P2OVDCONbits.POVD1L = 0;

    P2DC1 = 0;
    P2TCONbits.PTEN = 1; // Enable PWM Timerbase!


}

char* itoa(int i, char b[]) {
    char const digit[] = "0123456789";
    char* p = b;
    if (i < 0) {
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do { //Move to where representation ends
        ++p;
        shifter = shifter / 10;
    } while (shifter);
    *p = '\0';
    do { //Move back, inserting digits as u go
        *--p = digit[i % 10];
        i = i / 10;
    } while (i);
    return b;
}

int main() {
    AD1PCFGL = 0xFFFE; //all pins set as digital except A0
    TRISA = 0;
    TRISB = 0;

    TRISAbits.TRISA0 = 1; //pin A0 set as input
    int ADCValue;
    int Vpot1;
    int Vpot2;
    int Vpot3;
//    int Vout1;
 //   int Vout2;
//    int Vout3;
    float Vout = 0;
    float DutyCycle = 0;

    AD1CON1bits.ADON = 0;
    AD1CON1bits.AD12B = 1;
    AD1CON1bits.ASAM = 0;
    AD1CON1 = 0x0E4;

    AD1CHS0 = 0; // set channel 0 for sampling
    AD1CSSL = 0;

    AD1CON2 = 0;
    AD1CON3 = 0x1F05;

    AD1CON1bits.ADON = 1; // turn ADC ON


    Lcd_Start();
    PWM_Init();
    while (1) {
        while (AD1CON1bits.DONE) {
            ADCValue = ADC1BUF0; // get ADC value
            ADCValue = ADCValue * 4.9;
            Vpot1 = ADCValue / 998;
            itoa(Vpot1, Buffer1); //saves voltage from potentiometer into a string
            Vpot2 = (ADCValue / 100) % 10;
            itoa(Vpot2, Buffer2);
            Vpot3 = (ADCValue / 10) % 10;
            itoa(Vpot3, Buffer3);
           /* Vout1 = atoi(Buffer1);
            Vout2 = atoi(Buffer2);
            Vout3 = atoi(Buffer3);*/
            Vout = (ADCValue / 1000) + (((ADCValue / 100) % 10) / 100) + (((ADCValue / 10) % 10) / 10);
        }
        DutyCycle = (Vout * 20) / 100;
        P2DC1 = DutyCycle * 2 * P2TPER;
        int i;
        Lcd_Clear();
        Lcd_Set_Cursor(1, 2);
        Lcd_Print_String("V_potentiometer:");
        Lcd_Set_Cursor(2, 2);
        for (i = 0; i < 1; i++) {
            Lcd_Print_Char(Buffer1[i]);
            Lcd_Print_String(".");
            Lcd_Print_Char(Buffer2[i]);
            Lcd_Print_Char(Buffer3[i]);
        }
        Lcd_Print_Char(0x00);
        Lcd_Print_Char(0x00);
        delay(200);

    }
}