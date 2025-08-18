/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  Kelvin 1-wire Module
 *  =====================
 *
 *  Copyright (C) 2015-2020 Ake Hedman, Grodans Paradis AB
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


#include "vscp_compiler.h"
#include "vscp_projdefs.h"

#define _XTAL_FREQ 40000000 // For delay functions

#if defined(_18F2580) 


#if defined(RELEASE)

#pragma config WDT = ON, WDTPS = 128
#pragma config OSC = HSPLL
#pragma config BOREN = BOACTIVE
#pragma config STVREN = ON
#pragma config BORV = 1 // 4.3V
#pragma config LVP = ON
#pragma config CPB = ON
#pragma config BBSIZ = 2048
#pragma config WRTD  = OFF

#pragma config EBTR0 = OFF
#pragma config EBTR1 = OFF
#pragma config EBTR2 = OFF
#pragma config EBTR3 = OFF

#pragma config EBTRB = OFF

#else

#pragma config WDT = OFF
#pragma config OSC = HSPLL
#pragma config PWRT = ON
#pragma config BOREN = BOACTIVE
#pragma config STVREN = ON
#pragma config BORV = 1 // 4.3V
#pragma config LVP = OFF
#pragma config CPB = OFF
#pragma config WRTD  = OFF

#pragma config EBTR0 = OFF
#pragma config EBTR1 = OFF
#pragma config EBTR2 = OFF
#pragma config EBTR3 = OFF

#pragma config EBTRB = OFF

#endif

#else if defined(_18F25K80) || defined(_18F26K80) || defined(_18F45K80) || defined(_18F46K80) || defined(_18F65K80) || defined(_18F66K80)


// CONFIG1L
#pragma config SOSCSEL = DIG    // RC0/RC is I/O
#pragma config RETEN = OFF      // Ultra low-power regulator is Disabled (Controlled by REGSLP bit).
#pragma config INTOSCSEL = HIGH // LF-INTOSC in High-power mode during Sleep.
#pragma config XINST = OFF      // No extended instruction set

// CONFIG1H
#pragma config FOSC = HS2       // Crystal 10 MHz
#pragma config PLLCFG = ON      // 4 x PLL

// CONFIG2H
#pragma config WDTPS = 1048576  // Watchdog prescaler
#pragma config BOREN = SBORDIS  // Brown out enabled
#pragma config BORV  = 0        // 3.0V

// CONFIG3H
#pragma config CANMX = PORTB    // ECAN TX and RX pins are located on RB2 and RB3, respectively.
#pragma config MSSPMSK = MSK7   // 7 Bit address masking mode.
#pragma config MCLRE = ON       // MCLR Enabled, RE3 Disabled.

// CONFIG4L
#pragma config STVREN = ON      // Stack Overflow Reset enabled
#pragma config BBSIZ = BB2K     // Boot block size 2K

#ifdef DEBUG
#pragma config WDTEN = OFF      // WDT disabled in hardware; SWDTEN bit disabled.
#else
#pragma config WDTEN = ON       // WDT enabled in hardware; 
#endif


#endif

#include <xc.h>
#include <timers.h>
#include <adc.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
#include <ECAN.h>
#include <vscp-firmware.h>
#include <vscp_class.h>
#include <vscp_type.h>
#include "main.h" 
#include "version.h"
#include "onewire.h"





uint8_t romAddrOneWire[ DS1820_ADDR_LEN ];
uint8_t bDoneFlagOneWire;   
uint8_t nLastDiscrepancyOnewire;

// One wire timing
uint16_t OW_MasterResetPulseTime;
uint8_t OW_PresenceWait;  
uint16_t OW_PresenceFin;
uint8_t OW_MasterBitStart;
uint8_t OW_DelayBitRead;
uint8_t OW_DelayBitWait;
uint8_t OW_DelayBitWrite;

// The device URL (max 32 characters including null termination)
const uint8_t vscp_deviceURL[] = "www.eurosource.se/1wire_1.xml";

// Global Variable Declarations

volatile uint16_t sendTimer;            // Timer for CAN send
volatile uint32_t measurement_clock;    // Clock for measurements
volatile uint32_t timeout_clock;        // Clock used for timeouts

uint8_t seconds;                        // counter for seconds

uint8_t seconds_temp[ 8 ];              // timers for temp event

// Alarm flag bits
uint8_t low_alarm;
uint8_t high_alarm;

uint32_t adc_value;                     // ADC result for sensor 8 (on-board))

//uint8_t romAddrOneWire[ DS1820_ADDR_LEN ];

// All nine temperature values Index 0 is on-board NTC sensor
double arrayTemp[ 9 ];

// Sensor that is searched
uint8_t currentSensor = 0;


//__EEPROM_DATA(0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88);

// This table translates registers in page 0 to EEPROM locations

const uint16_t reg2eeprom_pg0[] = {
    /* REG0_KELVIN1W_ZONE                   */          VSCP_EEPROM_VSCP_END + 0,
    /* REG0_KELVIN1W_SUBZONE                */          VSCP_EEPROM_VSCP_END + 1,
                                                        
    /* REG0_KELVIN1W_TEMP0_ZONE             */          VSCP_EEPROM_VSCP_END + 2,
    /* REG0_KELVIN1W_TEMP0_SUBZONE          */          VSCP_EEPROM_VSCP_END + 3,
    /* REG0_KELVIN1W_TEMP1_ZONE             */          VSCP_EEPROM_VSCP_END + 4,
    /* REG0_KELVIN1W_TEMP1_SUBZONE          */          VSCP_EEPROM_VSCP_END + 5,
    /* REG0_KELVIN1W_TEMP2_ZONE             */          VSCP_EEPROM_VSCP_END + 6,
    /* REG0_KELVIN1W_TEMP2_SUBZONE          */          VSCP_EEPROM_VSCP_END + 7,
    /* REG0_KELVIN1W_TEMP3_ZONE             */          VSCP_EEPROM_VSCP_END + 8,
    /* REG0_KELVIN1W_TEMP3_SUBZONE          */          VSCP_EEPROM_VSCP_END + 10,
    /* REG0_KELVIN1W_TEMP4_ZONE             */          VSCP_EEPROM_VSCP_END + 11,
    /* REG0_KELVIN1W_TEMP4_SUBZONE          */          VSCP_EEPROM_VSCP_END + 12,
    /* REG0_KELVIN1W_TEMP5_ZONE             */          VSCP_EEPROM_VSCP_END + 13,
    /* REG0_KELVIN1W_TEMP5_SUBZONE          */          VSCP_EEPROM_VSCP_END + 14,
    /* REG0_KELVIN1W_TEMP6_ZONE             */          VSCP_EEPROM_VSCP_END + 15,
    /* REG0_KELVIN1W_TEMP6_SUBZONE          */          VSCP_EEPROM_VSCP_END + 16,
    /* REG0_KELVIN1W_TEMP7_ZONE             */          VSCP_EEPROM_VSCP_END + 17,
    /* REG0_KELVIN1W_TEMP7_SUBZONE          */          VSCP_EEPROM_VSCP_END + 18,
                                                        
    /* REG0_KELVIN1W_CTRL_REG0_LOW          */          VSCP_EEPROM_VSCP_END + 19,
    /* REG0_KELVIN1W_CTRL_REG0_HIGH         */          VSCP_EEPROM_VSCP_END + 20, 
    /* REG0_KELVIN1W_CTRL_REG1_LOW          */          VSCP_EEPROM_VSCP_END + 21,
    /* REG0_KELVIN1W_CTRL_REG1_HIGH         */          VSCP_EEPROM_VSCP_END + 22,
    /* REG0_KELVIN1W_CTRL_REG2_LOW          */          VSCP_EEPROM_VSCP_END + 23,
    /* REG0_KELVIN1W_CTRL_REG2_HIGH         */          VSCP_EEPROM_VSCP_END + 24,
    /* REG0_KELVIN1W_CTRL_REG3_LOW          */          VSCP_EEPROM_VSCP_END + 25,
    /* REG0_KELVIN1W_CTRL_REG3_HIGH         */          VSCP_EEPROM_VSCP_END + 26,
    /* REG0_KELVIN1W_CTRL_REG4_LOW          */          VSCP_EEPROM_VSCP_END + 27,
    /* REG0_KELVIN1W_CTRL_REG4_HIGH         */          VSCP_EEPROM_VSCP_END + 28,
    /* REG0_KELVIN1W_CTRL_REG5_LOW          */          VSCP_EEPROM_VSCP_END + 29,
    /* REG0_KELVIN1W_CTRL_REG5_HIGH         */          VSCP_EEPROM_VSCP_END + 30,
    /* REG0_KELVIN1W_CTRL_REG6_LOW          */          VSCP_EEPROM_VSCP_END + 31,
    /* REG0_KELVIN1W_CTRL_REG6_HIGH         */          VSCP_EEPROM_VSCP_END + 32,
    /* REG0_KELVIN1W_CTRL_REG7_LOW          */          VSCP_EEPROM_VSCP_END + 33,
    /* REG0_KELVIN1W_CTRL_REG7_HIGH         */          VSCP_EEPROM_VSCP_END + 34,
                                                        
    /* REG0_KELVIN1W_REPORT_INTERVAL_0      */          VSCP_EEPROM_VSCP_END + 35,
    /* REG0_KELVIN1W_REPORT_INTERVAL_1      */          VSCP_EEPROM_VSCP_END + 36,
    /* REG0_KELVIN1W_REPORT_INTERVAL_2      */          VSCP_EEPROM_VSCP_END + 37,
    /* REG0_KELVIN1W_REPORT_INTERVAL_3      */          VSCP_EEPROM_VSCP_END + 38,
    /* REG0_KELVIN1W_REPORT_INTERVAL_4      */          VSCP_EEPROM_VSCP_END + 39,
    /* REG0_KELVIN1W_REPORT_INTERVAL_5      */          VSCP_EEPROM_VSCP_END + 40,
    /* REG0_KELVIN1W_REPORT_INTERVAL_6      */          VSCP_EEPROM_VSCP_END + 41,
    /* REG0_KELVIN1W_REPORT_INTERVAL_7      */          VSCP_EEPROM_VSCP_END + 42, 
                                                        
    /* REG0_KELVIN1W_BCONSTANT0_HIGH        */          VSCP_EEPROM_VSCP_END + 43,
    /* REG0_KELVIN1W_BCONSTANT0_LOW         */          VSCP_EEPROM_VSCP_END + 44,
  
    /* REG0_KELVIN1W_MASTER_RESET_PULSE_MSB */          VSCP_EEPROM_VSCP_END + 45,
    /* REG0_KELVIN1W_MASTER_RESET_PULSE_LSB */          VSCP_EEPROM_VSCP_END + 46,
    /* REG0_KELVIN1W_PRESENCE_WAIT          */          VSCP_EEPROM_VSCP_END + 47,
    /* REG0_KELVIN1W_PRESENCE_FIN_MSB       */          VSCP_EEPROM_VSCP_END + 48,
    /* REG0_KELVIN1W_PRESENCE_FIN_LSB       */          VSCP_EEPROM_VSCP_END + 49,
    /* REG0_KELVIN1W_MASTER_BITSTART_DELAY  */          VSCP_EEPROM_VSCP_END + 50,
    /* REG0_KELVIN1W_BIT_READ_DELAY         */          VSCP_EEPROM_VSCP_END + 51,
    /* REG0_KELVIN1W_BIT_READ_WAIT          */          VSCP_EEPROM_VSCP_END + 53,
    /* REG0_KELVIN1W_BIT_WRITE_DELAY        */          VSCP_EEPROM_VSCP_END + 53,                                                        
};

// Page 1 is temperature values

// This table translates registers in page 2 to EEPROM location

