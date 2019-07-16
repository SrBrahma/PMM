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
    void  printMotd();
    LaunchSim         mLaunchSim;
    MeasuresAnalyzer  mAltitudeAnalyzer;

    struct { int liftOff; int drogue; int main;} mAltAnalyzerIndexes;
    bool     mHadLiftOff, mOrderedDrogue, mOrderedMain;

    uint32_t recovery0DisableAtMillis;
    uint32_t recovery1DisableAtMillis;
};

#endif