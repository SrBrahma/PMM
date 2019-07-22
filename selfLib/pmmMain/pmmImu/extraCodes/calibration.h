#ifndef PMM_IMU_EXTRA_CODES_CALIBRATION_h
#define PMM_IMU_EXTRA_CODES_CALIBRATION_h

#include "pmmImu/imu.h"



int MPUcalibration()
{
    int samples = 1000;
    Serial.printf("Started to calibrate Accelerometer and Gyroscope (%i samples)... Won't take too much time. DON'T MOVE IT!\n", samples);
    PmmImu imu;
    if (imu.initMpu()) {
        Serial.printf("Couldn't initialize the MPU! Exiting the calibration.\n");
        return 1;
    imu.getMPU6050Ptr()->calibrateAccelerometer(samples);
    imu.getMPU6050Ptr()->calibrateGyroscope(samples);
    Serial.printf("Successfully calibrated!\n");
    return 0;
}

int HMCCalibration()
{
    Serial.printf("Started to calibrate the Magnetometer. Keep rotating it in the Z axis for some time (30s ~ 1min), in a flat surface. Then, press any key to exit.\n", samples);
    PmmImu imu;
    if (imu.initMagnetometer()) {
        Serial.printf("Couldn't initialize the Magnetometer! Exiting the calibration.\n");
        return 1;
    imu.getMPU6050Ptr()->calibrateAccelerometer(samples);
    imu.getMPU6050Ptr()->calibrateGyroscope(samples);
    Serial.printf("Successfully calibrated!\n");
    return 0;
}