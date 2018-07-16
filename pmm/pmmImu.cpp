// GY-80 IMU Library especially written for use with Teensy 3.6
// Code written by Marcelo Maronas @ Minerva Rockets (Federal University of Rio de Janeiro Rocketry Team) - February 19, 2018
// Edited by Henrique Bruno @ Minerva Rockets - April 14, 2018
// Using Adafruit Libraries.
// Contact : marcelomaronas at poli.ufrj.br
// For more codes : github.com/engmaronas
#include <pmmConsts.h>
#include <pmmImu.h>
#include <pmmErrorsCentral.h>
#include <MPU6050.h>
#include <HMC5883L.h>
#include <BMP085.h>
#include <Wire.h>
#include <I2Cdev.h>

PmmImu::PmmImu()
{
    mNextMillisBarometer = 0;
}

int PmmImu::initAccelerometer() //ADXL45 SETUP
{
    if(!mAccelerometer.begin()) /* Initialise the sensor */
        return 1;
    mAccelerometer.setRange(ADXL345_RANGE_16_G); /* Set the range to whatever is appropriate for your project */
    return 0;
}   //ADXL45 SETUP END

int PmmImu::initGyroscope() //L2G4200D Setup
{
    if (!mGyroscope.init())
        return 1;

    mGyroscope.enableDefault();
    return 0;
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
    mPmmErrorsCentral = pmmErrorsCentral;
    if (initBMP())
    {
        mPmmErrorsCentral->setBarometerIsWorking(0);
        //DEBUG_PRINT("BAROMETER INIT ERROR");
        //pmmErrorsCentral.reportError(ERROR_BAROMETER_INIT, 0, sdIsWorking, rfIsWorking);
    }
    if (initAccelerometer())
    {
        mPmmErrorsCentral->setAccelerometerIsWorking(0);
        //DEBUG_PRINT("ACCELEROMETER INIT ERROR");
        //pmmErrorsCentral.reportError(ERROR_ACCELEROMETER_INIT, 0, sdIsWorking, rfIsWorking);
    }
    if (initGyroscope())
    {
        mPmmErrorsCentral->setGyroscopeIsWorking(0);
        //DEBUG_PRINT("GYROSCOPE INIT ERROR");
        //pmmErrorsCentral.reportError(ERROR_GYROSCOPE_INIT, 0, sdIsWorking, rfIsWorking);
    }
    if (initMagnetometer())
    {
        mPmmErrorsCentral->setMagnetometerIsWorking(0);
        //DEBUG_PRINT("MAGNETOMETER INIT ERROR");
        //pmmErrorsCentral.reportError(ERROR_MAGNETOMETER_INIT, 0, sdIsWorking, rfIsWorking);
    }
    return 0;
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
    mAccelerometer.getEvent(&mEvent);

    mPmmImuStruct.accelerometer[0] = mEvent.acceleration.x;
    mPmmImuStruct.accelerometer[1] = mEvent.acceleration.y;
    mPmmImuStruct.accelerometer[2] = mEvent.acceleration.z;
    return 0;
}

int PmmImu::updateMagnetometer()
{
    mMagnetometer.getEvent(&mEvent);

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