const uint16_t reg2eeprom_pg2[] = {                                                        
    /* REG2_KELVIN1W_LOW_ALARM_SET_POINT0_MSB */        VSCP_EEPROM_VSCP_END + 54,
    /* REG2_KELVIN1W_LOW_ALARM_SET_POINT0_LSB */        VSCP_EEPROM_VSCP_END + 55,
    /* REG2_KELVIN1W_LOW_ALARM_SET_POINT1_MSB */        VSCP_EEPROM_VSCP_END + 56,
    /* REG2_KELVIN1W_LOW_ALARM_SET_POINT1_LSB */        VSCP_EEPROM_VSCP_END + 57,
    /* REG2_KELVIN1W_LOW_ALARM_SET_POINT2_MSB */        VSCP_EEPROM_VSCP_END + 58,
    /* REG2_KELVIN1W_LOW_ALARM_SET_POINT2_LSB */        VSCP_EEPROM_VSCP_END + 59,
    /* REG2_KELVIN1W_LOW_ALARM_SET_POINT3_MSB */        VSCP_EEPROM_VSCP_END + 60,
    /* REG2_KELVIN1W_LOW_ALARM_SET_POINT3_LSB */        VSCP_EEPROM_VSCP_END + 61,
    /* REG2_KELVIN1W_LOW_ALARM_SET_POINT4_MSB */        VSCP_EEPROM_VSCP_END + 62,
    /* REG2_KELVIN1W_LOW_ALARM_SET_POINT4_LSB */        VSCP_EEPROM_VSCP_END + 63,
    /* REG2_KELVIN1W_LOW_ALARM_SET_POINT5_MSB */        VSCP_EEPROM_VSCP_END + 64,
    /* REG2_KELVIN1W_LOW_ALARM_SET_POINT5_LSB */        VSCP_EEPROM_VSCP_END + 65,
    /* REG2_KELVIN1W_LOW_ALARM_SET_POINT6_MSB */        VSCP_EEPROM_VSCP_END + 66,
    /* REG2_KELVIN1W_LOW_ALARM_SET_POINT6_LSB */        VSCP_EEPROM_VSCP_END + 67,
    /* REG2_KELVIN1W_LOW_ALARM_SET_POINT7_MSB */        VSCP_EEPROM_VSCP_END + 68,
    /* REG2_KELVIN1W_LOW_ALARM_SET_POINT7_LSB */        VSCP_EEPROM_VSCP_END + 69,   
                                                        
    /* REG2_KELVIN1W_HIGH_ALARM_SET_POINT0_MSB */       VSCP_EEPROM_VSCP_END + 70,
    /* REG2_KELVIN1W_HIGH_ALARM_SET_POINT0_LSB */       VSCP_EEPROM_VSCP_END + 71,
    /* REG2_KELVIN1W_HIGH_ALARM_SET_POINT1_MSB */       VSCP_EEPROM_VSCP_END + 72,
    /* REG2_KELVIN1W_HIGH_ALARM_SET_POINT1_LSB */       VSCP_EEPROM_VSCP_END + 73,
    /* REG2_KELVIN1W_HIGH_ALARM_SET_POINT2_MSB */       VSCP_EEPROM_VSCP_END + 74,
    /* REG2_KELVIN1W_HIGH_ALARM_SET_POINT2_LSB */       VSCP_EEPROM_VSCP_END + 75,
    /* REG2_KELVIN1W_HIGH_ALARM_SET_POINT3_MSB */       VSCP_EEPROM_VSCP_END + 76,
    /* REG2_KELVIN1W_HIGH_ALARM_SET_POINT3_LSB */       VSCP_EEPROM_VSCP_END + 77,
    /* REG2_KELVIN1W_HIGH_ALARM_SET_POINT4_MSB */       VSCP_EEPROM_VSCP_END + 78,
    /* REG2_KELVIN1W_HIGH_ALARM_SET_POINT4_LSB */       VSCP_EEPROM_VSCP_END + 79,
    /* REG2_KELVIN1W_HIGH_ALARM_SET_POINT5_MSB */       VSCP_EEPROM_VSCP_END + 80,
    /* REG2_KELVIN1W_HIGH_ALARM_SET_POINT5_LSB */       VSCP_EEPROM_VSCP_END + 81,
    /* REG2_KELVIN1W_HIGH_ALARM_SET_POINT6_MSB */       VSCP_EEPROM_VSCP_END + 82,
    /* REG2_KELVIN1W_HIGH_ALARM_SET_POINT6_LSB */       VSCP_EEPROM_VSCP_END + 83,
    /* REG2_KELVIN1W_HIGH_ALARM_SET_POINT7_MSB */       VSCP_EEPROM_VSCP_END + 84,
    /* REG2_KELVIN1W_HIGH_ALARM_SET_POINT7_LSB */       VSCP_EEPROM_VSCP_END + 85,

    /* REG2_KELVIN1W_ABSOLUTE_LOW_TEMP0_MSB    */       VSCP_EEPROM_VSCP_END + 86,
    /* REG2_KELVIN1W_ABSOLUTE_LOW_TEMP0_LSB    */       VSCP_EEPROM_VSCP_END + 87,
    /* REG2_KELVIN1W_ABSOLUTE_LOW_TEMP1_MSB    */       VSCP_EEPROM_VSCP_END + 88,
    /* REG2_KELVIN1W_ABSOLUTE_LOW_TEMP1_LSB    */       VSCP_EEPROM_VSCP_END + 89,
    /* REG2_KELVIN1W_ABSOLUTE_LOW_TEMP2_MSB    */       VSCP_EEPROM_VSCP_END + 90,
    /* REG2_KELVIN1W_ABSOLUTE_LOW_TEMP2_LSB    */       VSCP_EEPROM_VSCP_END + 91,
    /* REG2_KELVIN1W_ABSOLUTE_LOW_TEMP3_MSB    */       VSCP_EEPROM_VSCP_END + 92,
    /* REG2_KELVIN1W_ABSOLUTE_LOW_TEMP3_LSB    */       VSCP_EEPROM_VSCP_END + 93,
    /* REG2_KELVIN1W_ABSOLUTE_LOW_TEMP4_MSB    */       VSCP_EEPROM_VSCP_END + 94,
    /* REG2_KELVIN1W_ABSOLUTE_LOW_TEMP4_LSB    */       VSCP_EEPROM_VSCP_END + 95,
    /* REG2_KELVIN1W_ABSOLUTE_LOW_TEMP5_MSB    */       VSCP_EEPROM_VSCP_END + 96,
    /* REG2_KELVIN1W_ABSOLUTE_LOW_TEMP5_LSB    */       VSCP_EEPROM_VSCP_END + 97,
    /* REG2_KELVIN1W_ABSOLUTE_LOW_TEMP6_MSB    */       VSCP_EEPROM_VSCP_END + 98,
    /* REG2_KELVIN1W_ABSOLUTE_LOW_TEMP6_LSB    */       VSCP_EEPROM_VSCP_END + 99,
    /* REG2_KELVIN1W_ABSOLUTE_LOW_TEMP7_MSB    */       VSCP_EEPROM_VSCP_END + 100,
    /* REG2_KELVIN1W_ABSOLUTE_LOW_TEMP7_LSB    */       VSCP_EEPROM_VSCP_END + 101,                                                        
                                                        
    /* REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP0_MSB     */     VSCP_EEPROM_VSCP_END + 102,
    /* REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP0_LSB     */     VSCP_EEPROM_VSCP_END + 103,
    /* REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP1_MSB     */     VSCP_EEPROM_VSCP_END + 104,
    /* REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP1_LSB     */     VSCP_EEPROM_VSCP_END + 105,
    /* REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP2_MSB     */     VSCP_EEPROM_VSCP_END + 106,
    /* REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP2_LSB     */     VSCP_EEPROM_VSCP_END + 107,
    /* REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP3_MSB     */     VSCP_EEPROM_VSCP_END + 108,
    /* REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP3_LSB     */     VSCP_EEPROM_VSCP_END + 109,
    /* REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP4_MSB     */     VSCP_EEPROM_VSCP_END + 110,
    /* REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP4_LSB     */     VSCP_EEPROM_VSCP_END + 111,
    /* REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP5_MSB     */     VSCP_EEPROM_VSCP_END + 112,
    /* REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP5_LSB     */     VSCP_EEPROM_VSCP_END + 113,
    /* REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP6_MSB     */     VSCP_EEPROM_VSCP_END + 114,
    /* REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP6_LSB     */     VSCP_EEPROM_VSCP_END + 115,
    /* REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP7_MSB     */     VSCP_EEPROM_VSCP_END + 116,
    /* REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP7_LSB     */     VSCP_EEPROM_VSCP_END + 117,

    /* REG2_KELVIN1W_HYSTERESIS_TEMP0            */     VSCP_EEPROM_VSCP_END + 118,
    /* REG2_KELVIN1W_HYSTERESIS_TEMP1            */     VSCP_EEPROM_VSCP_END + 119,
    /* REG2_KELVIN1W_HYSTERESIS_TEMP2            */     VSCP_EEPROM_VSCP_END + 120,
    /* REG2_KELVIN1W_HYSTERESIS_TEMP3            */     VSCP_EEPROM_VSCP_END + 121,
    /* REG2_KELVIN1W_HYSTERESIS_TEMP4            */     VSCP_EEPROM_VSCP_END + 122,
    /* REG2_KELVIN1W_HYSTERESIS_TEMP5            */     VSCP_EEPROM_VSCP_END + 123,
    /* REG2_KELVIN1W_HYSTERESIS_TEMP6            */     VSCP_EEPROM_VSCP_END + 124, 
    /* REG2_KELVIN1W_HYSTERESIS_TEMP7            */     VSCP_EEPROM_VSCP_END + 125, 
                                                        
    /* REG2_KELVIN1W_CALIBRATION_TEMP0_MSB       */     VSCP_EEPROM_VSCP_END + 126,
    /* REG2_KELVIN1W_CALIBRATION_TEMP0_LSB       */     VSCP_EEPROM_VSCP_END + 127,
    /* REG2_KELVIN1W_CALIBRATION_TEMP1_MSB       */     VSCP_EEPROM_VSCP_END + 128,
    /* REG2_KELVIN1W_CALIBRATION_TEMP1_LSB       */     VSCP_EEPROM_VSCP_END + 129,
    /* REG2_KELVIN1W_CALIBRATION_TEMP2_MSB       */     VSCP_EEPROM_VSCP_END + 130,
    /* REG2_KELVIN1W_CALIBRATION_TEMP2_LSB       */     VSCP_EEPROM_VSCP_END + 131,
    /* REG2_KELVIN1W_CALIBRATION_TEMP3_MSB       */     VSCP_EEPROM_VSCP_END + 132,
    /* REG2_KELVIN1W_CALIBRATION_TEMP3_LSB       */     VSCP_EEPROM_VSCP_END + 133,
    /* REG2_KELVIN1W_CALIBRATION_TEMP4_MSB       */     VSCP_EEPROM_VSCP_END + 134,
    /* REG2_KELVIN1W_CALIBRATION_TEMP4_LSB       */     VSCP_EEPROM_VSCP_END + 135,
    /* REG2_KELVIN1W_CALIBRATION_TEMP5_MSB       */     VSCP_EEPROM_VSCP_END + 136,
    /* REG2_KELVIN1W_CALIBRATION_TEMP5_LSB       */     VSCP_EEPROM_VSCP_END + 137,
    /* REG2_KELVIN1W_CALIBRATION_TEMP6_MSB       */     VSCP_EEPROM_VSCP_END + 138,
    /* REG2_KELVIN1W_CALIBRATION_TEMP6_LSB       */     VSCP_EEPROM_VSCP_END + 139,
    /* REG2_KELVIN1W_CALIBRATION_TEMP7_MSB       */     VSCP_EEPROM_VSCP_END + 140,
    /* REG2_KELVIN1W_CALIBRATION_TEMP7_LSB       */     VSCP_EEPROM_VSCP_END + 141,
};

// This table translates registers in page 3 to EEPROM locations

