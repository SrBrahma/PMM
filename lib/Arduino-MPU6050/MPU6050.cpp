/*
MPU6050.cpp - Class file for the MPU6050 Triple Axis Gyroscope & Accelerometer Arduino Library.

Version: 1.0.3
(c) 2014-2015 Korneliusz Jarzebski
www.jarzebski.pl

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

https://www.digikey.com/en/pdf/i/invensense/mpu-hardware-offset-registers
*/

#if ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include <Wire.h>
#include <math.h>

#include <MPU6050.h>

#define GRAVITY_VALUE 9.80665 // https://en.wikipedia.org/wiki/Gravity_of_Earth

MPU6050::MPU6050(TwoWire &i2cChannel) : mWire(i2cChannel) {}

int MPU6050::begin(mpu6050_dps_t gyroScale, mpu6050_range_t accelRange, int mpuAddress)
{
    // Set Address
    mMpuAddress = mpuAddress;

    mWire.begin();

    // Reset threshold values
    mGyroscopeThreshold = 0;

    // Check MPU6050 Who Am I Register
    uint8_t whoAmI;
    if (read8(MPU6050_REG_WHO_AM_I, &whoAmI))
        return 1;

    if (!(whoAmI == 0x68 || whoAmI == 0x72)) // For some reason my device returns 0x72
        return 2;

    // Set Clock Source
    setClockSource(MPU6050_CLOCK_PLL_XGYRO);

    // Set Scale & Range
    setGyroscopeScale(gyroScale);
    setAccelerometerRange(accelRange);

    // Disable Sleep Mode
    setSleepEnabled(false);

    return 0;
}

void MPU6050::setAccelerometerRange(mpu6050_range_t range)
{
    switch (range)
    {
        case MPU6050_RANGE_2G:
            mRangePerDigit = .000061f;
            break;
        case MPU6050_RANGE_4G:
            mRangePerDigit = .000122f;
            break;
        case MPU6050_RANGE_8G:
            mRangePerDigit = .000244f;
            break;
        case MPU6050_RANGE_16G:
            mRangePerDigit = .0004882f;
            break;
        default:
            break;
    }

    uint8_t value;

    read8(MPU6050_REG_ACCEL_CONFIG, &value);
    value &= 0b11100111;
    value |= (range << 3);
    write8(MPU6050_REG_ACCEL_CONFIG, value);
}

mpu6050_range_t MPU6050::getAccelerometerRange()
{
    uint8_t value;
    read8(MPU6050_REG_ACCEL_CONFIG, &value);
    value &= 0b00011000;
    value >>= 3;
    return (mpu6050_range_t)value;
}

void MPU6050::setGyroscopeScale(mpu6050_dps_t scale)
{
    switch (scale)
    {
        case MPU6050_SCALE_250DPS:
            mDegreesPerDigit = .007633f;
            break;
        case MPU6050_SCALE_500DPS:
            mDegreesPerDigit = .015267f;
            break;
        case MPU6050_SCALE_1000DPS:
            mDegreesPerDigit = .030487f;
            break;
        case MPU6050_SCALE_2000DPS:
            mDegreesPerDigit = .060975f;
            break;
        default:
            break;
    }

    uint8_t value;
    read8(MPU6050_REG_GYRO_CONFIG, &value);
    value &= 0b11100111;
    value |= (scale << 3);
    write8(MPU6050_REG_GYRO_CONFIG, value);
}

mpu6050_dps_t MPU6050::getGyroscopeScale()
{
    uint8_t value;
    read8(MPU6050_REG_GYRO_CONFIG, &value);
    value &= 0b00011000;
    value >>= 3;
    return (mpu6050_dps_t)value;
}

void MPU6050::getPitchAndRoll(Vector normAccel, float *pitch, float *roll)
{
    *pitch = -(atan2(normAccel.XAxis, sqrt(normAccel.YAxis * normAccel.YAxis + normAccel.ZAxis * normAccel.ZAxis)) * 180.0) / M_PI;
    *roll  =  (atan2(normAccel.YAxis, normAccel.ZAxis) * 180.0) / M_PI;
}

