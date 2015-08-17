#ifndef _Config_H
#define _Config_H


//******* D E F I N I T I O N S ************************************************/
#define CLK_FREQ_25000000 		 	0   				 //25MHz
#define CLK_FREQ_24000000 			0	   				 //24MHz
#define CLK_FREQ_20000000 			1   				 //20MHz
#define CLK_FREQ_16000000 			0   				 //16MHz
#define CLK_FREQ_12000000			0   				 //12MHz

#if((CLK_FREQ_25000000+CLK_FREQ_24000000+CLK_FREQ_20000000+CLK_FREQ_16000000+CLK_FREQ_12000000) != 1)
#error Select One Frequency for 1Us Delay Calculation.
#endif

//**********************************************************************
//This wait MACRO gives 1Us delay depending upon clock Speed Select;
//Wait_delay (200) generates a 200Us.
//			  loop:	  nop\							// 1 INST cycle
//					  nop\							// 1 INST cycle
//					  nop\							// 1 INST cycle
//					  decfsz macro_delay,1,1\       // 1 or 3INST cycle
//					  bra loop\						// 2 INST cycle
//**********************************************************************

//**********************************************************************
//#define CLOCK_FREQUENCY  	25000000   				 //25MHz
//Note: 1 INST Cycle = ((1/ CLOCK_SPEED) * 4) = (1/25000000)*4 = 0.16Us
//		Total INST Cycle in a loop = 0.16*6 = 0.96 ~ 1Us.
//**********************************************************************
#if(CLK_FREQ_25000000)
		#define wait(a) _asm movlw a\
					  movwf macro_delay,1\
			  loop:	  nop\
					  nop\
					  nop\
					  decfsz macro_delay,1,1\
					  bra loop\
					 _endasm
					 
		#define BAUD_RATE	39
//**********************************************************************
//#define CLOCK_FREQUENCY  	24000000   				 //24MHz
//Note: 1 INST Cycle = ((1/ CLOCK_SPEED) * 4) = (1/24000000)*4 = 0.1667Us
//		Total INST Cycle in a loop = 0.1667*6 = 1.0007 ~ 1Us.
//**********************************************************************

#elif (CLK_FREQ_24000000)
		#define wait(a) _asm movlw a\
					  movwf macro_delay,1\
			  loop:	  nop\
					  nop\
					  nop\
					  decfsz macro_delay,1,1\
					  bra loop\
					 _endasm

		#define BAUD_RATE	38
//**********************************************************************
//#define CLOCK_FREQUENCY  	20000000   				 //20MHz
//Note: 1 INST Cycle = ((1/ CLOCK_SPEED) * 4) = (1/20000000)*4 = 0.2Us
//		Total INST Cycle in a loop = 0.2*5 = 1.00 ~ 1Us.
//**********************************************************************

#elif (CLK_FREQ_20000000)
		#define wait(a) _asm movlw a\
					  movwf macro_delay,1\
			  loop:	  nop\
					  nop\
					  decfsz macro_delay,1,1\
					  bra loop\
					 _endasm

	    #define BAUD_RATE	31
//**********************************************************************
//#define CLOCK_FREQUENCY  	16000000   				 //16MHz
//Note: 1 INST Cycle = ((1/ CLOCK_SPEED) * 4) = (1/16000000)*4 = 0.25Us
//		Total INST Cycle in a loop = 0.25*4 = 1.00 ~ 1Us.
//**********************************************************************


#elif (CLK_FREQ_16000000)
		#define wait(a) _asm movlw a\
					  movwf macro_delay,1\
			  loop:	  nop\
					  decfsz macro_delay,1,1\
					  bra loop\
				 _endasm


	    #define BAUD_RATE	25
//**********************************************************************
//#define CLOCK_FREQUENCY  	12000000   				 //12MHz
//Note: 1 INST Cycle = ((1/ CLOCK_SPEED) * 4) = (1/12000000)*4 = 0.3333Us
//		Total INST Cycle in a loop = 0.33333*3 = 0.9999 ~ 1Us.
//**********************************************************************


#elif (CLK_FREQ_12000000)
		#define wait(a) _asm movlw a\
					  movwf macro_delay,1\
			  loop:	  decfsz macro_delay,1,1\
					  bra loop\
					 _endasm
					 
		#define BAUD_RATE	18
					
#endif


//*************************************************************************/



/******* G E N E R I C   D E F I N I T I O N S ************************************************/

#define	HIGH	1
#define	LOW		0
#define	OUTPUT	0
#define	INPUT 	1
#define	SET		1
#define	CLEAR	0


/**PORT D E F I N I T I O N S ****************************************************/

//UART PORT DEFINITIONS
#define UART_RX_DIRECTION   TRISCbits.TRISC7
#define UART_TX_DIRECTION   TRISCbits.TRISC6
#define UART_RX_STATUS		RCSTA
#define UART_TX_STATUS		TXSTA

//ONE WIRE PORT PIN DEFINITION
///****************************************************
// This Configuration is required to make any PIC MicroController
// I/O pin as Open drain to drive 1-wire.
///****************************************************
#define OW_PIN_DIRECTION 	LATBbits.LATB2
#define OW_WRITE_PIN  		TRISBbits.TRISB2
#define OW_READ_PIN			PORTBbits.RB2

/******* G E N E R I C   D E F I N I T I O N S ************************************************/

#define DELAY_6Us	6
#define DELAY_9Us	9
#define DELAY_10Us	10
#define DELAY_55Us	55
#define DELAY_60Us	60
#define DELAY_64Us	64
#define DELAY_70Us	70
#define DELAY_205Us	205			// DELAY_410Us = DELAY_205Us + DELAY_205Us
#define DELAY_240Us	240			// DELAY_480Us = DELAY_240Us + DELAY_240Us // since the variable is declared as unsigned char




/**************************************************************
	 function definition
**************************************************************/
void initEUSART(void);                 
void USART_RxTx(void);
void high_isr(void);
void low_isr(void);

/***********************************************************
   V A R I A B L E S
************************************************************/


#endif