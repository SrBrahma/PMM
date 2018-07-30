/*  Code written by Marcelo Maroñas, Eduardo Alves, Lucas Ribeiro, HENRIQUE BRUNO and Victor de Lucca @ Minerva Rockets (Federal University of Rio de Janeiro Rocketry Team) - March 06, 2018
 *  This is an adaptation necessary for using the GY80 IMU module with the microcontroller Teensy version 3.6.
 *  This the very basic code using the library.The library was written using the best libraries I could find at the moment for each GY80 sub-module (Gyro, Accel, Magne, BMP085) and
 *  putting them together in an lightweight and easy to understand code.Dont use it with Arduino, there's a lighter version of GY80 library that doesnt need so much memory, check in my GitHub.
 *  The libraries for each sub-modules are, in majority, adapted adafruit libraries, and because of it,
 *  they are very heav.But in the counterpart, they also are very robust and have methods for everything that you need to do with the sensor.
 *  You can choose to print values to debug and test in the serial monitor.
 *  The data is printed in a CSV way, so you can copy and paste the serial monitor info into a notepad file and save as a CSV that can be opened in Excel or other CSV softwares.
 *  The structure IMU_s is given by :
 *      IMU_s->double acelerometro[3]; Where positions 0, 1 and 2 in the array are acelerometer x, y and z values respectively, in m/s².
 *      IMU_s->int magnetometro[3]; Where positions 0, 1 and 2 in the array are magnetic field x, y and z values respectively, in vector form.
 *      IMU_s->int giroscopio[3]; Where positions 0, 1 and 2 in the array are gyroscope x, y and z values respectively, in angular acceleration.
 *      IMU_s->double barometro[3]; Where positions 0, 1 and 2 in the array are pressure(in Pa), altitude(in Meters) and temperature(in Celsius) respectively.
 *  Contact : marcelomaronas at poli.ufrj.br
 *  For more codes : github.com/engmaronas
 * Conexões
 * IMU:
 * Teensy 3.6 (3.3V) ----------------> VDD (Pin 3)
 * GND ------------------------------> GND
 * SCL0 (Pin 19) --------------------> SCL
 * SDA0 (Pin 20) --------------------> SDA
 *
 * Lora:
 * Teensy 3.6 (3.3V) ----------------> Vin
 * GND ------------------------------> GND
 * Pin 16 ---------------------------> G0
 * SCK0(Pin 13) ---------------------> SCK
 * MISO0 (Pin 12) -------------------> MISO
 * MOSI0 (Pin 11) -------------------> MOSI
 * Pin 15 ---------------------------> CS
 * pin 17 ---------------------------> RTS
 *
 * GPS:
 * Teensy 3.6 (3.3V) ----------------> Vcc in
 * GND-------------------------------> GND
 * RX4 (Pin 31) ---------------------> TX
 * TX4 (Pin 32) ---------------------> RX
 *
 * https://www.pjrc.com/teensy/pinout.html
 *
 */
// https://forum.pjrc.com/threads/39158-Using-SdFat-to-acces-Teensy-3-6-SD-internal-card-(-amp-with-audio-board)


#include <pmm.h>
#include <pmmConsts.h>
#include <pmmImu.h>
#include <pmmPackageLog.h>
#include <pmmErrorsCentral.h>
#include <pmmHealthSignals.h>

#if PMM_USE_TELEMETRY
    #include <pmmTelemetry.h>
#endif

#if PMM_USE_GPS
    #include <pmmGps.h>
#endif

#if PMM_USE_SD
    #include <pmmSd.h>
#endif



void Pmm::init()
{
    #if PMM_DEBUG_SERIAL
        unsigned long serialDebugTimeout = millis();
        Serial.begin(9600);     // Initialize the debug Serial Port. The value doesn't matter, as Teensy will set it to maximum. https://forum.pjrc.com/threads/27290-Teensy-Serial-Print-vs-Arduino-Serial-Print
        Serial.println("Pmm: Serial initialized!");
        #if PMM_DEBUG_SERIAL_TIMEOUT_ENABLED
        while (!Serial && (millis() - serialDebugTimeout < PMM_DEBUG_SERIAL_TIMEOUT_MILLIS));        // wait for serial port to connect. Needed for native USB port only

        #else
        while (!Serial);

        #endif
        if (Serial)
            PMM_DEBUG_PRINT("Serial initialized!");
    #endif

    mPmmErrorsCentral.init(&mPackageLogId);

        #if PMM_USE_TELEMETRY                       /* Telemetry */
    mPmmTelemetry.init(&mPmmErrorsCentral, &mPmmPackageLog);
        #endif

        #if PMM_USE_GPS                             /* GPS */
    mPmmGps.init(&mPmmErrorsCentral);
        #endif

        #if PMM_USE_SD                              /* SD */
    mPmmSd.init(&mPmmErrorsCentral);
        #endif

    mPmmImu.init(&mPmmErrorsCentral); /* IMU */

    mPackageLogId = 0;
    mPackageTimeMs = 0;

    mPmmPackageLog.addPackageBasicInfo(&mPackageLogId, &mPackageTimeMs);
    mPmmPackageLog.addImu(mPmmImu.getImuStructPtr());
    mPmmPackageLog.addGps(mPmmGps.getGpsStructPtr());

    PMM_DEBUG_PRINT("\n =-=-=-=-=-=-=-=- PMM - Minerva Rockets - UFRJ =-=-=-=-=-=-=-=- \n");
    mPmmPackageLog.debugPrintLogHeader();
    // PMM_DEBUG_PRINT(SD_LOG_HEADER);
}

void Pmm::update()
{
    //PMM_DEBUG_PRINT_MORE("Pmm [M]: Looped!");
    //PMM_DEBUG_PRINT(i++);
    mPackageTimeMs = millis();                  // Packet time, in miliseconds. (unsigned long)

    mPmmImu.update();
    //PMM_DEBUG_PRINT_MORE("Pmm [M]: Updated Imu!");

    /* GPS */
    #if PMM_USE_GPS
        mPmmGps.update();
        //PMM_DEBUG_PRINT_MORE(Pmm [M]: Updated Gps!");
    #endif

//---------------SD Logging Code---------------//
    #if PMM_USE_SD
    #endif

//-------------- Send RF package ---------------//
    #if PMM_USE_TELEMETRY
        mPmmTelemetry.updateTransmission();
        //PMM_DEBUG_PRINT_MORE("Pmm [M]: Updated Telemetry!");
    #endif

    #if PMM_DEBUG_SERIAL
        mPmmPackageLog.debugPrintLogContent();
        Serial.println();
    #endif

    //mPmmErrorsCentral.updateLedsAndBuzzer();
    mPackageLogId ++;

    /*if (packetIDul % 100 == 0)
    {
        Serial.print("timeMsBetween 100 cycles = "); Serial.println(millis() - timePrint);
        timePrint = millis();
    }*/
}
