
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

 /*** DS1820 ****************************************************************
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

#include <p18cxxx.h>
#include <float.h>
#include <math.h>
#include "onewire.h"

// -----------------------------------------------------------------------------
//                         External prototypes 
// -----------------------------------------------------------------------------

extern void output_low( uint8_t pin );
extern void output_high( uint8_t pin );
extern uint8_t  input( uint8_t pin );
extern void output( uint8_t pin );

// -----------------------------------------------------------------------------
//                          Static variables                                
// -----------------------------------------------------------------------------

static uint8_t bDoneFlag;
static uint8_t nLastDiscrepancy_u8;
static uint8_t nRomAddr_au8[ DS1820_ADDR_LEN ];


///////////////////////////////////////////////////////////////////////////////
// Celsius2Fahrenheit
//

double Celsius2Fahrenheit(double tc)
{
    return ( (9 * tc + 16000) / 5);
}

///////////////////////////////////////////////////////////////////////////////
// Fahrenheit2Celsius
//

double Fahrenheit2Celsius(double tf)
{
    return ( (5 * tf - 16000) / 9);
}


///////////////////////////////////////////////////////////////////////////////
// Celsius2Kelvin
//

double Celsius2Kelvin(double tc)
{
    return (27316.0 + tc);
}

///////////////////////////////////////////////////////////////////////////////
// Kelvin2Celsius
//

double Kelvin2Celsius(double tf)
{
    return ( tf - 273.16);
}





// -----------------------------------------------------------------------------
//                          DS1820 Low-Level Functions                         
// -----------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////////
// DS1820_DelayUs
//
// Delay for the given number of micro seconds.
//
// @param dly_us      number of micro seconds to delay
//

#define DS1820_DelayUs( dly_us )    delay_us( dly_us )


////////////////////////////////////////////////////////////////////////////////
// DS1820_DelayMs
//
// Delay for the given number of milliseconds.
//
// @param dly_ms number of milliseconds to delay
// 

#define DS1820_DelayMs( dly_ms )    delay_ms( dly_ms )


////////////////////////////////////////////////////////////////////////////////
// DS1820_DisableInterrupts
//
// Disable interrupts
//


#ifdef DS1820_INTERRUPT_LOCK
#define DS1820_DisableInterrupts()  disable_interrupts(GLOBAL)
#else
#define DS1820_DisableInterrupts()
#endif



////////////////////////////////////////////////////////////////////////////////
// FUNCTION:   DS1820_EnableInterrupts
// PURPOSE:    Enable interrupts
//


#ifdef DS1820_INTERRUPT_LOCK
#define DS1820_EnableInterrupts()   enable_interrupts(GLOBAL)
#else
#define DS1820_EnableInterrupts()
#endif


////////////////////////////////////////////////////////////////////////////////
// DS1820_Reset
//
// Initialises the DS1820 device.
//
// @return FALSE if at least one device is on the 1-wire bus, TRUE otherwise

uint8_t DS1820_Reset( uint8_t pin )
{
   uint8_t bPresPulse;

   DS1820_DisableInterrupts();
   
   // Reset pulse 
   output_low( pin );
   DS1820_DelayUs( DS1820_RST_PULSE );
   output_high( pin );

   // Wait until pullup pull 1-wire bus to high 
   DS1820_DelayUs (DS1820_PRESENCE_WAIT );

   // Get presence pulse 
   bPresPulse = input( pin );

   DS1820_DelayUs( 424 );
   
   DS1820_EnableInterrupts();

   return bPresPulse;
}


////////////////////////////////////////////////////////////////////////////////
// DS1820_ReadBit
//
// Reads a single bit from the DS1820 device.
//
// @return uint8_t value of the bit which as been read form the DS1820

uint8_t DS1820_ReadBit( uint8_t pin )
{
   uint8_t bBit;

   DS1820_DisableInterrupts();
   
   output_low( pin );
   DS1820_DelayUs( DS1820_MSTR_BITSTART );
   input( pin );
   DS1820_DelayUs( DS1820_BITREAD_DLY );

   bBit = input( pin );
   
   DS1820_EnableInterrupts();

   return bBit;
}


////////////////////////////////////////////////////////////////////////////////
// DS1820_WriteBit
//
// Writes a single bit to the DS1820 device.
//
// @param bBitvalue of bit to be written
// 

void DS1820_WriteBit( uint8_t pin, uint8_t bBit )
{
   DS1820_DisableInterrupts();
   
   output_low( pin );
   DS1820_DelayUs( DS1820_MSTR_BITSTART );

   if ( bBit != FALSE ) {
      output_high( pin );
   }

   DS1820_DelayUs( DS1820_BITWRITE_DLY );
   output_high( pin );
   
   DS1820_EnableInterrupts();
}


////////////////////////////////////////////////////////////////////////////////
// DS1820_ReadByte
//
// Reads a single byte from the DS1820 device.
//
// @return uint8_t byte which has been read from the DS1820
//

uint8_t DS1820_ReadByte( uint8_t pin )
{
   uint8_t i;
   uint8_t value = 0;

   for (i=0 ;i<8; i++ ) {
      if ( DS1820_ReadBit( pin ) ) {
         value |= (1 << i);
      }
      DS1820_DelayUs(120);
   }
   return value;
}


////////////////////////////////////////////////////////////////////////////////
// DS1820_WriteByte
//
// Writes a single byte to the DS1820 device.
//
// @param      val_u8         byte to be written
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

   DS1820_DelayUs( 105 );
}



/* -------------------------------------------------------------------------- */
/*                             API Interface                                  */
/* -------------------------------------------------------------------------- */


