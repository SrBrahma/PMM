/* pmmImu.cpp
 * Code for the Inertial Measure Unit (IMU!)
 *
 * By Henrique Bruno Fantauzzi de Almeida (SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <MPU6050.h>
#include <HMC5883L.h>
#include <Adafruit_BMP085_U.h>
#include <SimpleKalmanFilter.h>
#include <Plotter.h>

#include "pmmConsts.h"
#include "pmmDebug.h"
#include "pmmImu/pmmImu.h"


PmmImu::PmmImu()
    : mAltitudeKalmanFilter (5, 5, 0.035),
      mAltitudeKalmanFilter2(5, 5, 0.035)
{
}


int PmmImu::initMpu()
{
    if (!mMpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
    {   
        mMpuIsWorking = 0;
        advPrintf("MPU6050 initialization failed!\n")
        return 1;
    }

    // Enable bypass mode, needed to use with HMC5883L (copied from HMC5883L lib)
    mMpu.setI2CMasterModeEnabled(false);
    mMpu.setI2CBypassEnabled    (true );
    mMpu.setSleepEnabled        (false);

    mMpuIsWorking = 1;
    imuDebugMorePrintf("MPU6050 initialized successfully!\n")

    return 0;
}



int PmmImu::initMagnetometer()
{
    if (!mMagnetometer.begin())
    {
        mMagnetometerIsWorking = 0;
        advPrintf("Magnetometer initialization failed!\n")
        return 1;
    }

    mMagnetometer.setSamples(HMC5883L_SAMPLES_8);
    mMagnetometer.setDataRate(HMC5883L_DATARATE_75HZ);

    mMagnetometerIsWorking = 1;
    imuDebugMorePrintf("Magnetometer initialized successfully!\n")

    return 0;
}



int PmmImu::initBmp()  //BMP085 Setup
{
    if (mBarometer.begin(BMP085_MODE_ULTRAHIGHRES))  
    {
        mBarometerIsWorking = 0;
        advPrintf("Barometer initialization failed!\n")
        return 1;
    }

    setReferencePressure(20);

    mBarometerIsWorking = 1;

    imuDebugMorePrintf("Barometer initialized successfully!\n")
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
    mPmmImuStruct.filteredAltitude = 0;

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

    //mPlotter.Begin(); // start plotter
  
    //mPlotter.AddTimeGraph("PMM altitude", 1000, "rawAltitude(m)", mPmmImuStruct.altitude, "semiFiltered(m)", mFiltered2, "filteredAltitude(m)", mPmmImuStruct.filteredAltitude);
    //mPlotter.SetColor(0, "red", "blue", "yellow");
    mFilteredAltitudeLastMillis = millis();
    return foundError;
}





int PmmImu::updateMpu()
{
    if (mMpuIsWorking)
    {
        mMpu.readNormalizedAccelerometer(mPmmImuStruct.accelerometerArray);
        mMpu.readNormalizedGyroscope(mPmmImuStruct.gyroscopeArray);
        mPmmImuStruct.temperatureMpu = mMpu.readTemperature();
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
    // read calibrated pressure value in hecto Pascals (hPa)
    if (mBarometerIsWorking)
    {

        switch(mBarometer.isDataReady())
        {
            
            case DATA_READY_TEMPERATURE:
                mBarometer.getTemperature(&mPmmImuStruct.temperatureBmp);
                break;

            case DATA_READY_PRESSURE:
                mBarometer.getPressure(&mPmmImuStruct.pressure);

                mPmmImuStruct.altitude = mBarometer.pressureToAltitude(mReferencePressure, mPmmImuStruct.pressure);

                /* Simulation
                mPmmImuStruct.altitude = (3000 * sin(2 * PI * (millis() - 2000) / 30000.0 )) + random(-10, 10);
                if (random(100) > 98)
                    mPmmImuStruct.altitude += random(-10000, 10000);
                if (random(100) > 95)
                    mPmmImuStruct.altitude += random(-1000, 1000);
                */
               
                // Avoid any big spikes before filtering
                if (mPmmImuStruct.altitude > mPmmImuStruct.filteredAltitude)
                {
                    if ((mPmmImuStruct.altitude - mPmmImuStruct.filteredAltitude) / (pow(millis() - mFilteredAltitudeLastMillis, 2) / 1000.0) < PMM_MAX_UP_ACCELERATION_M_S_2)
                    {
                        mPmmImuStruct.filteredAltitude = mAltitudeKalmanFilter.updateEstimate(mPmmImuStruct.altitude);
                        mFilteredAltitudeLastMillis = millis();
                    }
                }
                else
                {
                    if ((mPmmImuStruct.filteredAltitude - mPmmImuStruct.altitude) / (pow(millis() - mFilteredAltitudeLastMillis, 2) / 1000.0) < PMM_MAX_DOWN_ACCELERATION_M_S_2)
                    {
                        mPmmImuStruct.filteredAltitude = mAltitudeKalmanFilter.updateEstimate(mPmmImuStruct.altitude);
                        mFilteredAltitudeLastMillis = millis();
                    }
                }
                
                mFiltered2 = mAltitudeKalmanFilter2.updateEstimate(mPmmImuStruct.altitude);

                //mPlotter.Plot();
                delay(1);

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
    return mPmmImuStruct.temperatureMpu;
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
    return mPmmImuStruct.altitude;
}
float PmmImu::getBarometerTemperature()
{
    return mPmmImuStruct.temperatureBmp;
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
    return &mPmmImuStruct.temperatureMpu;
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
    return &mPmmImuStruct.altitude;
}
float* PmmImu::getBarometerTemperaturePtr()
{
    return &mPmmImuStruct.temperatureBmp;
}
pmmImuStructType* PmmImu::getImuStructPtr()
{
    return &mPmmImuStruct;
}
