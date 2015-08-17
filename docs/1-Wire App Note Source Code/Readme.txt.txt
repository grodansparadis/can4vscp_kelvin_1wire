The 1-Wire Protocol is registered trade mark of Dallas/Maxim semiconductor.

This application source code is mainly intended for PIC users to easily develop 1-Wire protocol using PIC microcontoller.

This application is developed to read 64-bit serial number from DS2411 1-Wire slave device using PIC18F4680 microcontroller, which will be displayed on the hyper terminal.

1) 1wire.c contains the information on 1-wire APIs explained in the Application Note.

2) demowire.c contains the information on how to read 64-bit serial number from DS2411. It initialize the USART to display 
   the serial number on the terminal.

3) 1-wire.h contains the information on function prototypes.

4) Config.h contains the information on how to select the various clocks. Depending upon the clock, the 'wait' macro will give approximately 1us delay. This application uses 20MHz clock frequency. This header file will also allow us to 
   configure UART pOrt pins as well as 1-wire port pin.