const uint16_t reg2eeprom_pg3[] = {
    /* REG3_KELVIN1W_CHANNEL_CONTROL_REGISTER    */     VSCP_EEPROM_VSCP_END + 142, // Dummy
    /* REG3_KELVIN1W_ROM_CODE_TEMP0_BYTE0        */     VSCP_EEPROM_VSCP_END + 143,
    /* REG3_KELVIN1W_ROM_CODE_TEMP0_BYTE1        */     VSCP_EEPROM_VSCP_END + 144,
    /* REG3_KELVIN1W_ROM_CODE_TEMP0_BYTE2        */     VSCP_EEPROM_VSCP_END + 145,
    /* REG3_KELVIN1W_ROM_CODE_TEMP0_BYTE3        */     VSCP_EEPROM_VSCP_END + 146,
    /* REG3_KELVIN1W_ROM_CODE_TEMP0_BYTE4        */     VSCP_EEPROM_VSCP_END + 147,
    /* REG3_KELVIN1W_ROM_CODE_TEMP0_BYTE5        */     VSCP_EEPROM_VSCP_END + 148,
    /* REG3_KELVIN1W_ROM_CODE_TEMP0_BYTE6        */     VSCP_EEPROM_VSCP_END + 149,
    /* REG3_KELVIN1W_ROM_CODE_TEMP0_BYTE7        */     VSCP_EEPROM_VSCP_END + 150, 
    /* REG3_KELVIN1W_ROM_CODE_TEMP1_BYTE0        */     VSCP_EEPROM_VSCP_END + 151,
    /* REG3_KELVIN1W_ROM_CODE_TEMP1_BYTE1        */     VSCP_EEPROM_VSCP_END + 152,
    /* REG3_KELVIN1W_ROM_CODE_TEMP1_BYTE2        */     VSCP_EEPROM_VSCP_END + 153,
    /* REG3_KELVIN1W_ROM_CODE_TEMP1_BYTE3        */     VSCP_EEPROM_VSCP_END + 154,
    /* REG3_KELVIN1W_ROM_CODE_TEMP1_BYTE4        */     VSCP_EEPROM_VSCP_END + 155,
    /* REG3_KELVIN1W_ROM_CODE_TEMP1_BYTE5        */     VSCP_EEPROM_VSCP_END + 156,
    /* REG3_KELVIN1W_ROM_CODE_TEMP1_BYTE6        */     VSCP_EEPROM_VSCP_END + 157,
    /* REG3_KELVIN1W_ROM_CODE_TEMP1_BYTE7        */     VSCP_EEPROM_VSCP_END + 158, 
    /* REG3_KELVIN1W_ROM_CODE_TEMP2_BYTE0        */     VSCP_EEPROM_VSCP_END + 159,
    /* REG3_KELVIN1W_ROM_CODE_TEMP2_BYTE1        */     VSCP_EEPROM_VSCP_END + 160,
    /* REG3_KELVIN1W_ROM_CODE_TEMP2_BYTE2        */     VSCP_EEPROM_VSCP_END + 161,
    /* REG3_KELVIN1W_ROM_CODE_TEMP2_BYTE3        */     VSCP_EEPROM_VSCP_END + 162,
    /* REG3_KELVIN1W_ROM_CODE_TEMP2_BYTE4        */     VSCP_EEPROM_VSCP_END + 163,
    /* REG3_KELVIN1W_ROM_CODE_TEMP2_BYTE5        */     VSCP_EEPROM_VSCP_END + 164,
    /* REG3_KELVIN1W_ROM_CODE_TEMP2_BYTE6        */     VSCP_EEPROM_VSCP_END + 165,
    /* REG3_KELVIN1W_ROM_CODE_TEMP2_BYTE7        */     VSCP_EEPROM_VSCP_END + 166, 
    /* REG3_KELVIN1W_ROM_CODE_TEMP3_BYTE0        */     VSCP_EEPROM_VSCP_END + 167,
    /* REG3_KELVIN1W_ROM_CODE_TEMP3_BYTE1        */     VSCP_EEPROM_VSCP_END + 168,
    /* REG3_KELVIN1W_ROM_CODE_TEMP3_BYTE2        */     VSCP_EEPROM_VSCP_END + 169,
    /* REG3_KELVIN1W_ROM_CODE_TEMP3_BYTE3        */     VSCP_EEPROM_VSCP_END + 170,
    /* REG3_KELVIN1W_ROM_CODE_TEMP3_BYTE4        */     VSCP_EEPROM_VSCP_END + 171,
    /* REG3_KELVIN1W_ROM_CODE_TEMP3_BYTE5        */     VSCP_EEPROM_VSCP_END + 172,
    /* REG3_KELVIN1W_ROM_CODE_TEMP3_BYTE6        */     VSCP_EEPROM_VSCP_END + 173,
    /* REG3_KELVIN1W_ROM_CODE_TEMP3_BYTE7        */     VSCP_EEPROM_VSCP_END + 174, 
    /* REG3_KELVIN1W_ROM_CODE_TEMP4_BYTE0        */     VSCP_EEPROM_VSCP_END + 175,
    /* REG3_KELVIN1W_ROM_CODE_TEMP4_BYTE1        */     VSCP_EEPROM_VSCP_END + 176,
    /* REG3_KELVIN1W_ROM_CODE_TEMP4_BYTE2        */     VSCP_EEPROM_VSCP_END + 177,
    /* REG3_KELVIN1W_ROM_CODE_TEMP4_BYTE3        */     VSCP_EEPROM_VSCP_END + 178,
    /* REG3_KELVIN1W_ROM_CODE_TEMP4_BYTE4        */     VSCP_EEPROM_VSCP_END + 179,
    /* REG3_KELVIN1W_ROM_CODE_TEMP4_BYTE5        */     VSCP_EEPROM_VSCP_END + 180,
    /* REG3_KELVIN1W_ROM_CODE_TEMP4_BYTE6        */     VSCP_EEPROM_VSCP_END + 181,
    /* REG3_KELVIN1W_ROM_CODE_TEMP4_BYTE7        */     VSCP_EEPROM_VSCP_END + 182, 
    /* REG3_KELVIN1W_ROM_CODE_TEMP5_BYTE0        */     VSCP_EEPROM_VSCP_END + 183,
    /* REG3_KELVIN1W_ROM_CODE_TEMP5_BYTE1        */     VSCP_EEPROM_VSCP_END + 184,
    /* REG3_KELVIN1W_ROM_CODE_TEMP5_BYTE2        */     VSCP_EEPROM_VSCP_END + 185,
    /* REG3_KELVIN1W_ROM_CODE_TEMP5_BYTE3        */     VSCP_EEPROM_VSCP_END + 186,
    /* REG3_KELVIN1W_ROM_CODE_TEMP5_BYTE4        */     VSCP_EEPROM_VSCP_END + 187,
    /* REG3_KELVIN1W_ROM_CODE_TEMP5_BYTE5        */     VSCP_EEPROM_VSCP_END + 188,
    /* REG3_KELVIN1W_ROM_CODE_TEMP5_BYTE6        */     VSCP_EEPROM_VSCP_END + 189,
    /* REG3_KELVIN1W_ROM_CODE_TEMP5_BYTE7        */     VSCP_EEPROM_VSCP_END + 190, 
    /* REG3_KELVIN1W_ROM_CODE_TEMP6_BYTE0        */     VSCP_EEPROM_VSCP_END + 191,
    /* REG3_KELVIN1W_ROM_CODE_TEMP6_BYTE1        */     VSCP_EEPROM_VSCP_END + 192,
    /* REG3_KELVIN1W_ROM_CODE_TEMP6_BYTE2        */     VSCP_EEPROM_VSCP_END + 193,
    /* REG3_KELVIN1W_ROM_CODE_TEMP6_BYTE3        */     VSCP_EEPROM_VSCP_END + 194,
    /* REG3_KELVIN1W_ROM_CODE_TEMP6_BYTE4        */     VSCP_EEPROM_VSCP_END + 195,
    /* REG3_KELVIN1W_ROM_CODE_TEMP6_BYTE5        */     VSCP_EEPROM_VSCP_END + 196,
    /* REG3_KELVIN1W_ROM_CODE_TEMP6_BYTE6        */     VSCP_EEPROM_VSCP_END + 197,
    /* REG3_KELVIN1W_ROM_CODE_TEMP6_BYTE7        */     VSCP_EEPROM_VSCP_END + 198, 
    /* REG3_KELVIN1W_ROM_CODE_TEMP7_BYTE0        */     VSCP_EEPROM_VSCP_END + 199,
    /* REG3_KELVIN1W_ROM_CODE_TEMP7_BYTE1        */     VSCP_EEPROM_VSCP_END + 200,
    /* REG3_KELVIN1W_ROM_CODE_TEMP7_BYTE2        */     VSCP_EEPROM_VSCP_END + 201,
    /* REG3_KELVIN1W_ROM_CODE_TEMP7_BYTE3        */     VSCP_EEPROM_VSCP_END + 202,
    /* REG3_KELVIN1W_ROM_CODE_TEMP7_BYTE4        */     VSCP_EEPROM_VSCP_END + 203,
    /* REG3_KELVIN1W_ROM_CODE_TEMP7_BYTE5        */     VSCP_EEPROM_VSCP_END + 204,
    /* REG3_KELVIN1W_ROM_CODE_TEMP7_BYTE6        */     VSCP_EEPROM_VSCP_END + 205,
    /* REG3_KELVIN1W_ROM_CODE_TEMP7_BYTE7        */     VSCP_EEPROM_VSCP_END + 206,                                                      
};

#define DECISION_MATRIX_EEPROM_START                    VSCP_EEPROM_VSCP_END + 207

///////////////////////////////////////////////////////////////////////////////
// Isr() 	- Interrupt Service Routine
//      	- Services Timer0 Overflow
//      	- Services GP3 Pin Change
//////////////////////////////////////////////////////////////////////////////

void interrupt low_priority interrupt_at_low_vector( void )
{
    // Check timer
    if (INTCONbits.TMR0IF) { // If A Timer0 Interrupt, Then

        // Reload value for 1 ms resolution
        WriteTimer0(TIMER0_RELOAD_VALUE);

        vscp_timer++;
        vscp_configtimer++;
        measurement_clock++;
        timeout_clock++;
        sendTimer++;

        // Check for init. button
        if ( !( PORTC & 0x01 ) ) {
            // Active
            vscp_initbtncnt++;
        }
        else {
            vscp_initbtncnt = 0;
        }

        // Status LED
        vscp_statuscnt++;
        if ( ( VSCP_LED_BLINK1 ==
                 vscp_initledfunc) && (vscp_statuscnt > 100)) {
            if ( PORTC & 0x02 ) {
                PORTC &= ~0x02;
            }
            else {
                PORTC |= 0x02;
            }
            vscp_statuscnt = 0;
        }
        else if (VSCP_LED_ON == vscp_initledfunc) {
            PORTC |= 0x02;
            vscp_statuscnt = 0;
        }
        else if (VSCP_LED_OFF == vscp_initledfunc) {
            PORTC &= ~0x02;
            vscp_statuscnt = 0;
        }

        INTCONbits.TMR0IF = 0; // Clear Timer0 Interrupt Flag

    } // Timer & button

    // Check ADC
    if ( PIR1bits.ADIF ) {

#if defined(_18F2580)        
        switch (0x3C & ADCON0) {
#else
        switch (0x7C & ADCON0) {   
#endif

            case SELECT_ADC_TEMP0:
                
                // Read conversion
                adc_value = ADRES;                
                break;
                
            default:
                ADCON0 = SELECT_ADC_TEMP0 + 1;
                break;

        }
        
        // Start new conversion
        ADCON0 = SELECT_ADC_TEMP0 + 1;

        // Start another conversion
        ConvertADC();

        PIR1bits.ADIF = 0; // Reset interrupt flag

    } // ADC

    return;
}

///////////////////////////////////////////////////////////////////////////////
// input
//
// Set pin as input
//  

uint8_t input( uint8_t pin )
{
    uint8_t rv = 1;
    
    switch ( pin ) {
        
        case TEMP_CHANNEL0:
            TRISCbits.TRISC7 = 1;
            rv = PORTCbits.RC7;
            break;
                    
        case TEMP_CHANNEL1:
            TRISCbits.TRISC6 = 1;
            rv = PORTCbits.RC6;
            break;
                    
        case TEMP_CHANNEL2:
            TRISCbits.TRISC4 = 1;
            rv = PORTCbits.RC4;
            break;
                    
        case TEMP_CHANNEL3:
            TRISCbits.TRISC3 = 1;
            rv = PORTCbits.RC3;
            break;            
            
    }
        
    return rv;
}

///////////////////////////////////////////////////////////////////////////////
// output
//
// Set pin as output
//  

extern void output( uint8_t pin )
{
    switch ( pin ) {
        
        case TEMP_CHANNEL0:
            TRISCbits.TRISC7 = 0;
            break;
                    
        case TEMP_CHANNEL1:
            TRISCbits.TRISC6 = 0;
            break;
                    
        case TEMP_CHANNEL2:
            TRISCbits.TRISC4 = 0;
            break;
                    
        case TEMP_CHANNEL3:
            TRISCbits.TRISC3 = 0;
            break;            
            
    }
        
}

///////////////////////////////////////////////////////////////////////////////
// output_low
//
// Set output pin low
//    
    
void output_low( uint8_t pin )
{
    switch ( pin ) {
        
        case TEMP_CHANNEL0:
            TRISCbits.TRISC7 = 0;
            LATC7 = 0;
            break;
                    
        case TEMP_CHANNEL1:
            TRISCbits.TRISC6 = 0;
            PORTCbits.RC6 = 0;
            break;
                    
        case TEMP_CHANNEL2:
            TRISCbits.TRISC4 = 0;
            PORTCbits.RC4 = 0;
            break;
                    
        case TEMP_CHANNEL3:
            TRISCbits.TRISC3 = 0;
            PORTCbits.RC3 = 0;
            break;            
            
    }
}

///////////////////////////////////////////////////////////////////////////////
// output_high
//
// Set output pin high
//  

void output_high( uint8_t pin )
{
    switch ( pin ) {
        
        case TEMP_CHANNEL0:
            TRISCbits.TRISC7 = 0;
            LATCbits.LATC7 = 1;
            break;
                    
        case TEMP_CHANNEL1:
            TRISCbits.TRISC6 = 0;
            PORTCbits.RC6 = 1;
            break;
                    
        case TEMP_CHANNEL2:
            TRISCbits.TRISC4 = 0;
            PORTCbits.RC4 = 1;
            break;
                    
        case TEMP_CHANNEL3:
            TRISCbits.TRISC3 = 0;
            PORTCbits.RC3 = 1;
            break;            
            
    }
}




//***************************************************************************
// Main() - Main Routine
//***************************************************************************

void main()
{
    uint32_t i;
    
    // Initialise module hardware
    init();
    
    // Initialise the VSCP functionality
    vscp_init(); 
    
    // Check VSCP persistent storage and
    // restore if needed
    vscp_check_pstorage();    
    
    // Initialisation of in memory variables
    OW_MasterResetPulseTime = ( eeprom_read( REG0_KELVIN1W_MASTER_RESET_PULSE_MSB ) << 8 +
                                    eeprom_read( REG0_KELVIN1W_MASTER_RESET_PULSE_LSB ) );;
    OW_PresenceWait = eeprom_read(reg2eeprom_pg0[ REG0_KELVIN1W_PRESENCE_WAIT ]);  
    OW_PresenceFin = ( eeprom_read( REG0_KELVIN1W_PRESENCE_FIN_MSB ) << 8 ) +
                        eeprom_read( REG0_KELVIN1W_PRESENCE_FIN_LSB );
    OW_MasterBitStart = eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_MASTER_BITSTART_DELAY ] );
    OW_DelayBitRead = eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_BIT_READ_DELAY ] );
    OW_DelayBitWait = eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_BIT_READ_WAIT ] );
    OW_DelayBitWrite = eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_BIT_WRITE_DELAY ] );
    

    while (TRUE) { // Loop Forever

        ClrWdt(); // Feed the dog

        if ( ( vscp_initbtncnt > 2500 ) &&
             ( VSCP_STATE_INIT != vscp_node_state ) ) {

            // Init. button pressed
            vscp_nickname = VSCP_ADDRESS_FREE;
            eeprom_write(VSCP_EEPROM_NICKNAME, VSCP_ADDRESS_FREE);
            vscp_init();

        }

        // Check for a valid  event
        vscp_imsg.flags = 0;
        vscp_getEvent();

        switch (vscp_node_state) {

            case VSCP_STATE_STARTUP: // Cold/warm reset

                // Get nickname from EEPROM
                if (VSCP_ADDRESS_FREE == vscp_nickname) {
                    // new on segment need a nickname
                    vscp_node_state = VSCP_STATE_INIT;
                }
                else {
                    // been here before - go on
                    vscp_node_state = VSCP_STATE_ACTIVE;
                    vscp_goActiveState();
                }
                break;

            case VSCP_STATE_INIT: // Assigning nickname
                vscp_handleProbeState();
                break;

            case VSCP_STATE_PREACTIVE: // Waiting for host initialisation
                vscp_goActiveState();
                break;

            case VSCP_STATE_ACTIVE: // The normal state

                // Check for incoming message?
                if (vscp_imsg.flags & VSCP_VALID_MSG) {

                    if ( VSCP_CLASS1_PROTOCOL == vscp_imsg.vscp_class ) {

                        // Handle protocol event
                        vscp_handleProtocolEvent();

                    }
                    else if ( ( VSCP_CLASS1_CONTROL == vscp_imsg.vscp_class ) &&
                                ( VSCP_TYPE_CONTROL_SYNC == vscp_imsg.vscp_type ) ) {
                        handle_sync();
                    }

                }
                break;

            case VSCP_STATE_ERROR: // Everything is *very* *very* bad.
                vscp_error();
                break;

            default: // Should not be here...
                vscp_node_state = VSCP_STATE_STARTUP;
                break;

        }


        // do a measurement if needed
        if ( measurement_clock > 1000 ) {

            measurement_clock = 0;
            seconds++;
            
            if (VSCP_STATE_ACTIVE == vscp_node_state) {
                doOneSecondWork();
            }
            

            // Temperature report timers are only updated if in active
            // state
            if (VSCP_STATE_ACTIVE == vscp_node_state) {
                for (i = 0; i < 8; i++) {
                    seconds_temp[ i ]++;
                }
            }

            if (seconds > 60) {
                seconds = 0;
            }

            // Do VSCP one second jobs
            // Check if alarm events should be sent
            if (VSCP_STATE_ACTIVE == vscp_node_state) {
                vscp_doOneSecondWork();
            }
      
        } // One second
        
        // Do some work
        if ( VSCP_STATE_ACTIVE == vscp_node_state ) {           
            doWork();
        }

    } // while
}

