// DataLogInfo File Structure
// All of this is poorly explained, but I really sucks explaining the crazy stuff I invent.
// Reading the code will make you fully understand.


// [Maximum DataLogInfo Payload Length : 2 Bytes][Total Packets : 1 Byte][LSb received packets : X Bytes][Data]

//      Maximum Packet Length:
// Your telemetry have a maximum packet length, and its Application Layer payload size is even lower due to the Transport Layer Header and the
// Application Layer header.
// Ex (values are hypothetical): Your telemetry maximum packet length is 32, and you are using the Protocol X which uses 5 bytes to its header,
// and the DataLogInfo Header uses another 5 bytes, so your maximum DataLogInfo Payload is 22 bytes. If your DataLogInfo has a total payload length
// of 30 bytes, you will first send the 22 bytes, and then, the remaining 8 bytes. So, the Maximum Packet Length is 22 bytes.

//      Total Packets:
// How many packets will build the entire package?

//      LSb received packets:
// Ex: If we have a Total Packets of 9, and we just received the packets 0, 1, 2 and 8:
//  Least significant Byte 1 of this field will contain:
// 0000 0001
//  Least significant Byte 0 of this field will contain:
// 0000 0111
// So, we are going to need ceil(TotalPackets/8.0) bytes to store the received packets,
// and each bit on these bytes represents a received packet, in a LSB format.

//      Data:
// The data. Each packet must have the Maximum Packet Length in length, except the last one, which will 

// The initial file size is (FileHeaderLength + MaximumDataLogInfoPayloadLength * Total Packets). Writing the last packet truncates the file.

#include "pmmModules/dataLog/dataLog.h"

int PmmModuleDataLog::saveDataLogInfo(char dirRelativePath[], uint16_t partsMaxLength, uint8_t currentPart, uint8_t totalParts)
{
}

int PmmModuleDataLog::saveOwnDataLogInfo()
{
}

int PmmModuleDataLog::saveReceivedDataLogInfo(uint8_t data[], uint8_t dataLength, uint8_t currentPart, uint8_t totalParts, uint8_t sourceAddress, uint8_t sourceSession)
{
}