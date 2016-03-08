/*********************************************************************
 *
 *                 1-Wire Protocol Application developed for DS2411
 *
 *********************************************************************
 * FileName:        demowire.c
 * Dependencies:
 * Processor:       PIC18
 * Complier:        MCC18 v3.13
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright © 2004-2007 Microchip Technology Inc.  All rights reserved.
 *
 * Microchip licenses to you the right to use, copy and distribute Software 
 * only when embedded on a Microchip microcontroller or digital signal 
 * controller and used with a Microchip radio frequency transceiver, which 
 * are integrated into your product or third party product (pursuant to the 
 * sublicense terms in the accompanying license agreement).  You may NOT 
 * modify or create derivative works of the Software.  
 *
 *
 * You should refer to the license agreement accompanying this Software for 
 * additional information regarding your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY 
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY 
 * OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR 
 * PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED 
 * UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF 
 * WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR 
 * EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, 
 * PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF 
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY 
 * THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER 
 * SIMILAR COSTS.
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Sasha     			12/20/07    Original
 ********************************************************************/

//** I N C L U D E S **********************************************************/
#include <p18f4580.h>
#include "Config.h"
#include "1wire.h"


//******* G E N E R I C   D E F I N I T I O N S ************************************************/
#define BAUD_CONTROL			00
#define READ_COMMAND_DS2411  	0x33         // Command to read the 64-bit serial number from 1-wire slave device.


//****** V A R I A B L E S ********************************************************/
unsigned char welcome_message[] = "I AM A MASTER DEVICE\r\n";
unsigned char not_present[] = "SLAVE Device is not present\r\n";
unsigned char dev_present [] = "SLAVE Device is present\r\n";
unsigned char ser_num [] = "SERIAL NUMBER:  \r\n";

//*************************************************************************

//****** P R O T O T Y P E S ******************************************************/
void PutString (unsigned char *mess);
unsigned char Getchar(void);
void Putchar (unsigned char);
unsigned char Detect_Slave_Device(void);
void hex_ascii(unsigned char);

//*************************************************************************

/******************************************************************************
/** I N T E R R U P T  V E C T O R S *****************************************/
/******************************************************************************/
#pragma code high_vector=0x08
void interrupt_at_high_vector(void)
{
   _asm goto high_isr _endasm
}
#pragma code

#pragma code low_vector=0x18
void interrupt_at_low_vector(void)
{
   _asm goto low_isr _endasm
}
#pragma code


/******************************************************************************
* Function:        void high_isr(void)
* PreCondition:    None
* Input:		   None
* Output:		   None	
* Side Effects:    None
* Overview:        None
*****************************************************************************/
#pragma interrupt high_isr
void high_isr(void)
{

}

/**********************************************************************
* Function:        void low_isr(void)
* PreCondition:    None
* Input:		   None
* Output:		   None	
* Side Effects:    None
* Overview:        None
***********************************************************************/
#pragma interruptlow low_isr
void low_isr(void)
{
}
#pragma code


/**********************************************************************
* Function:        main function.
* PreCondition:    None
* Input:		   None
* Output:		   None	
* Overview:		   1-wire protocol develop for DS2411
***********************************************************************/

void main(void)
{

   unsigned char temp;
   unsigned char temp1;
   unsigned char serial_number[8];

   initEUSART();							// Initilization of USART

   PutString(welcome_message);				// Display Welocme Message on Terminal.

   if(!Detect_Slave_Device())	    		// Is slave present???
   		PutString(not_present);				// Send message to terminal showing 1-wire slave device not present.
   else
   		PutString(dev_present);				// Send message to terminal showing 1-wire slave device present.
   		
   OW_write_byte (READ_COMMAND_DS2411);		// Send a command to read a serial number

   PutString(ser_num);						// Display the message on terminal.
	
   for(temp = 0; temp<8; temp++)
   		serial_number[temp] = OW_read_byte();	// Read 64-bit registration (48-bit serial number) number from 1-wire Slave Device

   for(temp = 0; temp<8; temp++)
	   hex_ascii(serial_number[temp]);			//Convert Hex Value into ASCII and send to terminal
   
    while(1);
  
} // END OF MAIN

/**********************************************************************
* Function:        unsigned char Detect_Slave_Device(void)
* PreCondition:    None
* Input:		   None	
* Output:		   1 - Not Present   0 - Present 	
* Overview:   	   To check the presence of slave device.    
***********************************************************************/