///////////////////////////////////////////////////////////////////////////////
// Init - Initialisation Routine
//  

void init()
{
    //uint8_t msgdata[ 8 ];

    // Initialise data
    init_app_ram();

    // Initialise the uP

    // PORTA
    // RA0/AN0 - input
    // RA1/AN1 - input
    // RA2/AN2 - input
    TRISA = 0x07;

    // PortB
    // RB7 - Not used.
    // RB& - Not used.
    // RB5 - Not used.
    // RB4/AN9 - input
    // RB3 CAN RX - input
    // RB2 CAN TX - output
    // RB1/AN8 - input
    // RB0/AN10 -input
    TRISB = 0x1B;

    // RC7 - Input - T4.
    // RC6 - Input - T3.
    // RC5 - Output - Not used.
    // RC4 - Input - T2
    // RC3 - Input - T1.
    // RC2 - Output - Not used.
    // RC1 - Output - Status LED - Default off
    // RC0 - Input  - Init. button

    PORTC = 0b11011001;
    TRISC = 0b11011001;    

    OpenTimer0(TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT & T0_PS_1_8);
    WriteTimer0(TIMER0_RELOAD_VALUE);
    
#if defined(_18F2580)     
    OpenADC(ADC_FOSC_32 & ADC_RIGHT_JUST & ADC_20_TAD,
                ADC_CH0 & ADC_INT_ON & ADC_11ANA &
                    ADC_VREFPLUS_VDD & ADC_VREFMINUS_VSS,
                15 );
#else if defined(_18F25K80) || defined(_18F26K80) || defined(_18F45K80) || defined(_18F46K80) || defined(_18F65K80) || defined(_18F66K80)
        
    // Disable comparators
    CM1CON = 0x00;
    CM2CON = 0x00;
    CVRCON = 0; 
    
    ANCON0bits.ANSEL0=1;

    // OpenADC_Page16
    OpenADC( ADC_FOSC_32 & ADC_RIGHT_JUST & ADC_20_TAD,
                ADC_CH0 & ADC_INT_ON,
                ADC_NEG_CH0 & ADC_REF_VDD_VDD & ADC_REF_VDD_VSS );
#endif  

    // Initialise CAN
    ECANInitialize();

    // Must be in configure mode to change many of settings.
    //ECANSetOperationMode(ECAN_OP_MODE_CONFIG);

    // Return to Normal mode to communicate.
    //ECANSetOperationMode(ECAN_OP_MODE_NORMAL);

    /*
            msgdata[ 0 ] = 1;
            msgdata[ 1 ] = 2;
            msgdata[ 2 ] = 3;
            msgdata[ 3 ] = 4;

            if ( vscp18f_sendMsg( 0x123,
     *                              msgdata,
     *                              4,
     *        CAN_TX_PRIORITY_0 & CAN_TX_XTD_FRAME & CAN_TX_NO_RTR_FRAME ) ) {
                    ;
            }

     */

    // Enable global interrupt
    INTCONbits.GIE = 1;
    
    // Start ADC conversions
    ConvertADC();

    return;
}

///////////////////////////////////////////////////////////////////////////////
// init._app._ram
//

void init_app_ram(void)
{
    char i;

    measurement_clock = 0; // start a new measurement cycle
    seconds = 0;

    // no temp. reads yet
    for ( i=0; i<9; i++ ) {
        arrayTemp[ i ] = 0;
    }
    
    currentSensor = 0;

    // No low temperature alarms
    low_alarm = 0;

    // No high temperature alarms
    high_alarm = 0;
}



///////////////////////////////////////////////////////////////////////////////
// init_app_eeprom
//

void init_app_eeprom(void)
{
    unsigned char i, j;
        
    // * * *  Page 0  * * *

    // Module zone/sub zone
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_ZONE ], 0 );
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_SUBZONE ], 0 );
    
    // Channel zone/sub zone
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP0_ZONE ], 1 );
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP0_SUBZONE ], 1 );
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP1_ZONE ], 2 );
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP1_SUBZONE ], 2 );
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP2_ZONE ], 3 );
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP2_SUBZONE ], 3 );
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP3_ZONE ], 4 );
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP3_SUBZONE ], 4 );
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP4_ZONE ], 5 );
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP4_SUBZONE ], 5 );
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP5_ZONE ], 6 );
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP5_SUBZONE ], 6 );
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP6_ZONE ], 7 );
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP6_SUBZONE ], 7 );
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP7_ZONE ], 8 );
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP7_SUBZONE ], 8 );
    
    for ( i=0; i<8; i++ ) {
        eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_CTRL_REG0_LOW + 2*i ], DEFAULT_CONTROL_REG_LOW );
        eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_CTRL_REG0_HIGH + 2*i ], DEFAULT_CONTROL_REG_HIGH );
        
        eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_REPORT_INTERVAL_0 + i ], 0 );
        
        eeprom_write( reg2eeprom_pg2[ REG2_KELVIN1W_LOW_ALARM_SET_POINT0_MSB + 2*i ], DEFAULT_LOW_ALARM_MSB );
        eeprom_write( reg2eeprom_pg2[ REG2_KELVIN1W_LOW_ALARM_SET_POINT0_LSB + 2*i ], DEFAULT_LOW_ALARM_LSB );
        eeprom_write( reg2eeprom_pg2[ REG2_KELVIN1W_HIGH_ALARM_SET_POINT0_MSB + 2*i ], DEFAULT_HIGH_ALARM_MSB );
        eeprom_write( reg2eeprom_pg2[ REG2_KELVIN1W_HIGH_ALARM_SET_POINT0_LSB + 2*i ], DEFAULT_HIGH_ALARM_LSB );
        
        eeprom_write( reg2eeprom_pg2[ REG2_KELVIN1W_ABSOLUTE_LOW_TEMP0_MSB + 2*i ], DEFAULT_ABSOLUTE_LOW_MSB);
        eeprom_write( reg2eeprom_pg2[ REG2_KELVIN1W_ABSOLUTE_LOW_TEMP0_LSB + 2*i ], DEFAULT_ABSOLUTE_LOW_LSB );
        eeprom_write( reg2eeprom_pg2[ REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP0_MSB + 2*i ], DEFAULT_ABSOLUTE_HIGH_MSB );
        eeprom_write( reg2eeprom_pg2[ REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP0_LSB + 2*i ], DEFAULT_ABSOLUTE_HIGH_LSB );
        
        eeprom_write( reg2eeprom_pg2[ REG2_KELVIN1W_HYSTERESIS_TEMP0 + i ], DEFAULT_HYSTERESIS );
        
        eeprom_write( reg2eeprom_pg2[ REG2_KELVIN1W_CALIBRATION_TEMP0_MSB + 2*i ], 0 );
        eeprom_write( reg2eeprom_pg2[ REG2_KELVIN1W_CALIBRATION_TEMP0_LSB + 2*i ], 0 );
    }
    
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_BCONSTANT0_MSB ], DEFAULT_B_CONSTANT_SENSOR0_MSB );
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_BCONSTANT0_LSB ], DEFAULT_B_CONSTANT_SENSOR0_LSB );
    
    // One wire timing
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_MASTER_RESET_PULSE_MSB ], ( DS1820_RST_PULSE >> 8 ) & 0xff );
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_MASTER_RESET_PULSE_LSB ], DS1820_RST_PULSE & 0xff );
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_PRESENCE_WAIT ], DS1820_PRESENCE_WAIT );
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_PRESENCE_FIN_MSB ], ( DS1820_PRESENCE_FIN >> 8 ) & 0xff );
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_PRESENCE_FIN_LSB ], DS1820_PRESENCE_FIN & 0xff );
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_MASTER_BITSTART_DELAY ], DS1820_MSTR_BITSTART );
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_BIT_READ_DELAY ], DS1820_BITREAD_DLY );
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_BIT_READ_WAIT ], DS1820_BITREAD_WAIT );
    eeprom_write( reg2eeprom_pg0[ REG0_KELVIN1W_BIT_WRITE_DELAY ], DS1820_BITWRITE_DLY );
    
    // This is a dummy for the channel control register
    eeprom_write( reg2eeprom_pg3[ REG3_KELVIN1W_CHANNEL_CONTROL_REGISTER ] , 0 );
    
    // ROM Code for eight sensors are set to no ROM code
    for ( i=0; i<64; i++ ) {
        eeprom_write( reg2eeprom_pg3[ REG3_KELVIN1W_ROM_CODE_TEMP0_MSB + i ] , 0 );
    }
    
    // * * * Decision Matrix * * *
    
    // All elements disabled.
    for ( i = 0; i<DESCION_MATRIX_ROWS; i++ ) {
        for ( j = 0; j < 8; j++ ) {
            eeprom_write( DECISION_MATRIX_EEPROM_START + 8*i + j, 0 );
        }
    }
    
    // Set DM filters
    calculateSetFilterMask();
    
}

///////////////////////////////////////////////////////////////////////////////
// GetROMCodeFromEEPROM
//
// Fill ROM address in working ROM code location and returns TRUE if
// ROM code is non zero.
//

BOOL loadROMCodeFromEEPROM( uint8_t currentSensor )
{
    uint8_t i;
    uint8_t sum=0;
    
    for ( i=0; i<8; i++ ) {
        sum |= ( romAddrOneWire[ 7-i ] = 
                eeprom_read( reg2eeprom_pg3[ REG3_KELVIN1W_ROM_CODE_TEMP0_BYTE0+ 8*currentSensor + i ] ) );
    }
    
    return sum;
}

///////////////////////////////////////////////////////////////////////////////
// saveROMCode
//

void saveROMCodeToEEPROM( uint8_t channel, uint8_t idxSensor )
{
    uint8_t i;
    
    for ( i=0; i<8; i++ ) {
        eeprom_write( reg2eeprom_pg3[ REG3_KELVIN1W_ROM_CODE_TEMP0_BYTE0 + 16*channel + 8*idxSensor + i ], 
                                        romAddrOneWire[ 7-i ] );
    }
}

///////////////////////////////////////////////////////////////////////////////
// doWork
//
// The actual work is done here.
//

void doWork(void)
{
/*
    double temp;
    char buf[80];

    adc_low = DS1820_FindFirstDevice( TEMP_CHANNEL3 );
    temp = DS1820_GetTempFloat( TEMP_CHANNEL3 );
    uint16_t raw = DS1820_GetTempRaw( TEMP_CHANNEL3, TRUE );
    DS1820_GetTempString( raw, buf );
    adc_low = 1;
*/ 
}

///////////////////////////////////////////////////////////////////////////////
// doOneSecondWork
//

