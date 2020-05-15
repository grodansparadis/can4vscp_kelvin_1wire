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

#ifndef ONEWIRE_H
#define ONEWIRE_H

#include <inttypes.h>
#include "main.h"

////////////////////////////////////////////////////////////////////////////////
//                                 Page 0
////////////////////////////////////////////////////////////////////////////////

// Page 0 registers
#define REG0_KELVIN1W_ZONE                              0
#define REG0_KELVIN1W_SUBZONE                           1

#define REG0_KELVIN1W_TEMP0_ZONE                        2
#define REG0_KELVIN1W_TEMP0_SUBZONE                     3
#define REG0_KELVIN1W_TEMP1_ZONE                        4
#define REG0_KELVIN1W_TEMP1_SUBZONE                     5
#define REG0_KELVIN1W_TEMP2_ZONE                        6
#define REG0_KELVIN1W_TEMP2_SUBZONE                     7
#define REG0_KELVIN1W_TEMP3_ZONE                        8
#define REG0_KELVIN1W_TEMP3_SUBZONE                     9
#define REG0_KELVIN1W_TEMP4_ZONE                        10
#define REG0_KELVIN1W_TEMP4_SUBZONE                     11
#define REG0_KELVIN1W_TEMP5_ZONE                        12
#define REG0_KELVIN1W_TEMP5_SUBZONE                     13
#define REG0_KELVIN1W_TEMP6_ZONE                        14
#define REG0_KELVIN1W_TEMP6_SUBZONE                     15
#define REG0_KELVIN1W_TEMP7_ZONE                        16
#define REG0_KELVIN1W_TEMP7_SUBZONE                     17

#define REG0_KELVIN1W_CTRL_REG0_LOW                     18
#define REG0_KELVIN1W_CTRL_REG0_HIGH                    19
#define REG0_KELVIN1W_CTRL_REG1_LOW                     20
#define REG0_KELVIN1W_CTRL_REG1_HIGH                    21
#define REG0_KELVIN1W_CTRL_REG2_LOW                     22
#define REG0_KELVIN1W_CTRL_REG2_HIGH                    23
#define REG0_KELVIN1W_CTRL_REG3_LOW                     24
#define REG0_KELVIN1W_CTRL_REG3_HIGH                    25
#define REG0_KELVIN1W_CTRL_REG4_LOW                     26
#define REG0_KELVIN1W_CTRL_REG4_HIGH                    27
#define REG0_KELVIN1W_CTRL_REG5_LOW                     28
#define REG0_KELVIN1W_CTRL_REG5_HIGH                    29
#define REG0_KELVIN1W_CTRL_REG6_LOW                     30
#define REG0_KELVIN1W_CTRL_REG6_HIGH                    31
#define REG0_KELVIN1W_CTRL_REG7_LOW                     32
#define REG0_KELVIN1W_CTRL_REG7_HIGH                    33

#define REG0_KELVIN1W_REPORT_INTERVAL_0                 34
#define REG0_KELVIN1W_REPORT_INTERVAL_1                 35
#define REG0_KELVIN1W_REPORT_INTERVAL_2                 36
#define REG0_KELVIN1W_REPORT_INTERVAL_3                 37
#define REG0_KELVIN1W_REPORT_INTERVAL_4                 38
#define REG0_KELVIN1W_REPORT_INTERVAL_5                 39
#define REG0_KELVIN1W_REPORT_INTERVAL_6                 40
#define REG0_KELVIN1W_REPORT_INTERVAL_7                 41

#define REG0_KELVIN1W_BCONSTANT0_MSB                    42
#define REG0_KELVIN1W_BCONSTANT0_LSB                    43

#define REG0_KELVIN1W_MASTER_RESET_PULSE_MSB            44
#define REG0_KELVIN1W_MASTER_RESET_PULSE_LSB            45
#define REG0_KELVIN1W_PRESENCE_WAIT                     46
#define REG0_KELVIN1W_PRESENCE_FIN_MSB                  47   
#define REG0_KELVIN1W_PRESENCE_FIN_LSB                  48
#define REG0_KELVIN1W_MASTER_BITSTART_DELAY             49
#define REG0_KELVIN1W_BIT_READ_DELAY                    50  
#define REG0_KELVIN1W_BIT_READ_WAIT                     51 
#define REG0_KELVIN1W_BIT_WRITE_DELAY                   52


