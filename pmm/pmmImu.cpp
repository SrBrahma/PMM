// GY-80 IMU Library especially written for use with Teensy 3.6
// Code written by Marcelo Maronas @ Minerva Rockets (Federal University of Rio de Janeiro Rocketry Team) - February 19, 2018
// Edited by Henrique Bruno @ Minerva Rockets - April 14, 2018
// Using Adafruit Libraries.
// Contact : marcelomaronas at poli.ufrj.br
// For more codes : github.com/engmaronas
#include <pmmConsts.h>
#include <pmmImu.h>
#include <pmmErrorsAndSignals.h>

PmmImu::PmmImu()
{
    mBmpObject = Adafruit_BMP085_Unified(10085);
    mAccelerometerObject = Adafruit_ADXL345_Unified(12345);
    mMagnetometerObject = Adafruit_HMC5883_Unified(12346);
    mNextMillisBarometer = 0;
}

int PmmImu::initAccelerometer() //ADXL45 SETUP
{
    if(!mAccelerometerObject.begin()) /* Initialise the sensor */
        return 1;
    mAccelerometerObject.setRange(ADXL345_RANGE_16_G); /* Set the range to whatever is appropriate for your project */
    return 0;
}   //ADXL45 SETUP END

int PmmImu::initGyroscope() //L2G4200D Setup
{
    if (!mGyroscopeObject.init())
        return 1;

    mGyroscopeObject.enableDefault();
    return 0;
}

int PmmImu::initMagnetometer() //HMC5884 Setup
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

int PmmImu::init(PmmErrorsAndSignals *pmmErrorsAndSignals)
{
    mPmmErrorsAndSignals = pmmErrorsAndSignals;
    if (initBMP())
    {
        mPmmErrorsAndSignals->setBarometerIsWorking(0);
        //DEBUG_PRINT("BAROMETER INIT ERROR");
        //pmmErrorsAndSignals.reportError(ERROR_BAROMETER_INIT, 0, sdIsWorking, rfIsWorking);
    }
    if (initAccelerometer())
    {
        mPmmErrorsAndSignals->setAccelerometerIsWorking(0);
        //DEBUG_PRINT("ACCELEROMETER INIT ERROR");
        //pmmErrorsAndSignals.reportError(ERROR_ACCELEROMETER_INIT, 0, sdIsWorking, rfIsWorking);
    }
    if (initGyroscope())
    {
        mPmmErrorsAndSignals->setGyroscopeIsWorking(0);
        //DEBUG_PRINT("GYROSCOPE INIT ERROR");
        //pmmErrorsAndSignals.reportError(ERROR_GYROSCOPE_INIT, 0, sdIsWorking, rfIsWorking);
    }
    if (initMagnetometer())
    {
        mPmmErrorsAndSignals->setMagnetometerIsWorking(0);
        //DEBUG_PRINT("MAGNETOMETER INIT ERROR");
        //pmmErrorsAndSignals.reportError(ERROR_MAGNETOMETER_INIT, 0, sdIsWorking, rfIsWorking);
    }
    return 0;
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
    mAccelerometerObject.getEvent(&mEvent);

    mPmmImuStruct.accelerometer[0] = mEvent.acceleration.x;
    mPmmImuStruct.accelerometer[1] = mEvent.acceleration.y;
    mPmmImuStruct.accelerometer[2] = mEvent.acceleration.z;
    return 0;
}

int PmmImu::updateMagnetometer()
{
    mMagnetometerObject.getEvent(&mEvent);

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
    if (mPmmErrorsAndSignals->getAccelerometerIsWorking())//accelIsWorking)
        getAccelerometer();

    if (mPmmErrorsAndSignals->getGyroscopeIsWorking())//gyroIsWorking)
        getGyroscope();

    if (mPmmErrorsAndSignals->getMagnetometerIsWorking())//magnIsWorking)
        getMagnetometer();

    if (millis() >= mNextMillisBarometer)
    {
        mNextMillisBarometer = millis() + DELAY_MS_BAROMETER;
        if (mPmmErrorsAndSignals->getBarometerIsWorking())//baroIsWorking)
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