void doOneSecondWork(void) 
{
    uint8_t tmp;
    uint8_t i;
    int16_t setpoint;
    
    // Read temperature (conversion started last round))
    
    // Read temp if there is a valid ROM code
    if ( loadROMCodeFromEEPROM( currentSensor ) ) {
        arrayTemp[ currentSensor ] = DS1820_LateGetTempFloat( currentSensor/2 );
    }
    else {
        // Non valid ROM code set temp to zero
        arrayTemp[ currentSensor ] = 0;
    }
    currentSensor++;
    if ( currentSensor > 7 ) currentSensor = 0;
    
    // Start next temp conversion if there is a valid ROM code
    if ( loadROMCodeFromEEPROM( currentSensor ) ) {
        DS1820_StartTempConversion( TEMP_CHANNEL0 + currentSensor/2 );
    }
    
    
    //*********************************************************************
    //                          On-board sensor
    //*********************************************************************
    
    uint16_t B;
    double resistance;
    double Rinf;
    double temp;
    double v;
    
    // Use B-constant
    // ==============
    // http://en.wikipedia.org/wiki/Thermistor
    // R1 = (R2V - R2V2) / V2  R2= 10K, V = 5V,  V2 = adc * voltage/1024
    // T = B / ln(r/Rinf)
    // Rinf = R0 e (-B/T0), R0=10K, T0 = 273.15 + 25 = 298.15

    B = construct_unsigned16( eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_BCONSTANT0_MSB ] ),
                                eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_BCONSTANT0_LSB ] ) );
               
    Rinf = 10000.0 * exp( B/-298.15 );
                
#if defined(_18F2580)   
    v = 5.0 * (double)adc_value/1024;
#else  
    v = 5.0 * (double)adc_value/4096;
#endif                
    // R1 = (R2V - R2V2) / V2  R2= 10K, V = 5V,  V2 = adc * voltage/4096
    resistance = ( 10000.0 * ( 5.0 - v ) ) / v;
    temp = (double)B / log( resistance/Rinf );
    temp -= 273.15; // Convert Kelvin to Celsius
                
    //avarage = testadc;
    /*  https://learn.adafruit.com/thermistor/using-a-thermistor
    avarage = (1023/avarage) - 1;
    avarage = 10000 / avarage;      // Resistance of termistor
    //temp = avarage/10000;           // (R/Ro)
    temp = 10000/avarage;
    temp = log(temp);               // ln(R/Ro)
    temp /= B;                      // 1/B * ln(R/Ro)
    temp += 1.0 / (25 + 273.15);    // + (1/To)
    temp = 1.0 / temp;              // Invert
    temp -= 273.15;
    */
    arrayTemp[ 8 ] = ( arrayTemp[ 8 ] + temp + 
            ( reg2eeprom_pg2[ REG2_KELVIN1W_CALIBRATION_TEMP8_MSB ] << 8 +
              reg2eeprom_pg2[ REG2_KELVIN1W_CALIBRATION_TEMP8_LSB ] )/100 ) / 2;
            
    //*********************************************************************
    // Check if this is the lowest temperature ever
    //*********************************************************************
    for ( i=0; i<8; i++ ) {
   
        if ( 100*arrayTemp[ i ] < 
                construct_signed16( eeprom_read( reg2eeprom_pg2[ REG2_KELVIN1W_ABSOLUTE_LOW_TEMP0_MSB ] + 2*i ), 
                                    eeprom_read( reg2eeprom_pg2[ REG2_KELVIN1W_ABSOLUTE_LOW_TEMP0_LSB ] + 2*i ) ) ) {
            // Store new lowest value
            eeprom_write( reg2eeprom_pg2[ REG2_KELVIN1W_ABSOLUTE_LOW_TEMP0_MSB ] + 2*i, 
                                            ( (uint16_t)( 100 * arrayTemp[ i ] ) ) >> 8 );
            eeprom_write( reg2eeprom_pg2[ REG2_KELVIN1W_ABSOLUTE_LOW_TEMP0_LSB ] + 2*i, 
                                            ( (uint16_t)( 100 * arrayTemp[ i ] ) ) & 0xff );
        }

        //*********************************************************************
        //           Check if this is the highest temperature ever
        //*********************************************************************
        
        if ( 100*arrayTemp[ i ] > 
                (double)construct_signed16( eeprom_read( reg2eeprom_pg2[ REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP0_MSB ] + 2*i), 
                                           eeprom_read( reg2eeprom_pg2[ REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP0_LSB ] + 2*i ) ) ) {
            // Store new lowest value
            eeprom_write( reg2eeprom_pg2[ REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP0_MSB ] + 2*i, 
                                            ( (uint16_t)( 100*arrayTemp[ i ] ) ) >> 8 );
            eeprom_write( reg2eeprom_pg2[ REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP0_LSB ] + 2*i,  
                                            ( (uint16_t)( 100*arrayTemp[ i ] ) ) & 0xff );
        }
        
        //*********************************************************************
        //         Check if temperature report events should be sent
        //*********************************************************************
        tmp = eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_REPORT_INTERVAL_0 ] + i);
        if ( tmp && ( seconds_temp[ i ] > tmp ) ) {

            // Send event
            if ( sendTempEvent( i ) ) {
                seconds_temp[ i ] = 0;
            }

        }

        //*********************************************************************
        //                      Check for continuous alarm 
        //*********************************************************************
        if ( MASK_CONTROL0_CONTINUOUS & 
                eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_CTRL_REG0_LOW ] + 2*i ) ) {

            // If low alarm active for sensor
            if (low_alarm & (1 << i)) {

                // Alarm must be enabled
                if ( eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_CTRL_REG0_LOW ] + 2*i ) & 
                        MASK_CONTROL0_LOW_ALARM ) {

                    vscp_omsg.priority = VSCP_PRIORITY_HIGH;
                    vscp_omsg.flags = VSCP_VALID_MSG + 3;

                    // Should ALARM or TURNON/TURNOFF events be sent
                    if ( eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_CTRL_REG0_LOW ] + 2*i) & 
                            MASK_CONTROL0_TURNX ) {

                        if ( eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_CTRL_REG0_LOW ] + 2*i ) & 
                                MASK_CONTROL0_TURNX_INVERT ) {
                            vscp_omsg.vscp_class = VSCP_CLASS1_CONTROL;
                            vscp_omsg.vscp_type = VSCP_TYPE_CONTROL_TURNON;
                        } 
                        else {
                            vscp_omsg.vscp_class = VSCP_CLASS1_CONTROL;
                            vscp_omsg.vscp_type = VSCP_TYPE_CONTROL_TURNOFF;
                        }

                    } 
                    else {
                        // Alarm event should be sent
                        vscp_omsg.vscp_class = VSCP_CLASS1_ALARM;
                        vscp_omsg.vscp_type = VSCP_TYPE_ALARM_ALARM;
                    }

                    vscp_omsg.data[ 0 ] = i; // Index = sensor
                    vscp_omsg.data[ 1 ] =
                            eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP0_ZONE ] + 2*i );      // Zone
                    vscp_omsg.data[ 2 ] =
                            eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP0_SUBZONE ] + 2*i );   // Sub zone

                    // Send event
                    // We allow for missing to send this event
                    // as it will be sent next second instead.
                    vscp_sendEvent();

                }
                
            }

            // If  high alarm active for sensor
            if ( high_alarm & (1 << i) ) {

                // Should ALARM or TURNON/TURNOFF events be sent
                if ( ( eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_CTRL_REG0_LOW ] + 2*i) & 
                                    MASK_CONTROL0_HIGH_ALARM ) ) {

                    vscp_omsg.priority = VSCP_PRIORITY_HIGH;
                    vscp_omsg.flags = VSCP_VALID_MSG + 3;

                    if ( eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_CTRL_REG0_LOW ] + 2*i) & 
                                        MASK_CONTROL0_TURNX ) {
                        
                        if ( eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_CTRL_REG0_LOW ] + 2*i) & 
                                        MASK_CONTROL0_TURNX_INVERT ) {
                            vscp_omsg.vscp_class = VSCP_CLASS1_CONTROL;
                            vscp_omsg.vscp_type = VSCP_TYPE_CONTROL_TURNOFF;
                        } 
                        else {
                            vscp_omsg.vscp_class = VSCP_CLASS1_CONTROL;
                            vscp_omsg.vscp_type = VSCP_TYPE_CONTROL_TURNON;
                        }
                    } 
                    else {
                        // Alarm event should be sent
                        vscp_omsg.vscp_class = VSCP_CLASS1_ALARM;
                        vscp_omsg.vscp_type = VSCP_TYPE_ALARM_ALARM;
                    }

                    vscp_omsg.data[ 0 ] = i;  // Index = sensor
                    vscp_omsg.data[ 1 ] = 
                            eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP0_ZONE ] + 2*i );      // Zone
                    vscp_omsg.data[ 2 ] = 
                            eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP0_SUBZONE ] + 2*i );   // Sub zone

                    // Send event
                    // We allow for missing to send this event
                    // as it will be sent next second instead.
                    vscp_sendEvent();
                    
                }
                
            }
        }       
        
        
      
        //*********************************************************************
        // Check if we have a low alarm condition 
        //*********************************************************************
        if ( low_alarm & ( 1 << i ) ) {

            // We have an alarm condition already
            setpoint = construct_signed16( eeprom_read( reg2eeprom_pg2[ REG2_KELVIN1W_LOW_ALARM_SET_POINT0_MSB ] + 2*i ),
                                            eeprom_read( reg2eeprom_pg2[ REG2_KELVIN1W_LOW_ALARM_SET_POINT0_LSB ] + 2*i ) ) +
                        (int8_t) eeprom_read( reg2eeprom_pg2[ REG2_KELVIN1W_HYSTERESIS_TEMP0 ] + i );

            // Check if it is no longer valid
            // that is under hysteresis so we can rest
            // alarm condition
            if ( arrayTemp[ i ] > ( setpoint * 100 ) ) {

                // Reset alarm condition
                low_alarm &= ~(1 << i);

            }

        } 
        else {

            // We do not have a low alarm condition already
            // check if we should have
            setpoint = construct_signed16( eeprom_read( reg2eeprom_pg2[ REG2_KELVIN1W_LOW_ALARM_SET_POINT0_MSB ] + 2*i ),
                                            eeprom_read( reg2eeprom_pg2[ REG2_KELVIN1W_LOW_ALARM_SET_POINT0_LSB ] + 2*i ) );

            if ( arrayTemp[ i ] < ( setpoint * 100 ) ) {

                // We have a low alarm condition
                low_alarm |= (1 << i);

                // Set module alarm flag
                // Note that this bit is set even if we are unable
                // to send an alarm event.
                vscp_alarmstatus |= MODULE_LOW_ALARM;

                // Should ALARM events be sent
                if ( eeprom_read(i + reg2eeprom_pg0[ REG0_KELVIN1W_CTRL_REG0_LOW ] ) & MASK_CONTROL0_LOW_ALARM ) {

                    vscp_omsg.priority = VSCP_PRIORITY_HIGH;
                    vscp_omsg.flags = VSCP_VALID_MSG + 3;

                    // Should TurnOn/TurnOff events be sent
                    if (eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_CTRL_REG0_LOW ] + i ) & 
                                                            MASK_CONTROL0_TURNX ) {

                        if (eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_CTRL_REG0_LOW ] + i ) & 
                                                            MASK_CONTROL0_TURNX_INVERT) {
                            vscp_omsg.vscp_class = VSCP_CLASS1_CONTROL;
                            vscp_omsg.vscp_type = VSCP_TYPE_CONTROL_TURNON;
                        } 
                        else {
                            vscp_omsg.vscp_class = VSCP_CLASS1_CONTROL;
                            vscp_omsg.vscp_type = VSCP_TYPE_CONTROL_TURNOFF;
                        }

                    }
                    else {
                        vscp_omsg.vscp_class = VSCP_CLASS1_ALARM;
                        vscp_omsg.vscp_type = VSCP_TYPE_ALARM_ALARM;
                    }

                    vscp_omsg.data[ 0 ] = i; // Index
                    vscp_omsg.data[ 1 ] = 
                            eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP0_ZONE ] + 2*i ) ;    // Zone
                    vscp_omsg.data[ 2 ] = 
                            eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP0_SUBZONE ] + 2*i ); // Sub zone

                    // Send event
                    if (!vscp_sendEvent()) {
                        // Could not send alarm event
                        // Reset alarm - we try again next round
                        low_alarm &= ~(1 << i);
                    }

                }
            }
        }
        
        
        //*********************************************************************
        // Check if we have a high alarm condition 
        //*********************************************************************
        if (high_alarm & (1 << i)) {

            // We have an alarm condition already

            setpoint = construct_signed16( eeprom_read( reg2eeprom_pg2[ REG2_KELVIN1W_HIGH_ALARM_SET_POINT0_MSB ] + 2 * i ),
                                            eeprom_read( reg2eeprom_pg2[ REG2_KELVIN1W_HIGH_ALARM_SET_POINT0_LSB ] + 2 * i ) ) -
                            (int8_t)eeprom_read( reg2eeprom_pg2[ REG2_KELVIN1W_HYSTERESIS_TEMP0 ] + i);

            // Under hysteresis so we can reset condition
            if ( arrayTemp[ i ] < (setpoint * 100) ) {

                // Reset alarm
                high_alarm &= ~(1 << i);

            }

        } 
        else {

            // We do not have an alarm condition
            // check for one

            setpoint = construct_signed16( eeprom_read( reg2eeprom_pg2[ REG2_KELVIN1W_HIGH_ALARM_SET_POINT0_MSB ] + 2 * i ),
                                            eeprom_read( reg2eeprom_pg2[ REG2_KELVIN1W_HIGH_ALARM_SET_POINT0_LSB ] + 2 * i ) );

            if ( arrayTemp[ i ] > (setpoint * 100)) {

                // We have a low alarm condition
                high_alarm |= (1 << i);

                // Set module alarm flag
                // Note that this bit is set even if we are unable
                // to send an alarm event.

                vscp_alarmstatus |= MODULE_HIGH_ALARM;

                // Should ALARM or TURNON/TURNOFF events be sent
                if ( eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_CTRL_REG0_LOW ] + i ) & 
                                                    MASK_CONTROL0_HIGH_ALARM ) {

                    vscp_omsg.priority = VSCP_PRIORITY_HIGH;
                    vscp_omsg.flags = VSCP_VALID_MSG + 3;

                    if ( eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_CTRL_REG0_LOW ] + i) & 
                                                    MASK_CONTROL0_TURNX ) {

                        if (eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_CTRL_REG0_LOW ] + i) & 
                                                    MASK_CONTROL0_TURNX_INVERT ) {
                            vscp_omsg.vscp_class = VSCP_CLASS1_CONTROL;
                            vscp_omsg.vscp_type = VSCP_TYPE_CONTROL_TURNOFF;
                        } 
                        else {
                            vscp_omsg.vscp_class = VSCP_CLASS1_CONTROL;
                            vscp_omsg.vscp_type = VSCP_TYPE_CONTROL_TURNON;
                        }
                    }
                    else {
                        vscp_omsg.vscp_class = VSCP_CLASS1_ALARM;
                        vscp_omsg.vscp_type = VSCP_TYPE_ALARM_ALARM;
                    }

                    vscp_omsg.data[ 0 ] = i; // Index
                    vscp_omsg.data[ 1 ] = 
                            eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP0_ZONE ] + 2 * i);      // Zone
                    vscp_omsg.data[ 2 ] = 
                            eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP0_SUBZONE ] + 2 * i);   // Sub zone

                    // Send event
                    if ( !vscp_sendEvent() ) {
                        // Could not send alarm event
                        // Reset alarm - we try again next round
                        high_alarm &= ~(1 << i);
                    }
                
                }
            
            }
        
        }
    
    }

}

