#include <Arduino.h> // For malloc()
#include <circularArray.h>
#include <generalUnitsOps.h>
#include <measuresAnalyzer.h>


MeasuresAnalyzer::
MeasuresAnalyzer(uint32_t minMicrosBetween, uint32_t maxAvgMicrosBetween, uint32_t microsWindow, int ignoreFirstXMeasures,
                 bool useKalmanFilter, float kFMeasureUncertainty, float kFProcessNoise)
{
    init(minMicrosBetween, maxAvgMicrosBetween, microsWindow);
}

MeasuresAnalyzer::MeasuresAnalyzer() {} // Call init after!

MeasuresAnalyzer::~MeasuresAnalyzer() { free(mConditions); }

int MeasuresAnalyzer::
init(uint32_t minMicrosBetween, uint32_t maxAvgMicrosBetween, uint32_t microsWindow, int ignoreFirstXMeasures,
     bool useKalmanFilter, float kFMeasureUncertainty, float kFProcessNoise)
{
    
    if (!mIsWorking)
    {
        mMinMicrosBetween    = minMicrosBetween;
        mMaxAvgMicrosBetween = maxAvgMicrosBetween;
        mMicrosWindow        = microsWindow;
        mMinMeasures         = mMicrosWindow / mMaxAvgMicrosBetween;

        if ((mUsingKalman = useKalmanFilter))
            mKalman.init(kFMeasureUncertainty, kFMeasureUncertainty, kFProcessNoise);

        mMeasuresLeftToIgnore = ignoreFirstXMeasures;

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

    // If the perTimeUnit is invalid to the given checkType, Seconds is selected, to avoid BIG user mistakes.
    if (checkType == CheckType::FirstDerivative && perTimeUnit == Time::DontApply)
        perTimeUnit = Time::Second;

    // https://stackoverflow.com/a/9269840
    mConditions[mCurrentConditions].minPositivesRatio = minPercent / 100;
    mConditions[mCurrentConditions].checkType         = checkType;
    mConditions[mCurrentConditions].relation          = relation;
    mConditions[mCurrentConditions].checkValue        = checkValue;
    mConditions[mCurrentConditions].perTimeUnit       = perTimeUnit;
    mConditions[mCurrentConditions].currentPositives  = 0;


    return mCurrentConditions++;
}


#include <pmmDebug.h>
int MeasuresAnalyzer::addMeasure(float measure, uint32_t timeMicros)
{
    if (!mIsWorking) // Check if the circular array was successfully allocated.
        return 1; 

    if (mMeasuresLeftToIgnore)
    {
        mMeasuresLeftToIgnore--;
        if (mUsingKalman)
            mKalman.updateEstimate(measure);
    }

    // 1) Get the time difference between the new and the previous measure.
    if (mCircularArray.length() > 0)
    {
        Measure lastMeasure = {0,0}; // [Shut up, compiler!]
        mCircularArray.getItemByLast(0, &lastMeasure);
        uint32_t microsBetween = timeDifference(timeMicros, lastMeasure.micros);

        // 1.1) Don't add the measure if the time difference is too small.
        if (microsBetween < mMinMicrosBetween)
            return 2;

        if (microsBetween > mMicrosWindow) // If a measure takes toooo long, reset.
        {
            reset();
            return 3;
        }
    }

    if (mUsingKalman)
        measure = mKalman.updateEstimate(measure);

    // 2) Remove oldest measures, if needed. This won't happen normally.
    if (mCircularArray.available() == 0)
        removeOldestMeasure();

    // 3) Add the new item.
    pushMeasure({measure, timeMicros});

    // 4) Removes the older measures until we only have only one measure that passes the time window.
    // We don't need to check the length before, as if the length is 1, the timeDiff will be 0.
    // Ex: We have a array of measures times in micros of [10, 20, 35, 45]. Our time window is 20 micros.
    // Our total time, is 35 (45-10). If we remove 10, our total time will be 25 (45-20), which still is greater than the window,
    // so we remove it. If we remove 20, our total time will be 10 (45-35), which is lesser than the window, so we DON'T remove it.
    // > So, we just need to calculate the timeDiff of the [last] time with the [first+1] time, and check if is greater than window.
    // It will keep at least 2 items. As the CircularArray circles the index, it -works-.
    while (timeDifference(mCircularArray.getItemByLast(0).micros, mCircularArray.getItemByFirst(1).micros) > mMicrosWindow)
        removeOldestMeasure(); // Removes the first/oldest measure.

    mCurrentTotalMicros = timeDifference(mCircularArray.getItemByLast(0).micros, mCircularArray.getItemByFirst(0).micros);
    return 0;
}



bool MeasuresAnalyzer::checkCondition(int conditionIndex)
{
    if (!mIsWorking || conditionIndex < 0 || conditionIndex >= mCurrentConditions)
        return false;

    // Avoids getting a positive for 1 positive out of only 1 measure.
    if (mCircularArray.length() < mMinMeasures)
        return false;

    if (mCurrentTotalMicros < mMicrosWindow)
        return false;

    double positivesRatio = 0;

    switch (mConditions[conditionIndex].checkType)
    {
        case CheckType::Values:
            if ((double)mCircularArray.length() < 1)
                return false;
            positivesRatio = mConditions[conditionIndex].currentPositives / (double)mCircularArray.length();
            break;

        case CheckType::FirstDerivative:
            if ((double)mCircularArray.length() < 2)
                return false;
            positivesRatio = mConditions[conditionIndex].currentPositives / ((double)(mCircularArray.length() - 1));
            break;
    }

    // Serial.printf("In cond %i, positivesRatio is %f. Min is %f.\n", conditionIndex, positivesRatio, mConditions[conditionIndex].minPositivesRatio);
    if (positivesRatio >= mConditions[conditionIndex].minPositivesRatio)
        return true;

    return false;
}



void MeasuresAnalyzer::pushMeasure(Measure measure)
{
    mCircularArray.push(measure); // Adds to the end a new item.

    for (int i = 0; i < mCurrentConditions; i++)
        if (checkMeasureCondition(0, mConditions[i]))
            mConditions[i].currentPositives++;
}
void MeasuresAnalyzer::removeOldestMeasure()
{
    for (int i = 0; i < mCurrentConditions; i++)
        if (checkMeasureCondition(1, mConditions[i]) && (mConditions[i].currentPositives > 0)) // We probably don't need this > 0. But, who knows?
            mConditions[i].currentPositives--;
    
    mCircularArray.shift(); // Removes the first/oldest measure.
}



bool MeasuresAnalyzer::checkMeasureCondition(int firstItemIs1LastIs0, Condition &condition)
{
    Measure measure0;

    switch (condition.checkType)
    {
        case CheckType::Values:
            measure0 = (firstItemIs1LastIs0? mCircularArray.getItemByFirst(0) : mCircularArray.getItemByLast(0));
            switch (condition.relation)
            {
                case Relation::AreGreaterThan:
                    if (measure0.value > condition.checkValue)
                        return true;
                    break;
                case Relation::AreLesserThan:
                    if (measure0.value < condition.checkValue)
                        return true;
                    break;
            }
        break;

        case CheckType::FirstDerivative:
            if (mCircularArray.length() < 2) // We need at least 2 values to compare.
                return false;

            Measure measure1;
            double firstDerivative;
            double timeDiff;

            measure0 = (firstItemIs1LastIs0? mCircularArray.getItemByFirst(0) : mCircularArray.getItemByLast(-1));
            measure1 = (firstItemIs1LastIs0? mCircularArray.getItemByFirst(1) : mCircularArray.getItemByLast( 0)); // newest

            firstDerivative = measure1.value  - measure0.value ;
            timeDiff        = measure1.micros - measure0.micros;

            switch (condition.perTimeUnit) {
                case Time::Second:
                    timeDiff /= 1000000; break;
                case Time::Millisecond:
                    timeDiff /=    1000; break;
                default:;
            }
            firstDerivative /= timeDiff;

            // Serial.printf("FirstDer is %f. CheckVal is %f.\n", firstDerivative, condition.checkValue);
            // Serial.printf("Vals %f & %f. Micros %lu & %lu.\n", measure1.value, measure0.value, measure1.micros, measure0.micros);

            switch (condition.relation)
            {
                case Relation::AreGreaterThan:
                    if (firstDerivative > condition.checkValue)
                        return true;
                    break;
                case Relation::AreLesserThan:
                    if (firstDerivative < condition.checkValue)
                        return true;
                    break;
            }
        break;
    }
    return false;
}

float MeasuresAnalyzer::getAverage(CheckType valueType, Time time)
{
    if (!mIsWorking) return 0;
    double average = 0;
    double arrayLength = mCircularArray.length();
    for (int i = 0; i < arrayLength; i++)
    {
        switch (valueType)
        {
        case CheckType::Values:
            average += mCircularArray.getItemByFirst(i).value / arrayLength;
            break;
        
        case CheckType::FirstDerivative:
            if (i == 0)
                break; // We need at least two values!

            double timeDiff = (mCircularArray.getItemByFirst(i).micros - mCircularArray.getItemByFirst(i - 1).micros);

            switch (time) {
                case Time::Second:
                    timeDiff /= 1000000; break;
                case Time::Millisecond:
                    timeDiff /=    1000; break;
                default:; }

            average += ((mCircularArray.getItemByFirst(i).value - mCircularArray.getItemByFirst(i - 1).value)
                        / timeDiff) / (arrayLength - 1); // -1 as we have on measure less.

            break;
        }
    }
    return average;
}

void MeasuresAnalyzer::reset()
{
    for (int i = 0; i < mCurrentConditions; i++)
        mConditions[i].currentPositives = 0;
    mCircularArray.reset();
}

void MeasuresAnalyzer::printMeasures(bool inverted)
{
    char buffer[2048] = {'\0'}; // No static needed, as it is called usually only once.

    for (int i = 0; i < mCircularArray.length(); i++)
    {
        if (i > 0)
            snprintf(buffer, 2048, "%s ", buffer); // add a space between.
        if (!inverted)
            snprintf(buffer, 2048, "%s[%i) %f %lu]", buffer, i, mCircularArray.getItemByFirst(i).value, mCircularArray.getItemByFirst(i).micros);
        else
            snprintf(buffer, 2048, "%s[%i) %f %lu]", buffer, i, mCircularArray.getItemByLast(-i).value, mCircularArray.getItemByLast(-i).micros);
    }

    Serial.println(buffer);
}