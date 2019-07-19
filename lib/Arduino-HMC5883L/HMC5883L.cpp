/*
HMC5883L.cpp - Class file for the HMC5883L Triple Axis Digital Compass Arduino Library.

Version: 1.1.0
(c) 2014 Korneliusz Jarzebski
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
*/

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>

#include "HMC5883L.h"

HMC5883L::HMC5883L(TwoWire &i2cChannel) : mWire(i2cChannel) {}

int HMC5883L::begin()
{
    mWire.begin();

    uint8_t valueA, valueB, valueC;

    // Those are returns for timeout.
    if (read8(HMC5883L_REG_IDENT_A, &valueA)) return 1;
    if (read8(HMC5883L_REG_IDENT_B, &valueB)) return 2;
    if (read8(HMC5883L_REG_IDENT_C, &valueC)) return 3;

    if ((valueA != 0x48) || (valueB != 0x34) || (valueC != 0x33))
        return 4;

    setMagnetometerRange(HMC5883L_RANGE_1_3GA);
    setMeasurementMode  (HMC5883L_CONTINOUS);
    setDataRate         (HMC5883L_DATARATE_15HZ);
    setSamples          (HMC5883L_SAMPLES_1);

    mDeclinationDegree = 0;

    return 0;
}

Vector HMC5883L::readRaw()
{
    int16_t value;

    read16S(HMC5883L_REG_OUT_X_M, &value);
    v.XAxis = value;
    read16S(HMC5883L_REG_OUT_Y_M, &value);
    v.YAxis = value;
    read16S(HMC5883L_REG_OUT_Z_M, &value);
    v.ZAxis = value;

    return v;
}

int HMC5883L::readRaw(float magnetometerArray[3])
{
    int16_t value[3];

    if (read16S(HMC5883L_REG_OUT_X_M, &value[0]))
        return 1;
    if (read16S(HMC5883L_REG_OUT_Y_M, &value[1]))
        return 1;
    if (read16S(HMC5883L_REG_OUT_Z_M, &value[2]))
        return 1;

    // Only transfer the values if no errors.
    magnetometerArray[0] = value[0];
    magnetometerArray[1] = value[1];
    magnetometerArray[2] = value[2];

    return 0;
}

Vector HMC5883L::readNormalized()
{
    int16_t value;

    read16S(HMC5883L_REG_OUT_X_M, &value);
    v.XAxis = ((float)value - xOffset) * mgPerDigit;
    read16S(HMC5883L_REG_OUT_Y_M, &value);
    v.YAxis = ((float)value - yOffset) * mgPerDigit;
    read16S(HMC5883L_REG_OUT_Z_M, &value);
    v.ZAxis = (float)value * mgPerDigit;

    return v;
}

int HMC5883L::readNormalized(float magnetometerArray[3])
{
    if (readRaw(magnetometerArray))
        return 1;

    magnetometerArray[0] = (magnetometerArray[0] - xOffset) * mgPerDigit;
    magnetometerArray[1] = (magnetometerArray[1] - yOffset) * mgPerDigit;
    magnetometerArray[2] =  magnetometerArray[2] * mgPerDigit;

    return 0;
}

int   HMC5883L::setDeclination(float degree)
{
    if (degree < -180 || degree > 180)  return 1;

    mDeclinationDegree = degree;

    return 0;
}

float HMC5883L::getDeclination()
{
    return mDeclinationDegree;
}

float HMC5883L::getHeading(float xAxis, float yAxis)
{
    // Calculate heading
    float heading = (atan2(yAxis, xAxis) * 180) / M_PI;

    return (heading + mDeclinationDegree);
}



void HMC5883L::setOffset(int xO, int yO)
{
    xOffset = xO;
    yOffset = yO;
}

