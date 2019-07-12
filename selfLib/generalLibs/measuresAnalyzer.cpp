#include <Arduino.h> // For malloc()
#include <circularArray.h>
#include <generalUnitsOps.h>
#include <measuresAnalyzer.h>


MeasuresAnalyzer::MeasuresAnalyzer(uint32_t minMicrosBetween, uint32_t maxAvgMicrosBetween, uint32_t microsWindow)
{
    init(minMicrosBetween, maxAvgMicrosBetween, microsWindow);
}

MeasuresAnalyzer::MeasuresAnalyzer() {} // Call init after!

MeasuresAnalyzer::~MeasuresAnalyzer() { free(mConditions); }

int MeasuresAnalyzer::init(uint32_t minMicrosBetween, uint32_t maxAvgMicrosBetween, uint32_t microsWindow)
{
    if (!mIsWorking)
    {
        mMinMicrosBetween    = minMicrosBetween;
        mMaxAvgMicrosBetween = maxAvgMicrosBetween;
        mMicrosWindow        = microsWindow;
        mMinMeasures         = mMicrosWindow / mMaxAvgMicrosBetween;

        // Create the circular array, as it initialized without size.
        mIsWorking = mCircularArray.realloc((microsWindow / minMicrosBetween) + ADDITIONAL_LENGTH);
    }
    return mIsWorking? 0 : 1;
}

int MeasuresAnalyzer::addCondition(float minPercent, CheckType checkType, Relation relation, double checkValue, Time perTimeUnit)
{
    Condition *newPtr = (Condition*)realloc(mConditions, sizeof(Condition) * (mCurrentConditions + 1));
    if (!newPtr)
        return -1;
    mConditions = newPtr;

    // https://stackoverflow.com/a/9269840
    mConditions[mCurrentConditions] = (Condition){minPercent/100, checkType, relation, checkValue, perTimeUnit, 0};
    return mCurrentConditions++;
}


#include <pmmDebug.h>
int MeasuresAnalyzer::addMeasure(float measure)
{
    if (!mIsWorking) // Check if the circular array was successfully allocated.
        return 1; 
    
    Serial.println("startedAs"); printMeasures();

    // 1) Get the time difference between the new and the previous measure.
    uint32_t nowMicros = micros();
    if (mCircularArray.length())
    {
        Measure lastMeasure;
        mCircularArray.getItemByLast(0, &lastMeasure);
        uint32_t microsBetween = timeDifference(nowMicros, lastMeasure.micros);

        advPrintf("microsBetween is %lu\n", microsBetween);
        // 1.1) Don't add the measure if the time difference is too small.
        if (microsBetween < mMinMicrosBetween)
        {
            advPrintf("time between too short (%luus, less than %lu)\n", microsBetween, mMinMicrosBetween);
            return 2;
        }
        if (microsBetween > mMicrosWindow)
        {
            advPrintf("took too long. reseted!\n");
            reset();
            return 3;
        }
    }


    // 2) Remove oldest measures, if needed. This won't happen normally.
    if (mCircularArray.available() == 0)
    {
        advPrintf("none available.\n");
        removeOldestMeasure();}

    // 3) Add the new item.
    pushMeasure({measure, nowMicros});

    // 4) Removes the older measures until we only have only one measure that passes the time window.
    // We don't need to check the length before, as if the length is 1, the timeDiff will be 0.
    // Ex: We have a array of measures times in micros of [10, 20, 35, 45]. Our time window is 20 micros.
    // Our total time, is 35 (45-10). If we remove 10, our total time will be 25 (45-20), which still is greater than the window,
    // so we remove it. If we remove 20, our total time will be 10 (45-35), which is lesser than the window, so we DON'T remove it.
    // > So, we just need to calculate the timeDiff of the [last] time with the [first+1] time, and check if is greater than window.
    // It will keep at least 2 items. As the CircularArray circles the index, it -works-.
    while (timeDifference(mCircularArray.getItemByLast(0).micros, mCircularArray.getItemByFirst(1).micros) > mMicrosWindow)
    {
        printMeasures(); 
        advPrintf("timeDif was %lu, window is %lu. Removed oldest\n", timeDifference(mCircularArray.getItemByLast(0).micros, mCircularArray.getItemByFirst(1).micros), mMicrosWindow);
        removeOldestMeasure(); // Removes the first/oldest measure.
    }

    Serial.println("endedAs"); printMeasures();
    return 0;
}



