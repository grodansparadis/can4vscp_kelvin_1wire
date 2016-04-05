/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  Kelvin 1-wire Module
 *  =====================
 *
 *  Copyright (C) 2015-2016 Ake Hedman, Grodans Paradis AB
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

#ifndef ONEWIRE_H
#define ONEWIRE_H

#include <inttypes.h>
#include "main.h"

// The four temperature channels
#define TEMP_CHANNEL1   0   // RC7
#define TEMP_CHANNEL2   1   // RC6
#define TEMP_CHANNEL3   2   // RC4
#define TEMP_CHANNEL4   3   // RC3

//
// Defaults
//

// Default value for control register
// bit 0,1 - 01 = Celsius
// Bit 2 - 0 Reserved bit
// Bit 3 = 0 Low alarm disabled
// Bit 4 = 0 High alarm disabled
// Bit 5 = 0 Alarm is sent
// Bit 6 = 0 Normal TurnOn/Off
// Bit 7 = 0 Non continues alarm
// 
#define DEFAULT_CONTROL_REG                 0b00000001

#define MASK_CONTROL0_UNIT                  0x03
#define MASK_CONTROL0_LOW_ALARM             0x08
#define MASK_CONTROL0_HIGH_ALARM            0x10
#define MASK_CONTROL0_TURNX                 0x20
#define MASK_CONTROL0_TURNX_INVERT          0x40
#define MASK_CONTROL0_CONTINUOUS            0x80

#define MASK_CONTROL1_CLASS0                0x03

// Default report interval for sensor 0-7
#define DEFAULT_REPORT_INTERVAL             0

// Default B constant for sensor 0
// Digikey 90-2456-1-ND
#define DEFAULT_B_CONSTANT_SENSOR0_MSB      0x0F
#define DEFAULT_B_CONSTANT_SENSOR0_LSB      0x68


// Default value for low alarm point -327.68 degrees Celsius
// A point that will not be reached
#define DEFAULT_LOW_ALARM_MSB               0x80
#define DEFAULT_LOW_ALARM_LSB               0xff

// Default value for high alarm point +327.67 degrees Celsius
// A point that will not be reached
#define DEFAULT_HIGH_ALARM_MSB              0x7f
#define DEFAULT_HIGH_ALARM_LSB              0xff

// Default sensor zon/subzone information
#define DEFAULT_SENSOR_ZONE                 0
#define DEFAULT_SENSOR_SUBZONE              0

// Default absolute low value
// Set to +327.67 degrees Celsius
#define DEFAULT_LOW_MSB                     0x7f
#define DEFAULT_LOW_LSB                     0xff

// Default absolute high value
// Set to -327.68 degrees Celsius
#define DEFAULT_HIGH_MSB                    0x80
#define DEFAULT_HIGH_LSB                    0x00

// Default hysteresis
#define DEFAULT_HYSTERESIS                  2

//
// Error
//

#define ERROR_GENERAL                       0x00
#define ERROR_SENSOR                        0x01
#define ERROR_MODULE                        0x02

//
// Temp. unit
//

#define TEMP_UNIT_KELVIN                    0
#define TEMP_UNIT_CELSIUS                   1
#define TEMP_UNIT_FAHRENHEIT                2


//
// Module alarm bits
//
#define MODULE_LOW_ALARM                    1
#define MODULE_HIGH_ALARM                   2

#define TEMP_REPORT_CLASS_10_INT            0   // CLASS=10, Report as int
#define TEMP_REPORT_CLASS_10_FLOAT          1   // CLASS=10, Report as float
#define TEMP_REPORT_CLASS_10_STR            2   // CLASS=10, Report as string

//
// Configuration 0 bits
//
// Default value for control register
// bit 0,1 - Temperature report format
// Bit 2 - Reserved bit
// Bit 3 - Low alarm enable
// Bit 4 - High alarm enable
// Bit 5 - Enable TurnOn/Off
// Bit 6 - Enable invert TurnOn/TurnOff
// Bit 7 - Continues alarm
#define CONFIG0_ENABLE_LOW_ALARM             (1<<3)
#define CONFIG0_ENABLE_HIGH_ALARM            (1<<4)
#define CONFIG0_ENABLE_TURNX                 (1<<5)
#define CONFIG0_ENABLE_TURNON_INVERT         (1<<6)
#define CONFIG0_ENABLE_CONTINOUS_EVENTS      (1<<7)

