/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  Kelvin 1-wire Module
 *  ====================
 *
 *  Copyright (C) 2015-2018 Ake Hedman, Grodans Paradis AB
 *                          http://www.grodansparadis.com
 *                          <akhe@grodansparadis.com>
 *
 *  This work is licensed under the Creative Common 
 *  Attribution-NonCommercial-ShareAlike 3.0 Unported license. The full
 *  license is available in the top folder of this project (LICENSE) or here
 *  http://creativecommons.org/licenses/by-nc-sa/3.0/legalcode
 *  It is also available in a human readable form here 
 *  http://creativecommons.org/licenses/by-nc-sa/3.0/
 * 
 *	This file is part of VSCP - Very Simple Control Protocol 	
 *	http://www.vscp.org
 *
 * ******************************************************************************
 */

#ifndef MAIN_H
#define MAIN_H

//Defines

#define	TRUE                                    1
#define	FALSE                                   0

//
// 8 MHz with PLL => 8 MHz
// 1:4 prescaler => 1 MHz (1 uS cycle )
// 1 ms == 1000 uS
// 65535 - 1000 = 64535 = 0xfc17
//
// Timer2 use 250 and prescaler 1:4
//
//#define TIMER0_RELOAD_VALUE                   0xfc17

//
// 10 MHz with PLL => 40 MHz
// 1:4 prescaler => 1.25 MHz ( 0.800 uS cycle )
// 1 ms == 1000 uS
// 65535 - 1250 = 64285 = 0xfb1d
//
// Timer2 use 156 and prescaler 1:8
//
#define TIMER0_RELOAD_VALUE                     0xfb1d

//
// Timer 2 is used as a 1 ms clock
// 156 is loaded eight time to give ~1250 cycles
// Timer2 use 156 and prescaler 1:4, Postscaler 1:16
// 100 ns * 56 * 4 * 16 ~ 1 ms
//
#define TIMER2_RELOAD_VALUE                     156

// IMPORTANT!!!!!
// bootflag and nickname must be at position 0/1 in EEPROM
// as the pic1 bootloader expect them to be there.

// EEPROM Storage
#define VSCP_EEPROM_BOOTLOADER_FLAG             0x00	// Reserved for bootloader

#define VSCP_EEPROM_NICKNAME                    0x01	// Persistant nickname id storage
#define VSCP_EEPROM_SEGMENT_CRC                 0x02	// Persistant segment crc storage
#define VSCP_EEPROM_CONTROL1                    0x03    // Persistent control byte
#define VSCP_EEPROM_CONTROL2                    0x04    // Persistent control byte

#define VSCP_EEPROM_REG_USERID                  0x05
#define VSCP_EEPROM_REG_USERID1                 0x06
#define VSCP_EEPROM_REG_USERID2                 0x07
#define VSCP_EEPROM_REG_USERID3                 0x08
#define VSCP_EEPROM_REG_USERID4                 0x09

// The following can be stored in flash or eeprom

#define VSCP_EEPROM_REG_MANUFACTUR_ID0          0x0A
#define VSCP_EEPROM_REG_MANUFACTUR_ID1          0x0B
#define VSCP_EEPROM_REG_MANUFACTUR_ID2          0x0C
#define VSCP_EEPROM_REG_MANUFACTUR_ID3          0x0D

#define VSCP_EEPROM_REG_MANUFACTUR_SUBID0       0x0E	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID1       0x0F	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID2       0x10	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID3       0x11

// The following can be stored in program ROM (recommended) or in EEPROM 

#define VSCP_EEPROM_REG_GUID                    0x12	// Start of GUID MSB	
// 		0x11 - 0x20				

#define VSCP_EEPROM_REG_DEVICE_URL              0x22	// Start of Device URL storage
// 		0x21 - 0x40

#define VSCP_EEPROM_VSCP_END                    0x42	// marks end of VSCP EEPROM usage
//   (next free position)


// Function Prototypes

void doWork(void);
void init(void);
void init_app_ram(void);
void init_app_eeprom(void);
void doOneSecondWork(void);
void handle_sync(void);
int16_t getCalibrationValue( uint8_t i );
int8_t convertTemperature(double temp, unsigned char unit );
int8_t sendTempEvent( uint8_t i );
void setEventData(int v, unsigned char unit);
void read_app_register(unsigned char reg);
void write_app_register(unsigned char reg, unsigned char val);
void calculateSetFilterMask( void );
uint8_t writeChannelControl( uint8_t val );
BOOL loadROMCodeFromEEPROM( uint8_t currentSensor );
void saveROMCodeToEEPROM( uint8_t channel, uint8_t idxSensor );
void reportTokenActivity( void );

// Temparture conversions
double Celsius2Fahrenheit(double tc);
double Fahrenheit2Celsius(double tf);
double Celsius2Kelvin(double tc);
double Kelvin2Celsius(double tf);

// Actions
void actionScan( uint8_t param );
void actionScanStore( uint8_t param );
void actionReport( uint8_t param );
void actionClearAlarm( uint8_t param );
void actionClearHigh( uint8_t param );
void actionClearLow( uint8_t param );

/*!
	Send Extended ID CAN frame
	@param id CAN extended ID for frame.
	@param size Number of data bytes 0-8
	@param pData Pointer to data bytes of frame.
	@return TRUE (!=0) on success, FALSE (==0) on failure.
*/
int8_t sendCANFrame( uint32_t id, uint8_t size, uint8_t *pData );

/*!
	Get extended ID CAN frame
	@param pid Pointer to CAN extended ID for frame.
	@param psize Pointer to number of databytes 0-8
	@param pData Pointer to databytes of frame.
	@return TRUE (!=0) on success, FALSE (==0) on failure.
*/
int8_t getCANFrame( uint32_t *pid, uint8_t *psize, uint8_t *pData );

#endif
