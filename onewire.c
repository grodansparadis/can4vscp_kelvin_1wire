
/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  Kelvin 1-wire Module
 *  ====================
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

 /*** DS1820 ******************************************************************
 * 
 *  Original code
 *  =============
 *  
 *    FILENAME:    ds1820.h
 *    DATE:        25.02.2005
 *    AUTHOR:      Christian Stadler
 *
 *    DESCRIPTION: Driver for DS1820 1-Wire Temperature sensor (Dallas)
 *
 ******************************************************************************/


#include "vscp_compiler.h"
#include "vscp_projdefs.h"

#define _XTAL_FREQ 40000000 // Fro delay functions

#include <xc.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include "onewire.h"

// -----------------------------------------------------------------------------
//                         External prototypes 
// -----------------------------------------------------------------------------

extern void output_low( uint8_t pin );
extern void output_high( uint8_t pin );
extern uint8_t input( uint8_t pin );
extern void output( uint8_t pin );

// -----------------------------------------------------------------------------
//                          Static variables                                
// -----------------------------------------------------------------------------

static uint8_t bDoneFlag;
static uint8_t nLastDiscrepancy;
static uint8_t romAddr[ DS1820_ADDR_LEN ];


///////////////////////////////////////////////////////////////////////////////
// Celsius2Fahrenheit
//

double Celsius2Fahrenheit( double tc )
{
    return ( ( 9 * tc + 16000 ) / 5);
}

///////////////////////////////////////////////////////////////////////////////
// Fahrenheit2Celsius
//

double Fahrenheit2Celsius( double tf )
{
    return ( ( 5 * tf - 16000 ) / 9 );
}


///////////////////////////////////////////////////////////////////////////////
// Celsius2Kelvin
//

double Celsius2Kelvin( double tc )
{
    return ( 27316.0 + tc );
}

///////////////////////////////////////////////////////////////////////////////
// Kelvin2Celsius
//

double Kelvin2Celsius( double tf )
{
    return ( tf - 273.16 );
}


// -----------------------------------------------------------------------------
//                          DS1820 Low-Level Functions                         
// -----------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////////
// DS1820_Reset
//
// Initialises the DS1820 device.
//
// @param Pin to work on
// @return FALSE if at least one device is on the 1-wire bus, TRUE otherwise
//

uint8_t DS1820_Reset( uint8_t pin )
{
    uint8_t bPresPulse;

    INTCONbits.GIE = 0;  // Disable interrupts
   
    // Reset pulse 
    output_low( pin );
    __delay_us( DS1820_RST_PULSE );
    output_high( pin );

    // Wait until pull-up pull 1-wire bus to high 
    __delay_us( DS1820_PRESENCE_WAIT );

    // Get presence pulse 
    bPresPulse = input( pin );

    __delay_us( DS1820_PRESENCE_FIN );      
   
    INTCONbits.GIE = 1;  // Enable interrupts

    return bPresPulse;
}


////////////////////////////////////////////////////////////////////////////////
// DS1820_ReadBit
//
// Reads a single bit from the DS1820 device.
//
// @param Pin to work on
// @return Value of the bit which as been read form the DS1820
//

uint8_t DS1820_ReadBit( uint8_t pin )
{
    uint8_t bBit;

    INTCONbits.GIE = 0;      // Disable interrupts
   
    output_low( pin );
    __delay_us( DS1820_MSTR_BITSTART );
    input( pin );
    __delay_us( DS1820_BITREAD_DLY );

    bBit = input( pin );
   
    INTCONbits.GIE = 1;      // Enable interrupts
    
    __delay_us( DS1820_BITREAD_WAIT );

    return bBit;
}


////////////////////////////////////////////////////////////////////////////////
// DS1820_WriteBit
//
// Writes a single bit to the DS1820 device.
//
// @param Pin to work on
// @param Bit to be written
// 

void DS1820_WriteBit( uint8_t pin, uint8_t bBit )
{
    INTCONbits.GIE = 0;      // Disable interrupts
   
    output_low( pin );
    __delay_us( DS1820_MSTR_BITSTART );

    if ( bBit != FALSE ) {
        output_high( pin );
    }
    
    __delay_us( DS1820_BITWRITE_DLY );
    output_high( pin );
   
    INTCONbits.GIE = 1;      // Enable interrupts
}