// By Henrique Bruno Fantauzzi de Almeida (SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil
// To calibrate, put your hardware in a stable and straight surface until the end of the process.
// https://forum.arduino.cc/index.php?action=dlattach;topic=418665.0;attach=229569
void MPU6050::calibrateAccelerometer(uint16_t samples)
{
    mpu6050_range_t previousAccelerometerRange = getAccelerometerRange();

    int32_t accelVectorSum[3] = {0, 0, 0}; // Make sure it initializes zeroed!
    uint16_t counter;
    Vector rawAccel;

    int16_t newOffset, previousOffset;

    setAccelerometerRange(MPU6050_RANGE_16G); // The versions below 1.2 of the MPU HW Offset registers say the test should happen
    // on +-8g Range. However, after hours getting results that didn't match the theory, found out the 1.2 version, which as I thought
    // (as I was getting the double of the expected results), corrected to the +-16g range.

    for (counter = 0; counter < samples; counter++)
    {
        rawAccel = readRawAccelerometer();

        accelVectorSum[0] += rawAccel.XAxis;
        accelVectorSum[1] += rawAccel.YAxis;
        accelVectorSum[2] += rawAccel.ZAxis;
    }

    // As this https://forum.arduino.cc/index.php?action=dlattach;topic=418665.0;attach=229569 says,
    // we must preserve the bit 0 of the default offset value, for each vector.
    
    previousOffset = getAccelerometerOffsetX();
    newOffset = previousOffset - (int16_t) (accelVectorSum[0] / counter); // The X vector must be 0 when on the normal position
    newOffset = (newOffset & 0xFFFE) | (previousOffset & 0x0001);         // To keep the previous value of the bit 0, as said in manual.
    setAccelerometerOffsetX(newOffset); 

    previousOffset = getAccelerometerOffsetY();
    newOffset = previousOffset - (int16_t) (accelVectorSum[1] / counter); // The Y vector must be 0 when on the normal position
    newOffset = (newOffset & 0xFFFE) | (previousOffset & 0x0001);         // To keep the previous value of the bit 0, as said in manual.
    setAccelerometerOffsetY(newOffset); 

    previousOffset = getAccelerometerOffsetZ();
    newOffset = previousOffset - (int16_t) ((accelVectorSum[2] / counter) - 2048); // The Z vector must be 1 when on the normal position
    newOffset = (newOffset & 0xFFFE) | (previousOffset & 0x0001);         // To keep the previous value of the bit 0, as said in manual.
    setAccelerometerOffsetZ(newOffset); 

    setAccelerometerRange(previousAccelerometerRange); // Restore previous range
}

// https://forum.arduino.cc/index.php?action=dlattach;topic=418665.0;attach=229569
void MPU6050::calibrateGyroscope(uint16_t samples)
{
    mpu6050_dps_t previousGyroscopeScale = getGyroscopeScale();

    int32_t gyroVectorSum[3] = {0, 0, 0}; // Make sure it initializes zeroed!
    uint16_t counter;
    Vector rawGyro;

    int16_t previousOffset;

    setGyroscopeScale(MPU6050_SCALE_1000DPS); //As seen in the link above, we must set the scale to +-1000dps

    for (counter = 0; counter < samples; counter++)
    {
        rawGyro = readRawGyroscope();

        gyroVectorSum[0] += rawGyro.XAxis;
        gyroVectorSum[1] += rawGyro.YAxis;
        gyroVectorSum[2] += rawGyro.ZAxis;
    }

    previousOffset = getGyroscopeOffsetX();
    setGyroscopeOffsetX(previousOffset - (int16_t) (gyroVectorSum[0] / counter));

    previousOffset = getGyroscopeOffsetY();
    setGyroscopeOffsetY(previousOffset - (int16_t) (gyroVectorSum[1] / counter));

    previousOffset = getGyroscopeOffsetZ();
    setGyroscopeOffsetZ(previousOffset - (int16_t) (gyroVectorSum[2] / counter));

    setGyroscopeScale(previousGyroscopeScale); // Restore previous range
}



Vector MPU6050::readRawAccelerometer()
{
    uint8_t buffer[6];
    read(MPU6050_REG_ACCEL_XOUT_H, 6, buffer);

    mRawAccelerometer.XAxis = (int16_t) (buffer[0] << 8 | buffer[1]);
    mRawAccelerometer.YAxis = (int16_t) (buffer[2] << 8 | buffer[3]);
    mRawAccelerometer.ZAxis = (int16_t) (buffer[4] << 8 | buffer[5]);

    return mRawAccelerometer;
}