bool MeasuresAnalyzer::checkCondition(int conditionIndex)
{
    if (!mIsWorking || conditionIndex < 0 || conditionIndex >= mCurrentConditions)
        return false;
    Serial.printf("Next, len is %i, minMeas is %i.\n", mCircularArray.length(), mMinMeasures);

    // Avoids getting a positive for 1 positive out of only 1 measure.
    if (mCircularArray.length() < mMinMeasures)
        return false;

    float positivesRatio = mConditions[conditionIndex].currentPositives / (float)mCircularArray.length();
    Serial.printf("In cond %i, positivesRatio is %f.\n", conditionIndex, positivesRatio);
    if (positivesRatio >= mConditions[conditionIndex].minPositivesRatio)
        return true;

    return false;
}



void MeasuresAnalyzer::pushMeasure(Measure measure)
{
    mCircularArray.push(measure); // Adds to the end a new item.

    for (int i = 0; i < mCurrentConditions; i++)
        if (checkMeasureCondition(0, &mConditions[mCurrentConditions]))
            mConditions[i].currentPositives++;
}
void MeasuresAnalyzer::removeOldestMeasure()
{
    for (int i = 0; i < mCurrentConditions; i++)
        if (checkMeasureCondition(1, &mConditions[mCurrentConditions]) && mConditions[i].currentPositives > 0) // We probably don't need this > 0. But, who knows?
            mConditions[i].currentPositives--;
    
    mCircularArray.shift(); // Removes the first/oldest measure.
}



bool MeasuresAnalyzer::checkMeasureCondition(int firstItemIs1LastIs0, const Condition *condition)
{
    Measure measure0 = firstItemIs1LastIs0? mCircularArray.getItemByFirst(0) : mCircularArray.getItemByLast(0);

    switch (condition->checkType)
    {
        case CheckType::Values:
            switch (condition->relation) {
                case Relation::AreGreaterThan:
                    if (measure0.value > condition->checkValue) return true;
                case Relation::AreLesserThan:
                    if (measure0.value < condition->checkValue) return true; }
        break;

        case CheckType::FirstDerivative:
            if (mCircularArray.length() < 2) // We need at least 2 values to compare.
                return false;
            
            Measure measure1;
            double firstDerivative;

            if (firstItemIs1LastIs0) {
                measure1 = mCircularArray.getItemByFirst(1);
                firstDerivative = (measure1.value - measure0.value) / (measure1.micros - measure0.micros);
            }
            else {
                measure1 = mCircularArray.getItemByLast(-1);
                firstDerivative = (measure0.value - measure1.value) / (measure0.micros - measure1.micros);
            }

            switch (condition->perTimeUnit) {
                case Time::Second:
                    firstDerivative /= 1000000; break;
                case Time::Millisecond:
                    firstDerivative /=    1000; break;
                default:; }

            switch (condition->relation) {
                case Relation::AreGreaterThan:
                    if (firstDerivative > condition->checkValue) return true;
                case Relation::AreLesserThan:
                    if (firstDerivative < condition->checkValue) return true; }
        break;
    }
    return false;
}

void MeasuresAnalyzer::reset()
{
    for (int i = 0; i < mCurrentConditions; i++)
        mConditions[i].currentPositives = 0;
    mCircularArray.reset();
}

void MeasuresAnalyzer::printMeasures()
{
    char buffer[2048] = {'\0'}; // No static needed, as it is called usually only once.

    for (int i = 0; i < mCircularArray.length(); i ++)
    {
        if (i > 0)
            snprintf(buffer, 2048, "%s ", buffer);
        snprintf(buffer, 2048, "%s[%i) %f %lu]", buffer, i, mCircularArray.getItemByFirst(i).value, mCircularArray.getItemByFirst(i).micros);
    }

    Serial.println(buffer);
}