////////////////////////////////////////////////////////////////////////////////
// DS1820_AddrDevice
//
// Addresses a single or all devices on the 1-wire bus.
//
// @param     nAddrMethod       use DS1820_CMD_MATCHROM to select a single
//                               device or DS1820_CMD_SKIPROM to select all
// 

void DS1820_AddrDevice( uint8_t pin, uint8_t nAddrMethod )
{
    uint8_t i;
   
    if ( nAddrMethod == DS1820_CMD_MATCHROM ) {
        // address single devices on bus
        DS1820_WriteByte( pin, DS1820_CMD_MATCHROM );      
        for (i = 0; i<DS1820_ADDR_LEN; i ++ ) {
             DS1820_WriteByte( pin, nRomAddr_au8[i] );
        }
    } 
    else {
        // address all devices on bus
        DS1820_WriteByte( pin, DS1820_CMD_SKIPROM);    
    }
}


////////////////////////////////////////////////////////////////////////////////
// DS1820_FindNextDevice
//
// Finds next device connected to the 1-wire bus.
// 
// @param OUTPUT nRomAddr_au8[] ROM code of the next device
// @return uint8_t                 TRUE if there are more devices on the 1-wire
//                                  bus, FALSE otherwise

uint8_t DS1820_FindNextDevice( uint8_t pin )
{
    uint8_t state_u8;
    uint8_t byteidx_u8;
    uint8_t mask_u8 = 1;
    uint8_t bitpos_u8 = 1;
    uint8_t nDiscrepancyMarker_u8 = 0;
    uint8_t bit_b;
    uint8_t bStatus;
    uint8_t next_b = FALSE;

    // init ROM address 
    for (byteidx_u8=0; byteidx_u8 < 8; byteidx_u8 ++) {
        nRomAddr_au8[byteidx_u8] = 0x00;
    }

    bStatus = DS1820_Reset( pin );      // reset the 1-wire 

    if ( bStatus || bDoneFlag ) {       // no device found 
        nLastDiscrepancy_u8 = 0;        // reset the search 
        return FALSE;
    }

    // send search rom command 
    DS1820_WriteByte( pin, DS1820_CMD_SEARCHROM );

    byteidx_u8 = 0;
    do {
        state_u8 = 0;

        // read bit 
        if ( DS1820_ReadBit( pin ) != 0 ) {
            state_u8 = 2;
        }
        DS1820_DelayUs(120);

        // read bit complement
        if ( DS1820_ReadBit( pin ) != 0 ) {
            state_u8 |= 1;
        }
        DS1820_DelayUs(120);

        /* description for values of state_u8: */
        /* 00    There are devices connected to the bus which have conflicting */
        /*       bits in the current ROM code bit position. */
        /* 01    All devices connected to the bus have a 0 in this bit position. */
        /* 10    All devices connected to the bus have a 1 in this bit position. */
        /* 11    There are no devices connected to the 1-wire bus. */

        // if there are no devices on the bus 
        if ( state_u8 == 3 ) {
            break;
        }
        else {
            // devices have the same logical value at this position 
            if ( state_u8 > 0 ) {
                // get bit value 
                bit_b = (uint8_t)(state_u8 >> 1);
            }
            // devices have confilcting bits in the current ROM code 
            else {
                // if there was a conflict on the last iteration 
                if ( bitpos_u8 < nLastDiscrepancy_u8 ) {
                    // take same bit as in last iteration 
                    bit_b = ( ( nRomAddr_au8[ byteidx_u8 ] & mask_u8 ) > 0 );
                }
                else {
                    bit_b = ( bitpos_u8 == nLastDiscrepancy_u8 );
                }

                if ( bit_b == 0 )
                {
                    nDiscrepancyMarker_u8 = bitpos_u8;
                }
            }

            // store bit in ROM address 
            if ( bit_b != 0 ) {
               nRomAddr_au8[ byteidx_u8 ] |= mask_u8;
            }
            else {
               nRomAddr_au8[ byteidx_u8 ] &= ~mask_u8;
            }

            DS1820_WriteBit( pin, bit_b );

            // increment bit position 
            bitpos_u8 ++;

            // calculate next mask value 
            mask_u8 = mask_u8 << 1;

            // check if this byte has finished 
            if ( mask_u8 == 0 ) {
                byteidx_u8 ++;  // advance to next byte of ROM mask 
                mask_u8 = 1;    // update mask 
            }
        }
    } while ( byteidx_u8 < DS1820_ADDR_LEN );


    // if search was unsuccessful then 
    if ( bitpos_u8 < 65 ) {
        // reset the last discrepancy to 0 
        nLastDiscrepancy_u8 = 0;
    }
    else {
        // search was successful 
        nLastDiscrepancy_u8 = nDiscrepancyMarker_u8;
        bDoneFlag = ( nLastDiscrepancy_u8 == 0 );

        // indicates search is not complete yet, more parts remain 
        next_b = TRUE;
    }

    return next_b;
}


