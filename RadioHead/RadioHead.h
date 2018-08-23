// RadioHead.h
// Author: Mike McCauley (mikem@airspayce.com) DO NOT CONTACT THE AUTHOR DIRECTLY
// Copyright (C) 2014 Mike McCauley
// $Id: RadioHead.h,v 1.70 2018/02/11 23:57:18 mikem Exp mikem $

/*! \mainpage RadioHead Packet Radio library for embedded microprocessors

This is the RadioHead Packet Radio library for embedded microprocessors.
It provides a complete object-oriented library for sending and receiving packetized messages
via a variety of common data radios and other transports on a range of embedded microprocessors.

The version of the package that this documentation refers to can be downloaded
from http://www.airspayce.com/mikem/arduino/RadioHead/RadioHead-1.83.zip
You can find the latest version of the documentation at http://www.airspayce.com/mikem/arduino/RadioHead

You can also find online help and discussion at
http://groups.google.com/group/radiohead-arduino
Please use that group for all questions and discussions on this topic.
Do not contact the author directly, unless it is to discuss commercial licensing.
Before asking a question or reporting a bug, please read
- http://en.wikipedia.org/wiki/Wikipedia:Reference_desk/How_to_ask_a_software_question
- http://www.catb.org/esr/faqs/smart-questions.html
- http://www.chiark.greenend.org.uk/~shgtatham/bugs.html

Caution: Developing this type of software and using data radios
successfully is challenging and requires a substantial knowledge
base in software and radio and data transmission technologies and
theory.  It may not be an appropriate project for beginners. If
you are a beginner, you will need to spend some time gaining
knowledge in these areas first.


\par Donations

This library is offered under a free GPL license for those who want to use it that way.
We try hard to keep it up to date, fix bugs
and to provide free support. If this library has helped you save time or money, please consider donating at
http://www.airspayce.com or here:

\htmlonly <form action="https://www.paypal.com/cgi-bin/webscr" method="post"><input type="hidden" name="cmd" value="_donations" /> <input type="hidden" name="business" value="mikem@airspayce.com" /> <input type="hidden" name="lc" value="AU" /> <input type="hidden" name="item_name" value="Airspayce" /> <input type="hidden" name="item_number" value="RadioHead" /> <input type="hidden" name="currency_code" value="USD" /> <input type="hidden" name="bn" value="PP-DonationsBF:btn_donateCC_LG.gif:NonHosted" /> <input type="image" alt="PayPal — The safer, easier way to pay online." name="submit" src="https://www.paypalobjects.com/en_AU/i/btn/btn_donateCC_LG.gif" /> <img alt="" src="https://www.paypalobjects.com/en_AU/i/scr/pixel.gif" width="1" height="1" border="0" /></form> \endhtmlonly

\subpage packingdata "Passing Sensor Data Between RadioHead nodes"

\par Trademarks

RadioHead is a trademark of AirSpayce Pty Ltd. The RadioHead mark was first used on April 12 2014 for
international trade, and is used only in relation to data communications hardware and software and related services.
It is not to be confused with any other similar marks covering other goods and services.

\par Copyright

This software is Copyright (C) 2011-2016 Mike McCauley. Use is subject to license
conditions. The main licensing options available are GPL V2 or Commercial:

\par Open Source Licensing GPL V2

This is the appropriate option if you want to share the source code of your
application with everyone you distribute it to, and you also want to give them
the right to share who uses it. If you wish to use this software under Open
Source Licensing, you must contribute all your source code to the open source
community in accordance with the GPL Version 2 when your application is
distributed. See https://www.gnu.org/licenses/gpl-2.0.html

\par Commercial Licensing

This is the appropriate option if you are creating proprietary applications
and you are not prepared to distribute and share the source code of your
application. Purchase commercial licenses at http://airspayce.binpress.com
*/



#ifndef RadioHead_h
#define RadioHead_h

// This is the address that indicates a broadcast
#define RH_BROADCAST_ADDRESS    0xFF
#define RH_THIS_SYSTEM_ADDRESS  0x01  // You may change it!

// Official version numbers are maintained automatically by Makefile:
#define RH_VERSION_MAJOR 1
#define RH_VERSION_MINOR 83

