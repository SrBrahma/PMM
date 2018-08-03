/* pmmConsts.h
 * This .h is for the base PMM uses. For a specific constant, create another .h .
 * PMM pins, PMM only macros etc.
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <Arduino.h> // for uint32_t type

#ifndef PMM_CONSTS_h
#define PMM_CONSTS_h

// =======================================================================
//  Debug
// =======================================================================
#define PMM_DEBUG_SERIAL                    1
#define PMM_DEBUG_SERIAL_MORE               1 // Additional debug messages.

#define PMM_DEBUG_SERIAL_TIMEOUT_ENABLED    1 /* [If debug is enabled] If 0, the code will wait indefinitely for the Serial connection with the
computer to the code proceed. If 1, the code will have a maximum wait time for the connection to take effect, and if this does not occur,
the code will proceed, without Serial debugging. Great case you forgot to disable the Debug before launching your rocket! */
#define PMM_DEBUG_SERIAL_TIMEOUT_MILLIS     10000 // The maximum wait time for the above option. Default is 10 seconds (10000ms).

#if PMM_DEBUG_SERIAL
    #define PMM_DEBUG_PRINT(x) Serial.println(x)
#else
    #define PMM_DEBUG_PRINT(x) do {} while (0)
#endif

#if PMM_DEBUG_SERIAL_MORE
    #define PMM_DEBUG_PRINT_MORE(x) Serial.println(x)
#else
    #define PMM_DEBUG_PRINT_MORE(x) do {} while (0)
#endif

#define PMM_DEBUG_PRINT_IMU_MORE 1 // Prints "barometer updated!" etc

// =======================================================================
//  General
// =======================================================================
#define PMM_USE_BUZZER              0
#define PMM_USE_LED_RECOVERY        0
#define PMM_USE_LED_ERRORS          0
#define PMM_USE_LED_ALL_OK_AND_RF   0

#define PMM_PIN_BUZZER                  99
#define PMM_PIN_LED_RECOVERY            99
#define PMM_PIN_LED_ERRORS              99
#define PMM_PIN_ALL_OK_AND_TELEMETRY    LED_BUILTIN

#define PMM_VARIABLE_STRING_LENGTH          22

#define PMM_IS_PDA 0
// =======================================================================
//  SD
// =======================================================================


#define PMM_USE_SD                  0
#define PMM_SD_FILENAME_MAX_LENGTH  64
const char PMM_SD_FILENAME_BASE_PREFIX[] = {"pmmLog"};
const char PMM_SD_FILENAME_BASE_SUFFIX[] = {".csv"};

// =======================================================================
//  Telemetry
// =======================================================================
#define PMM_USE_TELEMETRY       1

const char PMM_TELEMETRY_HEADER_LOG[5]      = {"MLOG"}; // These are strings for snprintf(), on string package.
const char PMM_TELEMETRY_HEADER_LOG_INFO[5] = {"MLIN"};
const char PMM_TELEMETRY_HEADER_STRING[5]   = {"MSTR"};


#define PMM_TELEMETRY_LOG_NUMBER_VARIABLES  32
#define PMM_TELEMETRY_MAX_PAYLOAD_LENGTH    251 // Must be 251 (LoRa max is 255, 4 bytes are used as default headers by the RadioHead lib (maybe will remove/change it on future)). read recv2() in RH_RF95.cpp.

// =======================================================================
//  LoRa
// =======================================================================
#define PMM_RF_INIT_MAX_TRIES   10

#define PMM_PIN_RFM95_CS        10
#define PMM_PIN_RFM95_RST       38
#define PMM_PIN_RFM95_INT       6
#define PMM_LORA_FREQUENCY      915.0

// =======================================================================
//  GPS
// =======================================================================
#define PMM_USE_GPS     1

#if PMM_USE_GPS

    #define PMM_GPS_EXTERNAL_CONFIGURATION  1 // To activate the lines below. Also changed the neoGps/GPSfix_cfg.h file.
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
