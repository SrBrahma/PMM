/* pmmTelemetry.cpp
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <pmmTelemetry.h>
#include <RH_RF95.h>

PmmTelemetry::PmmTelemetry()
{
}

int PmmTelemetry::init(PmmErrorsCentral *pmmErrorsCentral)
{
    mPmmErrorsCentral = pmmErrorsCentral;
    RH_RF95 mRf95(PIN_RFM95_CS, PIN_RFM95_INT);

    pinMode(PMM_PIN_RFM95_RST, OUTPUT);
    digitalWrite(PMM_PIN_RFM95_RST, HIGH);

    delay(100); digitalWrite(PMM_PIN_RFM95_RST, LOW); delay(10); digitalWrite(PMM_PIN_RFM95_RST, HIGH); delay(10);

    int rf_initCounter = 0;
    while (!(rfIsWorking = mRf95.init()) and (rf_initCounter++ < RF_INIT_MAX_TRIES))
    {
        #if PMM_DEBUG_SERIAL
            Serial.print("LoRa didn't initialized, attempt number "); Serial.println(rf_initCounter);
        #endif
    }

    if (!rfIsWorking)
        pmmErrorsCentral.reportError(ERROR_RF_INIT, 0, sdIsWorking, rfIsWorking);

    else // if RF is working
    {
        if (!(rfIsWorking = mRf95.setFrequency(RF95_FREQ)))
        {
            DEBUG_PRINT("LoRa setFrequency failed!");
            pmmErrorsCentral.reportError(ERROR_RF_SET_FREQ, 0, sdIsWorking, rfIsWorking);
        }
        else // if RF is working
        {
            mRf95.setTxPower(23, false);
            DEBUG_PRINT("LoRa initialized successfully!");
        }
    }
}

int
PmmTelemetry::updateTransmission()
{
    if (millis() >= nextMillis_rf)
    {
        nextMillis_rf = millis() + DELAY_MS_RF;
        if (rfIsWorking)
        {
            pmmErrorsCentral.blinkRfLED(HIGH);
            mRf95.sendArrayOfPointersOf4Bytes(rf_radioPacket, RF_WORDS_IN_PACKET);
            pmmErrorsCentral.blinkRfLED(LOW);
        }
        return 1;
    }
    return 0;
}

int
PmmTelemetry::updateReception()
{
    if (mRf95.recv2(rfPayload))
    {
        if (!memcmp(rfPayload, RF_HEADER_LOG, 4)) // MLOG
        {
            // save in bin
            // save in .csv
            return 1;
        }
        else if (!memcmp(rfPayload, RF_HEADER_EXTRA_LOG, 4)) // MSTR
        {
            // save in txt

            return 1;
        }
        else if (!memcmp(rfPayload, RF_HEADER_VARS_INFO, 4)) // MVIN
        {
            return 1;
        }
        else
            return 0;
    }
    else
        return 0;
}

int
PmmTelemetry::setTxPower(int value)
{
    value <= 5? value = 5;
    value >= 23? value = 23;

    mRf95.setTxPower(value, false);
    return 0;
}