//
// Configuration 1 bits
//
// Bit 0,1 - Class to use for temperature reports

#define VSCP_EEPROM_END                     0

// EEPROM registers for module persistent data

#define EEPROM_ZONE                         0x41	// Zone node belongs to
#define EEPROM_SUBZONE                      0x42	// Sub zone node belongs to

// Sensor zone/sub zone information

#define EEPROM_SENSOR0_ZONE                 0x73
#define EEPROM_SENSOR0_SUBZONE              0x74

#define EEPROM_SENSOR1_ZONE                 0x75
#define EEPROM_SENSOR1_SUBZONE              0x76

#define EEPROM_SENSOR2_ZONE                 0x77
#define EEPROM_SENSOR2_SUBZONE              0x78

#define EEPROM_SENSOR3_ZONE                 0x79
#define EEPROM_SENSOR3_SUBZONE              0x7A

#define EEPROM_SENSOR4_ZONE                 0x7B
#define EEPROM_SENSOR4_SUBZONE              0x7C

#define EEPROM_SENSOR5_ZONE                 0x7D
#define EEPROM_SENSOR5_SUBZONE              0x7E

#define EEPROM_SENSOR6_ZONE                 0x7D
#define EEPROM_SENSOR6_SUBZONE              0x7E

#define EEPROM_SENSOR7_ZONE                 0x7D
#define EEPROM_SENSOR7_SUBZONE              0x7E

// Control registers

#define EEPROM_CONTROLREG0                  0x43
#define EEPROM_CONTROLREG1                  0x44
#define EEPROM_CONTROLREG2                  0x45
#define EEPROM_CONTROLREG3                  0x46
#define EEPROM_CONTROLREG4                  0x47
#define EEPROM_CONTROLREG5                  0x48
#define EEPROM_CONTROLREG6                  0x48
#define EEPROM_CONTROLREG7                  0x48

// Report interval

#define EEPROM_REPORT_INTERVAL0             0x49
#define EEPROM_REPORT_INTERVAL1             0x4A
#define EEPROM_REPORT_INTERVAL2             0x4B
#define EEPROM_REPORT_INTERVAL3             0x4C
#define EEPROM_REPORT_INTERVAL4             0x4D
#define EEPROM_REPORT_INTERVAL5             0x4E
#define EEPROM_REPORT_INTERVAL6             0x4E
#define EEPROM_REPORT_INTERVAL7             0x4E


// Low alarms

#define EEPROM_LOW_ALARM0_MSB               0x5B
#define EEPROM_LOW_ALARM0_LSB               0x5C

#define EEPROM_LOW_ALARM1_MSB               0x5D
#define EEPROM_LOW_ALARM1_LSB               0x5E

#define EEPROM_LOW_ALARM2_MSB               0x5F
#define EEPROM_LOW_ALARM2_LSB               0x60

#define EEPROM_LOW_ALARM3_MSB               0x61
#define EEPROM_LOW_ALARM3_LSB               0x62

#define EEPROM_LOW_ALARM4_MSB               0x63
#define EEPROM_LOW_ALARM4_LSB               0x64

#define EEPROM_LOW_ALARM5_MSB               0x65
#define EEPROM_LOW_ALARM5_LSB               0x66

#define EEPROM_LOW_ALARM6_MSB               0x65
#define EEPROM_LOW_ALARM6_LSB               0x66

#define EEPROM_LOW_ALARM7_MSB               0x65
#define EEPROM_LOW_ALARM7_LSB               0x66

// High alarms

#define EEPROM_HIGH_ALARM0_MSB              0x67
#define EEPROM_HIGH_ALARM0_LSB              0x68