unsigned char Detect_Slave_Device(void)
{
	if (!OW_reset_pulse())
		return HIGH;
	else 		
		return LOW;
}	



/**********************************************************************
* Function:        void wait (unsigned char delay_time)
* PreCondition:    None
* Input:		   Required Delay value	
* Output:
* Side Effects:
* Overview:
*					Function delay_5Us with clock 25MHz;
*					calculation: 
*					20*0x16 + (7+n)*0.16*delay_time in Us
*					20	: single instructions including call and return.
*					n   : 1 (number of NOPs).
*					8	: (7 + n) ....7 Instructions used by 'for' loop and 1 nop instruction.
* 					0.16: (1/25000000)*4 ...Clock 25MHz
* 					delay_needed in Us.
* 					Calculated delay_time_value in Us.
*					delay_time_value = (delay_needed - (20*0.16))/(8*0.16)
*					delay_time_value = (delay_needed - 3.2) / (1.28)
************************************************************************/
//#ifdef FOR_LOOP_DELAY
//void wait (unsigned char delay_time)
//{
//	unsigned char temp;
//	
//	for(temp = 0; (temp<(delay_time)); temp++)
//	_asm nop _endasm
//}	
//#endif

/**********************************************************************
* Function:        void hex_ascii(unsigned char display_data)
* PreCondition:    None
* Input:		   HEX Value 
* Output:		   None	 
* Side Effects:
* Overview:		 Convert Hex Value to ASCII and send to PC;
***********************************************************************/
void hex_ascii(unsigned char display_data)
{
	unsigned char temp;
	temp = ((display_data & 0xF0)>>4);
	if (temp <= 0x09)
		Putchar(temp+'0');	
	else
		Putchar(temp+'0'+0x07);	

	temp = display_data & 0x0F;
	if (temp <= 0x09)
		Putchar(temp+'0');	
	else
		Putchar(temp+'0'+0x07);	
	
	Putchar('\r');
	Putchar('\n');
}	
	
/**********************************************************************
* Function:        void initEUSART (void)
* PreCondition:    None
* Input:		   None	
* Output:		   None
* Side Effects:
* Overview:		  Initialization of USART 
					1. Enable Transmitter and select High Baud Rate in TXSTA
					2. Select a value in SPGRB register to generate 9600 baud rate 
					3. Enable Serial port in RCSTA register
					4. Enable continuous reception in RCSTA

***********************************************************************/
void initEUSART (void)
{
	  //Configure RC6/TX/CK and RC7/RX/DT as a USART:
	  UART_RX_DIRECTION = INPUT;
	  UART_TX_DIRECTION = OUTPUT;
	  
 	  UART_TX_STATUS = CLEAR;           		// Reset TXSTA 
	  UART_RX_STATUS = CLEAR;		   			// Reset RCSTA	

 	  UART_TX_STATUS = 0x20;		    		// Load TXSTA from TxConUser selected)
	  SPBRG = BAUD_RATE;       					// Write baudrate to SPBRG
	  UART_RX_STATUS = 0x90;		    
	  BAUDCON = BAUD_CONTROL;
   /**************************************************************************************************/
}

/**********************************************************************
* Function:        void PutString(unsigned char *mess)
* PreCondition:    None
* Input:		   Pointer to display message	
* Output:		   None
* Side Effects:
* Overview:		   Sends the string to terminal till it finds NULL character.
***********************************************************************/
void PutString(unsigned char *mess)
{
	while(*mess != '\0')
	{
  		Putchar(*mess++);
	}
}

/**********************************************************************
* Function:        unsigned char Getchar(void)
* PreCondition:    None
* Input:		   None	
* Output:		   Return a byte received from UART	
* Side Effects:
* Overview:		  Wait till a byte received from termianl and then returns the received data	
***********************************************************************/
unsigned char Getchar(void)
{
	unsigned char recv_data;

	while(!PIR1bits.RCIF);
	recv_data = RCREG;
	PIR1bits.RCIF = CLEAR;
}

/**********************************************************************
* Function:        void Putchar (unsigned char send_data)
* PreCondition:    None
* Input:		   Byte to transmit		
* Output:		   None		
* Side Effects:
* Overview:		   Transmits a byte to terminal through UART.	
***********************************************************************/
void Putchar (unsigned char send_data)
{ 
  	TXREG = send_data;	// Also transmit USART status value
	while (!TXSTAbits.TRMT);	// wait here transmission to complete.	
}

/********************************************************************************************
                  E N D     O F     M A I N   
*********************************************************************************************/