////////////////////////////////////////////////////////////////////////////////
// DS1820_ReadByte
//
// Reads a single byte from the DS1820 device.
//
// @param Pin to work on
// @return Byte which has been read from the DS1820
//

uint8_t DS1820_ReadByte( uint8_t pin )
{
    uint8_t i;
    uint8_t value = 0;

    for (i=0 ;i<8; i++ ) {
        if ( DS1820_ReadBit( pin ) ) {
            value |= (1 << i);
        }
        __delay_us(120);
    }
    
    return value;
}


////////////////////////////////////////////////////////////////////////////////
// DS1820_WriteByte
//
// Writes a single byte to the DS1820 device.
//
// @param Pin to work on
// @param Byte to write
// 

void DS1820_WriteByte( uint8_t pin, uint8_t val_u8 )
{
   uint8_t i;
   uint8_t temp;

   for (i=0 ;i<8; i++) {            // Writes byte, one bit at a time
      temp = val_u8 >> i;           // Shifts val right 'i' spaces 
      temp &= 0x01;                 // Copy that bit to temp 
      DS1820_WriteBit( pin, temp);  // Write bit in temp into 
   }

   __delay_us( 105 );
}



/* -------------------------------------------------------------------------- */
/*                             API Interface                                  */
/* -------------------------------------------------------------------------- */


////////////////////////////////////////////////////////////////////////////////
// DS1820_AddrDevice
//
// Addresses a single or all devices on the 1-wire bus.
//
// @param Pin to work on
// @param Use DS1820_CMD_MATCHROM to select a single device or
//        DS1820_CMD_SKIPROM to select all
// 

void DS1820_AddrDevice( uint8_t pin, uint8_t nAddrMethod )
{
    uint8_t i;
   
    if ( DS1820_CMD_MATCHROM == nAddrMethod  ) {
        
        // address single devices on bus
        DS1820_WriteByte( pin, DS1820_CMD_MATCHROM );      
        for (i = 0; i<DS1820_ADDR_LEN; i ++ ) {
             DS1820_WriteByte( pin, romAddr[i] );
        }
    } 
    else {
        // address all devices on bus
        DS1820_WriteByte( pin, DS1820_CMD_SKIPROM );    
    }
    
}


////////////////////////////////////////////////////////////////////////////////
// DS1820_FindNextDevice
//
// Finds next device connected to the 1-wire bus. romAddr holds the 
// resulting address
// 
// @param Pin to work on
// @return TRUE if there are more devices on the 1-wire
//         bus, FALSE otherwise
//

uint8_t DS1820_FindNextDevice( uint8_t pin )
{
    uint8_t idxByte;
    uint8_t state;
    uint8_t mask = 1;
    uint8_t bitpos = 1;
    uint8_t nDiscrepancyMarker = 0;
    uint8_t bit_b;
    uint8_t bStatus;
    uint8_t next_b = FALSE;

    // initialise ROM address 
    memset( romAddr, 0, 8 );

    bStatus = DS1820_Reset( pin );      // reset the 1-wire 

    if ( bStatus || bDoneFlag ) {       // no device found 
        nLastDiscrepancy = 0;           // reset the search 
        return FALSE;
    }

    // send search ROM command 
    DS1820_WriteByte( pin, DS1820_CMD_SEARCHROM );

    idxByte = 0;
    do {
        state = 0;

        // read bit 
        if ( DS1820_ReadBit( pin ) != 0 ) {
            state = 2;
        }
        __delay_us(120);

        // read bit complement
        if ( DS1820_ReadBit( pin ) != 0 ) {
            state |= 1;
        }
        __delay_us( 120 );

        // description for values of state: 
        // 00    There are devices connected to the bus which have conflicting 
        //       bits in the current ROM code bit position. 
        // 01    All devices connected to the bus have a 0 in this bit position. 
        // 10    All devices connected to the bus have a 1 in this bit position. 
        // 11    There are no devices connected to the 1-wire bus. 

        // if there are no devices on the bus 
        if ( state == 3 ) {
            break;
        }
        else {
            // devices have the same logical value at this position 
            if ( state > 0 ) {
                // get bit value 
                bit_b = (uint8_t)(state >> 1);
            }
            // devices have conflicting bits in the current ROM code 
            else {
                // if there was a conflict on the last iteration 
                if ( bitpos < nLastDiscrepancy ) {
                    // take same bit as in last iteration 
                    bit_b = ( ( romAddr[ idxByte ] & mask ) > 0 );
                }
                else {
                    bit_b = ( bitpos == nLastDiscrepancy );
                }

                if ( bit_b == 0 ) {
                    nDiscrepancyMarker = bitpos;
                }
            }

            // store bit in ROM address 
            if ( bit_b != 0 ) {
               romAddr[ idxByte ] |= mask;
            }
            else {
               romAddr[ idxByte ] &= ~mask;
            }

            DS1820_WriteBit( pin, bit_b );

            // increment bit position 
            bitpos++;

            // calculate next mask value 
            mask = mask << 1;

            // check if this byte has finished 
            if ( 0 == mask ) {
                idxByte++;      // advance to next byte of ROM mask 
                mask = 1;       // update mask 
            }
        }
        
    } while ( idxByte < DS1820_ADDR_LEN );


    // if search was unsuccessful then 
    if ( bitpos < 65 ) {
        // reset the last discrepancy to 0 
        nLastDiscrepancy = 0;
    }
    else {
        // search was successful 
        nLastDiscrepancy = nDiscrepancyMarker;
        bDoneFlag = ( nLastDiscrepancy == 0 );

        // indicates search is not complete yet, more parts remain 
        next_b = TRUE;
    }

    return next_b;
}


