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

int PmmImu::updateScales() // https://electronics.stackexchange.com/a/176705
// The I2CDdev lib says the sensitivity of the accelerometer is 8k for the 2g, but it's wrong!
// It's actually 16k, as 16k * 2(g) = 32k, the half of 64k (2^16, the register size), as it is signed.
{
    switch(mMpu.getFullScaleAccelRange())
    {
        case MPU6050_ACCEL_FS_2:
            mAccelerometerScale = 16384;
            break;
        case MPU6050_ACCEL_FS_4:
            mAccelerometerScale = 8192;
            break;
        case MPU6050_ACCEL_FS_8:
            mAccelerometerScale = 4096;
            break;
        case MPU6050_ACCEL_FS_16:
            mAccelerometerScale = 2048;
            break;
    }

    switch(mMpu.getFullScaleGyroRange()) // Values are the division of 32768 by the gyro resolution
    {
        case MPU6050_GYRO_FS_250:
            mGyroscopeScale = 131.072;
            break;
        case MPU6050_GYRO_FS_500:
            mGyroscopeScale = 65.536;
            break;
        case MPU6050_GYRO_FS_1000:
            mGyroscopeScale = 32.768;
            break;
        case MPU6050_GYRO_FS_2000:
            mGyroscopeScale = 16.384;
            break;
    }

    switch(mMagnetometer.getGain()) // https://github.com/pganssle/HMC5883L/blob/master/HMC5883L.cpp maybe I should use better libs...
    {
        case HMC5883L_GAIN_1370:
            mMagnetometerScale = 0.73;
            break;
        case HMC5883L_GAIN_1090:
            mMagnetometerScale = 0.92;
            break;
        case HMC5883L_GAIN_820:
            mMagnetometerScale = 1.22;
            break;
        case HMC5883L_GAIN_660:
            mMagnetometerScale = 1.52;
            break;
        case HMC5883L_GAIN_440:
            mMagnetometerScale = 2.27;
            break;
        case HMC5883L_GAIN_390:
            mMagnetometerScale = 2.56;
            break;
        case HMC5883L_GAIN_330:
            mMagnetometerScale = 3.03;
            break;
        case HMC5883L_GAIN_220:
            mMagnetometerScale = 4.35;
            break;
    }
    return 0;
}



int PmmImu::initMpu()
{
    mMpu.initialize();

    if(mMpu.testConnection())
    {
        PMM_DEBUG_PRINT("PmmImu #1: MPU6050 INIT FAILED");
        return 1;
    }
    /*
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip*/
    PMM_DEBUG_PRINT_MORE("PmmImu [M]: MPU6050 initialized successfully!");
    return 0;

}



int PmmImu::initMagnetometer()
{
    mMagnetometer.initialize();

    if (mMagnetometer.testConnection())
    {
        PMM_DEBUG_PRINT("PmmImu #2: MAGNETOMETER INIT ERROR");
        return 1;
    }
    //mMagnetometer.setMode(HMC5883L_MODE_CONTINUOUS); // works without.
    PMM_DEBUG_PRINT_MORE("PmmImu [M]: Magnetometer initialized successfully!");
    return 0;
}



int PmmImu::initBmp()  //BMP085 Setup
{
    mBarometer.initialize();
    if(!mBarometer.testConnection())
    {
        PMM_DEBUG_PRINT("PmmImu #3: BAROMETER INIT ERROR");
        return 1;
    }
    PMM_DEBUG_PRINT_MORE("PmmImu [M]: BMP initialized successfully!");
    return 0;
}



int PmmImu::init(PmmErrorsCentral *pmmErrorsCentral)
{
    mNextMillisBarometer = 0;
    mPmmErrorsCentral = pmmErrorsCentral;
    mMagnetometerDeclinationRad = 369.4/1000; // https://www.meccanismocomplesso.org/en/arduino-magnetic-magnetic-magnetometer-hmc5883l/

    Wire.begin(); // as seen in the sensors example!

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
        foundError = 1;
    }

    if (initMagnetometer())
    {
        mPmmErrorsCentral->reportErrorByCode(ERROR_MAGNETOMETER_INIT);
        foundError = 1;
    }

    updateScales();

    return foundError? 1: 0;
}



