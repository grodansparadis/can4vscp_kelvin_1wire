<h1>Kelvin 1Wire</h1>

<img src="http://grodansparadis.com/images/vscp_logo.jpg" width="100" />

This project is part of the <a href="http://www.vscp.org">VSCP (Very Simple Control Protocol) project</a>. 

<hr>

The Kelvin 1-wire module measures temperature. It measures temperatures from -55°C to +125°C (-67°F to +257°F) with a ±0.5°C accuracy from -10°C to +85°C. It does so by allowing connection of up to eight 1-wire temperature sensors and by measuring the temperature on the surface of the module itself. The measured temperature can be reported to other CAN4VSCP modules or to one or several servers so it can be collected in a database or visualized and analyzed in a diagram. The accuracy of the sensors can be increase further by calibration.

Temperature data can be presented by the module in Kelvin, Celsius or Fahrenheit. It is also possible to select format of the data so it is presented as a normalized integer, a floating point value or as a string.

Temperatures is reported automatically and it is possible to set alarms for high/low levels. Absolute low and high temperatures are collected and it is easy to construct autonomous and reliable temperature regulation systems.

The module fully adopts to the CAN4VSCP specification and can be powered directly over the bus with a 9-28V DC power source. It has a rich register set for configuration and any information events defined. It also have a decision matrix for easy dynamic event handling.

VSCP CAN modules are designed to work on a CAN4CAN bus which use ordinary RJ-45 connectors and is powered with 9-28V DC over the same cable. This means there is no need for a separate power cable. All that is needed is a CAT5 or better twisted pair cable. Buss length can be a maximum of 500 meters with drops of maximum 24 meters length (up to a total of 120 meters). As for all CAN4VSCP modules the communication speed is fixed at 125 kbps.

All VSCP modules contains information of there own setup, manual, hardware version, manufacturer etc. You just ask the module for the information you need and you will get it. When they are started up they have a default functionality that often is all that is needed to get a working setup. If the module have something to report it will send you an event and if it is setup to react on a certain type of event it will do it's work when you send event(s) to it. 

## Project files

### User manual 
  * [User Manual](https://grodansparadis.github.io/can4vscp_kelvin_1wire/#)

### Schematic, PCB, 3D files etc
  * [Schematics reversion A](https://github.com/grodansparadis/can4vscp_kelvin_1wire/blob/master/eagle/kelvin_1wire_brd_rev_a.png)
 * Hardware design files is made in [KiCad](https://kicad.org) and can be found in the `kicad` directory. Valid from reversion A of the hardware.
   * Gerber files for PCB production can be found in the `gerber` directory (in the `kicad` folder).
 * Eagle schema and board files can be found in the `eagle` directory. They are no longer actively updated.

 ### Firmware

 The firmware is developed in [MPLAB X IDE](https://www.microchip.com/mplab/mplab-x-ide) using the [XC8 compiler](https://www.microchip.com/mplab/compilers).

  * Binary release files is available [here](https://github.com/grodansparadis/can4vscp_kelvin_1wire/releases)

### MDF - Module Description File(s)
  * [MDF file version: A Release date: 2020-05-15](http://www.eurosource.se/1wire_1.xml.xml)


### Support
If you need support, please open an issue in the [GitHub repository](https://github.com/grodansparadis/can4vscp_kelvin_1wire/issues).

### Buy a ready made modules
You can buy a ready made module from [Grodans Paradis](http://www.grodansparadis.com).