////////////////////////////////////////////////////////////////////////////////
// DS1820_FindFirstDevice
//
// Starts the device search on the 1-wire bus. romAddr will hold address of
// first device.
//
// @param Pin to work on
// @return TRUE if there are more devices on the 1-wire
//         bus, FALSE otherwise

uint8_t DS1820_FindFirstDevice( uint8_t pin )
{
    nLastDiscrepancy = 0;
    bDoneFlag = FALSE;

    return ( DS1820_FindNextDevice( pin ) );
}


////////////////////////////////////////////////////////////////////////////////
// DS1820_WriteEEPROM
//
// Writes to the DS1820 EEPROM memory (2 bytes available).
//
// @param Pin to work on
// @param nTHigh high byte of EEPROM
// @param nTLow  low byte of EEPROM
// 

void DS1820_WriteEEPROM( uint8_t pin, uint8_t high, uint8_t low )
{
    // --- write to scratchpad ----------------------------------------------- 
    DS1820_Reset( pin );
    DS1820_AddrDevice( pin, DS1820_CMD_MATCHROM );
    DS1820_WriteByte( pin, DS1820_CMD_WRITESCRPAD ); // start conversion 
    DS1820_WriteByte( pin, high );
    DS1820_WriteByte( pin, low );

    __delay_us( 10 );

    DS1820_Reset( pin );
    DS1820_AddrDevice( pin, DS1820_CMD_MATCHROM );
    DS1820_WriteByte( pin, DS1820_CMD_COPYSCRPAD ); // start conversion 

    __delay_ms( 10 );
}


////////////////////////////////////////////////////////////////////////////////
// DS1820_GetTempRaw
//
// Get temperature raw value from single DS1820 device.
//
//             Scratchpad Memory Layout
//             Byte  Register
//             0     Temperature_LSB
//             1     Temperature_MSB
//             2     Temp Alarm High / User Byte 1
//             3     Temp Alarm Low / User Byte 2
//             4     Reserved
//             5     Reserved
//             6     Count_Remain
//             7     Count_per_C
//             8     CRC
//
//             Temperature calculation for DS18S20 (Family Code 0x10):
//             =======================================================
//                                             (Count_per_C - Count_Remain)
//             Temperature = temp_raw - 0.25 + ----------------------------
//                                                     Count_per_C
//
//             Where temp_raw is the value from the temp_MSB and temp_LSB with
//             the least significant bit removed (the 0.5C bit).
//
//
//             Temperature calculation for DS18B20 (Family Code 0x28):
//             =======================================================
//                      bit7   bit6   bit5   bit4   bit3   bit2   bit1   bit0
//             LSB      2^3    2^2    2^1    2^0    2^-1   2^-2   2^-3   2^-4
//                      bit15  bit14  bit13  bit12  bit3   bit2   bit1   bit0
//             MSB      S      S      S      S      S      2^6    2^5    2^4
//
//             The temperature data is stored as a 16-bit sign-extended two?s
//             complement number in the temperature register. The sign bits (S)
//             indicate if the temperature is positive or negative: for
//             positive numbers S = 0 and for negative numbers S = 1.
//
// @param Pin to work on
// @return raw temperature value with a resolution
//         of 1/256 degrees C
//

