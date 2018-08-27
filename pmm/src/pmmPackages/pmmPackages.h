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


#define PMM_TELEMETRY_PORT_LOG          0x10    // These are for now, just random numbers.
#define PMM_TELEMETRY_PORT_LOG_INFO     0x20
#define PMM_TELEMETRY_PORT_STRING       0x40
#define PMM_TELEMETRY_PORT_REQUEST      0x80



#endif