////////////////////////////////////////////////////////////////////////////////
//                                 Page 1
////////////////////////////////////////////////////////////////////////////////


#define REG1_KELVIN1W_TEMPERATURE0_MSB                  0
#define REG1_KELVIN1W_TEMPERATURE0_BYTE0                0
#define REG1_KELVIN1W_TEMPERATURE0_BYTE1                1
#define REG1_KELVIN1W_TEMPERATURE0_BYTE2                2
#define REG1_KELVIN1W_TEMPERATURE0_BYTE3                3
#define REG1_KELVIN1W_TEMPERATURE0_LSB                  3

#define REG1_KELVIN1W_TEMPERATURE1_MSB                  4
#define REG1_KELVIN1W_TEMPERATURE1_BYTE0                4
#define REG1_KELVIN1W_TEMPERATURE1_BYTE1                5
#define REG1_KELVIN1W_TEMPERATURE1_BYTE2                6
#define REG1_KELVIN1W_TEMPERATURE1_BYTE3                7
#define REG1_KELVIN1W_TEMPERATURE1_LSB                  7

#define REG1_KELVIN1W_TEMPERATURE2_MSB                  8
#define REG1_KELVIN1W_TEMPERATURE2_BYTE0                8
#define REG1_KELVIN1W_TEMPERATURE2_BYTE1                9
#define REG1_KELVIN1W_TEMPERATURE2_BYTE2                10
#define REG1_KELVIN1W_TEMPERATURE2_BYTE3                11
#define REG1_KELVIN1W_TEMPERATURE2_LSB                  11

#define REG1_KELVIN1W_TEMPERATURE3_MSB                  12
#define REG1_KELVIN1W_TEMPERATURE3_BYTE0                12
#define REG1_KELVIN1W_TEMPERATURE3_BYTE1                13
#define REG1_KELVIN1W_TEMPERATURE3_BYTE2                14
#define REG1_KELVIN1W_TEMPERATURE3_BYTE3                15
#define REG1_KELVIN1W_TEMPERATURE3_LSB                  15

#define REG1_KELVIN1W_TEMPERATURE4_MSB                  16
#define REG1_KELVIN1W_TEMPERATURE4_BYTE0                16
#define REG1_KELVIN1W_TEMPERATURE4_BYTE1                17
#define REG1_KELVIN1W_TEMPERATURE4_BYTE2                18
#define REG1_KELVIN1W_TEMPERATURE4_BYTE3                19
#define REG1_KELVIN1W_TEMPERATURE4_LSB                  19

#define REG1_KELVIN1W_TEMPERATURE5_MSB                  20
#define REG1_KELVIN1W_TEMPERATURE5_BYTE0                20
#define REG1_KELVIN1W_TEMPERATURE5_BYTE1                21
#define REG1_KELVIN1W_TEMPERATURE5_BYTE2                22
#define REG1_KELVIN1W_TEMPERATURE5_BYTE3                23
#define REG1_KELVIN1W_TEMPERATURE5_LSB                  23

#define REG1_KELVIN1W_TEMPERATURE6_MSB                  24
#define REG1_KELVIN1W_TEMPERATURE6_BYTE0                24
#define REG1_KELVIN1W_TEMPERATURE6_BYTE1                25
#define REG1_KELVIN1W_TEMPERATURE6_BYTE2                26
#define REG1_KELVIN1W_TEMPERATURE6_BYTE3                27
#define REG1_KELVIN1W_TEMPERATURE6_LSB                  27

