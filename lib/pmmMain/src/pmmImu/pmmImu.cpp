/* pmmImu.cpp
 * Code for the Inertial Measure Unit (IMU!)
 *
 * By Henrique Bruno Fantauzzi de Almeida (SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <MPU6050.h>
#include <HMC5883L.h>
#include <BMP085.h>

#include "pmmConsts.h"
#include "pmmDebug.h"
#include "pmmImu/pmmImu.h"


PmmImu::PmmImu()
{
}


int PmmImu::initMpu()
{
    if (!mMpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
    {   
        mMpuIsWorking = 0;
        PMM_DEBUG_PRINTLN("PmmImu #1: MPU6050 INIT FAILED");
        return 1;
    }

    mMpuIsWorking = 1;
    PMM_DEBUG_PRINTLN_MORE("PmmImu [M]: MPU6050 initialized successfully!");

    return 0;
}



int PmmImu::initMagnetometer()
{
    if (!mMagnetometer.begin())
    {
        mMagnetometerIsWorking = 0;
        PMM_DEBUG_PRINTLN("PmmImu #2: MAGNETOMETER INIT ERROR");
        return 1;
    }

    mMagnetometerIsWorking = 1;
    PMM_DEBUG_PRINTLN_MORE("PmmImu [M]: Magnetometer initialized successfully!");

    return 0;
}



int PmmImu::initBmp()  //BMP085 Setup
{
    // This is ultra low power is actually better, as we will lose less time on delay(), and we do the filtering ourselves. // Read the datasheet.
    if (!mBarometer.begin(BMP085_ULTRA_LOW_POWER))  
    {
        mBarometerIsWorking = 0;
        PMM_DEBUG_PRINTLN("PmmImu #3: BAROMETER INIT ERROR");
        return 1;
    }

    mReferencePressure = mBarometer.readPressure();
    mBarometerIsWorking = 1;
    PMM_DEBUG_PRINTLN_MORE("PmmImu [M]: BMP initialized successfully!");
    return 0;
}





int PmmImu::init()
{
    
    mMagnetometerDeclinationRad = 369.4 / 1000; // https://www.meccanismocomplesso.org/en/arduino-magnetic-magnetic-magnetometer-hmc5883l/

    int foundError = 0;

    if (initBmp())  // later will use macros
    {
        foundError |= 0b001;
    }

    if (initMpu())
    {
        foundError |= 0b010;
    }

    if (initMagnetometer())
    {
        foundError |= 0b100;
    }

    return foundError;
}





int PmmImu::updateMpu()
{
    if (mMpuIsWorking)
    {
        mMpu.readNormalizedAccelerometer(mPmmImuStruct.accelerometerArray);
        mMpu.readNormalizedGyroscope(mPmmImuStruct.gyroscopeArray);
        mPmmImuStruct.temperature = mMpu.readTemperature();
        PMM_IMU_DEBUG_PRINT_MORE("PmmImu [M]: Mpu updated!");
        return 0;
    }
    else
        return 1;
}



int PmmImu::updateMagnetometer() // READ https://www.meccanismocomplesso.org/en/arduino-magnetic-magnetic-magnetometer-hmc5883l/
{
    if (mMagnetometerIsWorking)
    {
        mMagnetometer.readNormalized(mPmmImuStruct.magnetometerArray);
        PMM_IMU_DEBUG_PRINT_MORE("PmmImu [M]: Magnetometer updated!");
        return 0;
    }
    else
        return 1;
}



int PmmImu::updateBmp()
{
    // Add a filter later!
    // read calibrated pressure value in Pascals (Pa)
    if (mBarometerIsWorking)
    {
        mPmmImuStruct.pressure = mBarometer.readPressure();
        mPmmImuStruct.altitudePressure = mBarometer.getAltitude(mPmmImuStruct.pressure, mReferencePressure);
        PMM_IMU_DEBUG_PRINT_MORE("PmmImu [M]: Barometer updated!");
        return 0;
    }
    else
        return 1;
}

int PmmImu::update()
{
    updateMpu();

    updateMagnetometer();

    updateBmp();

    return 0;
}





void PmmImu::getAccelerometer(float destinationArray[3])
{
    memcpy(destinationArray, mPmmImuStruct.accelerometerArray, 3);
}
void PmmImu::getGyroscope(float destinationArray[3])
{
    memcpy(destinationArray, mPmmImuStruct.gyroscopeArray, 3);
}
void PmmImu::getMagnetometer(float destinationArray[3])
{
    memcpy(destinationArray, mPmmImuStruct.magnetometerArray, 3);
}
float PmmImu::getBarometer()
{
    return mPmmImuStruct.pressure;
}
float PmmImu::getAltitudeBarometer()
{
    return mPmmImuStruct.altitudePressure;
}
float PmmImu::getTemperature()
{
    return mPmmImuStruct.temperature;
}
pmmImuStructType PmmImu::getImuStruct()
{
    return mPmmImuStruct;
}



float* PmmImu::getAccelerometerPtr()
{
    return mPmmImuStruct.accelerometerArray;
}
float* PmmImu::getGyroscopePtr()
{
    return mPmmImuStruct.gyroscopeArray;
}
float* PmmImu::getMagnetometerPtr()
{
    return mPmmImuStruct.magnetometerArray;
}
float* PmmImu::getBarometerPtr()
{
    return &mPmmImuStruct.pressure;
}
float* PmmImu::getAltitudeBarometerPtr()
{
    return &mPmmImuStruct.altitudePressure;
}
float* PmmImu::getTemperaturePtr()
{
    return &mPmmImuStruct.temperature;
}
pmmImuStructType* PmmImu::getImuStructPtr()
{
    return &mPmmImuStruct;
}