void HMC5883L::setMagnetometerRange(hmc5883l_range_t range)
{
    switch(range)
    {
        case HMC5883L_RANGE_0_88GA:
            mgPerDigit = 0.073f;
            break;

        case HMC5883L_RANGE_1_3GA:
            mgPerDigit = 0.92f;
            break;

        case HMC5883L_RANGE_1_9GA:
            mgPerDigit = 1.22f;
            break;

        case HMC5883L_RANGE_2_5GA:
            mgPerDigit = 1.52f;
            break;

        case HMC5883L_RANGE_4GA:
            mgPerDigit = 2.27f;
            break;

        case HMC5883L_RANGE_4_7GA:
            mgPerDigit = 2.56f;
            break;

        case HMC5883L_RANGE_5_6GA:
            mgPerDigit = 3.03f;
            break;

        case HMC5883L_RANGE_8_1GA:
            mgPerDigit = 4.35f;
            break;

        default:
            break;
    }

    write8(HMC5883L_REG_CONFIG_B, range << 5);
}

hmc5883l_range_t HMC5883L::getMagnetometerRange(void)
{
    uint8_t value;
    read8(HMC5883L_REG_CONFIG_B, &value);
    return (hmc5883l_range_t)(value >> 5);
}

void HMC5883L::setMeasurementMode(hmc5883l_mode_t mode)
{
    uint8_t value;

    read8(HMC5883L_REG_MODE, &value);
    value &= 0b11111100;
    value |= mode;

    write8(HMC5883L_REG_MODE, value);
}

hmc5883l_mode_t HMC5883L::getMeasurementMode(void)
{
    uint8_t value;

    read8(HMC5883L_REG_MODE, &value);
    value &= 0b00000011;

    return (hmc5883l_mode_t)value;
}

void HMC5883L::setDataRate(hmc5883l_dataRate_t dataRate)
{
    uint8_t value;

    read8(HMC5883L_REG_CONFIG_A, &value);
    value &= 0b11100011;
    value |= (dataRate << 2);

    write8(HMC5883L_REG_CONFIG_A, value);
}

hmc5883l_dataRate_t HMC5883L::getDataRate(void)
{
    uint8_t value;

    read8(HMC5883L_REG_CONFIG_A, &value);
    value  &= 0b00011100;
    value >>= 2;

    return (hmc5883l_dataRate_t)value;
}

void HMC5883L::setSamples(hmc5883l_samples_t samples)
{
    uint8_t value;

    read8(HMC5883L_REG_CONFIG_A, &value);
    value &= 0b10011111;
    value |= (samples << 5);

    write8(HMC5883L_REG_CONFIG_A, value);
}

hmc5883l_samples_t HMC5883L::getSamples(void)
{
    uint8_t value;

    read8(HMC5883L_REG_CONFIG_A, &value);
    value  &= 0b01100000;
    value >>= 5;

    return (hmc5883l_samples_t)value;
}

// Write byte to register
void HMC5883L::write8(uint8_t reg, uint8_t value)
{
    mWire.beginTransmission(HMC5883L_ADDRESS);

    mWire.write(reg);
    mWire.write(value);

    mWire.endTransmission();
}

// Read byte from register
int HMC5883L::read8(uint8_t reg, uint8_t* value)
{
    mWire.beginTransmission(HMC5883L_ADDRESS);

    mWire.write(reg);

    mWire.endTransmission();

    mWire.beginTransmission(HMC5883L_ADDRESS);
    mWire.requestFrom(HMC5883L_ADDRESS, 1);

    uint32_t startMillis = millis();
    while(!mWire.available())
        if (millis() > startMillis + 10) // Maximum wait of 10ms. Avoid infinite loop.
            return 1;

    *value = mWire.read();

    return 0;
}

// Read word from register
int HMC5883L::read16S(uint8_t reg, int16_t* value) // S is for Signed (int16_t)
{
    mWire.beginTransmission(HMC5883L_ADDRESS);

    mWire.write(reg);

    mWire.endTransmission();

    mWire.beginTransmission(HMC5883L_ADDRESS);
    mWire.requestFrom(HMC5883L_ADDRESS, 2);

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