/* pmmTelemetry.cpp
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <pmmTelemetry.h>
#include <RH_RF95.h>
#include <pmmConsts.h>

PmmTelemetry::PmmTelemetry(): // https://stackoverflow.com/a/12927220
    mRf95(PMM_PIN_RFM95_CS, PMM_PIN_RFM95_INT)
{}

int PmmTelemetry::init(PmmErrorsCentral *pmmErrorsCentral)
{
    mPmmErrorsCentral = pmmErrorsCentral;
    RH_RF95 mRf95(PMM_PIN_RFM95_CS, PMM_PIN_RFM95_INT);
    int initCounter = 0;
    int tempRfIsWorking = 1;
    pinMode(PMM_PIN_RFM95_RST, OUTPUT);
    digitalWrite(PMM_PIN_RFM95_RST, HIGH);

    /* These delays are the default of the lora code. Maybe they aren't even needed. */
    delay(100);
    digitalWrite(PMM_PIN_RFM95_RST, LOW);
    delay(10);
    digitalWrite(PMM_PIN_RFM95_RST, HIGH);
    delay(10);

    // mRf95.init() returns false if didn't initialized successfully.
    while (!mRf95.init()) // Keep trying! ...
    {
        #if PMM_DEBUG_SERIAL
            Serial.print("LoRa didn't initialized, attempt number "); Serial.println(initCounter);
        #endif
        if (++initCounter >= PMM_RF_INIT_MAX_TRIES) // Until counter
        {
            mPmmErrorsCentral->reportErrorByCode(ERROR_RF_INIT);
            PMM_DEBUG_PRINT("LoRa didn't initialized after all these attempts.");
            return 1;
        }
    }

    /* So it initialized! */

    mRf95.setFrequency(PMM_LORA_FREQUENCY);
    mRf95.setTxPower(23, false);
    PMM_DEBUG_PRINT_MORE("LoRa initialized successfully!");
}

int PmmTelemetry::updateTransmission()
{
    if (millis() >= mNextMillisPackageLog)
    {
        mNextMillisPackageLog = millis() + 300;
        if (mPmmErrorsCentral->getTelemetryIsWorking())
        {
            //mPmmErrorsCentral->blinkRfLED(HIGH);
            mRf95.sendArrayOfPointersOf4Bytes(mRfPayload, RF_WORDS_IN_PACKET);
            //mPmmErrorsCentral->blinkRfLED(LOW);
        }
        return 1;
    }
    return 0;
}

int PmmTelemetry::updateReception()
{
    if (mRf95.recv2(mRfPayload))
    {
        if (!memcmp(mRfPayload, PMM_TELEMETRY_HEADER_LOG, 4)) // MLOG
        {
            // save in bin
            // save in .csv
            return 1;
        }
        else if (!memcmp(mRfPayload, PMM_TELEMETRY_HEADER_STRING, 4)) // MSTR
        {
            // save in txt

            return 1;
        }
        else if (!memcmp(mRfPayload, PMM_TELEMETRY_HEADER_LOG_INFO, 4)) // MVIN
        {
            return 1;
        }
        else
            return 0;
    }
    else
        return 0;
}

/* for the future
int
PmmTelemetry::setTxPower(int value)
{
    value <= 5? value = 5;
    value >= 23? value = 23;

    mRf95.setTxPower(value, false);
    return 0;
} */