// Symbolic names for currently supported platform types
#define RH_PLATFORM_ARDUINO          1
#define RH_PLATFORM_MSP430           2
#define RH_PLATFORM_STM32            3
#define RH_PLATFORM_GENERIC_AVR8     4
#define RH_PLATFORM_UNO32            5
#define RH_PLATFORM_UNIX             6
#define RH_PLATFORM_STM32STD         7
#define RH_PLATFORM_STM32F4_HAL      8
#define RH_PLATFORM_RASPI            9
#define RH_PLATFORM_NRF51            10
#define RH_PLATFORM_ESP8266          11
#define RH_PLATFORM_STM32F2          12
#define RH_PLATFORM_CHIPKIT_CORE     13
#define RH_PLATFORM_ESP32            14
#define RH_PLATFORM_NRF52            15

////////////////////////////////////////////////////
// Select platform automatically, if possible
#ifndef RH_PLATFORM
 #if (MPIDE>=150 && defined(ARDUINO))
  // Using ChipKIT Core on Arduino IDE
  #define RH_PLATFORM RH_PLATFORM_CHIPKIT_CORE
 #elif defined(MPIDE)
  // Uno32 under old MPIDE, which has been discontinued:
  #define RH_PLATFORM RH_PLATFORM_UNO32
#elif defined(NRF51)
  #define RH_PLATFORM RH_PLATFORM_NRF51
#elif defined(NRF52)
  #define RH_PLATFORM RH_PLATFORM_NRF52
 #elif defined(ESP8266)
  #define RH_PLATFORM RH_PLATFORM_ESP8266
 #elif defined(ESP32)
  #define RH_PLATFORM RH_PLATFORM_ESP32
 #elif defined(ARDUINO)
  #define RH_PLATFORM RH_PLATFORM_ARDUINO
 #elif defined(__MSP430G2452__) || defined(__MSP430G2553__)
  #define RH_PLATFORM RH_PLATFORM_MSP430
 #elif defined(MCU_STM32F103RE)
  #define RH_PLATFORM RH_PLATFORM_STM32
 #elif defined(STM32F2XX)
  #define RH_PLATFORM RH_PLATFORM_STM32F2
 #elif defined(USE_STDPERIPH_DRIVER)
  #define RH_PLATFORM RH_PLATFORM_STM32STD
 #elif defined(RASPBERRY_PI)
  #define RH_PLATFORM RH_PLATFORM_RASPI
#elif defined(__unix__) // Linux
  #define RH_PLATFORM RH_PLATFORM_UNIX
#elif defined(__APPLE__) // OSX
  #define RH_PLATFORM RH_PLATFORM_UNIX
 #else
  #error Platform not defined!
 #endif
#endif

#if defined(__AVR_ATtiny84__) || defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtinyX4__) || defined(__AVR_ATtinyX5__) || defined(__AVR_ATtiny2313__) || defined(__AVR_ATtiny4313__) || defined(__AVR_ATtinyX313__)
 #define RH_PLATFORM_ATTINY
#endif

////////////////////////////////////////////////////
// Platform specific headers:
#if (RH_PLATFORM == RH_PLATFORM_ARDUINO)
 #if (ARDUINO >= 100)
  #include <Arduino.h>
 #else
  #include <wiring.h>
 #endif
 #ifdef RH_PLATFORM_ATTINY
  #warning Arduino TinyCore does not support hardware SPI. Use software SPI instead.
 #else
  #include <SPI.h>
  #define RH_HAVE_HARDWARE_SPI
  #define RH_HAVE_SERIAL
 #endif

#elif (RH_PLATFORM == RH_PLATFORM_ESP8266) // ESP8266 processor on Arduino IDE
 #include <Arduino.h>
 #include <SPI.h>
 #define RH_HAVE_HARDWARE_SPI
 #define RH_HAVE_SERIAL

#elif (RH_PLATFORM == RH_PLATFORM_ESP32)   // ESP32 processor on Arduino IDE
 #include <Arduino.h>
 #include <SPI.h>
 #define RH_HAVE_HARDWARE_SPI
 #define RH_HAVE_SERIAL

#elif (RH_PLATFORM == RH_PLATFORM_MSP430) // LaunchPad specific
 #include "legacymsp430.h"
 #include "Energia.h"
 #include <SPI.h>
 #define RH_HAVE_HARDWARE_SPI
 #define RH_HAVE_SERIAL

#elif (RH_PLATFORM == RH_PLATFORM_UNO32 || RH_PLATFORM == RH_PLATFORM_CHIPKIT_CORE)
 #include <WProgram.h>
 #include <string.h>
 #include <SPI.h>
 #define RH_HAVE_HARDWARE_SPI
 #define memcpy_P memcpy
 #define RH_HAVE_SERIAL

