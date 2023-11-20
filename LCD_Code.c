#include <xc.h>
#include "newfile.h"
#include <time.h>


#define RS PORTBbits.RB0
#define EN PORTBbits.RB1
#define D4 PORTBbits.RB4
#define D5 PORTBbits.RB7
#define D6 PORTBbits.RB12
#define D7 PORTBbits.RB13


void delay(int number_of_seconds) 
{ 
    int milli_seconds = 1000 * number_of_seconds; 
    clock_t start_time = clock(); 
    while (clock() < start_time + milli_seconds) 
    ; 

} 

void Lcd_SetBit(char data_bit) 
{
    if(data_bit& 1) 
        D4 = 1;
    else
        D4 = 0;

    if(data_bit& 2)
        D5 = 1;
    else
        D5 = 0;

    if(data_bit& 4)
        D6 = 1;
    else
        D6 = 0;

    if(data_bit& 8) 
        D7 = 1;
    else
        D7 = 0;
}

void Lcd_Cmd(char a)
{
    RS = 0;           
    Lcd_SetBit(a); //Incoming Hex value
    EN  = 1;         
    delay(4);
    EN  = 0;      
}

void Lcd_Clear()
{
    Lcd_Cmd(0); //Clear the LCD
    Lcd_Cmd(1); //Move the curser to first position
}

void Lcd_Set_Cursor(char a, char b)
{
    char temp;
    char z=0;
    char y=0;
    if(a== 1)
    {
        temp = 0x80 + b - 1; //80H is used to move the cursor
        z = temp>> 4; //Lower 8-bits
        y = temp & 0x0F; //Upper 8-bits
        Lcd_Cmd(z); //Set Row
        Lcd_Cmd(y); //Set Column
    }
    else if(a== 2)
    {
        temp = 0xC0 + b - 1;
        z = temp>>4; //Lower 8-bits
        y = temp & 0x0F; //Upper 8-bits
        Lcd_Cmd(z); //Set Row
        Lcd_Cmd(y); //Set Column
    }
}

void Lcd_Start()
{
  Lcd_SetBit(0x00);
  int i;
  for(i=32767; i<=0; i--) Nop() ;  
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

void Lcd_Print_Char(char data)  //Send 8-bits through 4-bit mode
{
   char Lower_Nibble;
   char Upper_Nibble;
   Lower_Nibble = data&0x0F;
   Upper_Nibble = data&0xF0;
   RS = 1;             // 
   Lcd_SetBit(Upper_Nibble>>4);             //Send upper half by shifting by 4
   EN = 1;
   int i;
   for(i=65535; i<=0; i--) Nop() ; 
   EN = 0;
   Lcd_SetBit(Lower_Nibble); //Send Lower half
   EN = 1;
   for(i=65535; i<=0; i--) Nop() ;
   EN = 0;
}

void Lcd_Print_String(char *a)
{
    int i;
    for(i=0;a[i]!='\0';i++)
       Lcd_Print_Char(a[i]);  //Split the string using pointers and call the Char function 
}



int main() {

    TRISA = 0;
    TRISB = 0;
    AD1PCFGL = 0xFFFF;
                     
    Lcd_Start();
    while(1){
        Lcd_Clear();
        Lcd_Set_Cursor(1,2);
        Lcd_Print_String("materia asta");
        Lcd_Set_Cursor(2,2);
        Lcd_Print_String("nu are sens");
        Lcd_Print_Char(0x00);
        Lcd_Print_Char(0x00);
        delay(200);
    }
        return 0;
}