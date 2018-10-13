/* pmmRecovery.cpp
 * Code for the rocket recovery system.
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma)
 * and Marcelo Maronas
 * Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <pmmRecovery.h>
/*
//---------------Recuperação---------------//
//if (((abs(imu_struct.acelerometro[0]) < 1) && (abs(imu_struct.acelerometro[1]) < 1) && (abs(imu_struct.acelerometro[2]) < 1)) && (lastAltitude - imu_struct.barometro[1] > recThreshold))
if (recCount == REC_COUNT_NEEDED)
{
  digitalWrite(2, HIGH);
  recoveryActivated = 1;
  PMM_DEBUG_PRINT("Recovery Activated!");
  recCount = 0;
  pmmErrorsCentral.reportRecuperation(packetIDul, sdIsWorking, rfIsWorking);
}
//Moving Average code
for (int n = 0; n < RECOVERY_AVERAGE_LENGTH; n++)
{
  recHolder = recBuffer[n];
  recBuffer[n] = recHolder2;
  recHolder2 = recHolder;
}
recBuffer[0] = imu_struct.barometro[1];
for (int n = 0; n < RECOVERY_AVERAGE_LENGTH; n++)
{
  barometerFiltered += recBuffer[n];
}
imu_struct.barometro[1] = barometerFiltered/RECOVERY_AVERAGE_LENGTH;
barometerFiltered = 0;
//End of MA code

//Recovery code incrementing the recCounter
if (lastAltitude - imu_struct.barometro[1] > recThreshold)
{
  recCount++;
  //Serial.print("recCount was activated and is in number :");Serial.println(recCount);
}
else
{
recCount = 0;
}
//
*/
