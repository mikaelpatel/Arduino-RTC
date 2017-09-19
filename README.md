# Arduino-RTC

The RTC library has been developed to allow a common interface for RTC
devices. The library includes a software based RTC, and device drivers
for DS1302 and DS1307 Real-Time Clock devices.

Version: 1.0

## Classes

* [Software Real-Time Clock, RTC](./src/RTC.h)
* [Real-Time Clock/Calender, DS1302](./src/Driver/DS1302.h)
* [Two-Wire Real-Time Clock/Calender, DS1307](./src/Driver/DS1307.h)

## Example Sketches

* [RTC](./examples/RTC)
* [RAM](./examples/RAM)

## Dependencies

* [General Purpose Input/Output library for Arduino, GPIO](https://github.com/mikaelpatel/Arduino-GPIO)
* [Two-Wire Interface library for Arduino, TWI](https://github.com/mikaelpatel/Arduino-TWI)