#define REG1_KELVIN1W_TEMPERATURE7_MSB                  28
#define REG1_KELVIN1W_TEMPERATURE7_BYTE0                28
#define REG1_KELVIN1W_TEMPERATURE7_BYTE1                29
#define REG1_KELVIN1W_TEMPERATURE7_BYTE2                30
#define REG1_KELVIN1W_TEMPERATURE7_BYTE3                31
#define REG1_KELVIN1W_TEMPERATURE7_LSB                  31

#define REG1_KELVIN1W_TEMPERATURE8_MSB                  32
#define REG1_KELVIN1W_TEMPERATURE8_BYTE0                32
#define REG1_KELVIN1W_TEMPERATURE8_BYTE1                33
#define REG1_KELVIN1W_TEMPERATURE8_BYTE2                34
#define REG1_KELVIN1W_TEMPERATURE8_BYTE3                35
#define REG1_KELVIN1W_TEMPERATURE8_LSB                  35



////////////////////////////////////////////////////////////////////////////////
//                                 Page 2
////////////////////////////////////////////////////////////////////////////////


#define REG2_KELVIN1W_LOW_ALARM_SET_POINT0_MSB          0
#define REG2_KELVIN1W_LOW_ALARM_SET_POINT0_LSB          1
#define REG2_KELVIN1W_LOW_ALARM_SET_POINT1_MSB          2
#define REG2_KELVIN1W_LOW_ALARM_SET_POINT1_LSB          3
#define REG2_KELVIN1W_LOW_ALARM_SET_POINT2_MSB          4
#define REG2_KELVIN1W_LOW_ALARM_SET_POINT2_LSB          5
#define REG2_KELVIN1W_LOW_ALARM_SET_POINT3_MSB          6
#define REG2_KELVIN1W_LOW_ALARM_SET_POINT3_LSB          7
#define REG2_KELVIN1W_LOW_ALARM_SET_POINT4_MSB          8
#define REG2_KELVIN1W_LOW_ALARM_SET_POINT4_LSB          9
#define REG2_KELVIN1W_LOW_ALARM_SET_POINT5_MSB          10
#define REG2_KELVIN1W_LOW_ALARM_SET_POINT5_LSB          11
#define REG2_KELVIN1W_LOW_ALARM_SET_POINT6_MSB          12
#define REG2_KELVIN1W_LOW_ALARM_SET_POINT6_LSB          13
#define REG2_KELVIN1W_LOW_ALARM_SET_POINT7_MSB          14
#define REG2_KELVIN1W_LOW_ALARM_SET_POINT7_LSB          15

#define REG2_KELVIN1W_HIGH_ALARM_SET_POINT0_MSB         16
#define REG2_KELVIN1W_HIGH_ALARM_SET_POINT0_LSB         17
#define REG2_KELVIN1W_HIGH_ALARM_SET_POINT1_MSB         18
#define REG2_KELVIN1W_HIGH_ALARM_SET_POINT1_LSB         19
#define REG2_KELVIN1W_HIGH_ALARM_SET_POINT2_MSB         20
#define REG2_KELVIN1W_HIGH_ALARM_SET_POINT2_LSB         21
#define REG2_KELVIN1W_HIGH_ALARM_SET_POINT3_MSB         22
#define REG2_KELVIN1W_HIGH_ALARM_SET_POINT3_LSB         23
#define REG2_KELVIN1W_HIGH_ALARM_SET_POINT4_MSB         24
#define REG2_KELVIN1W_HIGH_ALARM_SET_POINT4_LSB         25
#define REG2_KELVIN1W_HIGH_ALARM_SET_POINT5_MSB         26
#define REG2_KELVIN1W_HIGH_ALARM_SET_POINT5_LSB         27
#define REG2_KELVIN1W_HIGH_ALARM_SET_POINT6_MSB         28
#define REG2_KELVIN1W_HIGH_ALARM_SET_POINT6_LSB         29
#define REG2_KELVIN1W_HIGH_ALARM_SET_POINT7_MSB         30
#define REG2_KELVIN1W_HIGH_ALARM_SET_POINT7_LSB         31