#elif (RH_PLATFORM == RH_PLATFORM_STM32F2) // Particle Photon with firmware-develop
 #include <stm32f2xx.h>
 #include <application.h>
 #include <math.h> // floor
 #define RH_HAVE_SERIAL
 #define RH_HAVE_HARDWARE_SPI

#elif (RH_PLATFORM == RH_PLATFORM_STM32STD) // STM32 with STM32F4xx_StdPeriph_Driver
 #include <stm32f4xx.h>
 #include <wirish.h>
 #include <stdint.h>
 #include <string.h>
 #include <math.h>
 #include <HardwareSPI.h>
 #define RH_HAVE_HARDWARE_SPI
 #define Serial SerialUSB
 #define RH_HAVE_SERIAL

#elif (RH_PLATFORM == RH_PLATFORM_GENERIC_AVR8)
 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include <util/delay.h>
 #include <string.h>
 #include <stdbool.h>
 #define RH_HAVE_HARDWARE_SPI
 #include <SPI.h>

// For Steve Childress port to ARM M4 w/CMSIS with STM's Hardware Abstraction lib.
// See ArduinoWorkarounds.h (not supplied)
#elif (RH_PLATFORM == RH_PLATFORM_STM32F4_HAL)
 #include <ArduinoWorkarounds.h>
 #include <stm32f4xx.h> // Also using ST's CubeMX to generate I/O and CPU setup source code for IAR/EWARM, not GCC ARM.
 #include <stdint.h>
 #include <string.h>
 #include <math.h>
 #define RH_HAVE_HARDWARE_SPI // using HAL (Hardware Abstraction Libraries from ST along with CMSIS, not arduino libs or pins concept.

#elif (RH_PLATFORM == RH_PLATFORM_NRF51)
 #define RH_HAVE_SERIAL
 #define PROGMEM
  #include <Arduino.h>

#elif (RH_PLATFORM == RH_PLATFORM_NRF52)
 #include <SPI.h>
 #define RH_HAVE_HARDWARE_SPI
 #define RH_HAVE_SERIAL
 #define PROGMEM
  #include <Arduino.h>

#else
 #error Platform unknown!
#endif

////////////////////////////////////////////////////
// This is an attempt to make a portable atomic block
#if (RH_PLATFORM == RH_PLATFORM_ARDUINO)
#if defined(__arm__)
  #include <RHutil/atomic.h>
 #else
  #include <util/atomic.h>
 #endif
 #define ATOMIC_BLOCK_START     ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
 #define ATOMIC_BLOCK_END }
#elif (RH_PLATFORM == RH_PLATFORM_CHIPKIT_CORE)
 // UsingChipKIT Core on Arduino IDE
 #define ATOMIC_BLOCK_START unsigned int __status = disableInterrupts(); {
 #define ATOMIC_BLOCK_END } restoreInterrupts(__status);
#elif (RH_PLATFORM == RH_PLATFORM_UNO32)
 // Under old MPIDE, which has been discontinued:
 #include <peripheral/int.h>
 #define ATOMIC_BLOCK_START unsigned int __status = INTDisableInterrupts(); {
 #define ATOMIC_BLOCK_END } INTRestoreInterrupts(__status);
#elif (RH_PLATFORM == RH_PLATFORM_STM32F2) // Particle Photon with firmware-develop
 #define ATOMIC_BLOCK_START { int __prev = HAL_disable_irq();
 #define ATOMIC_BLOCK_END  HAL_enable_irq(__prev); }
#elif (RH_PLATFORM == RH_PLATFORM_ESP8266)
// See hardware/esp8266/2.0.0/cores/esp8266/Arduino.h
 #define ATOMIC_BLOCK_START { uint32_t __savedPS = xt_rsil(15);
 #define ATOMIC_BLOCK_END xt_wsr_ps(__savedPS);}
#else
 // TO BE DONE:
 #define ATOMIC_BLOCK_START
 #define ATOMIC_BLOCK_END
#endif

////////////////////////////////////////////////////
// Try to be compatible with systems that support yield() and multitasking
// instead of spin-loops
// Recent Arduino IDE or Teensy 3 has yield()
#if (RH_PLATFORM == RH_PLATFORM_ARDUINO && ARDUINO >= 155 && !defined(RH_PLATFORM_ATTINY)) || (TEENSYDUINO && defined(__MK20DX128__))
 #define YIELD yield();
#elif (RH_PLATFORM == RH_PLATFORM_ESP8266)
// ESP8266 also has it
 #define YIELD yield();
#else
 #define YIELD
#endif