////////////////////////////////////////////////////////////////////////////////
// DS1820_FindFirstDevice
//
// Starts the device search on the 1-wire bus.
//
// @param out     nRomAddr_au8[]       ROM code of the first device
// @return uint8_t                 TRUE if there are more devices on the 1-wire
//                                  bus, FALSE otherwise

uint8_t DS1820_FindFirstDevice( uint8_t pin )
{
    nLastDiscrepancy_u8 = 0;
    bDoneFlag = FALSE;

    return ( DS1820_FindNextDevice( pin ) );
}


////////////////////////////////////////////////////////////////////////////////
// DS1820_WriteEEPROM
//
// Writes to the DS1820 EEPROM memory (2 bytes available).
//
// @param nTHigh high byte of EEPROM
// @param nTLow  low byte of EEPROM
// 

void DS1820_WriteEEPROM( uint8_t pin, uint8_t nTHigh, uint8_t nTLow )
{
    // --- write to scratchpad ----------------------------------------------- 
    DS1820_Reset( pin );
    DS1820_AddrDevice( pin, DS1820_CMD_MATCHROM );
    DS1820_WriteByte( pin, DS1820_CMD_WRITESCRPAD ); // start conversion 
    DS1820_WriteByte( pin, nTHigh );
    DS1820_WriteByte( pin, nTLow );

    DS1820_DelayUs( 10 );

    DS1820_Reset( pin );
    DS1820_AddrDevice( pin, DS1820_CMD_MATCHROM );
    DS1820_WriteByte( pin, DS1820_CMD_COPYSCRPAD ); // start conversion 

    delay_ms( 10 );
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
//             The temperature data is stored as a 16-bit sign-extended two’s
//             complement number in the temperature register. The sign bits (S)
//             indicate if the temperature is positive or negative: for
//             positive numbers S = 0 and for negative numbers S = 1.
//
// @return     sint16         raw temperature value with a resolution
//                            of 1/256°C

int16_t DS1820_GetTempRaw( uint8_t pin )
{
    uint8_t i;
    uint16_t temp_u16;
    uint16_t highres_u16;
    uint8_t scrpad[DS1820_SCRPADMEM_LEN];

    // --- start temperature conversion -------------------------------------- 
    DS1820_Reset( pin );
    DS1820_AddrDevice( pin, DS1820_CMD_MATCHROM);       // address the device 
    output_high( pin );
    DS1820_WriteByte( pin, DS1820_CMD_CONVERTTEMP );    // start conversion 
    //DS1820_DelayMs(DS1820_TEMPCONVERT_DLY);           // wait for temperature conversion 
    DS1820_DelayMs(750);


    // --- read sratchpad ---------------------------------------------------- 
    DS1820_Reset( pin );
    DS1820_AddrDevice( pin, DS1820_CMD_MATCHROM);       // address the device 
    DS1820_WriteByte( pin, DS1820_CMD_READSCRPAD);      // read scratch pad 

    // read scratch pad data 
    for (i=0; i<DS1820_SCRPADMEM_LEN; i++ ) {
        scrpad[i] = DS1820_ReadByte( pin );
    }


    /* --- calculate temperature --------------------------------------------- */
    /* Formular for temperature calculation: */
    /* Temp = Temp_read - 0.25 + ((Count_per_C - Count_Remain)/Count_per_C) */

    // get raw value of temperature (0.5°C resolution) 
    temp_u16 = 0;
    temp_u16 = (uint16_t)((uint16_t)scrpad[DS1820_REG_TEMPMSB] << 8);
    temp_u16 |= (uint16_t)(scrpad[DS1820_REG_TEMPLSB]);

    if ( nRomAddr_au8[0] == DS1820_FAMILY_CODE_DS18S20 ) {
        // get temperature value in 1°C resolution 
        temp_u16 >>= 1;
    
        // temperature resolution is TEMP_RES (0x100), so 1°C equals 0x100 
        // => convert to temperature to 1/256°C resolution 
        temp_u16 = ((uint16_t)temp_u16 << 8);
    
        // now substract 0.25°C 
        temp_u16 -= ((uint16_t)TEMP_RES >> 2);
    
        // now calculate high resolution 
        highres_u16 = scrpad[DS1820_REG_CNTPERSEC] - scrpad[DS1820_REG_CNTREMAIN];
        highres_u16 = ((uint16_t)highres_u16 << 8);
        if ( scrpad[ DS1820_REG_CNTPERSEC ] ) {
            highres_u16 = highres_u16 / (uint16_t)scrpad[ DS1820_REG_CNTPERSEC ];
        }
    
        // now calculate result 
        highres_u16 = highres_u16 + temp_u16;
    }
    else {
        // 12 bit temperature value has 0.0625°C resolution 
        // shift left by 4 to get 1/256°C resolution 
        highres_u16 = temp_u16;
        highres_u16 <<= 4;
    }

    return ( highres_u16 );
}


////////////////////////////////////////////////////////////////////////////////
// DS1820_GetTempFloat
//
// Converts internal temperature value to string (physical value).
//
// @return    float          temperature value with as float value

float DS1820_GetTempFloat( uint8_t pin )
{
    return ( (float)DS1820_GetTempRaw( pin ) / (float)TEMP_RES );
}


////////////////////////////////////////////////////////////////////////////////
// DS1820_GetTempString
//
// Converts internal temperature value to string (physical value).
//
// @param tRaw_s16       internal temperature value
// @param OUTPUT:     strTemp_pc     user string buffer to write temperature value
// @return sint16         temperature value with an internal resolution
//                            of TEMP_RES

void DS1820_GetTempString( int16_t tRaw_s16, char *strTemp_pc )
{
    sint16 tPhyLow_s16;
    sint8 tPhy_s8;

    // convert from raw value (1/256°C resolution) to physical value 
    tPhy_s8 = (sint8)(tRaw_s16/TEMP_RES );

    // convert digits from raw value (1/256°C resolution) to physical value 
    // tPhyLow_u16 = tInt_s16 % TEMP_RES;
    tPhyLow_s16 = tRaw_s16 & 0xFF;      // this operation is the same as 
                                        // but saves flash memory tInt_s16 % TEMP_RES 
    tPhyLow_s16 = tPhyLow_s16 * 100;
    tPhyLow_s16 = (uint16_t)tPhyLow_s16 / TEMP_RES;

    // write physical temperature value to string 
    sprintf(strTemp_pc, "%d.%02d", tPhy_s8, (sint8)tPhyLow_s16);
}