int MPU6050::readRawAccelerometer(float rawAccelerometer[3])
{
    uint8_t buffer[6];
    if (read(MPU6050_REG_ACCEL_XOUT_H, 6, buffer))
        return 1;

    rawAccelerometer[0] = (int16_t) (buffer[0] << 8 | buffer[1]); // The (int16_t) is necessary as the *buffer is uint8_t
    rawAccelerometer[1] = (int16_t) (buffer[2] << 8 | buffer[3]);
    rawAccelerometer[2] = (int16_t) (buffer[4] << 8 | buffer[5]);

    return 0;
}

Vector MPU6050::readScaledAccelerometer()
{
    readRawAccelerometer();

    mNormalizedAccelerometer.XAxis = mRawAccelerometer.XAxis * mRangePerDigit;
    mNormalizedAccelerometer.YAxis = mRawAccelerometer.YAxis * mRangePerDigit;
    mNormalizedAccelerometer.ZAxis = mRawAccelerometer.ZAxis * mRangePerDigit;

    return mNormalizedAccelerometer;
}

Vector MPU6050::readNormalizedAccelerometer()
{
    readRawAccelerometer();

    mNormalizedAccelerometer.XAxis = mRawAccelerometer.XAxis * mRangePerDigit * GRAVITY_VALUE;
    mNormalizedAccelerometer.YAxis = mRawAccelerometer.YAxis * mRangePerDigit * GRAVITY_VALUE;
    mNormalizedAccelerometer.ZAxis = mRawAccelerometer.ZAxis * mRangePerDigit * GRAVITY_VALUE;

    return mNormalizedAccelerometer;
}

int MPU6050::readNormalizedAccelerometer(float accelerometerArray[3])
{
    if (readRawAccelerometer(accelerometerArray))
        return 1;

    accelerometerArray[0] *= mRangePerDigit * GRAVITY_VALUE;
    accelerometerArray[1] *= mRangePerDigit * GRAVITY_VALUE;
    accelerometerArray[2] *= mRangePerDigit * GRAVITY_VALUE;

    return 0;
}

Vector MPU6050::readRawGyroscope()
{
    uint8_t buffer[6];
    read(MPU6050_REG_GYRO_XOUT_H, 6, buffer);

    mRawGyroscope.XAxis = (int16_t) (buffer[0] << 8 | buffer[1]);
    mRawGyroscope.YAxis = (int16_t) (buffer[2] << 8 | buffer[3]);
    mRawGyroscope.ZAxis = (int16_t) (buffer[4] << 8 | buffer[5]);

    return mRawGyroscope;
}
int MPU6050::readRawGyroscope(float rawGyroscope[3])
{
    uint8_t buffer[6];
    if (read(MPU6050_REG_GYRO_XOUT_H, 6, buffer))
        return 1;

    rawGyroscope[0] = (int16_t) (buffer[0] << 8 | buffer[1]);
    rawGyroscope[1] = (int16_t) (buffer[2] << 8 | buffer[3]);
    rawGyroscope[2] = (int16_t) (buffer[4] << 8 | buffer[5]);

    return 0;
}



Vector MPU6050::readNormalizedGyroscope()
{
    readRawGyroscope();

    if (mGyroscopeThreshold)
    {
        if (abs(mRawGyroscope.XAxis) < mGyroscopeThreshold)     mRawGyroscope.XAxis = 0;
        if (abs(mRawGyroscope.YAxis) < mGyroscopeThreshold)     mRawGyroscope.YAxis = 0;
        if (abs(mRawGyroscope.ZAxis) < mGyroscopeThreshold)     mRawGyroscope.ZAxis = 0;
    }

    mNormalizedGyroscope.XAxis = mRawGyroscope.XAxis * mDegreesPerDigit;
    mNormalizedGyroscope.YAxis = mRawGyroscope.YAxis * mDegreesPerDigit;
    mNormalizedGyroscope.ZAxis = mRawGyroscope.ZAxis * mDegreesPerDigit;

    return mNormalizedGyroscope;
}

int MPU6050::readNormalizedGyroscope(float gyroscopeArray[3])
{
    if (readRawGyroscope(gyroscopeArray))
        return 1;

    gyroscopeArray[0] *= mDegreesPerDigit;
    gyroscopeArray[1] *= mDegreesPerDigit;
    gyroscopeArray[2] *= mDegreesPerDigit;

    if (mGyroscopeThreshold)
    {
        if (abs(gyroscopeArray[0]) < mGyroscopeThreshold)     gyroscopeArray[0] = 0;
        if (abs(gyroscopeArray[1]) < mGyroscopeThreshold)     gyroscopeArray[1] = 0;
        if (abs(gyroscopeArray[2]) < mGyroscopeThreshold)     gyroscopeArray[2] = 0;
    }

    return 0;
}

