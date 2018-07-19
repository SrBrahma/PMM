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
    if (!mGyroscope.init())
        return 1;

    mGyroscope.enableDefault();
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
    if(!mMagnetometer.begin()) // Initialise the sensor
        return 1;
    return 0;
}

int PmmImu::initBMP()  //BMP085 Setup
{
    mBarometer.initialize();
    if(!mBarometer.testConnection())
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
    mGyroscope.read();

    mPmmImuStruct.gyroscope[0] = mGyroscope.g.x;
    mPmmImuStruct.gyroscope[1] = mGyroscope.g.y;
    mPmmImuStruct.gyroscope[2] = mGyroscope.g.z;
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
    mBarometer.setControl(BMP085_MODE_TEMPERATURE);

    // read calibrated temperature value in degrees Celsius
    mPmmImuStruct.temperature = mBarometer.getTemperatureC();

    // request pressure (3x oversampling mode, high detail, 23.5ms delay)
    mBarometer.setControl(BMP085_MODE_PRESSURE_3);

    // read calibrated pressure value in Pascals (Pa)
    mPmmImuStruct.barometer = mBarometer.getPressure();

    // calculate absolute altitude in meters based on known pressure
    // (may pass a second "sea level pressure" parameter here,
    // otherwise uses the standard value of 101325 Pa)
    mPmmImuStruct.altitudeBarometer = mBarometer.getAltitude(pressure);
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
    return mPmmImuStruct.mBarometer;
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
    return &mPmmImuStruct.mBarometer;
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
