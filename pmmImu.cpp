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





int PmmImu::getGyroscope()
{
    mGyroscopeObject.read();

    gyroscope[0] = mGyroscopeObject.g.x;
    gyroscope[1] = mGyroscopeObject.g.y;
    gyroscope[2] = mGyroscopeObject.g.z;
    return 0;
}

int PmmImu::getAccelerometer()
{
    mAccelerometerObject.getEvent(&mEvent);

    accelerometer[0] = mEvent.acceleration.x;
    accelerometer[1] = mEvent.acceleration.y;
    accelerometer[2] = mEvent.acceleration.z;
    return 0;
}

int PmmImu::getMagnetometer()
{
    mMagnetometerObject.getEvent(&mEvent);

    magnetometer[0] = (float) mEvent.magnetic.x;
    magnetometer[1] = (float) mEvent.magnetic.y;
    magnetometer[2] = (float) mEvent.magnetic.z;
    return 0;
}

int PmmImu::getBMP()
{
    mBmpObject.getEvent(&mEvent);

    barometer[0] = (float) mEvent.pressure;
    barometer[1] = (float) mBmpObject.pressureToAltitude(SENSORS_PRESSURE_SEALEVELHPA, mEvent.pressure);
    temperature = mBmpObject.getTemperature();
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
