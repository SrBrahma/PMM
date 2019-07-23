/* pmmImu.cpp
 * Code for the Inertial Measure Unit (IMU!)
 *
 * By Henrique Bruno Fantauzzi de Almeida (SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <MPU6050.h>
#include <HMC5883L.h>
#include <Adafruit_BMP085_U.h>
#include <generalUnitsOps.h> // For coord32ToFloat()

#include "pmmEeprom/eeprom.h"

#include "pmmConsts.h"
#include "pmmDebug.h"

#include "pmmImu/imu.h"

//https://stackoverflow.com/a/30364704
PmmImu::PmmImu()
    : mMpu(PMM_IMU_I2C_CHANNEL), mBarometer(PMM_IMU_I2C_CHANNEL), mMagnetometer(PMM_IMU_I2C_CHANNEL) 
    {}

int  PmmImu::init()
{
    int returnValue = 0;
    if (initMpu())          returnValue |= 0b001;
    if (initBmp())          returnValue |= 0b010;
    if (initMagnetometer()) returnValue |= 0b100;
    return returnValue;
}

int  PmmImu::update()
{
    int returnValue = 0;

    returnValue |= updateMpu();
    returnValue |= updateMagnetometer();
    returnValue |= updateBmp();

    return returnValue;
}



int  PmmImu::initMpu()
{
    if (mMpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
    {
        mMpuIsWorking = false;
        advPrintf("MPU6050 initialization failed!\n")
        return 1;
    }

    // Enable bypass mode, needed to use with HMC5883L (copied from HMC5883L lib)
    mMpu.setI2CMasterModeEnabled(false);
    mMpu.setI2CBypassEnabled    (true );
    mMpu.setSleepEnabled        (false);

    mMpuIsWorking = true;
    imuDebugMorePrintf("MPU6050 initialized successfully!\n")

    return 0;
}

int PmmImu::updateMpu()
{
    if (!mMpuIsWorking)
        return 1;

    mMpu.readNormalizedAccelerometer(mPmmImuStruct.accelerometerArray);
    mMpu.readNormalizedGyroscope(mPmmImuStruct.gyroscopeArray);
    mPmmImuStruct.temperatureMpu = mMpu.readTemperature();
    imuDebugMorePrintf("Mpu updated!\n")
    return 0;
}


// Some HMC5883L are actually "QMC5883L". This creates some problems.
// https://circuitdigest.com/microcontroller-projects/digital-compass-with-arduino-and-hmc5883l-magnetometer
// https://core-electronics.com.au/attachments/QMC5883L-Datasheet-1.0.pdf
int  PmmImu::initMagnetometer()
{
    int returnVal;
    if ((returnVal = mMagnetometer.begin())) {
        mMagnetometerIsWorking = false;
        advPrintf("Magnetometer initialization failed! mMagnetometer.begin() return code is %i. As there is the QMC5883 vs HMC5883 problem, this problem shouldn't be a big problem.\n", returnVal);

        return 0;
    }

    mMagnetometerIsWorking = true;

    mMagnetometer.setSamples(HMC5883L_SAMPLES_8);
    mMagnetometer.setDataRate(HMC5883L_DATARATE_75HZ);

    PmmEeprom pmmEeprom;
    mMagnetometer.setOffset(pmmEeprom.getMagnetometerCalibrationX(), pmmEeprom.getMagnetometerCalibrationY());

    setDeclination(PMM_MAGNETIC_DECLINATION_DEGREE);

    imuDebugMorePrintf("Magnetometer initialized successfully!\n")
    return 0;
}

int  PmmImu::getDecByCoord(float* returnDeclination, float latitude, float longitude)
{
    return ::getDecByCoord(returnDeclination, latitude, longitude);
}




// Uses coordinates to get declination, using my another code.
int  PmmImu::setDeclination(int32_t latitude, int32_t longitude)
{
    return setDeclination(coord32ToFloat(latitude), coord32ToFloat(longitude));
}
int  PmmImu::setDeclination(float latitude, float longitude)
{
    float declination;
    getDecByCoord(&declination, latitude, longitude);

    mMagnetometer.setDeclination(declination);
    return 0;
}

int  PmmImu::setDeclination(float degrees)
{
    mMagnetometer.setDeclination(degrees);
    return 0;
}

float PmmImu::getDeclination()
{
    return mMagnetometer.getDeclination();
}



int  PmmImu::initBmp()  //BMP085 Setup
{
    int rtnVal;
    if ((rtnVal = mBarometer.begin(BMP085_MODE_ULTRAHIGHRES)))
    {
        mBarometerIsWorking = 0;
        advPrintf("Barometer initialization failed!\n")
        return 1;
    }
    mBarometerIsWorking = 1;
    mReferencePressure = 1013.25;
    // setReferencePressure(20);

    imuDebugMorePrintf("Barometer initialized successfully!\n")
    return 0;
}



int PmmImu::updateMagnetometer() // READ https://www.meccanismocomplesso.org/en/arduino-magnetic-magnetic-magnetometer-hmc5883l/
{
    if (!mMagnetometerIsWorking)
        return 1;

    mMagnetometer.readNormalized(mPmmImuStruct.magnetometerArray);
    imuDebugMorePrintf("Magnetometer updated!\n");
    return 0;
}



int PmmImu::updateBmp()
{
    int rtnVal = 0;
    if (!mBarometerIsWorking)
        return 1;

    // read calibrated pressure value in hecto Pascals (hPa)
    switch(mBarometer.isDataReady())
    {
        case DATA_READY_TEMPERATURE:
            mBarometer.getTemperature(&mPmmImuStruct.temperatureBmp);
            break;

        case DATA_READY_PRESSURE:
            mBarometer.getPressure(&mPmmImuStruct.pressure);
            mPmmImuStruct.altitude = mBarometer.pressureToAltitude(mReferencePressure, mPmmImuStruct.pressure);
            rtnVal |= BarGotPressure;
            break;
    }

    imuDebugMorePrintf("Barometer updated!\n")
    return rtnVal;
}



// There can be applied another methods of power saving. Future!
int PmmImu::setImuMode(ImuMode mode)
{
    switch (mode)
    {
        case ImuMode::Sleep:
            mMpu.setSleepEnabled(true);
            mMagnetometer.setMeasurementMode(HMC5883L_IDLE);
            break;

        case ImuMode::Active:
            mMpu.setSleepEnabled(false);
            mMagnetometer.setMeasurementMode(HMC5883L_CONTINOUS);
            break;
    }
    return 0;
}



void  PmmImu::getAccelerometer(float destinationArray[3]) { memcpy(destinationArray, mPmmImuStruct.accelerometerArray, 3); }
void  PmmImu::getGyroscope    (float destinationArray[3]) { memcpy(destinationArray, mPmmImuStruct.gyroscopeArray,     3); }
float PmmImu::getMpuTemperature()                         { return mPmmImuStruct.temperatureMpu;                           }
void  PmmImu::getMagnetometer (float destinationArray[3]) { memcpy(destinationArray, mPmmImuStruct.magnetometerArray,  3); }
float PmmImu::getBarometerPressure   ()                   { return mPmmImuStruct.pressure;                                 }
float PmmImu::getBarometerAltitude   ()                   { return mPmmImuStruct.altitude;                                 }
float PmmImu::getBarometerTemperature()                   { return mPmmImuStruct.temperatureBmp;                           }
pmmImuStructType PmmImu::getImuStruct()                   { return mPmmImuStruct;                                          }

float* PmmImu::getAccelerometerPtr       () { return mPmmImuStruct.accelerometerArray; }
float* PmmImu::getGyroscopePtr           () { return mPmmImuStruct.gyroscopeArray;     }
float* PmmImu::getMpuTemperaturePtr      () { return &mPmmImuStruct.temperatureMpu;    }
float* PmmImu::getMagnetometerPtr        () { return mPmmImuStruct.magnetometerArray;  }
float* PmmImu::getBarometerPressurePtr   () { return &mPmmImuStruct.pressure;          }
float* PmmImu::getBarometerAltitudePtr   () { return &mPmmImuStruct.altitude;          }
float* PmmImu::getBarometerTemperaturePtr() { return &mPmmImuStruct.temperatureBmp;    }
pmmImuStructType* PmmImu::getImuStructPtr() { return &mPmmImuStruct;                   }

MPU6050*  PmmImu::getMPU6050Ptr()       { return &mMpu;             }
HMC5883L* PmmImu::getHMC5883LPtr()      { return &mMagnetometer;    }

float PmmImu::getBearingDegree() {
    return mMagnetometer.getBearingDegree(mPmmImuStruct.magnetometerArray[0], mPmmImuStruct.magnetometerArray[1]);
}