#define REG2_KELVIN1W_ABSOLUTE_LOW_TEMP0_MSB            32
#define REG2_KELVIN1W_ABSOLUTE_LOW_TEMP0_LSB            33
#define REG2_KELVIN1W_ABSOLUTE_LOW_TEMP1_MSB            34
#define REG2_KELVIN1W_ABSOLUTE_LOW_TEMP1_LSB            35
#define REG2_KELVIN1W_ABSOLUTE_LOW_TEMP2_MSB            36
#define REG2_KELVIN1W_ABSOLUTE_LOW_TEMP2_LSB            37
#define REG2_KELVIN1W_ABSOLUTE_LOW_TEMP3_MSB            38
#define REG2_KELVIN1W_ABSOLUTE_LOW_TEMP3_LSB            39
#define REG2_KELVIN1W_ABSOLUTE_LOW_TEMP4_MSB            40
#define REG2_KELVIN1W_ABSOLUTE_LOW_TEMP4_LSB            41
#define REG2_KELVIN1W_ABSOLUTE_LOW_TEMP5_MSB            42
#define REG2_KELVIN1W_ABSOLUTE_LOW_TEMP5_LSB            43
#define REG2_KELVIN1W_ABSOLUTE_LOW_TEMP6_MSB            44
#define REG2_KELVIN1W_ABSOLUTE_LOW_TEMP6_LSB            45
#define REG2_KELVIN1W_ABSOLUTE_LOW_TEMP7_MSB            46
#define REG2_KELVIN1W_ABSOLUTE_LOW_TEMP7_LSB            47

#define REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP0_MSB           48
#define REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP0_LSB           49
#define REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP1_MSB           50
#define REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP1_LSB           51
#define REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP2_MSB           52
#define REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP2_LSB           53
#define REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP3_MSB           54
#define REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP3_LSB           55
#define REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP4_MSB           56
#define REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP4_LSB           57
#define REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP5_MSB           58
#define REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP5_LSB           59
#define REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP6_MSB           60
#define REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP6_LSB           61
#define REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP7_MSB           62
#define REG2_KELVIN1W_ABSOLUTE_HIGH_TEMP7_LSB           63

#define REG2_KELVIN1W_HYSTERESIS_TEMP0                  64
#define REG2_KELVIN1W_HYSTERESIS_TEMP1                  65
#define REG2_KELVIN1W_HYSTERESIS_TEMP2                  66
#define REG2_KELVIN1W_HYSTERESIS_TEMP3                  67
#define REG2_KELVIN1W_HYSTERESIS_TEMP4                  68
#define REG2_KELVIN1W_HYSTERESIS_TEMP5                  69
#define REG2_KELVIN1W_HYSTERESIS_TEMP6                  70
#define REG2_KELVIN1W_HYSTERESIS_TEMP7                  71

#define REG2_KELVIN1W_CALIBRATION_TEMP0_MSB             72
#define REG2_KELVIN1W_CALIBRATION_TEMP0_LSB             73
#define REG2_KELVIN1W_CALIBRATION_TEMP1_MSB             74
#define REG2_KELVIN1W_CALIBRATION_TEMP1_LSB             75
#define REG2_KELVIN1W_CALIBRATION_TEMP2_MSB             76
#define REG2_KELVIN1W_CALIBRATION_TEMP2_LSB             77
#define REG2_KELVIN1W_CALIBRATION_TEMP3_MSB             78
#define REG2_KELVIN1W_CALIBRATION_TEMP3_LSB             79
#define REG2_KELVIN1W_CALIBRATION_TEMP4_MSB             80
#define REG2_KELVIN1W_CALIBRATION_TEMP4_LSB             81
#define REG2_KELVIN1W_CALIBRATION_TEMP5_MSB             82
#define REG2_KELVIN1W_CALIBRATION_TEMP5_LSB             83
#define REG2_KELVIN1W_CALIBRATION_TEMP6_MSB             84
#define REG2_KELVIN1W_CALIBRATION_TEMP6_LSB             85
#define REG2_KELVIN1W_CALIBRATION_TEMP7_MSB             86
#define REG2_KELVIN1W_CALIBRATION_TEMP7_LSB             87
#define REG2_KELVIN1W_CALIBRATION_TEMP8_MSB             88
#define REG2_KELVIN1W_CALIBRATION_TEMP8_LSB             89


