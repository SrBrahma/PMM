#include <math.h>
#include "magDecByCoord.h"



#if    MAG_DEC_BY_COORD_ANGLE_STEP == 1
#include "tables/1.h"

#elif  MAG_DEC_BY_COORD_ANGLE_STEP == 2
#include "tables/2.h"

#elif  MAG_DEC_BY_COORD_ANGLE_STEP == 3
#include "tables/3.h"

#elif  MAG_DEC_BY_COORD_ANGLE_STEP == 4
#include "tables/4.h"

#elif  MAG_DEC_BY_COORD_ANGLE_STEP == 5
#include "tables/5.h"

#elif  MAG_DEC_BY_COORD_ANGLE_STEP == 6
#include "tables/6.h"

#elif  MAG_DEC_BY_COORD_ANGLE_STEP == 7
#include "tables/7.h"

#elif  MAG_DEC_BY_COORD_ANGLE_STEP == 8
#include "tables/8.h"

#elif  MAG_DEC_BY_COORD_ANGLE_STEP == 9
#include "tables/9.h"

#elif  MAG_DEC_BY_COORD_ANGLE_STEP == 10
#include "tables/10.h"

#endif


// This code just translates the coords to the table index.
int getDecByCoord(float* returnDeclination, float latitude, float longitude)
{
    if (!returnDeclination) return 1;
    if (latitude  < -90)    return 2;
    if (latitude  >  90)    return 3;
    if (longitude < -180)   return 4;
    if (latitude  >  180)   return 5;

    int row, maxRow, column, maxColumn;

    maxRow    =  ceil(181.0 / MAG_DEC_BY_COORD_ANGLE_STEP) - 1;
    maxColumn =  ceil(361.0 / MAG_DEC_BY_COORD_ANGLE_STEP) - 1;

    latitude  +=  90;
    longitude += 180;

    row    = round(latitude  / MAG_DEC_BY_COORD_ANGLE_STEP);
    column = round(longitude / MAG_DEC_BY_COORD_ANGLE_STEP);

    if (row    > maxRow)    row    = 0;
    if (column > maxColumn) column = 0;

    *returnDeclination = ((float) MAGNETIC_DECLINATION_TABLE[row][column]) / 100.0;

    return 0;
}