///////////////////////////////////////////////////////////////////////////////
// sendTempEvent
//

int8_t sendTempEvent( uint8_t nTemp )
{
    uint8_t *p;
    double newval;
    int32_t ival;
    
    vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
    vscp_omsg.vscp_class = VSCP_CLASS1_MEASUREMENT;
    vscp_omsg.vscp_type = VSCP_TYPE_MEASUREMENT_TEMPERATURE;

    // Convert to correct unit
    switch( 0x03 & eeprom_read( nTemp + reg2eeprom_pg0[ REG0_KELVIN1W_CTRL_REG0_LOW ] ) ) {
    
        case TEMP_UNIT_KELVIN:
            // Convert to Kelvin
            newval = Celsius2Kelvin( arrayTemp[ nTemp ] );
            break;
    
        case TEMP_UNIT_FAHRENHEIT:
            // Convert to Fahrenheit
            newval = Celsius2Fahrenheit( arrayTemp[ nTemp ] );
            break;
    
        default:    
            // Defaults to Celsius
            newval = arrayTemp[ nTemp ];
            break;

    }
    
    // Pack event data in selected format
    switch ( eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_CTRL_REG0_HIGH ] + 2*nTemp ) & 
                                            MASK_CONTROL1_REMOTE_FORMAT ) {
    
        case REPORT_TEMP_AS_NORMINT:
            
            vscp_omsg.flags = VSCP_VALID_MSG + 5;
            
            // Data format
            vscp_omsg.data[ 0 ] = 0b10000000 | // Normalized integer
                ((0x03 & eeprom_read( nTemp + reg2eeprom_pg0[ REG0_KELVIN1W_CTRL_REG0_LOW ] ) ) << 3 ) | // Unit
                nTemp; // Sensor
            
            // Exponent 
            vscp_omsg.data[ 1 ] = 0x83;

            ival = (int32_t)newval*1000;
            vscp_omsg.data[ 2 ] = ( ival >> 16 ) & 0xff;
            vscp_omsg.data[ 3 ] = ( ival >> 8 ) & 0xff;
            vscp_omsg.data[ 4 ] = ival & 0xff;
            break;
            
        case REPORT_TEMP_AS_STRING:
                
            // Data format
            vscp_omsg.data[ 0 ] = 0b01000000 | // Normalized integer
                ((0x03 & eeprom_read( nTemp + reg2eeprom_pg0[ REG0_KELVIN1W_CTRL_REG0_LOW ] ) ) << 3 ) | // Unit
                nTemp; // Sensor
            
            {
                unsigned char buf[ 8 ];
                
                memset( buf, 0, 8 );
                sprintf( buf, "%3.3f", newval );
                if ( strlen( buf ) >= 7 ) {
                    memcpy( vscp_omsg.data + 1, buf, 7 );
                    vscp_omsg.flags = VSCP_VALID_MSG + 1 + 7;
                }
                else {
                    strcpy( vscp_omsg.data + 1, buf );
                    vscp_omsg.flags = VSCP_VALID_MSG + 1 + strlen( vscp_omsg.data + 1 );
                }
                
            }
            
            break;    

        default:    
        case REPORT_TEMP_AS_FLOAT:
            
            vscp_omsg.flags = VSCP_VALID_MSG + 5;
            
            // Data format
            vscp_omsg.data[ 0 ] = 0b10100000 | // normalized integer
                ((0x03 & eeprom_read( nTemp + reg2eeprom_pg0[ REG0_KELVIN1W_CTRL_REG0_LOW ] ) ) << 3 ) | // Unit
                nTemp; // Sensor
            
            // Data
            p = (uint8_t *)&newval;
            vscp_omsg.data[ 1 ] = *(p+3);   // Send it big-endian (PIC & PC are little-endian)
            vscp_omsg.data[ 2 ] = *(p+2);
            vscp_omsg.data[ 3 ] = *(p+1);
            vscp_omsg.data[ 4 ] = *(p+0);
            break;    
    }
    
    // Send event
    if ( !vscp_sendEvent() ) {
        return FALSE;
    }

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// setEventData
//

void setEventData(int v, unsigned char unit)
{
    double newval;
    int ival;

    if (TEMP_UNIT_KELVIN == unit) {
        // Convert to Kelvin
        newval = Celsius2Kelvin( v );
    } 
    else if (TEMP_UNIT_FAHRENHEIT == unit) {
        // Convert to Fahrenheit
        newval = Celsius2Fahrenheit( v );
    } 
    else {
        // Defaults to Celsius
        newval = v;
    }

    ival = (int) newval;

    vscp_omsg.data[ 2 ] = ((ival & 0xff00) >> 8);
    vscp_omsg.data[ 3 ] = (ival & 0xff);
}


///////////////////////////////////////////////////////////////////////////////
// convertTemperature
//

int8_t convertTemperature( double temp, unsigned char unit)
{
    int newval;

    if (TEMP_UNIT_KELVIN == unit) {
        // Convert to Kelvin
        newval = (int) Celsius2Kelvin(temp);
    } 
    else if (TEMP_UNIT_FAHRENHEIT == unit) {
        // Convert to Fahrenheit
        newval = (int) Celsius2Fahrenheit(temp);
    } 
    else {
        // Defaults to Celsius
        newval = (int) temp;
    }

    return newval;
}

///////////////////////////////////////////////////////////////////////////////
// handle_sync
//
// Report all temperature values
//

