#include <Arduino.h> // for uint32_t type

#ifndef PMM_CONSTS_h
#define PMM_CONSTS_h

// This .h is for the base PMM uses. For a specific constant, create another .h .
// PMM pins, PMM only macros etc.


// =======================================================================
//  SD
// =======================================================================

#define PMM_USE_SD 1


// =======================================================================
//  LoRa
// =======================================================================
#define PIN_RFM95_CS 99
#define PIN_RFM95_RST 99            // Reset
#define PIN_RFM95_INT 99            // Interrupt
#define LORA_FREQUENCY 915.0         // Mhz

// =======================================================================
//  RF and DATA_LIST
// =======================================================================
const char RF_HEADER_LOG[5] = {"MNRV"};
const char RF_HEADER_EXTRA_LOG[5] = {"MNEX"};
const char RF_HEADER_VARS_INFO[5] = {"MNVI"};

#define RF_MAX_APOGEE 3500   // Filter eventual garbage that may come to the display area


#define EXTRA_LOG_MAX_STRING_LENGTH 128
#define EXTRA_LOG_MAX_PACKETS 1000
#define EXTRA_LOG_ARRAY_LENGTH EXTRA_LOG_MAX_STRING_LENGTH * EXTRA_LOG_MAX_PACKETS

// =======================================================================
//  GPS
// =======================================================================

#define PMM_USE_GPS 1

#if PMM_USE_GPS
    #define PMM_GPS_GET_SPEEDS 0
#endif





#endif