////////////////////////////////////////////////////
// digitalPinToInterrupt is not available prior to Arduino 1.5.6 and 1.0.6
// See http://arduino.cc/en/Reference/attachInterrupt
#ifndef NOT_AN_INTERRUPT
 #define NOT_AN_INTERRUPT -1
#endif
#ifndef digitalPinToInterrupt
 #if (RH_PLATFORM == RH_PLATFORM_ARDUINO) && !defined(__arm__)

  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
   // Arduino Mega, Mega ADK, Mega Pro
   // 2->0, 3->1, 21->2, 20->3, 19->4, 18->5
   #define digitalPinToInterrupt(p) ((p) == 2 ? 0 : ((p) == 3 ? 1 : ((p) >= 18 && (p) <= 21 ? 23 - (p) : NOT_AN_INTERRUPT)))

  #elif defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__)
   // Arduino 1284 and 1284P - See Manicbug and Optiboot
   // 10->0, 11->1, 2->2
   #define digitalPinToInterrupt(p) ((p) == 10 ? 0 : ((p) == 11 ? 1 : ((p) == 2 ? 2 : NOT_AN_INTERRUPT)))

  #elif defined(__AVR_ATmega32U4__)
   // Leonardo, Yun, Micro, Pro Micro, Flora, Esplora
   // 3->0, 2->1, 0->2, 1->3, 7->4
   #define digitalPinToInterrupt(p) ((p) == 0 ? 2 : ((p) == 1 ? 3 : ((p) == 2 ? 1 : ((p) == 3 ? 0 : ((p) == 7 ? 4 : NOT_AN_INTERRUPT)))))

  #else
   // All other arduino except Due:
   // Serial Arduino, Extreme, NG, BT, Uno, Diecimila, Duemilanove, Nano, Menta, Pro, Mini 04, Fio, LilyPad, Ethernet etc
   // 2->0, 3->1
   #define digitalPinToInterrupt(p)  ((p) == 2 ? 0 : ((p) == 3 ? 1 : NOT_AN_INTERRUPT))

  #endif

 #elif (RH_PLATFORM == RH_PLATFORM_UNO32) || (RH_PLATFORM == RH_PLATFORM_CHIPKIT_CORE)
  // Hmmm, this is correct for Uno32, but what about other boards on ChipKIT Core?
  #define digitalPinToInterrupt(p) ((p) == 38 ? 0 : ((p) == 2 ? 1 : ((p) == 7 ? 2 : ((p) == 8 ? 3 : ((p) == 735 ? 4 : NOT_AN_INTERRUPT)))))

 #else
  // Everything else (including Due and Teensy) interrupt number the same as the interrupt pin number
  #define digitalPinToInterrupt(p) (p)
 #endif
#endif

// On some platforms, attachInterrupt() takes a pin number, not an interrupt number
#if (RH_PLATFORM == RH_PLATFORM_ARDUINO) && defined (__arm__) && (defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_SAM_DUE))
 #define RH_ATTACHINTERRUPT_TAKES_PIN_NUMBER
#endif

// Slave select pin, some platforms such as ATTiny do not define it.
#ifndef SS
 #define SS 10
#endif

// These defs cause trouble on some versions of Arduino
#undef abs
#undef round
#undef double

// Sigh: there is no widespread adoption of htons and friends in the base code, only in some WiFi headers etc
// that have a lot of excess baggage
#if RH_PLATFORM != RH_PLATFORM_UNIX && !defined(htons)
// #ifndef htons
// These predefined macros available on modern GCC compilers
 #if   __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  // Atmel processors
  #define htons(x) ( ((x)<<8) | (((x)>>8)&0xFF) )
  #define ntohs(x) htons(x)
  #define htonl(x) ( ((x)<<24 & 0xFF000000UL) | \
                   ((x)<< 8 & 0x00FF0000UL) | \
                   ((x)>> 8 & 0x0000FF00UL) | \
                   ((x)>>24 & 0x000000FFUL) )
  #define ntohl(x) htonl(x)

 #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  // Others
  #define htons(x) (x)
  #define ntohs(x) (x)
  #define htonl(x) (x)
  #define ntohl(x) (x)

 #else
  #error "Dont know how to define htons and friends for this processor"
 #endif
#endif



// Uncomment this is to enable Encryption (see RHEncryptedDriver):
// But ensure you have installed the Crypto directory from arduinolibs first:
// http://rweather.github.io/arduinolibs/index.html
//#define RH_ENABLE_ENCRYPTION_MODULE

#endif
