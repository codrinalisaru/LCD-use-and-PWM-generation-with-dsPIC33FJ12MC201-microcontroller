This code was used in my bachelor's degree thesis, as part of a DC-DC source, and the code's purpose was to display a measured voltage and a reference voltage and also to generate PWM signal as a back-up measure for a NE555 circuit. 

The .pdf file contains some simulations of the code, in Proteus, and some simulations of the whole circuit, made in LTspice. 

The files posted here on github and what you need to know about them:
- LCD_Code.c : an example of how to use an LCD display and how to use pins and ports;
- PWM_Generation.c : an example of how to use the PWM generator circuit of the dsPIC33FJ12 microcontroller;
- PWM_generation+LCD_display.c : an example of how to use the 2 features above, combined. 

Inside the folder Licenta.X you will find:
- the main C file: newfile.c;
- the header file: newfile.h; 
- the hex file in dist - default - production, file you will use to simulate the microcontroller's behaviour and the code, in a program such as Proteus. 

You will find some useful comments in the C main file also. 

I created the code in MPLAB X IDE, version 5.15