#define EEPROM_HIGH_ALARM1_MSB              0x69
#define EEPROM_HIGH_ALARM1_LSB              0x6A

#define EEPROM_HIGH_ALARM2_MSB              0x6B
#define EEPROM_HIGH_ALARM2_LSB              0x6C

#define EEPROM_HIGH_ALARM3_MSB              0x6D
#define EEPROM_HIGH_ALARM3_LSB              0x6E

#define EEPROM_HIGH_ALARM4_MSB              0x6F
#define EEPROM_HIGH_ALARM4_LSB              0x70

#define EEPROM_HIGH_ALARM5_MSB              0x71
#define EEPROM_HIGH_ALARM5_LSB              0x72

#define EEPROM_HIGH_ALARM6_MSB              0x71
#define EEPROM_HIGH_ALARM6_LSB              0x72

#define EEPROM_HIGH_ALARM7_MSB              0x71
#define EEPROM_HIGH_ALARM7_LSB              0x72



// Absolute low temperatures

#define EEPROM_ABSOLUT_LOW0_MSB             0x7F
#define EEPROM_ABSOLUT_LOW0_LSB             0x80

#define EEPROM_ABSOLUT_LOW1_MSB             0x81
#define EEPROM_ABSOLUT_LOW1_LSB             0x82

#define EEPROM_ABSOLUT_LOW2_MSB             0x83
#define EEPROM_ABSOLUT_LOW2_LSB             0x84

#define EEPROM_ABSOLUT_LOW3_MSB             0x85
#define EEPROM_ABSOLUT_LOW3_LSB             0x86

#define EEPROM_ABSOLUT_LOW4_MSB             0x87
#define EEPROM_ABSOLUT_LOW4_LSB             0x88

#define EEPROM_ABSOLUT_LOW5_MSB             0x89
#define EEPROM_ABSOLUT_LOW5_LSB             0x8A

#define EEPROM_ABSOLUT_LOW6_MSB             0x89
#define EEPROM_ABSOLUT_LOW6_LSB             0x8A

#define EEPROM_ABSOLUT_LOW7_MSB             0x89
#define EEPROM_ABSOLUT_LOW7_LSB             0x8A

// Absolute high temperatures

#define EEPROM_ABSOLUT_HIGH0_MSB            0x8B
#define EEPROM_ABSOLUT_HIGH0_LSB            0x8C

#define EEPROM_ABSOLUT_HIGH1_MSB            0x8D
#define EEPROM_ABSOLUT_HIGH1_LSB            0x8E

#define EEPROM_ABSOLUT_HIGH2_MSB            0x8F
#define EEPROM_ABSOLUT_HIGH2_LSB            0x90

#define EEPROM_ABSOLUT_HIGH3_MSB            0x91
#define EEPROM_ABSOLUT_HIGH3_LSB            0x92

#define EEPROM_ABSOLUT_HIGH4_MSB            0x93
#define EEPROM_ABSOLUT_HIGH4_LSB            0x94

#define EEPROM_ABSOLUT_HIGH5_MSB            0x95
#define EEPROM_ABSOLUT_HIGH5_LSB            0x96

#define EEPROM_ABSOLUT_HIGH6_MSB            0x95
#define EEPROM_ABSOLUT_HIGH6_LSB            0x96

#define EEPROM_ABSOLUT_HIGH7_MSB            0x95
#define EEPROM_ABSOLUT_HIGH7_LSB            0x96

// Sensor hysteresis

#define EEPROM_HYSTERESIS_SENSOR0           0x97
#define EEPROM_HYSTERESIS_SENSOR1           0x98
#define EEPROM_HYSTERESIS_SENSOR2           0x99
#define EEPROM_HYSTERESIS_SENSOR3           0x9A
#define EEPROM_HYSTERESIS_SENSOR4           0x9B
#define EEPROM_HYSTERESIS_SENSOR5           0x9C
#define EEPROM_HYSTERESIS_SENSOR6           0x9D
#define EEPROM_HYSTERESIS_SENSOR7           0x9E



