#ifndef PMM_PACKAGES_h
#define PMM_PACKAGES_h

typedef enum
{
    PMM_PORT_NONE,
    PMM_PORT_LOG,
    PMM_PORT_LOG_INFO,
    PMM_PORT_STRING,
    PMM_PORT_REQUEST
} PmmPortType;


#define PMM_PORT_LOG          0x10    // These are for now, just random numbers.
#define PMM_PORT_LOG_INFO     0x20
#define PMM_PORT_STRING       0x40
#define PMM_PORT_REQUEST      0x80



#endif
