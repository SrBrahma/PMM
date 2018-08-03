/* pmmTelemetry.cpp
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <pmmTelemetry.h>
#include <RH_RF95.h>
#include <pmmConsts.h>
#include <pmmPackageLog.h>

PmmTelemetry::PmmTelemetry(): // https://stackoverflow.com/a/12927220
    mRf95(PMM_PIN_RFM95_CS, PMM_PIN_RFM95_INT)
{}

int PmmTelemetry::init(PmmErrorsCentral *pmmErrorsCentral, PmmPackageLog *pmmPackageLog)
{
    int initCounter = 0;
    mPreviousPackageLogTransmissionMillis = mPackageLogDelayMillis = 0;

    mPmmErrorsCentral = pmmErrorsCentral;
    mPmmPackageLog = pmmPackageLog;

    //RH_RF95 mRf95(PMM_PIN_RFM95_CS, PMM_PIN_RFM95_INT);

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
            Serial.print("PmmTelemetry: LoRa didn't initialized, attempt number "); Serial.println(initCounter);
        #endif
        if (++initCounter >= PMM_RF_INIT_MAX_TRIES) // Until counter
        {
            mPmmErrorsCentral->reportErrorByCode(ERROR_RF_INIT);
            PMM_DEBUG_PRINT("PmmTelemetry #1: LoRa didn't initialized after all these attempts.");
            return 1;
        }
    }

    /* So it initialized! */

    mRf95.setFrequency(PMM_LORA_FREQUENCY);
    mRf95.setTxPower(10, false);
    PMM_DEBUG_PRINT_MORE("PmmTelemetry: LoRa initialized successfully!");
    return 0;
}

int PmmTelemetry::updateTransmission()
{
    uint32_t tempMillis;

    if (millis() >= mPreviousPackageLogTransmissionMillis + mPackageLogDelayMillis)
    {
        tempMillis = millis();

        mRf95.waitPacketSent();

        mPackageLogDelayMillis += millis() - tempMillis;

        #if PMM_DEBUG_SERIAL_MORE
            Serial.print("PmmTelemetry [M]: Time taken waiting previous package to be sent = ");
            Serial.print(millis() - tempMillis);
            Serial.println("ms.");

            Serial.print("PmmTelemetry [M]: Delay is = ");
            Serial.print(mPackageLogDelayMillis);
            Serial.println("ms.");
        #endif


        if (mPmmErrorsCentral->getTelemetryIsWorking())
        {
            //mPmmErrorsCentral->blinkRfLED(HIGH);
            //PMM_DEBUG_PRINT("SENDING!");
            mRf95.sendArrayOfPointersOfSmartSizes(mPmmPackageLog->getVariableAddressArray(), mPmmPackageLog->getVariableSizeArray(),
                                                  mPmmPackageLog->getNumberOfVariables(), mPmmPackageLog->getPackageLogSizeInBytes());
            //mRf95.send(arr, 4);

            //PMM_DEBUG_PRINT("SENT 2");

            //mPmmErrorsCentral->blinkRfLED(LOW);
        }
        mPreviousPackageLogTransmissionMillis = millis();
        return 1;
    }
    return 0;
}

int PmmTelemetry::updateReception()
{
    if (mRf95.recv2(mRfPayload))
    {
        #if PMM_IS_PDA
        if (!memcmp(mRfPayload, PMM_TELEMETRY_HEADER_LOG, 4)) // MLOG
        {
            mPmmPackageLog->
            return 1;
        }
        else if (!memcmp(mRfPayload, PMM_TELEMETRY_HEADER_STRING, 4)) // MSTR
        {
            // save in txt

            return 1;
        }
        else if (!memcmp(mRfPayload, PMM_TELEMETRY_HEADER_LOG_INFO, 4)) // MLIN
        {
            return 1;
        }
        else
            return 0;
        #endif
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
