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
 *  * Caso queiria usar outra entrada RX/TX consultar pinagem em: https://www.pjrc.com/teensy/pinout.html
 * E trocar Serial2 pelo numero do Serial a utilizar
 */
// https://forum.pjrc.com/threads/39158-Using-SdFat-to-acces-Teensy-3-6-SD-internal-card-(-amp-with-audio-board)
//---------------Inclusão de bibliotecas---------------//
#include <pmm.h>
#include <pmmConsts.h>
#include <pmmImu.h>
#include <pmmPackage.h>
#include <pmmErrorsAndSignals.h>

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
    /* Errors and Signals */
    PmmErrorsAndSignals pmmErrorsAndSignals;
    pmmErrorsAndSignals.init();

    /* Telemetry */
    #if PMM_USE_TELEMETRY
        PmmTelemetry pmmTelemetry;
        pmmTelemetry.init();
    #endif

    /* GPS */
    #if PMM_USE_GPS
        PmmGps pmmGps;
        Gps_structType gps_struct;
    #endif

    /* SD */
    #if PMM_USE_SD
        PmmSd pmmSd;
        pmmSd.init();
    #endif

    /* IMU */
    PmmImu pmmImu;
    pmmImu.init();

    mPackageId = 0;
    mPackageTimeMs = 0;
    // An array of pointers. 17 variables of 4 bytes.
    pmmPackage.addPackageBasicInfo(&mPackageId, &mPackageTimeMs);
    pmmPackage.addMagnetometer(&pmmImu.magnetometer);
    pmmPackage.addGyroscope(&pmmImu.gyroscope);
    pmmPackage.addAccelerometer(&pmmImu.accelerometer);
    pmmPackage.addBarometer(&pmmImu.barometer);
    pmmPackage.addTemperature(&pmmImu.temperature);
    pmmPackage.addGps(
    uint8_t *rf_radioPacket[RF_BYTES_IN_PACKET] =
    {
        (uint8_t*) & RF_VALIDATION_HEADER,
        (uint8_t*) & packetIDfloat,
        (uint8_t*) & packetTimeFloatS,
        (uint8_t*) & gps_struct.latitude,
        (uint8_t*) & gps_struct.longitude,
        (uint8_t*) & gps_struct.altitude,
        (uint8_t*) & gps_struct.horizontalSpeed,
        (uint8_t*) & gps_struct.speedNorth,
        (uint8_t*) & gps_struct.speedEast,
        (uint8_t*) & gps_struct.speedUp,
        (uint8_t*) & gps_struct.headingDegree,
        (uint8_t*) & gps_struct.satellites,
        (uint8_t*) & imu_struct.barometro[0], // pressure
        (uint8_t*) & imu_struct.barometro[1], // altitude
        (uint8_t*) & imu_struct.barometro[2], // temperature
        (uint8_t*) & imu_struct.acelerometro[0],
        (uint8_t*) & imu_struct.acelerometro[1],
        (uint8_t*) & imu_struct.acelerometro[2],
        (uint8_t*) & imu_struct.giroscopio[0],
        (uint8_t*) & imu_struct.giroscopio[1],
        (uint8_t*) & imu_struct.giroscopio[2],
        (uint8_t*) & imu_struct.magnetometro[0],
        (uint8_t*) & imu_struct.magnetometro[1],
        (uint8_t*) & imu_struct.magnetometro[2]
    };
    #if PMM_SERIAL_DEBUG
        Serial.begin(250000); //Initialize Serial Port at 9600 baudrate.
        while (!Serial); // wait for serial port to connect. Needed for native USB port only
    #endif

    #if PMM_USE_SD
        pmmSd.init();
    #endif

// SETUP SD END //
    pmmErrorsAndSignals.init(&rf95, pmmSd.getFileId());

// ---- GPS
    #if PMM_USE_GPS
        pmmGps.init();
    #endif

//---------------Setup LORA---------------//
    #if PMM_USE_TELEMETRY
        pmmTelemetry.init();
    #endif

    pmmImu.init();
//---------------Setup Modulo SD---------------//
    DEBUG_PRINT("\nMinerva Rockets - UFRJ");
    DEBUG_PRINT(SD_LOG_HEADER);

//END of Setup  ---------------------------------------------------------------------------------------------------------//
}

void Pmm::update()
{
    packetTimeMs = millis();                  // Packet time, in miliseconds. (unsigned long)

    pmmImu.update();

    DEBUG_MAINLOOP_PRINT(1);

    //---------------GPS Venus---------------//

    #if PMM_USE_GPS
        pmmGps.update(&gps_struct);
    #endif

    DEBUG_MAINLOOP_PRINT(7);
//---------------Code for serial debugging---------------//
    #if PMM_SERIAL_DEBUG
        Serial.print(logString);
    #endif

//---------------SD Logging Code---------------//
    #if PMM_USE_SD
    #endif
    DEBUG_MAINLOOP_PRINT(9);

//-------------- Send RF package ---------------//
    #if PMM_USE_TELEMETRY
        pmmTelemetry.updateTransmission();
    #endif

    DEBUG_MAINLOOP_PRINT(10);

    pmmErrorsAndSignals.updateLedsAndBuzzer();
    packageID ++;

    /*if (packetIDul % 100 == 0)
    {
        Serial.print("timeMsBetween 100 cycles = "); Serial.println(millis() - timePrint);
        timePrint = millis();
    }*/
}
