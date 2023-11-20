#include <xc.h>
#include "newfile.h"
#include <time.h>
#include <p33Fxxxx.h>
#include <stdlib.h>

#define RS PORTBbits.RB13
#define EN PORTBbits.RB8
#define D4 PORTBbits.RB4
#define D5 PORTBbits.RB9
#define D6 PORTBbits.RB12
#define D7 PORTBbits.RB0

char Buffer1[1];
char Buffer2[1];
char Buffer3[1];

char Buffer4[2];
char Buffer5[1];

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
    Lcd_Cmd(1); //Move the cursor  to first position
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
    AD1PCFGL = 0xFFF7; //all pins set as digital except AN3
    TRISA = 0;
    TRISB = 0;

    TRISBbits.TRISB1 = 1; //pin AN3 set as input
    int ADCValue;
    int Vpot1;
    int Vpot2;
    int Vpot3;

    int Vout;
    int O1, O2;


    AD1CON1 = 0x0106;
    AD1CHS0 = 0x0303; // set channel 0 for sampling
    AD1CSSL = 0x0008;
    AD1CON1 = 0x0E4;
    AD1CON2 = 0;
    AD1CON3 = 0x1F05; // Sample time manual, Tad = internal 2 Tcy
    AD1CON1bits.ADON = 1; // turn ADC ON

    while (1) {
        Lcd_Start();
        AD1CON1bits.SAMP = 0;
        while (AD1CON1bits.DONE) {
            ADCValue = ADC1BUF0; // get ADC value
            ADCValue = ADCValue * 4.9;
            Vout = ADCValue;

            Vpot1 = ADCValue / 998;
            itoa(Vpot1, Buffer1); //saves voltage from potentiometer into a string
            Vpot2 = (ADCValue / 100) % 10;
            itoa(Vpot2, Buffer2);
            Vpot3 = (ADCValue / 10) % 10;
            itoa(Vpot3, Buffer3);

            O1 = Vout / 125.002;
            itoa(O1, Buffer4);
            O2 = (Vout / 100) % 10;
            itoa(O2, Buffer5);


        }

        int i;
        Lcd_Clear();
        Lcd_Set_Cursor(1, 2);
        Lcd_Print_String("V_ref:");
        Lcd_Set_Cursor(2, 2);
        for (i = 0; i < 1; i++) {
            Lcd_Print_Char(Buffer1[i]);
            Lcd_Print_String(".");
            Lcd_Print_Char(Buffer2[i]);
            Lcd_Print_Char(Buffer3[i]);
            Lcd_Print_String(" V");
        }
        delay(200);

        Lcd_Clear();
        Lcd_Set_Cursor(1, 2);
        Lcd_Print_String("V_out:");
        Lcd_Set_Cursor(2, 2);
        Lcd_Print_Char(Buffer4[0]);
        Lcd_Print_Char(Buffer4[1]);
        Lcd_Print_String(".");
        Lcd_Print_Char(Buffer5[0]);
        Lcd_Print_String(" V");
        Lcd_Print_Char(0x00);
        delay(200);

    }
}