// ADCCON0 ADC select bits
#define SELECT_ADC_TEMP0                    (10<<2)  // ADC10
#define SELECT_ADC_TEMP1                    (2<<2)   // ADC2
#define SELECT_ADC_TEMP2                    (1<<2)   // ADC1
#define SELECT_ADC_TEMP3                    (0<<2)   // ADC0
#define SELECT_ADC_TEMP4                    (9<<2)   // ADC9
#define SELECT_ADC_TEMP5                    (8<<2)   // ADC8


// ----------------------------------------------------------------------------- 
//                                DS1820                                      
// ----------------------------------------------------------------------------- 

#define TEMP_RES              0x100 // temperature resolution => 1/256°C = 0.0039°C 


// -----------------------------------------------------------------------------
//                         DS1820 Timing Parameters                           
// -----------------------------------------------------------------------------

#define DS1820_RST_PULSE       480   // master reset pulse time in [us] 
//#define DS1820_PRESENCE_WAIT   40    // delay after master reset pulse in [us] 
#define DS1820_PRESENCE_WAIT   70
//#define DS1820_PRESENCE_FIN    480   // delay after reading of presence pulse [us] 
#define DS1820_PRESENCE_FIN    410

//#define DS1820_MSTR_BITSTART   2     // delay time for bit start by master 
#define DS1820_MSTR_BITSTART   6
//#define DS1820_BITREAD_DLY     5     // bit read delay 
#define DS1820_BITREAD_DLY     9
#define DS1820_BITREAD_WAIT    55

//#define DS1820_BITWRITE_DLY    100   // bit write delay 
#define DS1820_BITWRITE_DLY    64

// -----------------------------------------------------------------------------
//                            DS1820 Registers                                
// -----------------------------------------------------------------------------

#define DS1820_REG_TEMPLSB              0
#define DS1820_REG_TEMPMSB              1
#define DS1820_REG_CNTREMAIN            6
#define DS1820_REG_CNTPERSEC            7
#define DS1820_SCRPADMEM_LEN            9     // length of scratchpad memory 

#define DS1820_ADDR_LEN                 8


// -----------------------------------------------------------------------------
//                            DS1820 Commands                                 
// -----------------------------------------------------------------------------

#define DS1820_CMD_SEARCHROM            0xF0
#define DS1820_CMD_READROM              0x33
#define DS1820_CMD_MATCHROM             0x55
#define DS1820_CMD_SKIPROM              0xCC
#define DS1820_CMD_ALARMSEARCH          0xEC
#define DS1820_CMD_CONVERTTEMP          0x44
#define DS1820_CMD_WRITESCRPAD          0x4E
#define DS1820_CMD_READSCRPAD           0xBE
#define DS1820_CMD_COPYSCRPAD           0x48
#define DS1820_CMD_RECALLEE             0xB8


#define DS1820_FAMILY_CODE_DS18B20      0x28
#define DS1820_FAMILY_CODE_DS18S20      0x10


// Temeparture conversions
double Celsius2Fahrenheit(double tc);
double Fahrenheit2Celsius(double tf);
double Celsius2Kelvin(double tc);
double Kelvin2Celsius(double tf);

// DS1820
uint8_t DS1820_Reset( uint8_t pin );
uint8_t DS1820_ReadBit( uint8_t pin );
void DS1820_WriteBit( uint8_t pin, uint8_t bBit );
uint8_t DS1820_ReadByte( uint8_t pin );
void DS1820_WriteByte( uint8_t pin, uint8_t val_u8 );
void DS1820_AddrDevice( uint8_t pin, uint8_t nAddrMethod );
uint8_t DS1820_FindNextDevice( uint8_t pin );
uint8_t DS1820_FindFirstDevice( uint8_t pin );
void DS1820_WriteEEPROM( uint8_t pin, uint8_t high, uint8_t low );
int16_t DS1820_GetTempRaw( uint8_t pin );
float DS1820_GetTempFloat( uint8_t pin );
void DS1820_GetTempString( int16_t rawTemp, char *strTemp );

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