int PmmImu::updateMpu()
{
    mMpu.getMotion6(&mAccelerometerRaw[0], &mAccelerometerRaw[1], &mAccelerometerRaw[2], &mGyroscopeRaw[0], &mGyroscopeRaw[1], &mGyroscopeRaw[2]);

    mPmmImuStruct.accelerometerArray[0] = mAccelerometerRaw[0] / mAccelerometerScale;
    mPmmImuStruct.accelerometerArray[1] = mAccelerometerRaw[1] / mAccelerometerScale;
    mPmmImuStruct.accelerometerArray[2] = mAccelerometerRaw[2] / mAccelerometerScale;

    mPmmImuStruct.gyroscopeArray[0] = mGyroscopeRaw[0] / mGyroscopeScale;
    mPmmImuStruct.gyroscopeArray[1] = mGyroscopeRaw[1] / mGyroscopeScale;
    mPmmImuStruct.gyroscopeArray[2] = mGyroscopeRaw[2] / mGyroscopeScale;

    return 0;
}



int PmmImu::updateMagnetometer() // READ https://www.meccanismocomplesso.org/en/arduino-magnetic-magnetic-magnetometer-hmc5883l/
{
    mMagnetometer.getHeading(&mMagnetometerRaw[0], &mMagnetometerRaw[1], &mMagnetometerRaw[2]);

    mPmmImuStruct.magnetometerArray[0] = mMagnetometerRaw[0] * mMagnetometerScale;
    mPmmImuStruct.magnetometerArray[1] = mMagnetometerRaw[1] * mMagnetometerScale;
    mPmmImuStruct.magnetometerArray[2] = mMagnetometerRaw[2] * mMagnetometerScale;

    mPmmImuStruct.headingRadian = atan2(mMagnetometerRaw[1], mMagnetometerRaw[0]); // argument is (mY, mX).

    mPmmImuStruct.headingRadian += mMagnetometerDeclinationRad; // Adds the declination

    if(mPmmImuStruct.headingRadian < 0)
        mPmmImuStruct.headingRadian += 2 * M_PI;
    mPmmImuStruct.headingDegree = mPmmImuStruct.headingRadian * 180/M_PI;
    return 0;
}



int PmmImu::updateBmp()
{
    mBarometer.setControl(BMP085_MODE_TEMPERATURE);

    // read calibrated temperature value in degrees Celsius
    mPmmImuStruct.temperature = mBarometer.getTemperatureC();

    // request pressure (3x oversampling mode, high detail, 23.5ms delay)
    mBarometer.setControl(BMP085_MODE_PRESSURE_3);

    // read calibrated pressure value in Pascals (Pa)
    mPmmImuStruct.pressure = mBarometer.getPressure();

    // calculate absolute altitude in meters based on known pressure
    // (may pass a second "sea level pressure" parameter here,
    // otherwise uses the standard value of 101325 Pa)
    mPmmImuStruct.altitudePressure = mBarometer.getAltitude(mPmmImuStruct.pressure);
    return 0;
}

int PmmImu::update()
{

    if (mPmmErrorsCentral->getAccelerometerIsWorking())//accelIsWorking)
    {
        updateMpu();
        #if PMM_DEBUG_PRINT_IMU_MORE
            PMM_DEBUG_PRINT_MORE("PmmImu [M]: Mpu updated!");
        #endif
    }
    /*
    if (mPmmErrorsCentral->getGyroscopeIsWorking())//gyroIsWorking)
        getGyroscope();
    */
    if (mPmmErrorsCentral->getMagnetometerIsWorking())//magnIsWorking)
    {
        updateMagnetometer();
        #if PMM_DEBUG_PRINT_IMU_MORE
            PMM_DEBUG_PRINT_MORE("PmmImu [M]: Magnetometer updated!");
        #endif
    }

    if (millis() >= mNextMillisBarometer)
    {
        mNextMillisBarometer = millis() + DELAY_MS_BAROMETER;
        if (mPmmErrorsCentral->getBarometerIsWorking())//baroIsWorking)
            updateBmp();
            #if PMM_DEBUG_PRINT_IMU_MORE
                PMM_DEBUG_PRINT_MORE("PmmImu [M]: Barometer updated!");
            #endif
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
