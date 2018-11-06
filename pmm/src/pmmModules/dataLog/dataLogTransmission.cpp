#include "pmmModules/dataLog/dataLog.h"

//        --------------- DataLog Header 1.0 ---------------
//        [Positions] : [Function] : [ Length in Bytes ]
//
//        a) [ 0 ] : [ CRC 8 of this header.. ] : [ 1 ]
//        b) [ 1 ] : [ Session Identifier.... ] : [ 1 ]
//        c) [2,3] : [ LogInfo related CRC 16 ] : [ 2 ]
//        d) [5,6] : [ CRC 16 of the Log .... ] : [ 2 ]
//
//                            Total header length = 7 bytes.
//        --------------------------------------------------

//        -------------- DataLog Payload 1.0 ---------------
//        [Positions] : [ Function ] : [ Length in Bytes ]
//
//        a) [7,+] : [ Data Log ] : [ LogLength ]
//        --------------------------------------------------