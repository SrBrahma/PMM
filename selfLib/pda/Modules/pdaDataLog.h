#ifndef PDA_MODULE_DATA_LOG_h
#define PDA_MODULE_DATA_LOG_h

#include "pmm.h"


class PdaModuleDataLog
{
public:
private:
    static const uint8_t MAX_POINTS_PER_COLUMN =   3;
    static const uint8_t GRAPH_INTERNAL_WIDTH  = 109;
    static const uint8_t GRAPH_INTERNAL_HEIGHT =  47;

    uint32_t mXAxis[GRAPH_INTERNAL_WIDTH][MAX_POINTS_PER_COLUMN];
    uint32_t mYAxis[GRAPH_INTERNAL_HEIGHT][MAX_POINTS_PER_COLUMN];
};

#endif