////////////////////////////////////////////////////////////////////////////////
//                                 Page 3
////////////////////////////////////////////////////////////////////////////////


#define REG3_KELVIN1W_CHANNEL_CONTROL_REGISTER          0

#define REG3_KELVIN1W_ROM_CODE_TEMP0_MSB                1 
#define REG3_KELVIN1W_ROM_CODE_TEMP0_BYTE0              1
#define REG3_KELVIN1W_ROM_CODE_TEMP0_BYTE1              2
#define REG3_KELVIN1W_ROM_CODE_TEMP0_BYTE2              3
#define REG3_KELVIN1W_ROM_CODE_TEMP0_BYTE3              4
#define REG3_KELVIN1W_ROM_CODE_TEMP0_BYTE4              5
#define REG3_KELVIN1W_ROM_CODE_TEMP0_BYTE5              6
#define REG3_KELVIN1W_ROM_CODE_TEMP0_BYTE6              7
#define REG3_KELVIN1W_ROM_CODE_TEMP0_BYTE7              8
#define REG3_KELVIN1W_ROM_CODE_TEMP0_LSB                8

#define REG3_KELVIN1W_ROM_CODE_TEMP1_MSB                9 
#define REG3_KELVIN1W_ROM_CODE_TEMP1_BYTE0              9
#define REG3_KELVIN1W_ROM_CODE_TEMP1_BYTE1              10
#define REG3_KELVIN1W_ROM_CODE_TEMP1_BYTE2              11
#define REG3_KELVIN1W_ROM_CODE_TEMP1_BYTE3              12
#define REG3_KELVIN1W_ROM_CODE_TEMP1_BYTE4              13
#define REG3_KELVIN1W_ROM_CODE_TEMP1_BYTE5              14
#define REG3_KELVIN1W_ROM_CODE_TEMP1_BYTE6              15
#define REG3_KELVIN1W_ROM_CODE_TEMP1_BYTE7              16
#define REG3_KELVIN1W_ROM_CODE_TEMP1_LSB                16

#define REG3_KELVIN1W_ROM_CODE_TEMP2_MSB                17 
#define REG3_KELVIN1W_ROM_CODE_TEMP2_BYTE0              17
#define REG3_KELVIN1W_ROM_CODE_TEMP2_BYTE1              18
#define REG3_KELVIN1W_ROM_CODE_TEMP2_BYTE2              19
#define REG3_KELVIN1W_ROM_CODE_TEMP2_BYTE3              20
#define REG3_KELVIN1W_ROM_CODE_TEMP2_BYTE4              21
#define REG3_KELVIN1W_ROM_CODE_TEMP2_BYTE5              22
#define REG3_KELVIN1W_ROM_CODE_TEMP2_BYTE6              23
#define REG3_KELVIN1W_ROM_CODE_TEMP2_BYTE7              24
#define REG3_KELVIN1W_ROM_CODE_TEMP2_LSB                24

#define REG3_KELVIN1W_ROM_CODE_TEMP3_MSB                25 
#define REG3_KELVIN1W_ROM_CODE_TEMP3_BYTE0              25
#define REG3_KELVIN1W_ROM_CODE_TEMP3_BYTE1              26
#define REG3_KELVIN1W_ROM_CODE_TEMP3_BYTE2              27
#define REG3_KELVIN1W_ROM_CODE_TEMP3_BYTE3              28
#define REG3_KELVIN1W_ROM_CODE_TEMP3_BYTE4              29
#define REG3_KELVIN1W_ROM_CODE_TEMP3_BYTE5              30
#define REG3_KELVIN1W_ROM_CODE_TEMP3_BYTE6              31
#define REG3_KELVIN1W_ROM_CODE_TEMP3_BYTE7              32
#define REG3_KELVIN1W_ROM_CODE_TEMP3_LSB                32