void MPU6050::setDHPFMode(mpu6050_dhpf_t dhpf)
{
    uint8_t value;
    read8(MPU6050_REG_ACCEL_CONFIG, &value);
    value &= 0b11111000;
    value |= dhpf;
    write8(MPU6050_REG_ACCEL_CONFIG, value);
}

void MPU6050::setDLPFMode(mpu6050_dlpf_t dlpf)
{
    uint8_t value;
    read8(MPU6050_REG_CONFIG, &value);
    value &= 0b11111000;
    value |= dlpf;
    write8(MPU6050_REG_CONFIG, value);
}

void MPU6050::setClockSource(mpu6050_clockSource_t source)
{
    uint8_t value;
    read8(MPU6050_REG_PWR_MGMT_1, &value);
    value &= 0b11111000;
    value |= source;
    write8(MPU6050_REG_PWR_MGMT_1, value);
}

mpu6050_clockSource_t MPU6050::getClockSource()
{
    uint8_t value;
    read8(MPU6050_REG_PWR_MGMT_1, &value);
    value &= 0b00000111;
    return (mpu6050_clockSource_t)value;
}

bool MPU6050::getSleepEnabled()
{
    uint8_t value;
    read1(MPU6050_REG_PWR_MGMT_1, 6, &value);
    return value;
}

void MPU6050::setSleepEnabled(bool state)
{
    write1(MPU6050_REG_PWR_MGMT_1, 6, state);
}

bool MPU6050::getIntZeroMotionEnabled()
{
    uint8_t value;
    read1(MPU6050_REG_INT_ENABLE, 5, &value);
    return value;
}

void MPU6050::setIntZeroMotionEnabled(bool state)
{
    write1(MPU6050_REG_INT_ENABLE, 5, state);
}

bool MPU6050::getIntMotionEnabled()
{
    uint8_t value;
    read1(MPU6050_REG_INT_ENABLE, 6, &value);
    return value;
}

void MPU6050::setIntMotionEnabled(bool state)
{
    write1(MPU6050_REG_INT_ENABLE, 6, state);
}

bool MPU6050::getIntFreeFallEnabled()
{
    uint8_t value;
    read1(MPU6050_REG_INT_ENABLE, 7, &value);
    return value;
}

void MPU6050::setIntFreeFallEnabled(bool state)
{
    write1(MPU6050_REG_INT_ENABLE, 7, state);
}

uint8_t MPU6050::getMotionDetectionThreshold()
{
    uint8_t value;
    read8(MPU6050_REG_MOT_THRESHOLD, &value);
    return value;
}

void MPU6050::setMotionDetectionThreshold(uint8_t threshold)
{
    write8(MPU6050_REG_MOT_THRESHOLD, threshold);
}

uint8_t MPU6050::getMotionDetectionDuration()
{
    uint8_t value;
    read8(MPU6050_REG_MOT_DURATION, &value);
    return value;
}

void MPU6050::setMotionDetectionDuration(uint8_t duration)
{
    write8(MPU6050_REG_MOT_DURATION, duration);
}

uint8_t MPU6050::getZeroMotionDetectionThreshold()
{
    uint8_t value;
    read8(MPU6050_REG_ZMOT_THRESHOLD, &value);
    return value;
}

void MPU6050::setZeroMotionDetectionThreshold(uint8_t threshold)
{
    write8(MPU6050_REG_ZMOT_THRESHOLD, threshold);
}

uint8_t MPU6050::getZeroMotionDetectionDuration()
{
    uint8_t value;
    read8(MPU6050_REG_ZMOT_DURATION, &value);
    return value;
}

void MPU6050::setZeroMotionDetectionDuration(uint8_t duration)
{
    write8(MPU6050_REG_ZMOT_DURATION, duration);
}

uint8_t MPU6050::getFreeFallDetectionThreshold()
{
    uint8_t value;
    read8(MPU6050_REG_FF_THRESHOLD, &value);
    return value;
}

void MPU6050::setFreeFallDetectionThreshold(uint8_t threshold)
{
    write8(MPU6050_REG_FF_THRESHOLD, threshold);
}

uint8_t MPU6050::getFreeFallDetectionDuration()
{
    uint8_t value;
    read8(MPU6050_REG_FF_DURATION, &value);
    return value;
}

