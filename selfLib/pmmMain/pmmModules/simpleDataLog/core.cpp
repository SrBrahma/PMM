#include "pmmModules/simpleDataLog/core.h"
#include "pmmDebug.h" // For advPrintf().

ModuleSimpleDataLogCore::ModuleSimpleDataLogCore() {}




int ModuleSimpleDataLogCore::buildCsvHeader(char destination[], int maxLength)
{
    if (!destination) return 1;
    destination[0] = '\0';
    for (int i = 0; i < mNumberVariables; i++) {
        if (i == 0)
            snprintf(destination, maxLength, "%s", mVarsNameArray[i]);
        else
            snprintf(destination, maxLength, "%s,%s", destination, mVarsNameArray[i]);
    }
    snprintf(destination, maxLength, "%s\n", destination);
    return 0;
}

int ModuleSimpleDataLogCore::buildCsvData(char destination[], int maxLength)
{
    if (!destination) return 1;
    destination[0] = '\0';
    for (int i = 0; i < mNumberVariables; i++) {
        if (i > 0)
            snprintf(destination, maxLength, "%s,", destination); // adds a comma.

        switch(mVarsTypeArray[i]) {
            case TYPE_ID_FLOAT: // first as it is more common
                snprintf(destination, maxLength, "%s%f",   destination, *(float*)    (mVarsAdrsArray[i])); break;
            case TYPE_ID_UINT32:
                snprintf(destination, maxLength, "%s%lu",  destination, *(uint32_t*) (mVarsAdrsArray[i])); break;
            case TYPE_ID_INT32:
                snprintf(destination, maxLength, "%s%li",  destination, *(int32_t*)  (mVarsAdrsArray[i])); break;
            case TYPE_ID_UINT8:
                snprintf(destination, maxLength, "%s%u",   destination, *(uint8_t*)  (mVarsAdrsArray[i])); break;
            case TYPE_ID_INT8:
                snprintf(destination, maxLength, "%s%i",   destination, *(int8_t*)   (mVarsAdrsArray[i])); break;
            case TYPE_ID_UINT16:
                snprintf(destination, maxLength, "%s%u",   destination, *(uint16_t*) (mVarsAdrsArray[i])); break;
            case TYPE_ID_INT16:
                snprintf(destination, maxLength, "%s%i",   destination, *(int16_t*)  (mVarsAdrsArray[i])); break;
            case TYPE_ID_UINT64:
                snprintf(destination, maxLength, "%s%llu", destination, *(uint64_t*) (mVarsAdrsArray[i])); break;
            case TYPE_ID_INT64:
                snprintf(destination, maxLength, "%s%lli", destination, *(int64_t*)  (mVarsAdrsArray[i])); break;
            case TYPE_ID_DOUBLE:
                snprintf(destination, maxLength, "%s%f",   destination, *(double*)   (mVarsAdrsArray[i])); break;
            default:    // If none above,
                snprintf(destination, maxLength, "%s%s",   destination, "ERROR HERE!"); break;
        } // switch end
    }
    snprintf(destination, maxLength, "%s\n", destination);
    return 0;
}


uint8_t ModuleSimpleDataLogCore::getVarSize(uint8_t variableType)
{
    switch (variableType) {
        case TYPE_ID_UINT8:  return 1;
        case TYPE_ID_INT8:   return 1;
        case TYPE_ID_UINT16: return 2;
        case TYPE_ID_INT16:  return 2;
        case TYPE_ID_UINT32: return 4;
        case TYPE_ID_INT32:  return 4;
        case TYPE_ID_FLOAT:  return 4;
        case TYPE_ID_UINT64: return 8;
        case TYPE_ID_INT64:  return 8;
        case TYPE_ID_DOUBLE: return 8;
        default: advPrintf("Invalid variable type to size!\n"); return 1;
    }
}



// Old stuff for adding gps data. You may use them for creating the funcs for them. Don't want to right now.
// #ifdef GPS_FIX_HEADING
//     if ((returnVal = includeVariable(gpsHeadingDegreeString, TYPE_ID_FLOAT, &pmmGpsStruct->headingDegree))) return returnVal;
// #endif

// #ifdef GPS_FIX_SPEED
//     if ((returnVal = includeVariable(gpsUpSpeedString, TYPE_ID_FLOAT, &pmmGpsStruct->upSpeed))) return returnVal;
//     if ((returnVal = includeVariable(gpsHorizontalSpeedString, TYPE_ID_FLOAT, &pmmGpsStruct->horizontalSpeed))) return returnVal;
//     #ifdef GPS_FIX_HEADING
//         if ((returnVal = includeVariable(gpsNorthSpeedString, TYPE_ID_FLOAT, &pmmGpsStruct->northSpeed))) return returnVal;
//         if ((returnVal = includeVariable(gpsEastSpeedString, TYPE_ID_FLOAT, &pmmGpsStruct->eastSpeed)))   return returnVal;
//     #endif