void handle_sync(void)
{
    uint8_t i;

    for (i=0; i<8; i++) {

        if ( ( ( 0xff == vscp_imsg.data[ 1 ] ) ||
                ( eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP0_ZONE ] + 2*i ) == vscp_imsg.data[ 1 ] ) ) &&
                ( ( 0xff == vscp_imsg.data[ 2 ] ) ||
                ( eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_TEMP0_SUBZONE ] + 2*i ) == vscp_imsg.data[ 2 ] ) ) ) {

            // We have a one second timeout
            timeout_clock = 0;
            while (!sendTempEvent(i)) {
                if (timeout_clock > 1000) break;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// vscp_readAppReg
//

uint8_t vscp_readAppReg(unsigned char reg)
{
    uint8_t rv;

    if ( 0 == vscp_page_select ) {

        if ( reg <= REG0_KELVIN1W_BIT_WRITE_DELAY ) {
            rv = eeprom_read( reg2eeprom_pg0[ reg ] );
        }

    }
    else if (1 == vscp_page_select) {
        
        if ( reg <= REG1_KELVIN1W_TEMPERATURE8_BYTE3 ) {
            uint8_t *p = (uint8_t *)&arrayTemp[reg/4];
            rv = *( p + (3-reg%4) );
            //rv = *( p + reg%4 );
        }
        
    }
    else if (2 == vscp_page_select) {
        
        if ( reg <= REG2_KELVIN1W_CALIBRATION_TEMP8_LSB ) {
            rv = eeprom_read( reg2eeprom_pg2[ reg ] );
        }
        
    }
    else if (3 == vscp_page_select) {
        
        if ( 0 == reg ) {
            rv = 0;
        }
        else if ( reg <= REG3_KELVIN1W_ROM_CODE_TEMP7_BYTE7 ) {
            rv = eeprom_read( reg2eeprom_pg3[ reg ] );
        }
        
        
    }
    else if (250 == vscp_page_select) {
        
        if ( reg < ( REG_DESCION_MATRIX + 8*DESCION_MATRIX_ROWS ) ) {
            rv = eeprom_read( DECISION_MATRIX_EEPROM_START + reg );
        }
        
    }

    return rv;

}

///////////////////////////////////////////////////////////////////////////////
// VSCP_writeAppReg
//

uint8_t vscp_writeAppReg(unsigned char reg, unsigned char val)
{
    uint8_t rv;

    rv = ~val; // error return

    if ( 0 == vscp_page_select ) {

        // Channel (sub)zones & control registers
        if ( reg <= REG0_KELVIN1W_BCONSTANT0_LSB ) {
            eeprom_write(reg2eeprom_pg0[ reg ], val);
            rv = eeprom_read(reg2eeprom_pg0[ reg ]);
        }
        else if ( reg == REG0_KELVIN1W_MASTER_RESET_PULSE_MSB ) {
            eeprom_write(reg2eeprom_pg0[ reg ], val);
            rv = eeprom_read(reg2eeprom_pg0[ reg ]);
        }
        else if ( reg == REG0_KELVIN1W_MASTER_RESET_PULSE_LSB ) {
            eeprom_write(reg2eeprom_pg0[ reg ], val);
            OW_MasterResetPulseTime = ( eeprom_read( REG0_KELVIN1W_MASTER_RESET_PULSE_MSB ) << 8 +
                                        eeprom_read( REG0_KELVIN1W_MASTER_RESET_PULSE_LSB ) );
            rv = eeprom_read(reg2eeprom_pg0[ reg ]);
        }
        else if ( reg == REG0_KELVIN1W_PRESENCE_WAIT ) {
            eeprom_write(reg2eeprom_pg0[ reg ], val);
            OW_PresenceWait = rv = eeprom_read(reg2eeprom_pg0[ reg ]);
        }
        else if ( reg == REG0_KELVIN1W_PRESENCE_FIN_MSB ) {
            eeprom_write(reg2eeprom_pg0[ reg ], val);
            rv = eeprom_read(reg2eeprom_pg0[ reg ]);
        }
        else if ( reg == REG0_KELVIN1W_PRESENCE_FIN_LSB ) {
            eeprom_write(reg2eeprom_pg0[ reg ], val);
            OW_PresenceFin = ( eeprom_read( REG0_KELVIN1W_PRESENCE_FIN_MSB ) << 8 ) +
                                        eeprom_read( REG0_KELVIN1W_PRESENCE_FIN_LSB );
            rv = eeprom_read(reg2eeprom_pg0[ reg ]);
        }
        else if ( reg == REG0_KELVIN1W_MASTER_BITSTART_DELAY ) {
            eeprom_write(reg2eeprom_pg0[ reg ], val);
            OW_MasterBitStart = rv = eeprom_read(reg2eeprom_pg0[ reg ]);
        }
        else if ( reg == REG0_KELVIN1W_BIT_READ_DELAY ) {
            eeprom_write(reg2eeprom_pg0[ reg ], val);
            OW_DelayBitRead = rv = eeprom_read(reg2eeprom_pg0[ reg ]);
        }
        else if ( reg == REG0_KELVIN1W_BIT_READ_WAIT ) {
            eeprom_write(reg2eeprom_pg0[ reg ], val);
            OW_DelayBitWait = rv = eeprom_read(reg2eeprom_pg0[ reg ]);
        }
        else if ( reg == REG0_KELVIN1W_BIT_WRITE_DELAY ) {
            eeprom_write(reg2eeprom_pg0[ reg ], val);
            OW_DelayBitWrite = rv = eeprom_read(reg2eeprom_pg0[ reg ]);
        }

    }
    else if ( 1 == vscp_page_select ) {
        
        // Can't write temperature registers
        
    }
    else if ( 2 == vscp_page_select ) {
        
        if ( reg <= REG2_KELVIN1W_CALIBRATION_TEMP8_LSB ) {
            eeprom_write( reg2eeprom_pg2[ reg ], val );
            rv = eeprom_read( reg2eeprom_pg2[ reg ] );
        }
        
    }
    else if ( 3 == vscp_page_select ) {
        
        if ( REG3_KELVIN1W_CHANNEL_CONTROL_REGISTER == reg ) {
            rv = writeChannelControl( val );
        }
        else if ( ( reg >= REG3_KELVIN1W_ROM_CODE_TEMP0_MSB ) && 
                    ( reg <= REG3_KELVIN1W_ROM_CODE_TEMP7_BYTE7 ) ) {
            eeprom_write( reg2eeprom_pg3[ reg ], val );
            rv = eeprom_read( reg2eeprom_pg3[ reg ] );
        }
        
    }
    else if ( 250 == vscp_page_select ) {
        
        if ( reg < (REG_DESCION_MATRIX + 8 * DESCION_MATRIX_ROWS  ) ) {
            eeprom_write( DECISION_MATRIX_EEPROM_START + reg, val );
            calculateSetFilterMask();  // Calculate new hardware filter
            rv = eeprom_read( DECISION_MATRIX_EEPROM_START + reg );
        }
        
    }

    return rv;
}

///////////////////////////////////////////////////////////////////////////////
// reportTokenActivity
//

void reportTokenActivity( void )
{
    vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
    vscp_omsg.vscp_class = VSCP_CLASS1_INFORMATION;
    vscp_omsg.vscp_type = VSCP_TYPE_INFORMATION_TOKEN_ACTIVITY;
    
    // * * * Frame 1 * * *
    
    vscp_omsg.flags = VSCP_VALID_MSG + 8;
                        
    // Data
    vscp_omsg.data[ 0 ] = 0b00000101;
    vscp_omsg.data[ 1 ] = reg2eeprom_pg0[ REG0_KELVIN1W_ZONE ];
    vscp_omsg.data[ 2 ] = reg2eeprom_pg0[ REG0_KELVIN1W_SUBZONE ];
    vscp_omsg.data[ 3 ] = 0;
    vscp_omsg.data[ 4 ] = romAddrOneWire[ 0 ];
    vscp_omsg.data[ 5 ] = romAddrOneWire[ 1 ];
    vscp_omsg.data[ 6 ] = romAddrOneWire[ 2 ];
    vscp_omsg.data[ 7 ] = romAddrOneWire[ 3 ];
    
    vscp_sendEvent();   // Send event
    
    // * * * Frame 2 * * *
    
    vscp_omsg.flags = VSCP_VALID_MSG + 8;
                        
    // Data
    vscp_omsg.data[ 0 ] = 0b00000101;
    vscp_omsg.data[ 1 ] = reg2eeprom_pg0[ REG0_KELVIN1W_ZONE ];
    vscp_omsg.data[ 2 ] = reg2eeprom_pg0[ REG0_KELVIN1W_SUBZONE ];
    vscp_omsg.data[ 3 ] = 1;
    vscp_omsg.data[ 4 ] = romAddrOneWire[ 4 ];
    vscp_omsg.data[ 5 ] = romAddrOneWire[ 5 ];
    vscp_omsg.data[ 6 ] = romAddrOneWire[ 6 ];
    vscp_omsg.data[ 7 ] = romAddrOneWire[ 7 ];
    
    vscp_sendEvent();   // Send event

}

///////////////////////////////////////////////////////////////////////////////
// writeChannelControl
//

uint8_t writeChannelControl( uint8_t val )
{
    uint8_t nFound0 = 0; // Number of found nodes on channel 0
    uint8_t nFound1 = 0; // Number of found nodes on channel 1
    uint8_t nFound2 = 0; // Number of found nodes on channel 2
    uint8_t nFound3 = 0; // Number of found nodes on channel 3
        
    // Should we search channel 0
    if ( val & 0b00000001 ) {
        
        // Initialise one-wire search
        nLastDiscrepancyOnewire = 0;
        bDoneFlagOneWire = FALSE;
        
        while( !bDoneFlagOneWire && DS1820_FindNextDevice( TEMP_CHANNEL0 ) ) {
            
            // Only handle sensors we recognise
            if ( ( DS1820_FAMILY_CODE_DS18S20 == romAddrOneWire[ OW_ROM_FAMILY_CODE ] ) || 
                 ( DS1820_FAMILY_CODE_DS18B20 == romAddrOneWire[ OW_ROM_FAMILY_CODE ] ) ) {
                   
                // Save only first two
                if ( nFound0 <= 1 ) { 
                    saveROMCodeToEEPROM( TEMP_CHANNEL0, nFound0 );
                }
                
                nFound0++; // Another one found
                
            }
            
            // Report Token activity if instructed to do so
            if ( val & 0b00010000 ) {
                reportTokenActivity();
            }  
            
        }
    }
    // Should we search channel 1    
    if ( val & 0b00000010 ) {
        
        // Initialise one-wire search
        nLastDiscrepancyOnewire = 0;
        bDoneFlagOneWire = FALSE;
        
        while( !bDoneFlagOneWire && DS1820_FindNextDevice( TEMP_CHANNEL1 ) ) {
            
            // Only handle sensors we recognise
            if ( ( DS1820_FAMILY_CODE_DS18S20 == romAddrOneWire[ OW_ROM_FAMILY_CODE ] ) || 
                 ( DS1820_FAMILY_CODE_DS18B20 == romAddrOneWire[ OW_ROM_FAMILY_CODE ] ) ) {
                
                // Save only first two
                if ( nFound1 <= 1 ) { 
                    saveROMCodeToEEPROM( TEMP_CHANNEL1, nFound1 );
                }
                
                nFound1++; // Another one found
                
            }
            
            // Report Token activity if instructed to do so
            if ( val & 0b00100000 ) {
                reportTokenActivity();
            }
            
        }
    }
    // Should we search channel 2
    if ( val & 0b00000100 ) {
        
        // Initialise one-wire search
        nLastDiscrepancyOnewire = 0;
        bDoneFlagOneWire = FALSE;
        
        while( !bDoneFlagOneWire && DS1820_FindNextDevice( TEMP_CHANNEL2 ) ) {
            
            // Only handle sensors we recognise
            if ( ( DS1820_FAMILY_CODE_DS18S20 == romAddrOneWire[ OW_ROM_FAMILY_CODE ] ) || 
                 ( DS1820_FAMILY_CODE_DS18B20 == romAddrOneWire[ OW_ROM_FAMILY_CODE ] ) ) {
                
                // Save only first two
                if ( nFound2 <= 1 ) { 
                    saveROMCodeToEEPROM( TEMP_CHANNEL2, nFound2 );
                }
                
                nFound2++; // Another one found
                
            }
            
            // Report Token activity if instructed to do so
            if ( val & 0b01000000 ) {
                reportTokenActivity();
            }
            
        }
    }
    // Should we search channel 3
    if ( val & 0b00001000 ) {
        
        // Initialise one-wire search
        nLastDiscrepancyOnewire = 0;
        bDoneFlagOneWire = FALSE;
        
        while( !bDoneFlagOneWire && DS1820_FindNextDevice( TEMP_CHANNEL3 ) ) {
            
            // Only handle sensors we recognise
            if ( ( DS1820_FAMILY_CODE_DS18S20 == romAddrOneWire[ OW_ROM_FAMILY_CODE ] ) || 
                 ( DS1820_FAMILY_CODE_DS18B20 == romAddrOneWire[ OW_ROM_FAMILY_CODE ] ) ) {
                
                // Save only first two
                if ( nFound3 <= 1 ) { 
                    saveROMCodeToEEPROM( TEMP_CHANNEL3, nFound3 );
                }
                
                nFound3++; // Another one found
                
            }
            
            // Report Token activity if instructed to do so
            if ( val & 0b10000000 ) {
                reportTokenActivity();
            }
            
        }
    }
    
    return val;
    
}



///////////////////////////////////////////////////////////////////////////////
//                        VSCP Required Methods
//////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// Get Major version number for this hardware module
//

unsigned char vscp_getMajorVersion()
{
    return FIRMWARE_MAJOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// Get Minor version number for this hardware module
//

unsigned char vscp_getMinorVersion()
{
    return FIRMWARE_MINOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// Get Sub minor version number for this hardware module
//

unsigned char vscp_getSubMinorVersion()
{
    return FIRMWARE_SUB_MINOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// getVSCP_GUID
//
// Get GUID from EEPROM
//

uint8_t vscp_getGUID(uint8_t idx)
{
    return eeprom_read(VSCP_EEPROM_REG_GUID + idx);
}

///////////////////////////////////////////////////////////////////////////////
// VSCP_setGUID
//

#ifdef ENABLE_WRITE_2PROTECTED_LOCATIONS
void vscp_setGUID( uint8_t idx, uint8_t data )
{
    if ( idx>15 ) return;
    eeprom_write( VSCP_EEPROM_REG_GUID + idx, data );
}
#endif

///////////////////////////////////////////////////////////////////////////////
// getDeviceURL
//
// Get device URL from EEPROM
//

uint8_t vscp_getMDF_URL(uint8_t idx)
{
    return vscp_deviceURL[ idx ];
}

///////////////////////////////////////////////////////////////////////////////
// Get Manufacturer id and subid from EEPROM
//

uint8_t vscp_getUserID(uint8_t idx)
{
    return eeprom_read(VSCP_EEPROM_REG_USERID + idx);
}

///////////////////////////////////////////////////////////////////////////////
//  setVSCPUserID
//

void vscp_setUserID(uint8_t idx, uint8_t data)
{
    eeprom_write(VSCP_EEPROM_REG_USERID + idx, data);
}

///////////////////////////////////////////////////////////////////////////////
// getVSCPManufacturerId
// 
// Get Manufacturer id and subid from EEPROM
//

uint8_t vscp_getManufacturerId(uint8_t idx)
{
    return eeprom_read(VSCP_EEPROM_REG_MANUFACTUR_ID0 + idx);
}

///////////////////////////////////////////////////////////////////////////////
// getVSCPManufacturerId
// 
// Get Manufacturer id and subid from EEPROM
//

void vscp_setManufacturerId(uint8_t idx, uint8_t data)
{
    eeprom_write(VSCP_EEPROM_REG_MANUFACTUR_ID0 + idx, data);
}

///////////////////////////////////////////////////////////////////////////////
// Get the bootloader algorithm code
//

uint8_t vscp_getBootLoaderAlgorithm(void)
{
    return VSCP_BOOTLOADER_PIC1;
}

///////////////////////////////////////////////////////////////////////////////
// Get the buffer size
//

uint8_t vscp_getBufferSize(void)
{
    return 8; // Standard CAN frame
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_getMatrixInfo
//

void vscp_getMatrixInfo(char *pData)
{
    pData[ 0 ] = DESCION_MATRIX_ROWS;  // Number of matrix rows
    pData[ 1 ] = REG_DESCION_MATRIX;   // Matrix start offset
    pData[ 2 ] = 0;                    // Matrix start page
    pData[ 3 ] = DESCION_MATRIX_PAGE;
    pData[ 4 ] = 0;                    // Matrix end page
    pData[ 5 ] = DESCION_MATRIX_PAGE;
    pData[ 6 ] = 0;
}


///////////////////////////////////////////////////////////////////////////////
// Do decision Matrix handling
// 
// The routine expects vscp_imsg to contain a valid incoming event
//

void doDM(void)
{
    unsigned char i;
    unsigned char dmflags;
    unsigned short class_filter;
    unsigned short class_mask;
    unsigned char type_filter;
    unsigned char type_mask;

    // Don't deal with the protocol functionality
    if ( VSCP_CLASS1_PROTOCOL == vscp_imsg.vscp_class ) return;

    for (i = 0; i<DESCION_MATRIX_ROWS; i++) {

        // Get DM flags for this row
        dmflags = eeprom_read( DECISION_MATRIX_EEPROM_START + (8 * i) );

        // Is the DM row enabled?
        if ( dmflags & VSCP_DM_FLAG_ENABLED ) {

            // Should the originating id be checked and if so is it the same?
            if ( ( dmflags & VSCP_DM_FLAG_CHECK_OADDR ) &&
                    ( vscp_imsg.oaddr != eeprom_read( DECISION_MATRIX_EEPROM_START + (8 * i) ) ) ) {
                continue;
            }

            // Check if zone should match and if so if it match
            if ( dmflags & VSCP_DM_FLAG_CHECK_ZONE ) {
                if ( 255 != vscp_imsg.data[ 1 ] ) {
                    if ( vscp_imsg.data[ 1 ] != eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_ZONE ] ) ) {
                        continue;
                    }
                }
            }

            // Check if sub zone should match and if so if it match
            if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE ) {
                if ( 255 != vscp_imsg.data[ 2 ] ) {
                    if ( vscp_imsg.data[ 2 ] != eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_SUBZONE ] ) ) {
                        continue;
                    }
                }
            }
            
            class_filter = ( ( dmflags & VSCP_DM_FLAG_CLASS_FILTER) << 8 ) +
                    eeprom_read( DECISION_MATRIX_EEPROM_START +
                                    (8 * i) +
                                    VSCP_DM_POS_CLASSFILTER );
            
            class_mask = ( ( dmflags & VSCP_DM_FLAG_CLASS_MASK ) << 7 ) +
                    eeprom_read( DECISION_MATRIX_EEPROM_START +
                                    (8 * i) +
                                    VSCP_DM_POS_CLASSMASK);
            
            type_filter = eeprom_read( DECISION_MATRIX_EEPROM_START +
                                        (8 * i) +
                                        VSCP_DM_POS_TYPEFILTER);
            
            type_mask = eeprom_read( DECISION_MATRIX_EEPROM_START +
                                        (8 * i) +
                                        VSCP_DM_POS_TYPEMASK);
                           
            if ( !( ( class_filter ^ vscp_imsg.vscp_class ) & class_mask ) &&
                    !( ( type_filter ^ vscp_imsg.vscp_type ) & type_mask ) ) {

                // OK Trigger this action
                switch ( eeprom_read( DECISION_MATRIX_EEPROM_START + 
                                        (8 * i) + 
                                        VSCP_DM_POS_ACTION ) ) {

                    case KELVIN1W_ACTION_NOOP:
                        break;
                        
                    case KELVIN1W_ACTION_SCAN:
                        actionScan( eeprom_read( DECISION_MATRIX_EEPROM_START + 
                                        (8 * i) + 
                                        VSCP_DM_POS_ACTIONPARAM ) );
                        break;
        
                    case KELVIN1W_ACTION_SCANSTORE:
                        actionScanStore( eeprom_read( DECISION_MATRIX_EEPROM_START + 
                                        (8 * i) + 
                                        VSCP_DM_POS_ACTIONPARAM ) );
                        break;

                    case KELVIN1W_ACTION_REPORT:
                        actionReport( eeprom_read( DECISION_MATRIX_EEPROM_START + 
                                        (8 * i) + 
                                        VSCP_DM_POS_ACTIONPARAM ) );
                        break;
                        
                    case KELVIN1W_ACTION_CLEAR_ALARM:
                        actionClearAlarm( eeprom_read( DECISION_MATRIX_EEPROM_START + 
                                        (8 * i) + 
                                        VSCP_DM_POS_ACTIONPARAM ) );
                        break;
                        
                    case KELVIN1W_ACTION_CLEAR_HIGH:
                        actionClearHigh( eeprom_read( DECISION_MATRIX_EEPROM_START + 
                                        (8 * i) + 
                                        VSCP_DM_POS_ACTIONPARAM ) );
                        break;
                        
                    case KELVIN1W_ACTION_CLEAR_LOW:
                        actionClearLow( eeprom_read( DECISION_MATRIX_EEPROM_START + 
                                        (8 * i) + 
                                        VSCP_DM_POS_ACTIONPARAM ) );
                        break;    
                        
                } // case
                
            } // Filter/mask
            
        } // Row enabled
        
    } // for each row
    
}

///////////////////////////////////////////////////////////////////////////////
// actionScan
//

void actionScan( uint8_t param )
{
    
}

///////////////////////////////////////////////////////////////////////////////
// actionScanStore
//

void actionScanStore( uint8_t param )
{
    
}

///////////////////////////////////////////////////////////////////////////////
// actionReport
//

void actionReport( uint8_t param )
{
    
}

///////////////////////////////////////////////////////////////////////////////
// actionClearAlarm
//

void actionClearAlarm( uint8_t param )
{
    
}

///////////////////////////////////////////////////////////////////////////////
// actionClearHigh
//

void actionClearHigh( uint8_t param )
{
    
}

///////////////////////////////////////////////////////////////////////////////
// actionClearLow
//

void actionClearLow( uint8_t param )
{
    
}


///////////////////////////////////////////////////////////////////////////////
//  vscp_getEmbeddedMdfInfo
//

void vscp_getEmbeddedMdfInfo(void)
{
    // No embedded DM so we respond with info about that

    vscp_omsg.priority = VSCP_PRIORITY_NORMAL;
    vscp_omsg.flags = VSCP_VALID_MSG + 3;
    vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
    vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

    vscp_omsg.data[ 0 ] = 0;
    vscp_omsg.data[ 1 ] = 0;
    vscp_omsg.data[ 2 ] = 0;

    // Send the event
    vscp_sendEvent();
}


///////////////////////////////////////////////////////////////////////////////
// vscp_getRegisterPagesUsed
//

uint8_t vscp_getRegisterPagesUsed(void)
{
    return 1;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getZone
//

uint8_t vscp_getZone(void)
{
   return eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_ZONE ] );
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getSubzone
//

uint8_t vscp_getSubzone(void)
{
    return eeprom_read( reg2eeprom_pg0[ REG0_KELVIN1W_ZONE ] );
}

///////////////////////////////////////////////////////////////////////////////
// vscp_goBootloaderMode
//

void vscp_goBootloaderMode( uint8_t algorithm )
{
    if ( VSCP_BOOTLOADER_PIC1 != algorithm  ) return;

    // OK, We should enter boot loader mode
    // 	First, activate bootloader mode
    eeprom_write(VSCP_EEPROM_BOOTLOADER_FLAG, VSCP_BOOT_FLAG);
    
    // Turn off CAN
    ECANSetOperationMode( ECAN_OP_MODE_CONFIG ); 
    
    // Reset the "thingi"
    asm("reset");
}

///////////////////////////////////////////////////////////////////////////////
//  getNickname
//

uint8_t vscp_readNicknamePermanent(void)
{
    return eeprom_read( VSCP_EEPROM_NICKNAME );
}

///////////////////////////////////////////////////////////////////////////////
//  setNickname
//

void vscp_writeNicknamePermanent(uint8_t nickname)
{
    eeprom_write( VSCP_EEPROM_NICKNAME, nickname );
}

///////////////////////////////////////////////////////////////////////////////
//  setVSCPControlByte
//

void vscp_setControlByte( uint8_t idx, uint8_t ctrl )
{
    if ( idx > 1 ) return;
    eeprom_write( VSCP_EEPROM_CONTROL1 + idx, ctrl );
}


///////////////////////////////////////////////////////////////////////////////
//  getVSCPControlByte
//

uint8_t vscp_getControlByte( uint8_t idx )
{
    if ( idx > 1 ) return 0;
    return eeprom_read( VSCP_EEPROM_CONTROL1 + idx );
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_init_pstorage(
//

void vscp_init_pstorage( void )
{
    init_app_eeprom();
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getFamilyCode
//

uint32_t vscp_getFamilyCode()
{
    return 0L;
}


///////////////////////////////////////////////////////////////////////////////
// vscp_getFamilyType
//

uint32_t vscp_getFamilyType()
{
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_restoreDefaults
//

void vscp_restoreDefaults()
{
    init_app_eeprom();
    init_app_ram();
}

///////////////////////////////////////////////////////////////////////////////
// sendVSCPFrame
//

int8_t sendVSCPFrame(uint16_t vscpclass,
        uint8_t vscptype,
        uint8_t nodeid,
        uint8_t priority,
        uint8_t size,
        uint8_t *pData)
{
    uint32_t id = ((uint32_t) priority << 26) |
            ((uint32_t) vscpclass << 16) |
            ((uint32_t) vscptype << 8) |
            nodeid; // nodeaddress (our address)

    if (!sendCANFrame(id, size, pData)) {
        // Failed to send message
        vscp_errorcnt++;
        return FALSE;
    }

    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// getVSCPFrame
//

int8_t getVSCPFrame( uint16_t *pvscpclass,
                        uint8_t *pvscptype,
                        uint8_t *pNodeId,
                        uint8_t *pPriority,
                        uint8_t *pSize,
                        uint8_t *pData)
{
    uint32_t id;

    if (!getCANFrame(&id, pSize, pData)) {
        return FALSE;
    }

    *pNodeId = id & 0x0ff;
    *pvscptype = (id >> 8) & 0xff;
    *pvscpclass = (id >> 16) & 0x1ff;
    *pPriority = (uint16_t) (0x07 & (id >> 26));

    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// sendCANFrame
//

int8_t sendCANFrame(uint32_t id, uint8_t dlc, uint8_t *pdata)
{
    uint8_t rv = FALSE;
    
    sendTimer = 0;
    while ( sendTimer < 1000 ) {
        if ( ECANSendMessage( id, pdata, dlc, ECAN_TX_XTD_FRAME ) ) {
            rv = TRUE;
            break;
        }
    }

    vscp_omsg.flags = 0;

    return rv;
}

///////////////////////////////////////////////////////////////////////////////
// getCANFrame
//

int8_t getCANFrame(uint32_t *pid, uint8_t *pdlc, uint8_t *pdata)
{
    ECAN_RX_MSG_FLAGS flags;

    // Don't read in new message if there already is a message
    // in the input buffer
    if (vscp_imsg.flags & VSCP_VALID_MSG) return FALSE;

    if (ECANReceiveMessage((unsigned long *) pid, (BYTE*) pdata, (BYTE*) pdlc, &flags)) {
        // RTR not interesting
        if (flags & ECAN_RX_RTR_FRAME) return FALSE;

        // Must be extended frame
        if (!(flags & ECAN_RX_XTD_FRAME)) return FALSE;

        return TRUE;
    }

    return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
// calculateSetFilterMask
//
// Calculate and set required filter and mask
// for the current decision matrix
//

void calculateSetFilterMask( void )
{
    uint8_t i,j;
    uint8_t lastOID;
    uint32_t rowmask;
    uint32_t rowfilter;

    // Reset filter masks
    uint32_t mask = 0xffffffff; // Just id 0x00000000 will come true
    uint32_t filter = 0x00000000;

    // Go through all DM rows
    for ( i=0; i<DESCION_MATRIX_ROWS; i++ ) {

        // No need to check not active DM rows
        if ( eeprom_read( VSCP_EEPROM_VSCP_END + 8*i + VSCP_DM_POS_FLAGS ) & VSCP_DM_FLAG_ENABLED ) {

            // build the mask
            // ==============
            // We receive
            //  - all priorities
            //  - hardcoded and not hardcoded
            //  - from all nodes

            rowmask =
                    // Bit 9 of class mask
                    ( (uint32_t)( eeprom_read( VSCP_EEPROM_VSCP_END + 8*i + VSCP_DM_POS_FLAGS ) & VSCP_DM_FLAG_CLASS_MASK ) << 23 ) |
                    // Rest of class mask
                    ( (uint32_t)eeprom_read( VSCP_EEPROM_VSCP_END + 8*i + VSCP_DM_POS_CLASSMASK ) << 16 ) |
                    // Type mask
                    ( (uint32_t)eeprom_read( VSCP_EEPROM_VSCP_END + 8*i + VSCP_DM_POS_TYPEMASK ) << 8 ) |
                    // Hardcoded bit
                    //( ( eeprom_read( VSCP_EEPROM_VSCP_END + 8*i + VSCP_DM_POS_FLAGS ) & VSCP_DM_FLAG_HARDCODED ) << 20 ) |   
					// OID  - handle later
					0xff;
                    

            // build the filter
            // ================

            rowfilter =
                    // Bit 9 of class filter
                    ( (uint32_t)( eeprom_read( VSCP_EEPROM_VSCP_END + 8*i + VSCP_DM_POS_FLAGS ) & VSCP_DM_FLAG_CLASS_FILTER ) << 24 ) |
                    // Rest of class filter
                    ( (uint32_t)eeprom_read( VSCP_EEPROM_VSCP_END + 8*i + VSCP_DM_POS_CLASSFILTER ) << 16 ) |
                    // Type filter
                    ( (uint32_t)eeprom_read( VSCP_EEPROM_VSCP_END + 8*i + VSCP_DM_POS_TYPEFILTER ) << 8 ) |
                    // OID Mask cleared if not same OID for all or one or more
                    // rows don't have OID check flag set.
                    eeprom_read( VSCP_EEPROM_VSCP_END + 8*i );

            if ( 0 == i ) filter = rowfilter;   // Hack for first iteration loop

            // Form the mask - if one mask have a don't care (0)
            // the final mask should also have a don't care on that position
            mask &= rowmask;

            // Check the calculated filter and the current
            // filter to see if the bits are the same. If they
            // are not then clear the mask at that position
            for ( j=0; j<32; j++ ) {
                // If two bits are different we clear the mask bit
                if ( ( ( filter >> j ) & 1 ) != ( ( rowfilter >> j ) & 1 ) ) {
                    mask &= ~(1<<j);
                }
            }

            // Form the filter
            // if two bits are not the same they will be zero
            // All zeros will be zero
            // All ones will be one
            filter &= rowfilter;

            // Not check OID?
            if ( !eeprom_read( VSCP_EEPROM_VSCP_END + 8*i + VSCP_DM_POS_FLAGS ) & VSCP_DM_FLAG_CHECK_OADDR ) {
                // No should not be checked for this position
                // This mean that we can't filter on a specific OID
                // so mask must be a don't care
                mask &= ~0xff;
            }

            if ( i ) {
                // If the current OID is different than the previous
                // we accept all
                for (j = 0; j < 8; j++) {
                    if ((lastOID >> i & 1)
                            != (eeprom_read(VSCP_EEPROM_VSCP_END + 8*i ) >> i & 1)) {
                        mask &= (1 << i);
                    }
                }

                lastOID = eeprom_read(VSCP_EEPROM_VSCP_END + 8*i );

            } 
            else {
                // First round we just store the OID
                lastOID = eeprom_read(VSCP_EEPROM_VSCP_END + 8*i );
            }

        }
    }
    
    // Must be in Config mode to change settings.
    ECANSetOperationMode( ECAN_OP_MODE_CONFIG );

    //Set mask 1
    ECANSetRXM1Value( mask, ECAN_MSG_XTD );

    // Set filter 1
    ECANSetRXF1Value( filter, ECAN_MSG_XTD );

    // Return to Normal mode to communicate.
    ECANSetOperationMode( ECAN_OP_MODE_NORMAL );
  
}