#define REG3_KELVIN1W_ROM_CODE_TEMP4_MSB                33 
#define REG3_KELVIN1W_ROM_CODE_TEMP4_BYTE0              33
#define REG3_KELVIN1W_ROM_CODE_TEMP4_BYTE1              34
#define REG3_KELVIN1W_ROM_CODE_TEMP4_BYTE2              35
#define REG3_KELVIN1W_ROM_CODE_TEMP4_BYTE3              36
#define REG3_KELVIN1W_ROM_CODE_TEMP4_BYTE4              37
#define REG3_KELVIN1W_ROM_CODE_TEMP4_BYTE5              38
#define REG3_KELVIN1W_ROM_CODE_TEMP4_BYTE6              39
#define REG3_KELVIN1W_ROM_CODE_TEMP4_BYTE7              40
#define REG3_KELVIN1W_ROM_CODE_TEMP4_LSB                40

#define REG3_KELVIN1W_ROM_CODE_TEMP5_MSB                41 
#define REG3_KELVIN1W_ROM_CODE_TEMP5_BYTE0              41
#define REG3_KELVIN1W_ROM_CODE_TEMP5_BYTE1              42
#define REG3_KELVIN1W_ROM_CODE_TEMP5_BYTE2              43
#define REG3_KELVIN1W_ROM_CODE_TEMP5_BYTE3              44
#define REG3_KELVIN1W_ROM_CODE_TEMP5_BYTE4              45
#define REG3_KELVIN1W_ROM_CODE_TEMP5_BYTE5              46
#define REG3_KELVIN1W_ROM_CODE_TEMP5_BYTE6              47
#define REG3_KELVIN1W_ROM_CODE_TEMP5_BYTE7              48
#define REG3_KELVIN1W_ROM_CODE_TEMP5_LSB                48

#define REG3_KELVIN1W_ROM_CODE_TEMP6_MSB                49 
#define REG3_KELVIN1W_ROM_CODE_TEMP6_BYTE0              49
#define REG3_KELVIN1W_ROM_CODE_TEMP6_BYTE1              50
#define REG3_KELVIN1W_ROM_CODE_TEMP6_BYTE2              51
#define REG3_KELVIN1W_ROM_CODE_TEMP6_BYTE3              52
#define REG3_KELVIN1W_ROM_CODE_TEMP6_BYTE4              53
#define REG3_KELVIN1W_ROM_CODE_TEMP6_BYTE5              54
#define REG3_KELVIN1W_ROM_CODE_TEMP6_BYTE6              55
#define REG3_KELVIN1W_ROM_CODE_TEMP6_BYTE7              56
#define REG3_KELVIN1W_ROM_CODE_TEMP6_LSB                56

#define REG3_KELVIN1W_ROM_CODE_TEMP7_MSB                57 
#define REG3_KELVIN1W_ROM_CODE_TEMP7_BYTE0              57
#define REG3_KELVIN1W_ROM_CODE_TEMP7_BYTE1              58
#define REG3_KELVIN1W_ROM_CODE_TEMP7_BYTE2              59
#define REG3_KELVIN1W_ROM_CODE_TEMP7_BYTE3              60
#define REG3_KELVIN1W_ROM_CODE_TEMP7_BYTE4              61
#define REG3_KELVIN1W_ROM_CODE_TEMP7_BYTE5              62
#define REG3_KELVIN1W_ROM_CODE_TEMP7_BYTE6              63
#define REG3_KELVIN1W_ROM_CODE_TEMP7_BYTE7              64
#define REG3_KELVIN1W_ROM_CODE_TEMP7_LSB                64


////////////////////////////////////////////////////////////////////////////////
//                                 Page 250
////////////////////////////////////////////////////////////////////////////////

// Decision Matrix


#define DESCION_MATRIX_ROWS             8       // Number of ros in decision matrix
#define REG_DESCION_MATRIX              0       // Decision matrix register start offset
#define DESCION_MATRIX_PAGE             250     // Decision matrix page

