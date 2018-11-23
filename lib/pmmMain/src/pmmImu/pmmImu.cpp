/* pmmImu.cpp
 * Code for the Inertial Measure Unit (IMU!)
 *
 * By Henrique Bruno Fantauzzi de Almeida (SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <MPU6050.h>
#include <HMC5883L.h>
#include <BMP085.h>

#include "pmmConsts.h"
#include "pmmImu/pmmImu.h"
#include "pmmErrorsCentral/pmmErrorsCentral.h"

PmmImu::PmmImu()
{
}


int PmmImu::initMpu()
{
    if (!mMpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
    {
        mPmmErrorsCentral->reportErrorByCode(ERROR_MAGNETOMETER_INIT);
        PMM_DEBUG_PRINTLN("PmmImu #1: MPU6050 INIT FAILED");
        return 1;
    }

    PMM_DEBUG_PRINTLN_MORE("PmmImu [M]: MPU6050 initialized successfully!");
    return 0;

}



int PmmImu::initMagnetometer()
{
    if (!mMagnetometer.begin())
    {
        PMM_DEBUG_PRINTLN("PmmImu #2: MAGNETOMETER INIT ERROR");
        return 1;
    }

    PMM_DEBUG_PRINTLN_MORE("PmmImu [M]: Magnetometer initialized successfully!");
    return 0;
}



int PmmImu::initBmp()  //BMP085 Setup
{
    if (!mBarometer.begin(BMP085_ULTRA_LOW_POWER))  // This is actually beter, as we will lose less time on delay(), and we do the filtering ourselves.
                                                    // Read the datasheet.
    {
        PMM_DEBUG_PRINTLN("PmmImu #3: BAROMETER INIT ERROR");
        return 1;
    }

    mReferencePressure = mBarometer.readPressure();

    PMM_DEBUG_PRINTLN_MORE("PmmImu [M]: BMP initialized successfully!");
    return 0;
}





int PmmImu::init(PmmErrorsCentral *pmmErrorsCentral)
{
    mNextMillisBarometer = 0;
    mPmmErrorsCentral = pmmErrorsCentral;
    mMagnetometerDeclinationRad = 369.4 / 1000; // https://www.meccanismocomplesso.org/en/arduino-magnetic-magnetic-magnetometer-hmc5883l/

    int foundError = 0;

    if (initBmp())
    {
        mPmmErrorsCentral->reportErrorByCode(ERROR_BAROMETER_INIT);
        foundError = 1;
    }

    if (initMpu())
    {
        mPmmErrorsCentral->reportErrorByCode(ERROR_ACCELEROMETER_INIT);
        mPmmErrorsCentral->reportErrorByCode(ERROR_GYROSCOPE_INIT);
        foundError = 2;
    }

    if (initMagnetometer())
    {
        mPmmErrorsCentral->reportErrorByCode(ERROR_MAGNETOMETER_INIT);
        foundError = 3;
    }

    return foundError;
}





int PmmImu::updateMpu()
{
    mMpu.readNormalizedAccelerometer(mPmmImuStruct.accelerometerArray);
    mMpu.readNormalizedGyroscope(mPmmImuStruct.gyroscopeArray);
    mPmmImuStruct.temperature = mMpu.readTemperature();

    return 0;
}



int PmmImu::updateMagnetometer() // READ https://www.meccanismocomplesso.org/en/arduino-magnetic-magnetic-magnetometer-hmc5883l/
{
    mMagnetometer.readNormalized(mPmmImuStruct.magnetometerArray);

    return 0;
}



int PmmImu::updateBmp()
{
    // Add a filter later!
    
    // read calibrated pressure value in Pascals (Pa)
    mPmmImuStruct.pressure = mBarometer.readPressure();
    mPmmImuStruct.altitudePressure = mBarometer.getAltitude(mPmmImuStruct.pressure, mReferencePressure);

    return 0;
}

int PmmImu::update()
{

    if (mPmmErrorsCentral->getAccelerometerIsWorking())//accelIsWorking)
    {
        updateMpu();
        PMM_IMU_DEBUG_PRINT_MORE("PmmImu [M]: Mpu updated!");
    }
    /*
    if (mPmmErrorsCentral->getGyroscopeIsWorking())//gyroIsWorking)
        getGyroscope();
    */
    if (mPmmErrorsCentral->getMagnetometerIsWorking())//magnIsWorking)
    {
        updateMagnetometer();
        PMM_IMU_DEBUG_PRINT_MORE("PmmImu [M]: Magnetometer updated!");
    }

    if (millis() >= mNextMillisBarometer)
    {
        mNextMillisBarometer = millis() + DELAY_MS_BAROMETER;
        if (mPmmErrorsCentral->getBarometerIsWorking())//baroIsWorking)
        {
            updateBmp();
            PMM_IMU_DEBUG_PRINT_MORE("PmmImu [M]: Barometer updated!");
        }
    }
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
