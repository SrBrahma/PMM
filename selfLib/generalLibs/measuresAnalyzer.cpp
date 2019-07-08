#include <Arduino.h> // For malloc()
#include <circularArray.h>
#include <generalUnitsOps.h>
#include <measuresAnalyzer.h>


MeasuresAnalyzer::MeasuresAnalyzer(uint32_t minMicrosBetween, uint32_t maxAvgMicrosBetween, uint32_t microsWindow)
{
    mMinMicrosBetween    = minMicrosBetween;
    mMaxAvgMicrosBetween = maxAvgMicrosBetween;
    mMicrosWindow        = microsWindow;
    mMinPoints           = mMicrosWindow / mMaxAvgMicrosBetween;

    // Create the circular array, as it initialized without size.
    mIsWorking = mCircularArray.realloc(minMicrosBetween * microsWindow + ADDITIONAL_LENGTH);
}

int MeasuresAnalyzer::addCondition(float minPercent, CheckType checkType, Relation relation, float checkValue, Time perTimeUnit)
{
    if (!realloc(&mConditions, sizeof(Condition) * (mCurrentConditions + 1)))
        return 1;
    mCurrentConditions++;
    return 0;
}

int MeasuresAnalyzer::addMeasure(float measure)
{
    if (!mIsWorking) // Check if the circular array was successfully allocated.
        return 1; 

    // 1) Get the time difference between the new and the previous measure.
    uint32_t nowMicros     = micros();
    if (mCircularArray.length())
    {
        Measure lastMeasure;
        mCircularArray.getItemByLast(0, &lastMeasure);
        uint32_t microsBetween = timeDifference(nowMicros, lastMeasure.micros);

        // 1.1) Don't add the measure if the time difference is too small.
        if (microsBetween < mMinMicrosBetween)
            return 2;
    }

    // 2) Remove oldest measures, if needed.
    int removedItems = 0;
    if (mCircularArray.available() == 0)
    {
        removeOldestMeasure();
        removedItems++;
    }

    // 3) Add the new item.
    mCircularArray.push({measure, nowMicros});

    // 4) Removes the older measures until we only have only one measure that passes the time window.
    // We don't need to check the length before, as if the length is 1, the timeDiff will be 0.
    // Ex: We have a array of measures times in micros of [10, 20, 35, 45]. Our time window is 20 micros.
    // Our total time, is 35 (45-10). If we remove 10, our total time will be 25 (45-20), which still is greater than the window,
    // so we remove it. If we remove 20, our total time will be 10 (45-35), which is lesser than the window, so we DON'T remove it.
    // > So, we just need to calculate the timeDiff of the [last] time with the [first+1] time, and check if is greater than window.
    // It will keep at least 2 items. As the CircularArray circles the index, it -works-.
    while (timeDifference(mCircularArray.getItemByLast(0).micros, mCircularArray.getItemByFirst(1).micros > mMicrosWindow))
    {
        removeOldestMeasure(); // Removes the first/oldest measure.
        removedItems++;
    }

    calculateChecks(removedItems);
    return 0;
}



void MeasuresAnalyzer::reset()
{
    for (int i = 0; i < mCurrentConditions; i++)
        mConditions[i].currentPositives = 0;
    mCircularArray.reset();
}


void MeasuresAnalyzer::pushMeasure(Measure measure)
{
    mCircularArray.push(measure); // Removes the first/oldest measure.

    for (int i = 0; i < mCurrentConditions; i++)
    {
        bool passedCondition = false;

        switch (mConditions[i].checkType)
        {
        case CheckType::Values:
            switch (mConditions[i].relation)
            {
            case Relation::AreGreaterThan:
                if (measure.value > mConditions[i].checkValue) passedCondition = true; break;
            case Relation::AreLesserThan:
                if (measure.value < mConditions[i].checkValue) passedCondition = true; break;
            }
        break;
        case CheckType::FirstDerivative:
            if (mCircularArray.length() >= 2) // We need two values to compare to. One was added at the begin of this func.
            {
                Measure previousMeasure = mCircularArray.getItemByLast(-1);
                float firstDerivative = ((measure.value - previousMeasure.value) / (measure.micros - previousMeasure.micros));

                switch (mConditions[i].relation)
                {
                case Relation::AreGreaterThan:
                    if (firstDerivative > mConditions[i].checkValue) passedCondition = true; break;
                case Relation::AreLesserThan:
                    if (firstDerivative < mConditions[i].checkValue) passedCondition = true; break;
                }
            }
        break;
        }

        if (passedCondition)
            mConditions[i].currentPositives++;
    }
}

void MeasuresAnalyzer::removeOldestMeasure()
{
    Measure oldestMeasure;
    mCircularArray.shift(&oldestMeasure); // Removes the first/oldest measure.

    for (int i = 0; i < mCurrentConditions; i++)
    {
        bool passedCondition = false;

        switch (mConditions[i].checkType)
        {
        case CheckType::Values:
            switch (mConditions[i].relation)
            {
            case Relation::AreGreaterThan:
                if (oldestMeasure.value > mConditions[i].checkValue) passedCondition = true; break;
            case Relation::AreLesserThan:
                if (oldestMeasure.value < mConditions[i].checkValue) passedCondition = true; break;
            }
        break;
        case CheckType::FirstDerivative:
            if (mCircularArray.length() >= 1) // We need another value to compare to. 1 because we already removed one, and have its value.
            {
                Measure firstItem = mCircularArray.getItemByFirst(0);
                float firstDerivative = ((firstItem.value - oldestMeasure.value) / (firstItem.micros - oldestMeasure.micros));

                switch (mConditions[i].relation)
                {
                case Relation::AreGreaterThan:
                    if (firstDerivative > mConditions[i].checkValue) passedCondition = true; break;
                case Relation::AreLesserThan:
                    if (firstDerivative < mConditions[i].checkValue) passedCondition = true; break;
                }
            }
        break;
        }

        if (passedCondition && mConditions[i].currentPositives > 0) // We probably don't need this > 0. But, who knows?
            mConditions[i].currentPositives--;
    }
}