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
#define PMM_USE_RF 1

#define PIN_RFM95_CS 99
#define PIN_RFM95_RST 99            // Reset
#define PIN_RFM95_INT 99            // Interrupt
#define LORA_FREQUENCY 915.0         // Mhz

// =======================================================================
//  RF and DATA_LIST
// =======================================================================
const char RF_HEADER_LOG[4] = {'M', 'N', 'R', 'V'};
const char RF_HEADER_VARS_INFO[4] = {'M', 'V', 'A', 'I'};
const char RF_HEADER_STRING[4] = {'M', 'S', 'T', 'R'};


#define PMM_TELEMETRY_LOG_NUMBER_VARIABLES 32
#define PMM_VARIABLE_STRING_LENGTH 22

#define EXTRA_LOG_MAX_STRING_LENGTH 128

#define EXTRA_LOG_ARRAY_LENGTH EXTRA_LOG_MAX_STRING_LENGTH * EXTRA_LOG_MAX_DISPLAYED

// =======================================================================
//  GPS
// =======================================================================

#define PMM_USE_GPS 1

#if PMM_USE_GPS

    #define PMM_GPS_EXTERNAL_CONFIGURATION 1 // To activate the lines below. Also changed the neoGps/GPSfix_cfg.h file.

    // External change of the neoGps/GPSfix_cfg.h file.
    // Comment / Uncomment which GPS informations you want to obtain. They will also be included in your logs and on the RF
    // package automatically.

                // these comments are which variables are added if the #define is enabled.
    #if PMM_GPS_EXTERNAL_CONFIGURATION

        //#define GPS_FIX_DATE // Not implemented yet on PMM.
        //#define GPS_FIX_TIME // Not implemented yet on PMM.

        #define GPS_FIX_LOCATION // Latitude and longitude
                // float gpsLat
                // float gpsLon

        //#define GPS_FIX_LOCATION_DMS // Advanced and not implemented on PMM.

        #define GPS_FIX_ALTITUDE
                // float gpsAltitude

        #define GPS_FIX_SPEED // need GPS_FIX_ALTITUDE activated
                // unsigned long lastReadMillis;
                // float horizontalSpeed;
                // float speedNorth;
                // float speedEast;
                // float speedUp;
                // float headingDegree;

        #ifdef GPS_FIX_SPEED // PMM: Automatically adds these following needed defines, if #define GPS_FIX_SPEED is uncommented.
            #define GPS_FIX_VELNED
            #define GPS_FIX_HEADING
        #endif

        #define GPS_FIX_SATELLITES
                // uint8_t satellites;

        //#define GPS_FIX_HDOP // Advanced and not implemented on PMM.
        //#define GPS_FIX_VDOP // Advanced and not implemented on PMM.
        //#define GPS_FIX_PDOP // Advanced and not implemented on PMM.
        //#define GPS_FIX_LAT_ERR // Advanced and not implemented on PMM.
        //#define GPS_FIX_LON_ERR // Advanced and not implemented on PMM.
        //#define GPS_FIX_ALT_ERR // Advanced and not implemented on PMM.
        //#define GPS_FIX_GEOID_HEIGHT // Advanced and not implemented on PMM: But should we?


    #endif // #if PMM_GPS_EXTERNAL_CONFIGURATION

#endif // #if PMM_USE_GPS

#endif // #ifndef PMM_CONSTS_h
