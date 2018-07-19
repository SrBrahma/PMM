/* pmmImu.cpp
 * Code for the Inertial Measure Unit (IMU!)
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <HMC5883L.h>
#include <BMP085.h>
#include <pmmConsts.h>
#include <pmmImu.h>
#include <pmmErrorsCentral.h>

PmmImu::PmmImu()
{
}
/*
int PmmImu::initAccelerometer() //ADXL45 SETUP
{
    if(!mAccelerometer.begin()) // Initialise the sensor
        return 1;
    mAccelerometer.setRange(ADXL345_RANGE_16_G); // Set the range to whatever is appropriate for your project
    return 0;
}   //ADXL45 SETUP END

int PmmImu::initGyroscope() //L2G4200D Setup
{
    if (!mGyroscopeObject.init())
        return 1;

    mGyroscopeObject.enableDefault();
    return 0;
}
*/
int PmmImu::initMpu()
{
    mMpu.initialize();

    if(mMpu.testConnection())
    {
        DEBUG_PRINT("MPU6050 init successful");
        return 0;
    }
    DEBUG_PRINT("MPU6050 init failed");
    return 1;
}

int PmmImu::initMagnetometer()
{
    if(!mMagnetometerObject.begin()) // Initialise the sensor
        return 1;
    return 0;
}

int PmmImu::initBMP()  //BMP085 Setup
{
    if(!mBmpObject.begin())
        return 1;
    return 0;
}

int PmmImu::init(PmmErrorsCentral *pmmErrorsCentral)
{
    mNextMillisBarometer = 0;
    mPmmErrorsCentral = pmmErrorsCentral;
    int foundError = 0;

    if (initBMP())
    {
        DEBUG_PRINT("BAROMETER INIT ERROR");
        mPmmErrorsCentral->reportErrorByCode(ERROR_BAROMETER_INIT);
        foundError = 1;
    }

    /* Now uses MPU6050! (Acc & gyro)
    if (initAccelerometer())
    {
        mPmmErrorsCentral->setAccelerometerIsWorking(0);
        //DEBUG_PRINT("ACCELEROMETER INIT ERROR");
        //pmmErrorsCentral.reportErrorByCode(ERROR_ACCELEROMETER_INIT);
    }
    if (initGyroscope())
    {
        mPmmErrorsCentral->setGyroscopeIsWorking(0);
        //DEBUG_PRINT("GYROSCOPE INIT ERROR");

    }*/
    if (initMpu())
    {
        DEBUG_PRINT("ACCELEROMETER INIT ERROR");
        mPmmErrorsCentral->reportErrorByCode(ERROR_ACCELEROMETER_INIT);
        mPmmErrorsCentral->reportErrorByCode(ERROR_GYROSCOPE_INIT);
        foundError = 1;
    }
    if (initMagnetometer())
    {
        DEBUG_PRINT("MAGNETOMETER INIT ERROR");
        mPmmErrorsCentral->reportErrorByCode(ERROR_MAGNETOMETER_INIT);
        foundError = 1;
    }
    return foundError? 1: 0;
}





int PmmImu::updateGyroscope()
{
    mGyroscopeObject.read();

    mPmmImuStruct.gyroscope[0] = mGyroscopeObject.g.x;
    mPmmImuStruct.gyroscope[1] = mGyroscopeObject.g.y;
    mPmmImuStruct.gyroscope[2] = mGyroscopeObject.g.z;
    return 0;
}

int PmmImu::updateAccelerometer()
{

    mPmmImuStruct.accelerometer[0] = mEvent.acceleration.x;
    mPmmImuStruct.accelerometer[1] = mEvent.acceleration.y;
    mPmmImuStruct.accelerometer[2] = mEvent.acceleration.z;
    return 0;
}

int PmmImu::updateMagnetometer()
{

    mPmmImuStruct.magnetometer[0] = (float) mEvent.magnetic.x;
    mPmmImuStruct.magnetometer[1] = (float) mEvent.magnetic.y;
    mPmmImuStruct.magnetometer[2] = (float) mEvent.magnetic.z;
    return 0;
}

int PmmImu::updateBMP()
{
    mBmpObject.getEvent(&mEvent);

    mPmmImuStruct.barometer = (float) mEvent.pressure;
    mPmmImuStruct.altitudeBarometer = (float) mBmpObject.pressureToAltitude(SENSORS_PRESSURE_SEALEVELHPA, mEvent.pressure);
    mPmmImuStruct.temperature = mBmpObject.getTemperature();
    return 0;
}

int PmmImu::update()
{
    if (mPmmErrorsCentral->getAccelerometerIsWorking())//accelIsWorking)
        getAccelerometer();

    if (mPmmErrorsCentral->getGyroscopeIsWorking())//gyroIsWorking)
        getGyroscope();

    if (mPmmErrorsCentral->getMagnetometerIsWorking())//magnIsWorking)
        getMagnetometer();

    if (millis() >= mNextMillisBarometer)
    {
        mNextMillisBarometer = millis() + DELAY_MS_BAROMETER;
        if (mPmmErrorsCentral->getBarometerIsWorking())//baroIsWorking)
            getBMP();
    }
    return 0;
}





void PmmImu::getAccelerometer(float destinationArray[3])
{
    memcpy(destinationArray, mPmmImuStruct.accelerometer, 3);
}
void PmmImu::getGyroscope(float destinationArray[3])
{
    memcpy(destinationArray, mPmmImuStruct.gyroscope, 3);
}
void PmmImu::getMagnetometer(float destinationArray[3])
{
    memcpy(destinationArray, mPmmImuStruct.magnetometer, 3);
}
float PmmImu::getBarometer()
{
    return mPmmImuStruct.barometer;
}
float PmmImu::getAltitudeBarometer()
{
    return mPmmImuStruct.altitudeBarometer;
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
    return &mPmmImuStruct.accelerometer;
}
float* PmmImu::getGyroscopePtr()
{
    return &mPmmImuStruct.gyroscope;
}
float* PmmImu::getMagnetometerPtr()
{
    return &mPmmImuStruct.magnetometer;
}
float* PmmImu::getBarometerPtr()
{
    return &mPmmImuStruct.barometer;
}
float* PmmImu::getAltitudeBarometerPtr()
{
    return &mPmmImuStruct.altitudeBarometer;
}
float* PmmImu::getTemperaturePtr()
{
    return &mPmmImuStruct.temperature;
}
pmmImuStructType* PmmImu::getImuStructPtr()
{
    return &mPmmImuStruct;
