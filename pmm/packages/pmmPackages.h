#ifndef PMM_PACKAGES_h
#define PMM_PACKAGES_h

typedef enum
{
    PMM_PACKAGE_NONE,
    PMM_PACKAGE_LOG,
    PMM_PACKAGE_LOG_INFO,
    PMM_PACKAGE_STRING,
    PMM_PACKAGE_REQUEST
} pmmPackageType;

const char PMM_TELEMETRY_HEADER_TYPE_LOG[5]      = {"MLOG"}; // These are strings, just for faster changing.
const char PMM_TELEMETRY_HEADER_TYPE_LOG_INFO[5] = {"MLIN"};
const char PMM_TELEMETRY_HEADER_TYPE_STRING[5]   = {"MSTR"};
const char PMM_TELEMETRY_HEADER_TYPE_REQUEST[5]  = {"MRQT"};



#endif
