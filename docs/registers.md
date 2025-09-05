# Introduction

All VSCP modules have a set of 8-bit registers defined to export it's functionality in a common way. Some of them (register 128-255) are predefined and the information in them are the same for all VSCP modules. See the [VSCP specification](https://grodansparadis.github.io/vscp-doc-spec/#/./vscp_register_abstraction_model) for a description of their content.

The lower 128 register space is used for module specific registers. It is also here you find registers with which you configure your module. Also registers where you typically can read status information such as measurement data from the module is here.

The lower register space is further expanded in register pages (0-65535).

The registers of the Kelvin NTC10K module control the behaviour of the module. You write to the registers to configure the module and read from the registers to get status information from the module.

With the registers you can set the unit in which the temperature measureent will be reported. You can read sensor values. Set periods for sensor value reporting. set high and low alarm points. Set hysteresis for the alarms. Set the B value for the NTC sensor. Set the zone and subzone for the sensor. Set absolute high and low values for the sensor. Set calibration values for the sensor. Set the voltage used for the sensor. Read raw A/D values from the sensor. An more.

Below is a description of the registers available for the Kelvin NTC10KA module.

# Register page 0

## Module zone registers

| Register | Page | Description |
| :--------: | :----: | ----------- |
|  0  |  0  | The zone this module belongs to. |
|  1  |  0  | The sub zone this module belongs to. |

## Sensor Control Registers

It is possible to configure which data is reported in the sensor control registers and how the data is reported.


| Register | Page | Description |
| :--------: | :----: | ----------- |
|  2  |  0  | Control byte for sensor 0. |
|  3  |  0  | Control byte for sensor 1. |
|  4  |  0  | Control byte for sensor 2. |
|  5  |  0  | Control byte for sensor 3. |
|  6  |  0  | Control byte for sensor 4. |
|  7  |  0  | Control byte for sensor 5. |

### Description of control register bits

