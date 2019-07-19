// This .h is for the base PMM uses. For a specific constant, create another .h .
// PMM pins, PMM only macros etc.
// By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma), Rio de Janeiro - Brazil

#ifndef PMM_CONSTS_h
#define PMM_CONSTS_h

#include <Arduino.h>    // For LED_BUILTIN define


// =======================================================================
//  Routine
// =======================================================================
    #define PMM_ROUTINE_ROCKET_AVIONIC                      0
    #define PMM_ROUTINE_LAUNCH_SIM                          1

    #define PMM_SYSTEM_ROUTINE     PMM_ROUTINE_ROCKET_AVIONIC

// =======================================================================
//  General
// =======================================================================
    #define PMM_THIS_NAME_DEFINE                     "Aurora" // Used in messageLog.cpp.

    // This system address. Two systems MUST NOT have the same address.
    #define PMM_TLM_ADDRESS_THIS_SYSTEM                     1   


    #define PMM_USE_BUZZER                                  0
    #define PMM_USE_LED_RECOVERY                            0
    #define PMM_USE_LED_ERRORS                              0
    #define PMM_USE_LED_ALL_OK_AND_RF                       0

    #define PMM_PIN_BUZZER                                 99
    #define PMM_PIN_LED_RECOVERY                           99
    #define PMM_PIN_LED_ERRORS                             99
    #define PMM_PIN_ALL_OK_AND_TELEMETRY          LED_BUILTIN



// =======================================================================
//  Debug
// =======================================================================
    #define PMM_DEBUG                                       1  // Toggles the debug mode, via Serial.

        #define PMM_DATA_LOG_DEBUG                          0  // Prints the values of the data of the DataLog; = Prints the main variables of your system.

        #define PMM_DEBUG_MORE                              0  // Additional debug messages, like "MPU6050 initialized successfully!". The [M] indicates it was enabled by this define.
            #define PMM_IMU_DEBUG_MORE                      0
            #define PMM_GPS_DEBUG_MORE                      0
            #define PMM_TLM_DEBUG_MORE                      0

        // [WARNING!!! READ THIS ONE!!!!] !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // [WARNING!!! READ THIS ONE!!!!] !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // [WARNING!!! READ THIS ONE!!!!] !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // If 0, the code will wait indefinitely for the Serial connection with the
        // computer to the code proceed. If 1, the code will have a maximum wait time for the connection to take effect, and if this does not occur,
        // the code will proceed, without Serial debugging. It's great if you forgot to disable the Debug before launching your rocket!
        #define PMM_DEBUG_TIMEOUT_ENABLED                   0
            #define PMM_DEBUG_TIMEOUT_MILLIS             3000 // The max wait time in millis, for the above option.


        // These 2 below are ways of calmly reading the init() messages, to see if everything was successful or something went wrong.

        // They only happen if PMM_DEBUG is 1 and if Serial is working.
        #define PMM_DEBUG_WAIT_FOR_ANY_KEY_PRESSED          1

        // The PMM_DEBUG_WAIT_AFTER_INIT won't happen if PMM_DEBUG_WAIT_FOR_ANY_KEY_PRESSED is 1.
        #define PMM_DEBUG_WAIT_AFTER_INIT                   0
            #define PMM_DEBUG_WAIT_X_MILLIS_AFTER_INIT   5000



// =======================================================================
//  Extra Codes
// =======================================================================
    // Extra codes are a fast way of testing a system, individually. It is very useful for debugging, testing and calibrating sensors,
    // formatting the SD, and whatever I may add on the future.
    // Besides this Enable, the Debug MUST also be enabled for the Extra Codes activation.
    #define PMM_EXTRA_CODES_ENABLE                          0

        #define PMM_SD_EXTRA_CODES                          0
        #define PMM_GPS_EXTRA_CODES                         0
        #define PMM_IMU_EXTRA_CODES                         0
        #define PMM_TLM_EXTRA_CODES                         0



// =======================================================================
//  IMU
// =======================================================================

    // Allows using different I2C channels. Default is 'Wire'. Depending on hardware, can be also
    // 'Wire1', 'Wire2'...
    #define PMM_IMU_I2C_CHANNEL                     Wire2


// =======================================================================
//  SD
// =======================================================================




// =======================================================================
//  LoRa
// =======================================================================

    // Selects the SPI channel. Default channel is 0. Can go up to 2. https://github.com/PaulStoffregen/RadioHead
    #define PMM_LORA_SPI_CHANNEL                            1

    #define PMM_PIN_RFM95_CS                               15
    #define PMM_PIN_RFM95_RST                              30
    #define PMM_PIN_RFM95_INT                              29

    #define PMM_LORA_FREQUENCY                          915.0

    #define PMM_LORA_TX_POWER                              13 // 5~23



// =======================================================================
//  GPS
// =======================================================================
    #define PMM_USE_GPS                                     1

        // Set the used GPS serial channel (Serial, Serial1...).
        #define PMM_GPS_PORT                          Serial2


#endif // #ifndef PMM_CONSTS_h