void MPU6050::setFreeFallDetectionDuration(uint8_t duration)
{
    write8(MPU6050_REG_FF_DURATION, duration);
}

bool MPU6050::getI2CMasterModeEnabled()
{
    uint8_t value;
    read1(MPU6050_REG_USER_CTRL, 5, &value);
    return value;
}

void MPU6050::setI2CMasterModeEnabled(bool state)
{
    write1(MPU6050_REG_USER_CTRL, 5, state);
}

bool MPU6050::getI2CBypassEnabled()
{
    uint8_t value;
    read1(MPU6050_REG_INT_PIN_CFG, 1, &value);
    return value;
}

void MPU6050::setI2CBypassEnabled(bool state)
{
    write1(MPU6050_REG_INT_PIN_CFG, 1, state);
}



void MPU6050::setAccelerometerPowerOnDelay(mpu6050_onDelay_t delay)
{
    uint8_t value;
    read8(MPU6050_REG_MOT_DETECT_CTRL, &value);
    value &= 0b11001111;
    value |= (delay << 4);
    write8(MPU6050_REG_MOT_DETECT_CTRL, value);
}

mpu6050_onDelay_t MPU6050::getAccelerometerPowerOnDelay()
{
    uint8_t value;
    read8(MPU6050_REG_MOT_DETECT_CTRL, &value);
    value &= 0b00110000;
    return (mpu6050_onDelay_t)(value >> 4);
}

uint8_t MPU6050::getIntStatus()
{
    uint8_t value;
    read8(MPU6050_REG_INT_STATUS, &value);
    return value;
}

Activities MPU6050::readActivites()
{
    uint8_t data;
    read8(MPU6050_REG_INT_STATUS, &data);

    mActivities.isOverflow =   ((data >> 4) & 1);
    mActivities.isFreeFall =   ((data >> 7) & 1);
    mActivities.isInactivity = ((data >> 5) & 1);
    mActivities.isActivity =   ((data >> 6) & 1);
    mActivities.isDataReady =  ((data >> 0) & 1);

    read8(MPU6050_REG_MOT_DETECT_STATUS, &data);

    mActivities.isNegActivityOnX = ((data >> 7) & 1);
    mActivities.isPosActivityOnX = ((data >> 6) & 1);

    mActivities.isNegActivityOnY = ((data >> 5) & 1);
    mActivities.isPosActivityOnY = ((data >> 4) & 1);

    mActivities.isNegActivityOnZ = ((data >> 3) & 1);
    mActivities.isPosActivityOnZ = ((data >> 2) & 1);

    return mActivities;
}



float MPU6050::readTemperature()
{
    int16_t T;
    read16S(MPU6050_REG_TEMP_OUT_H, &T);
    return (float)T/340 + 36.53;
}

int16_t MPU6050::getGyroscopeOffsetX()
{
    int16_t value;
    read16S(MPU6050_REG_GYRO_XOFFS_H, &value);
    return value;
}

int16_t MPU6050::getGyroscopeOffsetY()
{
    int16_t value;
    read16S(MPU6050_REG_GYRO_YOFFS_H, &value);
    return value;
}

int16_t MPU6050::getGyroscopeOffsetZ()
{
    int16_t value;
    read16S(MPU6050_REG_GYRO_ZOFFS_H, &value);
    return value;
}

void MPU6050::setGyroscopeOffsetX(int16_t offset)
{
    write16(MPU6050_REG_GYRO_XOFFS_H, offset);
}

void MPU6050::setGyroscopeOffsetY(int16_t offset)
{
    write16(MPU6050_REG_GYRO_YOFFS_H, offset);
}

void MPU6050::setGyroscopeOffsetZ(int16_t offset)
{
    write16(MPU6050_REG_GYRO_ZOFFS_H, offset);
}

int16_t MPU6050::getAccelerometerOffsetX()
{
    int16_t value;
    read16S(MPU6050_REG_ACCEL_XOFFS_H, &value);
    return value;
}

int16_t MPU6050::getAccelerometerOffsetY()
{
    int16_t value;
    read16S(MPU6050_REG_ACCEL_YOFFS_H, &value);
    return value;
}

int16_t MPU6050::getAccelerometerOffsetZ()
{
    int16_t value;
    read16S(MPU6050_REG_ACCEL_ZOFFS_H, &value);
    return value;
}

void MPU6050::setAccelerometerOffsetX(int16_t offset)
{
    write16(MPU6050_REG_ACCEL_XOFFS_H, offset);
}

