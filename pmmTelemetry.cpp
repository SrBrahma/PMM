#include <pmmTelemetry.h>


PmmTelemetry::PmmTelemetry()
{
}

PmmTelemetry::init()
{
    pinMode(PIN_RFM95_RST, OUTPUT);
    digitalWrite(PIN_RFM95_RST, HIGH);

    delay(100); digitalWrite(PIN_RFM95_RST, LOW); delay(10); digitalWrite(PIN_RFM95_RST, HIGH); delay(10);

    int rf_initCounter = 0;
    while (!(rfIsWorking = rf95.init()) and (rf_initCounter++ < RF_INIT_MAX_TRIES))
    {
        #if DEBUG_SERIAL
            Serial.print("LoRa didn't initialized, attempt number "); Serial.println(rf_initCounter);
        #endif
    }

    if (!rfIsWorking)
        pmmErrorsAndSignals.reportError(ERROR_RF_INIT, 0, sdIsWorking, rfIsWorking);

    else // if RF is working
    {
        if (!(rfIsWorking = rf95.setFrequency(RF95_FREQ)))
        {
            DEBUG_PRINT("LoRa setFrequency failed!");
            pmmErrorsAndSignals.reportError(ERROR_RF_SET_FREQ, 0, sdIsWorking, rfIsWorking);
        }
        else // if RF is working
        {
            rf95.setTxPower(23, false);
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
            pmmErrorsAndSignals.blinkRfLED(HIGH);
            rf95.sendArrayOfPointersOf4Bytes(rf_radioPacket, RF_WORDS_IN_PACKET);
            pmmErrorsAndSignals.blinkRfLED(LOW);
        }
        return 1;
    }
    return 0;
}

int
PmmTelemetry::updateReception()
{
    if (rf95.recv2(rfPayload))
    {
        if (!memcmp(rfPayload, RF_HEADER_LOG, 4)) // MLOG
        {
            // save in bin
            // save in .csv
            return 1;
        }
        else if (!memcmp(rfPayload, RF_HEADER_EXTRA_LOG, 4)) // M
        {
            // save in txt

            return 1;
        }
        else if (!memcmp(rfPayload, RF_HEADER_VARS_INFO, 4))
        {
            return 1;
        }
        else
            return 0;
    }
    else
        return 0;
}

variableId++;
