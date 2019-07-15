// By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma)
// Code to analyse measures, to know if a percentage of the measurements pass a condition,
// which can be the values itself or the first derivative, that are lesser or greater than a given value.
// This was done in order to detect a rocket liftoff or its descending, using barometer data.
// The first 1-2 values may be discarded, as to get the speed (first derivative), for example, we need
// 2 values to compare.

// If the new measure is entered with a time difference lesser than the minMicrosBetween, it is discarded.
// If the new measure is entered with a time difference greater than the maxMicrosBetween, it is added, but,
//   won't count towards the positives conditions check count.

// The microsWindow will be the same for all conditions you add.

// After thinking about it for almost 2 years, and after several methods, I think this is the best I have came up with.

#ifndef MEASURES_ANALYZER_h
#define MEASURES_ANALYZER_h

#include <stdint.h> // For uint32_t.
#include <SimpleKalmanFilter.h.>
#include <circularArray.h>

//
// Be careful that the constructor will malloc(mMinTotalMillis / minMillisPerMeasure) * 8 bytes.

// Type of measure is float. I could use a template, but most hobbyists don't know it. C++17 gives the possibility of
//   default template without using <>, but platformIO still uses C++14. And I don't want to use special flags.
class MeasuresAnalyzer
{
public:
    enum class CheckType {Values, FirstDerivative};
    enum class Relation  {AreLesserThan, AreGreaterThan};
    enum class Time      {DontApply, Second, Millisecond, Microsecond};

    typedef struct {
        float     value;
        uint32_t  micros;
    } Measure;

    typedef struct {
        double    minPositivesRatio; // 0.0 to 1.0, specifies how many (positives/measures) it must have to detect as a true condition.
        CheckType checkType;
        Relation  relation;
        double    checkValue;
        Time      perTimeUnit;
        int       currentPositives;
    } Condition;

    MeasuresAnalyzer(uint32_t minMicrosBetween, uint32_t maxAvgMicrosBetween, uint32_t microsWindow, int ignoreFirstXMeasures = 0,
                     bool useKalmanFilter = false, float kFMeasureUncertainty = 1, float kFProcessNoise = 0.01);
    // If using the empty constructor, you will need to call init().
    MeasuresAnalyzer();

    // To free the mConditions.
    ~MeasuresAnalyzer();
    
    // Only needed if constructed the object without arguments. Won't happen anything if used the full constructor.
    int  init(uint32_t minMicrosBetween, uint32_t maxAvgMicrosBetween, uint32_t microsWindow, int ignoreFirstXMeasures = 0,
              bool useKalmanFilter = false, float kFMeasureUncertainty = 1, float kFProcessNoise = 0.01);
    
    // It is float instead of double, to use less space on the circular array. Also, Certainly your measure precision
    // is lesser than the precision given by double instead of float.
    int  addMeasure(float measure, uint32_t timeMicros = micros());

    // The condition can be readden as
    // [minPercent]% of [checkType] [relation] [checkValue] units [perTimeUnit]
    // Ex: "90% of the FirstDerivatives AreGreatherThan 10 units/second"
    // Returns the condition index. If error, negative value is returned.
    int  addCondition(float minPercent, CheckType checkType, Relation relation, double checkValue, Time perTimeUnit);

    bool checkCondition(int conditionIndex);

    // Useful for debugging. Returns the average of the values/first derivative etc that are on the mCircularArray.
    // Second argument isn't needed when first arg is values.
    float getAverage(CheckType valueType = CheckType::Values, Time time = Time::Second);

    void reset();

private:
    // Adds a new measure to the end of the circular array (push()).
    // Also, it increases the Condition.currentPositives, if the pushed measure is a condition positive.
    void      pushMeasure(Measure measure);

    bool      checkMeasureCondition(int firstItemIs1LastIs0, Condition &condition);

    // Removes the oldested measure in the circular array.
    // Also, it decreases the 'Condition.currentPositives', if the removed measure was a condition positive.
    void      removeOldestMeasure();

    void      printMeasures(bool inverted = false);


    SimpleKalmanFilter mKalman;
    int                mUsingKalman;

    int       mMeasuresLeftToIgnore;

    Condition *mConditions = NULL;
    uint8_t   mCurrentConditions;

    int       mCanCheckCondition;
    int       mCanFirstDerivative;

    CircularArray<Measure> mCircularArray;

    uint32_t  mMicrosWindow;

    uint32_t  mCurrentTotalMicros;

    uint32_t  mMinMicrosBetween;
    uint32_t  mMaxAvgMicrosBetween;
    int       mMinMeasures;

    static constexpr uint8_t ADDITIONAL_LENGTH = 5;

    bool      mIsWorking = false;
};

#endif