void MPU6050::setAccelerometerOffsetY(int16_t offset)
{
    write16(MPU6050_REG_ACCEL_YOFFS_H, offset);
}

void MPU6050::setAccelerometerOffsetZ(int16_t offset)
{
    write16(MPU6050_REG_ACCEL_ZOFFS_H, offset);
}

// Get current threshold value
float MPU6050::getGyroscopeThreshold()
{
    return mGyroscopeThreshold;
}

// Set treshold value
void MPU6050::setGyroscopeThreshold(float percentOfMaximumValue) // Argument is in %. So, 1.5 = 1.5%.
{
    if (percentOfMaximumValue > 0)
        mGyroscopeThreshold = (32767 / 100.0) * percentOfMaximumValue;
    else
        mGyroscopeThreshold = 0;
}



// Write byte to register
int MPU6050::write8(uint8_t reg, uint8_t value)
{
    mWire.beginTransmission(mMpuAddress);

    mWire.write(reg);
    mWire.write(value);

    mWire.endTransmission();

    return 0;
}

// Read byte from register
int MPU6050::read8(uint8_t reg, uint8_t* value)
{
    mWire.beginTransmission(mMpuAddress);

    mWire.write(reg);

    mWire.endTransmission();

    mWire.beginTransmission(mMpuAddress);
    mWire.requestFrom(mMpuAddress, (uint8_t) 1);

    uint32_t startMillis = millis();
    while(!mWire.available())
    {
        if (millis() > startMillis + 5) // Maximum wait of 5ms. Avoid infinite loop.
            return 1;
    }

    *value = mWire.read();

    return 0;
}

// Read word from register
int MPU6050::read16S(uint8_t reg, int16_t* value) // S is for Signed (int16_t)
{
    mWire.beginTransmission(mMpuAddress);

    mWire.write(reg);

    mWire.endTransmission();

    mWire.beginTransmission(mMpuAddress);
    mWire.requestFrom(mMpuAddress, (uint8_t) 2);

    uint32_t startMillis = millis();
    while(!mWire.available())
    {
        if (millis() > startMillis + 5) // Maximum wait of 5ms. Avoid infinite loop.
            return 1;
    }

    uint8_t vha = mWire.read();
    uint8_t vla = mWire.read();

    *value = vha << 8 | vla;

    return 0;
}

int MPU6050::read(uint8_t reg, uint8_t numberBytes, uint8_t buffer[], bool reverse)
{
    mWire.beginTransmission(mMpuAddress);

    mWire.write(reg);

    mWire.endTransmission();

    mWire.beginTransmission(mMpuAddress);

    mWire.requestFrom(mMpuAddress, numberBytes);

    uint32_t startMillis = millis();
    while(mWire.available() < numberBytes)
    {
        if (millis() > startMillis + 5) // Maximum wait of 5ms. Avoid infinite loop.
            return 1;
    }

    if (!reverse)
        while (numberBytes--)
            *(buffer++) = mWire.read();

    else
        while (numberBytes--)
            buffer[numberBytes] = mWire.read();

    return 0;
}

int MPU6050::write16(uint8_t reg, int16_t value)
{
    mWire.beginTransmission(mMpuAddress);

    mWire.write(reg);
    mWire.write((uint8_t)(value >> 8));
    mWire.write((uint8_t)value);

    mWire.endTransmission();

    return 0;
}

// Read register bit
int MPU6050::read1(uint8_t reg, uint8_t pos, uint8_t* value)
{
    if (read8(reg, value))
        return 1;

    *value = (*value >> pos) & 1;

    return 0;
}

// Write register bit
int MPU6050::write1(uint8_t reg, uint8_t pos, bool state)
{
    uint8_t value;
    if (read8(reg, &value))
        return 1;

    if (state)
        value |= (1 << pos);
    else 
        value &= ~(1 << pos);

    write8(reg, value);

    return 0;
}



void MPU6050::resetDevice(mpu6050_dps_t gyroScale, mpu6050_range_t accelRange, int mpuAddress)
{
    write1(MPU6050_REG_PWR_MGMT_1, 7, 1); // as seen in the register map
    delay(100);

    // Reset threshold values
    mGyroscopeThreshold = 0;

    setClockSource(MPU6050_CLOCK_PLL_XGYRO);

    setGyroscopeScale(gyroScale);
    setAccelerometerRange(accelRange);
    
    // Disable Sleep Mode
    setSleepEnabled(false);
}