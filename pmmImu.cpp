// GY-80 IMU Library especially written for use with Teensy 3.6
// Code written by Marcelo Maronas @ Minerva Rockets (Federal University of Rio de Janeiro Rocketry Team) - February 19, 2018
// Edited by Henrique Bruno @ Minerva Rockets - April 14, 2018
// Using Adafruit Libraries.
// Contact : marcelomaronas at poli.ufrj.br
// For more codes : github.com/engmaronas

#include <pmmImu.h>
#include <pmmConsts.h>

PmmImu::PmmImu()
{
    mBmpObject = Adafruit_BMP085_Unified(10085);
    mAccelerometerObject = Adafruit_ADXL345_Unified(12345);
    mMagnetometerObject = Adafruit_HMC5883_Unified(12346);
    mGyroscopeObject;
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
    if (!gyroscope.init())
        return 1;

    gyroscope.enableDefault();
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
    if(!bmp.begin())
        return 1;
    return 0;
}

int PmmImu::init()
{
    if (InitBMP())
    {
        baroIsWorking = 0;
        DEBUG_PRINT("BAROMETER INIT ERROR");
        pmmErrorsAndSignals.reportError(ERROR_BAROMETER_INIT, 0, sdIsWorking, rfIsWorking);
    }
    if (InitAcel())
    {
        accelIsWorking = 0;
        DEBUG_PRINT("ACCELEROMETER INIT ERROR");
        pmmErrorsAndSignals.reportError(ERROR_ACCELEROMETER_INIT, 0, sdIsWorking, rfIsWorking);
    }
    if (InitGyro())
    {
        gyroIsWorking = 0;
        DEBUG_PRINT("GYROSCOPE INIT ERROR");
        pmmErrorsAndSignals.reportError(ERROR_GYROSCOPE_INIT, 0, sdIsWorking, rfIsWorking);
    }
    if (InitMag())
    {
        magnIsWorking = 0;
        DEBUG_PRINT("MAGNETOMETER INIT ERROR");
        pmmErrorsAndSignals.reportError(ERROR_MAGNETOMETER_INIT, 0, sdIsWorking, rfIsWorking);
    }
}





int PmmImu::getGyroscope() {
    gyroscope.read();

    imu->gyroscope[0] = (float) gyroscope.g.x;
    imu->gyroscope[1] = (float) gyroscope.g.y;
    imu->gyroscope[2] = (float) gyroscope.g.z;
    return 0;
}

int PmmImu::getAccelerometer() {
    mAccelerometerObject.getEvent(&mEvent);

    imu->accelerometer[0] = mEvent.acceleration.x;
    imu->accelerometer[1] = mEvent.acceleration.y;
    imu->accelerometer[2] = mEvent.acceleration.z;
    return 0;
}

int PmmImu::getMagnetometer() {
    mMagnetometerObject.getEvent(&mEvent);

    imu->magnetometer[0] = (float) mEvent.magnetic.x;
    imu->magnetometer[1] = (float) mEvent.magnetic.y;
    imu->magnetometer[2] = (float) mEvent.magnetic.z;
    return 0;
}

int PmmImu::getBMP() {
    bmp.getEvent(&mEvent);

    barometer = (float) mEvent.pressure;
    altitude = (float) mBmpObject.pressureToAltitude(SENSORS_PRESSURE_SEALEVELHPA, mEvent.pressure);
    temperature = bmp.getTemperature();
    return 0;
}

int PmmImu::update()
{

    if (accelIsWorking)
        getAccelerometer();

    if (gyroIsWorking)
        getGyroscope();

    if (magnIsWorking)
        getMagnetometer();
        
    if (millis() >= mNextMillisBarometer)
    {
        nextMillis_barometer = millis() + DELAY_MS_BAROMETER;
        if (baroIsWorking)
            getBMP();
    }
}
