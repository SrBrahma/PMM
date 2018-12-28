/* pmmConsts.h
 * This .h is for the base PMM uses. For a specific constant, create another .h .
 * PMM pins, PMM only macros etc.
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_CONSTS_h
#define PMM_CONSTS_h

#include <Arduino.h>    // For LED_BUILTIN define



// =======================================================================
//  General
// =======================================================================
    #define PMM_THIS_NAME_DEFINE            "Aurora" // Used in pmmPackageString.cpp.

    #define PMM_IS_PDA                      0

    #define PMM_USE_BUZZER                  0
    #define PMM_USE_LED_RECOVERY            0
    #define PMM_USE_LED_ERRORS              0
    #define PMM_USE_LED_ALL_OK_AND_RF       0

    #define PMM_PIN_BUZZER                  99
    #define PMM_PIN_LED_RECOVERY            99
    #define PMM_PIN_LED_ERRORS              99
    #define PMM_PIN_ALL_OK_AND_TELEMETRY    LED_BUILTIN

    #define PMM_MAX_UP_ACCELERATION_M_S_2      250.0 // Used in the filtering of the altitude. 300m/s^2 is ~30g.
    #define PMM_MAX_DOWN_ACCELERATION_M_S_2    20.0  // 25m/s^2 to avoid any problem.  
    typedef enum
    {
        MODE_SLEEP,
        MODE_READY,
        MODE_DEPLOYED,
        MODE_FINISHED
    } pmmSystemState;

// =======================================================================
//  Debug
// =======================================================================
    #define PMM_DEBUG                               0 // Will enable debug messages via Serial. All error messages will be displayed with this, and some minor messages.
    #define PMM_DEBUG_MORE                          0 // Additional debug messages, like "MPU6050 initialized successfully!". The [M] indicates it was enabled by this define.
    // For this to work, PMM_DEBUG must be 1. Also, the main classes usually have another define like PMM_IMU_DEBUG_MORE, so you can activate the additional messages you want.

    #define PMM_DEBUG_TIMEOUT_ENABLED               1 // [If debug is enabled] If 0, the code will wait indefinitely for the Serial connection with the
    // computer to the code proceed. If 1, the code will have a maximum wait time for the connection to take effect, and if this does not occur,
    // the code will proceed, without Serial debugging. It's great if you forgot to disable the Debug before launching your rocket!
    #define PMM_DEBUG_TIMEOUT_MILLIS                15000 // The maximum wait time for the above option. Default is 10 seconds (10000ms).

    // These 2 below are ways of calmly reading the init() messages, to see if everything was successful or something went wrong.
    // The PMM_DEBUG_WAIT_X_MILLIS_AFTER_INIT won't happen if PMM_DEBUG_WAIT_FOR_ANY_KEY_PRESSED is 1.
    #define PMM_DEBUG_WAIT_FOR_ANY_KEY_PRESSED      0
    #define PMM_DEBUG_WAIT_X_MILLIS_AFTER_INIT      5000



// =======================================================================
//  Extra Codes
// =======================================================================
    // Extra codes are a fast way of testing a system, individually. It is very useful for debugging, testing and calibrating sensors,
    // formatting the SD, and whatever I may add on the future.
    // Besides this Enable, the Debug MUST also be enabled for the Extra Codes activation.
    #define PMM_EXTRA_CODES_ENABLE                  0

        #define PMM_SD_EXTRA_CODES                  1   // For now, there are only extra codes for the SD.
        #define PMM_GPS_EXTRA_CODES                 1
        #define PMM_IMU_EXTRA_CODES                 1
        #define PMM_TELEMETRY_EXTRA_CODES           0



// =======================================================================
//  IMU
// =======================================================================
    #define PMM_USE_IMU                             1



// =======================================================================
//  SD
// =======================================================================
    #define PMM_USE_SD                              1



// =======================================================================
//  Telemetry
// =======================================================================
    #define PMM_USE_TELEMETRY                           1
    #define PMM_TELEMETRY_ADDRESS_THIS_SYSTEM_CONST_H   1   // This system address. Two systems MUST NOT have the same address. Control them.



// =======================================================================
//  LoRa
// =======================================================================
    #define PMM_RF_INIT_MAX_TRIES               10

    #define PMM_PIN_RFM95_CS                    9
    #define PMM_PIN_RFM95_RST                   38
    #define PMM_PIN_RFM95_INT                   5

    #define PMM_LORA_FREQUENCY                  915.0

    #define PMM_LORA_TX_POWER                   13 // 5~23



// =======================================================================
//  GPS
// =======================================================================
    #define PMM_USE_GPS                         1

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