| Bit | Description |
| :--: | ----------- |
|  0,1  | Temperature `unit` to use <br />  **00** - Kelvin. <br /> **01** - Celsius (default). <br />  **10** - Fahrenheit. <br /> **11** - Reserved. |
|  2  | Reserved. |
|  3  | Set to one to enable `low alarm`. |
|  4  | Set to one to enable `high alarm`. |
|  5  | Send `**TurnOn/TurnOff` events instead of `Alarm` if set to one. |
|  6  | Controls when `**TurnOn/TurnOff` events will be sent. <br /><br /> **If set to zero**:<br />  **TurnOn** event is sent when low temperature is reached as set in `low temperature alarm register`.<br />  **TurnOff** event is sent when high temperature is reached as set in `high temperature alarm register`.<br /> <br />**If set to one**:<br />  **TurnOff** event is sent when low temperature is reached a temperature as set in `low temperature alarm register`.<br />  **TurnOn** event is sent when high temperature is reached as set in `high temperature alarm register`.<br /> <br /> The hysteresis value in the [`see sensor hysteresis registers`](./registers?id=sensor-hysteresis-registers) will effect `**TurnOn/TurnOff` events. The temperature must go above or below the hysteresis value before a new event will be sent out. |
|  7  | **If this bit is set** `Alarm events` (or TurnOff/TurnOn events) will be sent continuous with a one second interval until the alarm register is read or the temperature goes to a non alarm state (with hysteresis taken into account) [`see low alarm registers`](#low-alarm-registers). `TurnOn/TurnOff` events will not be affected by a cleared [alarm register (0x80)](https://grodansparadis.github.io/vscp-doc-spec/#/./vscp_register_abstraction_model?id=register-abstraction-modelidregister_abstraction_model) (alarm events will) and will be sent until this bit is cleared or the temperature goes to a non alarm state (with hysteresis taken into account). |

  * `ClASS1.ALARM, Alarm occured event` is documented [here](https://grodansparadis.github.io/vscp-doc-spec/#/./class1.alarm?id=type2).
  * `CLASS1.CONTROL, TurnOff event` is documented [here](https://grodansparadis.github.io/vscp-doc-spec/#/./class1.control?id=type6).
  * `CLASS1.CONTROL, TurnOn event` is documented [here](https://grodansparadis.github.io/vscp-doc-spec/#/./class1.control?id=type5).


## Temperature registers

| Register | Page | Description |
| :--------: | :----: | ----------- |
|  8  |  0  | Temperature sensor 0 MSB (on board sensor). |
|  9  |  0  | Temperature sensor 0 LSB (on board sensor). |
|  10  |  0  | Temperature sensor 1 MSB. |
|  11  |  0  | Temperature sensor 1 LSB. |
|  12  |  0  | Temperature sensor 2 MSB. |
|  13  |  0  | Temperature sensor 2 LSB. |
|  14  |  0  | Temperature sensor 3 MSB. |
|  15  |  0  | Temperature sensor 3 LSB. |
|  16  |  0  | Temperature sensor 4 MSB. |
|  17  |  0  | Temperature sensor 4 LSB. |
|  18  |  0  | Temperature sensor 5 MSB. |
|  19  |  0  | Temperature sensor 5 LSB. |

All values stored as a twos complement number as temperature * 100.

## Report interval registers

| Register | Page | Description |
| :--------: | :----: | ----------- |
|  20  |  0  | Report interval for sensor 0 in seconds (Set to zero for no report). |
|  21  |  0  | Report interval for sensor 1 in seconds (Set to zero for no report). |
|  22  |  0  | Report interval for sensor 2 in seconds (Set to zero for no report). |
|  23  |  0  | Report interval for sensor 3 in seconds (Set to zero for no report). |
|  24  |  0  | Report interval for sensor 4 in seconds (Set to zero for no report). |
|  25  |  0  | Report interval for sensor 5 in seconds (Set to zero for no report). |


## β constant registers

| Register | Page | Description |
| :--------: | :----: | ----------- |
|  38  |  0  | B constant for sensor 0 MSB. |
|  39  |  0  | B constant for sensor 0 LSB. |
|  40  |  1  | B constant for sensor 1 MSB. |
|  41  |  1  | B constant for sensor 1 LSB. |
|  42  |  2  | B constant for sensor 2 MSB. |
|  43  |  2  | B constant for sensor 2 LSB. |
|  44  |  3  | B constant for sensor 3 MSB. |
|  45  |  3  | B constant for sensor 3 LSB. |
|  46  |  4  | B constant for sensor 4 MSB. |
|  47  |  4  | B constant for sensor 4 LSB. |
|  48  |  5  | B constant for sensor 5 MSB. |
|  49  |  5  | B constant for sensor 5 LSB. |

You find the B value for a specific NTC temperature sensor in it's datasheet. The correct B value set here is essential for the temperature calculations to be correct.

You can us [this web-page](http://www.thinksrs.com/downloads/programs/Therm%20Calc/NTCCalibrator/NTCcalculator.htm) for a 10K thermistor if you have a sensor with an unknown beta value. The tool is also useful if yuu want to calibrate your sensor over a sspecific temperature range.


## Low alarm registers

| Register | Page | Description |
| :--------: | :----: | ----------- |
|  50  |  0  | Low alarm set point for sensor 0 MSB. |
|  51  |  0  | Low alarm set point for sensor 0 LSB. |
|  52  |  0  | Low alarm set point for sensor 1 MSB. |
|  53  |  0  | Low alarm set point for sensor 1 LSB. |
|  54  |  0  | Low alarm set point for sensor 2 MSB. |
|  55  |  0  | Low alarm set point for sensor 2 LSB. |
|  56  |  0  | Low alarm set point for sensor 3 MSB. |
|  57  |  0  | Low alarm set point for sensor 3 LSB. |
|  58  |  0  | Low alarm set point for sensor 4 MSB. |
|  59  |  0  | Low alarm set point for sensor 4 LSB. |
|  60  |  0  | Low alarm set point for sensor 5 MSB. |
|  61  |  0  | Low alarm set point for sensor 5 LSB. |

Writing any of these registers will reset high/low alarm conditions for that sensor.

The value is stored as a twos complement number representing the temperature. **Note** note that the value is not stored as temperature times 100 as for some other registers.

The hysteresis value affects the low alarm after a low alarm condition has been set. The temperature must increase with `hysteresis degrees` before the low alarm condition is resetted.


## High alarm registers

| Register | Page | Description |
| :--------: | :----: | ----------- |
|  62  |  0  | High alarm set point for sensor 0 MSB. |
|  63  |  0  | High alarm set point for sensor 0 LSB. |
|  64  |  0  | High alarm set point for sensor 1 MSB. |
|  65  |  0  | High alarm set point for sensor 1 LSB. |
|  66  |  0  | High alarm set point for sensor 2 MSB. |
|  67  |  0  | High alarm set point for sensor 2 LSB. |
|  68  |  0  | High alarm set point for sensor 3 MSB. |
|  69  |  0  | High alarm set point for sensor 3 LSB. |
|  70  |  0  | High alarm set point for sensor 4 MSB. |
|  71  |  0  | High alarm set point for sensor 4 LSB. |
|  72  |  0  | High alarm set point for sensor 5 MSB. |
|  73  |  0  | High alarm set point for sensor 5 LSB. |

Writing any of these registers will reset high/low alarm conditions for that sensor.

The value is stored as a twos complement number representing the temperature. **Note** note that the value is not stored as temperature times 100 as for some other registers.

The hysteresis value affects the high alarm after a high alarm condition has been set. The temperature must decrease with `hysteresis degrees` before the high alarm condition is resetted.

## Sensor zone information registers

Zone and subzone is used to identify the sensor that originated an event. For measurements the [senoridex](https://grodansparadis.github.io/vscp-doc-spec/#/./vscp_measurements?id=datacoding) is used to identify the sensor. The zone and subzone can also be used to identify the location of the sensor.

| Register | Page | Description |
| :------: | :--: | ----------- |
|  74  |  0  | Zone for sensor 0 alarms. |
|  75  |  0  | Sub zone for sensor 0 alarms. |
|  76  |  0  | Zone for sensor 1 alarms. |
|  77  |  0  | Sub zone for sensor 1 alarms. |
|  78  |  0  | Zone for sensor 2 alarms. |
|  79  |  0  | Sub zone for sensor 2 alarms. |
|  80  |  0  | Zone for sensor 3 alarms. |
|  81  |  0  | Sub zone for sensor 3 alarms. |
|  82  |  0  | Zone for sensor 4 alarms. |
|  83  |  0  | Sub zone for sensor 4 alarms. |
|  84  |  0  | Zone for sensor 5 alarms. |
|  85  |  0  | Sub zone for sensor 5 alarms. |

## Sensor absolute low temperature registers

| Register | Page | Description |
| :------: | :--: | ----------- |
|  86  |  0  | Absolute low for sensor 0 MSB (Write to reset). |
|  87  |  0  | Absolute low for sensor 0 LSB (Write to reset). |
|  88  |  0  | Absolute low for sensor 1 MSB (Write to reset). |
|  89  |  0  | Absolute low for sensor 1 LSB (Write to reset). |
|  90  |  0  | Absolute low for sensor 2 MSB (Write to reset). |
|  91  |  0  | Absolute low for sensor 2 LSB (Write to reset). |
|  92  |  0  | Absolute low for sensor 3 MSB (Write to reset). |
|  93  |  0  | Absolute low for sensor 3 LSB (Write to reset). |
|  94  |  0  | Absolute low for sensor 4 MSB (Write to reset). |
|  95  |  0  | Absolute low for sensor 4 LSB (Write to reset). |
|  96  |  0  | Absolute low for sensor 5 MSB (Write to reset). |
|  97  |  0  | Absolute low for sensor 5 LSB (Write to reset). |

The value is stored as a twos complement number as temperature * 100.  Writing any of the registers will reset the value.

## Sensor absolute high temperature registers

| Register | Page | Description |
| :------: | :--: | ----------- |
|  98  |  0  | Absolute high for sensor 0 MSB (Write to reset). |
|  99  |  0  | Absolute high for sensor 0 LSB (Write to reset). |
|  100  |  0  | Absolute high for sensor 1 MSB (Write to reset). |
|  101  |  0  | Absolute high for sensor 1 LSB (Write to reset). |
|  102  |  0  | Absolute high for sensor 2 MSB (Write to reset). |
|  103  |  0  | Absolute high for sensor 2 LSB (Write to reset). |
|  104  |  0  | Absolute high for sensor 3 MSB (Write to reset). |
|  105  |  0  | Absolute high for sensor 3 LSB (Write to reset). |
|  106  |  0  | Absolute high for sensor 4 MSB (Write to reset). |
|  107  |  0  | Absolute high for sensor 4 LSB (Write to reset). |
|  108  |  0  | Absolute high for sensor 5 MSB (Write to reset). |
|  109  |  0  | Absolute high for sensor 5 LSB (Write to reset). |

The value is stored as a twos complement number as temperature * 100.  Writing any of the registers will reset the stored value.

## Sensor hysteresis registers

| Register | Page | Description |
| :------: | :--: | ----------- |
|  110  |  0  | Hysteresis in degrees Celsius for sensor 0 (default is 5 degrees). |
|  111  |  0  | Hysteresis in degrees Celsius for sensor 1 (default is 5 degrees). |
|  112  |  0  | Hysteresis in degrees Celsius for sensor 2 (default is 5 degrees). |
|  113  |  0  | Hysteresis in degrees Celsius for sensor 3 (default is 5 degrees). |
|  114  |  0  | Hysteresis in degrees Celsius for sensor 4 (default is 5 degrees). |
|  115  |  0  | Hysteresis in degrees Celsius for sensor 5 (default is 5 degrees). |

The value here will affect the high and low alarms

### Voltage calibration registers

The Voltage used to calculate the temperature for a sensor have effect on the error. The voltage is theoretically 5V but will in reality be something else but still close to 5V. These register can be used to set a calibrated voltage obtained from actual measurements. The value stored in the two registers is the voltage times 10000 so 5V for example should be stored as 50000.

| Register | Page | Description |
| :------: | :--: | ----------- |
|  118  |  0  | MSB of Voltage (Default is 0xC3). |
|  119  |  0  | MSB of Voltage (Default is 0x50). |


# Register page 1


## Raw A/D values

Raw A/D values from the temperature sensor network can be read if it is a need for that. This can be useful for special nonstandard setups. The A/D converter is a 10-bit converter so the value can be 0-1023 and is stored MSB first in the registers (not in the converter itself though) as all values are stored in VSCP. The registers can only be read, a write is ignored.

| Register | Page | Description |
| :------: | :--: | ----------- |
|  72  |  1  | MSB of A/D converter value for sensor 0. |
|  73  |  1  | LSB of A/D converter value for sensor 0. |
|  74  |  1  | MSB of A/D converter value for sensor 1. |
|  75  |  1  | LSB of A/D converter value for sensor 1. |
|  76  |  1  | MSB of A/D converter value for sensor 2. |
|  77  |  1  | LSB of A/D converter value for sensor 2. |
|  78  |  1  | MSB of A/D converter value for sensor 3. |
|  79  |  1  | LSB of A/D converter value for sensor 3. |
|  80  |  1  | MSB of A/D converter value for sensor 4. |
|  81  |  1  | LSB of A/D converter value for sensor 4. |
|  82  |  1  | MSB of A/D converter value for sensor 5. |
|  83  |  1  | LSB of A/D converter value for sensor 5. |

## Sensor calibration registers

The value in the calibration register is added to the measured sensor value before it's reported. The calibration value is stored as a two's complement temperature * 100 to allow for both positive and negative numbers. So 24.23 degrees is stored as 2423 and so on. This means that 100 will add one degree to the measured value while 65436 (0xFF9c) is -100 and will subtract one degree form the measured value. If you are unsecure on how two complement numbers work there is a guide [here](https://www.cs.cornell.edu/~tomf/notes/cps104/twoscomp.html)

| Register | Page | Description |
| :------: | :--: | ----------- |
|  84  |  1  | MSB of calibration value for sensor 0. |
|  85  |  1  | LSB of calibration value for sensor 0. |
|  86  |  1  | MSB of calibration value for sensor 1. |
|  87  |  1  | LSB of calibration value for sensor 1. |
|  88  |  1  | MSB of calibration value for sensor 2. |
|  89  |  1  | LSB of calibration value for sensor 2. |
|  90  |  1  | MSB of calibration value for sensor 3. |
|  91  |  1  | LSB of calibration value for sensor 3. |
|  92  |  1  | MSB of calibration value for sensor 4. |
|  93  |  1  | LSB of calibration value for sensor 4. |
|  94  |  1  | MSB of calibration value for sensor 5. |
|  95  |  1  | LSB of calibration value for sensor 5. |


  
[filename](./bottom-copyright.md ':include')