#define KELVIN1W_ACTION_NOOP            0
#define KELVIN1W_ACTION_SCAN            1
#define KELVIN1W_ACTION_SCANSTORE       2
#define KELVIN1W_ACTION_REPORT          3
#define KELVIN1W_ACTION_CLEAR_ALARM     4
#define KELVIN1W_ACTION_CLEAR_HIGH      5
#define KELVIN1W_ACTION_CLEAR_LOW       6

// The four temperature channels
#define TEMP_CHANNEL0                   0   // RC7
#define TEMP_CHANNEL1                   1   // RC6
#define TEMP_CHANNEL2                   2   // RC4
#define TEMP_CHANNEL3                   3   // RC3

// Temperature report format (high control low 2 bits))
#define REPORT_TEMP_AS_NORMINT          0
#define REPORT_TEMP_AS_FLOAT            1
#define REPORT_TEMP_AS_STRING           2

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
#define DEFAULT_CONTROL_REG_LOW             0b00000001
#define DEFAULT_CONTROL_REG_HIGH            0b00000001

#define MASK_CONTROL0_UNIT                  0x03
#define MASK_CONTROL0_LOW_ALARM             0x08
#define MASK_CONTROL0_HIGH_ALARM            0x10
#define MASK_CONTROL0_TURNX                 0x20
#define MASK_CONTROL0_TURNX_INVERT          0x40
#define MASK_CONTROL0_CONTINUOUS            0x80

#define MASK_CONTROL1_REMOTE_FORMAT         0x03

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
#define DEFAULT_ABSOLUTE_LOW_MSB            0x7f
#define DEFAULT_ABSOLUTE_LOW_LSB            0xff

// Default absolute high value
// Set to -327.68 degrees Celsius
#define DEFAULT_ABSOLUTE_HIGH_MSB           0x80
#define DEFAULT_ABSOLUTE_HIGH_LSB           0x00

// Default hysteresis
#define DEFAULT_HYSTERESIS                  2

//
// Errors
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


// Frame positions in ROM code
#define OW_ROM_FAMILY_CODE          0
#define OW_ROM_CRC                  7


#define TEMP_RES                    0x100 // temperature resolution => 1/256°C = 0.0039°C 


// -----------------------------------------------------------------------------
//                         DS1820 Timing Parameters                           
// -----------------------------------------------------------------------------

// uncomment to use standard timing
//#define DS1820_USE_STANDARD_TIMING


#ifdef DS1820_USE_STANDARD_TIMING

#define DS1820_RST_PULSE                480 // Master reset pulse time in [us]     
#define DS1820_PRESENCE_WAIT            70  // Delay after master reset pulse in [us]  
#define DS1820_PRESENCE_FIN             410 // Delay after reading of presence pulse [us]    
#define DS1820_MSTR_BITSTART            6   // Delay time for bit start by master    
#define DS1820_BITREAD_DLY              9   // Bit read delay 
#define DS1820_BITREAD_WAIT             55  // Bit read wait  
#define DS1820_BITWRITE_DLY             64  // Bit write delay

#else

#define DS1820_RST_PULSE                480 // Master reset pulse time in [us]         
#define DS1820_PRESENCE_WAIT            40  // Delay after master reset pulse in [us]     
#define DS1820_PRESENCE_FIN             480 // Delay after reading of presence pulse [us]    
#define DS1820_MSTR_BITSTART            2   // Delay time for bit start by master      
#define DS1820_BITREAD_DLY              5   // bit read delay
#define DS1820_BITREAD_WAIT             55  // Bit read wait
#define DS1820_BITWRITE_DLY             100 // Bit write delay

#endif

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
void DS1820_StartTempConversion( uint8_t pin );
int16_t DS1820_GetTempRaw( uint8_t pin, BOOL bStartAndWait  );
double DS1820_GetTempFloat( uint8_t pin );
double DS1820_LateGetTempFloat( uint8_t pin );
void DS1820_GetTempString( int16_t rawTemp, char *strTemp );


#endif