/* pmmImu.h
 * Code for the Inertial Measure Unit (IMU!)
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma), Rio de Janeiro - Brazil */

#define PMM_CALIBRATE_BAROMETER_USING_INITIAL_ALTITUDE
#define PMM_ALTITUDE_AT_STARTING_PLACE

#ifndef PMM_IMU_h
#define PMM_IMU_h


#include <MPU6050.h>
#include <HMC5883L.h>
#include <Adafruit_BMP085_U.h>

#include <magDecByCoord.h>

#include <SimpleKalmanFilter.h>
#include <Plotter.h>

#include "pmmImu/imuType.h"
#include "pmmConsts.h"
#include "pmmDebug.h"



class PmmImu
{

public:
    PmmImu();

    int init();     // Must be executed, so the object is passed. Also, inits everything.

    // You can get those values by using a & (bitwise And) on the return value.
    enum
    {   AllOk = 0,
        MpuError = 0x001,
        BarError = 0x010, BarGotPressure = 0x020,
        MagError = 0x100} ImuUpdateRtn;

    int update();   // Gets all the sensors
    enum class ImuMode {Sleep, Active};
    int setImuMode(ImuMode mode);


    pmmImuStructType  getImuStruct();
    pmmImuStructType* getImuStructPtr();


    // MPU
    int    initMpu();
    int    updateMpu();
    void   getAccelerometer(float destinationArray[3]);  float* getAccelerometerPtr();
    void   getGyroscope(float destinationArray[3]);      float* getGyroscopePtr();
    float  getMpuTemperature();                          float* getMpuTemperaturePtr();
    // -=-=-=-=-=-

    // Barometer
    int    initBmp();
    int    updateBmp();

    // This function should be reworked. (by HB 18/07/2019)
    // To REALLY calibrate the barometer, you actually need the initial altitude at the measured location,
    // then use the seaLevelForAltitude() from the bmp lib, to get the sea-level pressure at the given time
    // (as weather conditions changes).
    // int    setReferencePressure(int samples = 10);

    float  getBarometerPressure();     float* getBarometerPressurePtr();
    float  getBarometerAltitudePtr();  float* getAltitudeBarometerPtr();
    float  getBarometerTemperature();  float* getBarometerTemperaturePtr();
    // -=-=-=-=-=-


    // Magnetometer
    int    initMagnetometer();
    int    updateMagnetometer();

    int    setDeclination(float degrees);
    int    getDecByCoord (float* returnDeclination, float latitude, float longitude);
    int    setDeclination(float latitude, float longitude); // Uses coordinates to get declination, using another my another code.
    int    setDeclination(uint32_t latitude, uint32_t longitude); // Same as above, but using the uint32_t type for the coordinates.
    float  getDeclination();

    void   getMagnetometer(float destinationArray[3]);  float* getMagnetometerPtr();
    // -=-=-=-=-=-



private:

    pmmImuStructType mPmmImuStruct;

    // MPU
    MPU6050  mMpu;
    bool     mMpuIsWorking;
    // -=-=-=-=-=-

    // Barometer
    BMP085   mBarometer;
    double   mReferencePressure;
    bool     mBarometerIsWorking;

    // Magnetometer
    HMC5883L mMagnetometer;
    bool     mMagnetometerIsWorking;
    // -=-=-=-=-=-

};



#endif