int16_t DS1820_GetTempRaw( uint8_t pin )
{
    uint8_t i;
    uint16_t temp;
    uint16_t highres;
    uint8_t scrpad[ DS1820_SCRPADMEM_LEN ];

    // --- start temperature conversion -------------------------------------- 
    DS1820_Reset( pin );
    DS1820_AddrDevice( pin, DS1820_CMD_MATCHROM );      // address the device 
    output_high( pin );
    DS1820_WriteByte( pin, DS1820_CMD_CONVERTTEMP );    // start conversion 
    
    // wait for temperature conversion 
    for ( i=0; i<76; i++ ) {
        __delaywdt_ms( 10 );
    }
    
    // --- read scratch pad ---------------------------------------------------- 
    DS1820_Reset( pin );
    DS1820_AddrDevice( pin, DS1820_CMD_MATCHROM );      // address the device 
    DS1820_WriteByte( pin, DS1820_CMD_READSCRPAD );     // read scratch pad 

    // read scratch pad data 
    for (i=0; i<DS1820_SCRPADMEM_LEN; i++ ) {
        scrpad[i] = DS1820_ReadByte( pin );
    }


    // --- calculate temperature --------------------------------------------- 
    // Formula for temperature calculation: 
    // Temp = Temp_read - 0.25 + ((Count_per_C - Count_Remain)/Count_per_C) 

    // get raw value of temperature (0.5 degrees C resolution) 
    temp = 0;
    temp = (uint16_t)((uint16_t)scrpad[ DS1820_REG_TEMPMSB ] << 8);
    temp |= (uint16_t)( scrpad[ DS1820_REG_TEMPLSB ] );

    if ( DS1820_FAMILY_CODE_DS18S20 == romAddr[ 0 ] ) {
        // get temperature value in 1 degrees C resolution 
        temp >>= 1;
    
        // temperature resolution is TEMP_RES (0x100), so 1 degrees C equals 0x100 
        // => convert to temperature to 1/256 degrees C resolution 
        temp = ((uint16_t)temp << 8);
    
        // now subtract 0.25 degrees C 
        temp -= ( (uint16_t)TEMP_RES >> 2 );
    
        // now calculate high resolution 
        highres = scrpad[DS1820_REG_CNTPERSEC] - scrpad[DS1820_REG_CNTREMAIN];
        highres = ((uint16_t)highres << 8);
        if ( scrpad[ DS1820_REG_CNTPERSEC ] ) {
            highres = highres / (uint16_t)scrpad[ DS1820_REG_CNTPERSEC ];
        }
    
        // now calculate result 
        highres = highres + temp;
    }
    else {
        // 12 bit temperature value has 0.0625 degrees C resolution 
        // shift left by 4 to get 1/256 degrees C resolution 
        highres = temp;
        highres <<= 4;
    }

    return ( highres );
}


////////////////////////////////////////////////////////////////////////////////
// DS1820_GetTempFloat
//
// Converts internal temperature value to string (physical value).
//
// @param Pin to work on
// @return Temperature value with as float value
//

float DS1820_GetTempFloat( uint8_t pin )
{
    return ( (float)DS1820_GetTempRaw( pin ) / (float)TEMP_RES );
}


////////////////////////////////////////////////////////////////////////////////
// DS1820_GetTempString
//
// Converts internal temperature value to string (physical value).
//
// @param tRaw  internal temperature value
// @param strTemp user string buffer to write temperature value
// @return Temperature value with an internal resolution of TEMP_RES


void DS1820_GetTempString( int16_t rawTemp, char *strTemp )
{
    int16_t tPhyLow;
    int8_t tPhy;

    // Convert from raw value (1/256 degrees C resolution) to physical value 
    tPhy = (int8_t)(rawTemp/TEMP_RES );

    // convert digits from raw value (1/256 degrees C resolution) to physical value 
    // tPhyLow_u16 = tInt_s16 % TEMP_RES;
    tPhyLow = rawTemp & 0xFF;   // this operation is the same as above
                                // but saves flash memory tInt_s16 % TEMP_RES 
    tPhyLow = tPhyLow * 100;
    tPhyLow = (uint16_t)tPhyLow / TEMP_RES;

    // write physical temperature value to string 
    sprintf( strTemp, "%d.%02d", tPhy, (int8_t)tPhyLow );
    
}

