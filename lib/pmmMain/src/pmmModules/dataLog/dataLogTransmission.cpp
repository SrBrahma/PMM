#include "pmmModules/dataLog/dataLog.h"

//        --------------- DataLog Header 1.0 ---------------
//        [Positions] : [Function] : [ Length in Bytes ]
//
//        a) [ 0 ] : [ CRC 8 of this header . ] : [ 1 ]
//        b) [ 1 ] : [ Session Identifier ... ] : [ 1 ]
//        c) [2,3] : [ DataLogInfo related CRC 16 ] : [ 2 ]
//        d) [4,5] : [ CRC 16 of the Log .... ] : [ 2 ]
//
//                            Total header length = 6 bytes.
//        --------------------------------------------------

//        -------------- DataLog Payload 1.0 ---------------
//        [Positions] : [ Function ] : [ Length in Bytes ]
//
//        a) [6,+] : [ Data Log ] : [ LogLength ]
//        --------------------------------------------------