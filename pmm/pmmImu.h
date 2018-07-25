/* pmmImu.h
 * Code for the Inertial Measure Unit (IMU!)
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */



#ifndef PMM_IMU_h
#define PMM_IMU_h

#define DELAY_MS_BAROMETER 100 //random value

#include <pmmConsts.h>
#include <pmmImu.h>
#include <pmmErrorsCentral.h>
#include <MPU6050.h>
#include <HMC5883L.h>
#include <BMP085.h>
#include <Wire.h>
#include <I2Cdev.h>

typedef struct
{
    float accelerometerArray[3]; //Posicoes 1,2,3, respectivamente sao as Aceleracoes em x,y,z
    float magnetometerArray[3]; //Posicoes 1,2,3, respectivamente sao as Campos Magneticos em x,y,z
    float gyroscopeArray[3]; //Posicoes 1, 2, 3, respectivamente sao a velocidade angular em x,y,z
    float pressure;
    float altitudePressure;
    float temperature;
    float headingDegree;
    float headingRadian;
} pmmImuStructType;

class PmmImu
{
private:
    BMP085 mBarometer;
    MPU6050 mMpu;
    HMC5883L mMagnetometer;

    int16_t mMagnetometerRaw[3];
    int16_t mAccelerometerRaw[3];
    int16_t mGyroscopeRaw[3];

    float mAccelerometerScale;
    float mGyroscopeScale;
    float mMagnetometerScale;
    
    float mMagnetometerDeclinationRad;

    unsigned long mNextMillisBarometer;
    PmmErrorsCentral *mPmmErrorsCentral;
    pmmImuStructType mPmmImuStruct;

    int initMpu();
    int initMagnetometer();
    int initBmp();

    int updateMpu();
    int updateMagnetometer();
    int updateBmp();

    int updateScales();
public:
    PmmImu();
    /*
    int initAccelerometer();
    int initGyroscope(); */

    int init(PmmErrorsCentral *pmmErrorsCentral); // Must be executed, so the object is passed. Also, inits everything.

    int update(); // Gets all the sensors

    /* These returns safely a copy of the variables */
    void  getAccelerometer(float destinationArray[3]);
    void  getGyroscope(float destinationArray[3]);
    void  getMagnetometer(float destinationArray[3]);
    float getBarometer();
    float getAltitudeBarometer();
    float getTemperature();
    pmmImuStructType getImuStruct();

    /* These returns a pointer to the original variables - UNSAFE! Be careful! */
    float* getAccelerometerPtr();
    float* getGyroscopePtr();
    float* getMagnetometerPtr();
    float* getBarometerPtr();
    float* getAltitudeBarometerPtr();
    float* getTemperaturePtr();
    pmmImuStructType* getImuStructPtr();
};



#endif
