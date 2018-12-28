/* pmmImu.cpp
 * Code for the Inertial Measure Unit (IMU!)
 *
 * By Henrique Bruno Fantauzzi de Almeida (SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <MPU6050.h>
#include <HMC5883L.h>
#include <Adafruit_BMP085_U.h>

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
        advPrintf("MPU6050 initialization failed!\n");
        return 1;
    }

    // Enable bypass mode, needed to use with HMC5883L (copied from HMC5883L lib)
    mMpu.setI2CMasterModeEnabled(false);
    mMpu.setI2CBypassEnabled    (true );
    mMpu.setSleepEnabled        (false);

    mMpuIsWorking = 1;
    debugMorePrintf("MPU6050 initialized successfully!\n");

    return 0;
}



int PmmImu::initMagnetometer()
{
    if (!mMagnetometer.begin())
    {
        mMagnetometerIsWorking = 0;
        advPrintf("Magnetometer initialization failed!\n");
        return 1;
    }

    mMagnetometer.setSamples(HMC5883L_SAMPLES_8);
    mMagnetometer.setDataRate(HMC5883L_DATARATE_75HZ);

    mMagnetometerIsWorking = 1;
    debugMorePrintf("Magnetometer initialized successfully!\n");

    return 0;
}



int PmmImu::initBmp()  //BMP085 Setup
{
    if (mBarometer.begin(BMP085_MODE_ULTRAHIGHRES))  
    {
        mBarometerIsWorking = 0;
        advPrintf("Barometer initialization failed!\n");
        return 1;
    }

    setReferencePressure();

    mBarometerIsWorking = 1;

    debugMorePrintf("Barometer initialized successfully!\n");
    return 0;
}


// Deppending on the number of measures, this may take a little while.
int PmmImu::setReferencePressure(unsigned samples)
{
    float sumPressure = 0;
    float pressure;

    for (unsigned counter = 0; counter < samples; counter ++)
    {
        unsigned counter2 = 0;

        while (mBarometer.isDataReady() != DATA_READY_PRESSURE)
        {
            if (counter2++ > 100)
            {
                advPrintf("No pressure was obtained after various attempts.\n")
                return 1;
            }
            delay(5);
        }

        mBarometer.getPressure(&pressure);
        sumPressure += pressure;

    }
    mReferencePressure = sumPressure / samples;

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
        mPmmImuStruct.mpuTemperature = mMpu.readTemperature();
        imuDebugMorePrintf("Mpu updated!\n")
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
        imuDebugMorePrintf("Magnetometer updated!\n");
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

        switch(mBarometer.isDataReady())
        {
            
            case DATA_READY_TEMPERATURE:
                mBarometer.getTemperature(&mPmmImuStruct.barometerTemperature);
                break;

            case DATA_READY_PRESSURE:
                mBarometer.getPressure(&mPmmImuStruct.pressure);

                mPmmImuStruct.altitudePressure = mBarometer.pressureToAltitude(mReferencePressure, mPmmImuStruct.pressure);
                break;
        }

        imuDebugMorePrintf("Barometer updated!\n")
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



int PmmImu::setSystemMode(pmmSystemState systemMode)
{

    switch (systemMode)
    {
        case MODE_SLEEP:
            mMpu.setSleepEnabled(true);
            mMagnetometer.setMeasurementMode(HMC5883L_IDLE);
            break;

        case MODE_READY:
            mMpu.setSleepEnabled(false);
            mMagnetometer.setMeasurementMode(HMC5883L_CONTINOUS);
            break;

        case MODE_DEPLOYED:
            mMpu.setSleepEnabled(false);
            mMagnetometer.setMeasurementMode(HMC5883L_CONTINOUS);
            break;

        case MODE_FINISHED:
            mMpu.setSleepEnabled(true);
            mMagnetometer.setMeasurementMode(HMC5883L_IDLE);
            break;
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
float PmmImu::getMpuTemperature()
{
    return mPmmImuStruct.mpuTemperature;
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
float PmmImu::getBarometerTemperature()
{
    return mPmmImuStruct.barometerTemperature;
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
float* PmmImu::getMpuTemperaturePtr()
{
    return &mPmmImuStruct.mpuTemperature;
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
float* PmmImu::getBarometerTemperaturePtr()
{
    return &mPmmImuStruct.barometerTemperature;
}
pmmImuStructType* PmmImu::getImuStructPtr()
{
    return &mPmmImuStruct;
}
