#include "pmmConsts.h"

#if PMM_SYSTEM_ROUTINE == PMM_ROUTINE_LAUNCH_SIM

#include <stdint.h>
#include <measuresAnalyzer.h>
#include "pmmRoutines/launchSim/launchSim.h"


// How many real milliseconds to wait between updates.
#define PMM_LAUNCH_SIMULATOR_DELAY_MS       5

// How many milliseconds are between each launch simulator updates.
#define PMM_LAUNCH_SIMULATOR_INTERVAL_MS   50

class RoutineLaunchSim
{
public:
    RoutineLaunchSim();

    void init();
    void update();

private:
    void  disableRecDeployIfTimePassed(uint32_t timeMillis);
    void  deployRecoveriesIfConditionsMet(uint32_t timeMillis, float altitude);
    void  printMotd();

    uint32_t mTimeMillis;
    uint32_t mMainLoopCounter;

    LaunchSim         mLaunchSim;
    MeasuresAnalyzer  mAltitudeAnalyzer;

    struct { int liftOff, drogue, mainAlt, mainVel;  } mAltAnalyzerIndexes;
    struct { bool liftOff, drogue, main; } mDetections;
    struct { bool drogue, main;          } mDeploying;

    struct { uint32_t drogue, main;      } mRecoveryStopDeployAtMillis;
};

#endif