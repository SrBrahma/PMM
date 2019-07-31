#ifndef PMM_EXTRA_CODES_IMU_ENABLE_CALIBRATION_h
#define PMM_EXTRA_CODES_IMU_ENABLE_CALIBRATION_h

#include "pmmImu/imu.h"
#include "pmmEeprom/eeprom.h"



int MPUcalibration()
{
    int samples = 1000;
    Serial.printf("Started to calibrate Accelerometer and Gyroscope (%i samples each)...\
                   Won't take too much time. DON'T MOVE IT!\n", samples);
    PmmImu imu;
    if (imu.initMpu()) {
        Serial.printf("Couldn't initialize the MPU! Exiting the calibration.\n");
        return 1;
    }
    imu.getMPU6050Ptr()->calibrateAccelerometer(samples);
    imu.getMPU6050Ptr()->calibrateGyroscope(samples);
    Serial.printf("Successfully calibrated!\n");
    return 0;
}

int HMCCalibration()
{
    Serial.printf("Started to calibrate the Magnetometer. Keep rotating it in the Z axis for some time (30s ~ 1min),\
                   in a flat surface. When the OffX and OffY stop to change, press any key to store them on the EEPROM and exit.\n");
    
    PmmImu imu;
    int minX, maxX, minY, maxY, offX, offY;
    minX = maxX = minY = maxY = offX = offY = 0;

    if (imu.initMagnetometer())     {
        Serial.printf("Couldn't initialize the Magnetometer! Exiting the calibration.\n");
        return 1;
    }
    while (!Serial.available())     {
        Vector mag = imu.getHMC5883LPtr()->readRaw();

        // Determine Min / Max values
        if (mag.XAxis < minX) minX = mag.XAxis;
        if (mag.XAxis > maxX) maxX = mag.XAxis;
        if (mag.YAxis < minY) minY = mag.YAxis;
        if (mag.YAxis > maxY) maxY = mag.YAxis;

        // Calculate offsets
        offX = (maxX + minX) / 2;
        offY = (maxY + minY) / 2;

        Serial.printf("OffsetX = %i, OffsetY = %i. When the offsets stop to change, press any key to store them (EEPROM) and exit!\n",
                       offX, offY);
        yield();
    }
    Serial.clear(); // So we won't leave the main switch.
    PmmEeprom pmmEeprom;
    pmmEeprom.setMagnetometerCalibrationX(offX);
    pmmEeprom.setMagnetometerCalibrationY(offY);
    Serial.printf("Successfully calibrated! Stored OffsetX as %i, and OffsetY as %i.\n\n", offX, offY);
    return 0;
}

#endif