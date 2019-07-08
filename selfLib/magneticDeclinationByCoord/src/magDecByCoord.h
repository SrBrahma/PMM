#ifndef MAG_DEC_BY_COORD
#define MAG_DEC_BY_COORD

// The latitude of the planet goes from -90 to 90.
// The longitude, from -180 to 180.
//
// The step means how many degrees we have to change to have another declination point.
// With a step of 10, we will have a table with 703 points, 19 lines from the latitude (-90, -80 ... 90; the lat 0° exists!),
// and 37 column from longitude (0° again!)
//
// The declination can go from -180 to 180 degrees. As all of this was planned to work on microcontrollers,
// one important aspect is the total memory used to store the table.
// We can't use int8_t as it goes from -128 to 127. Using float would cost 4 bytes per point.
// Half-precision (2 bytes float) doesn't exist in vanilla C/C++.
// I came with the idea of using int16_t, which goes from -32k to 32k, and to have a better precision,
// the values from the table are multiplied by 100, so a declination of 150.34 is stored as 15034,
// and then will be divided by 100 to get the actual declination.

// The total table size will be (2 * ceil(181.0/Steps) * ceil(361.0/steps) bytes.
// Where 2 is the size of the type (int16_t);
// 181 is how many degrees we have on latitude, with the 0°;
// 361 is how many degrees we have on longitude, with the 0°.
// So with a Step of 10, we will use 1.406 bytes of Program Memory (not RAM!)

// Quick reference table of memory used by Step
// Step    Memory Used
//  1     130,682 Bytes
//  2      32,942 Bytes
//  3      14,762 Bytes
//  4       8,372 Bytes
//  5       5,402 Bytes
//  6       3,782 Bytes
//  7       2,704 Bytes
//  8       2,116 Bytes
//  9       1,722 Bytes
// 10       1,406 Bytes


// You should ajust the Step according to the microcontroler Program Memory available and your accuracy needings.
// For Teensy 3.6 for example, we have 1MB of Progmem, so if the entire project isn't big enough, you could use the Step of 1.
// Also, future microcontrollers surely will have very larger Progmems.
// A theoric step of 0.5 would use 1,045,456 Bytes of memory. But, all the uncertainties, from the sensor, ambient noises, from the calculation,
// from the sensor height (as the height also changes the declination)... Would certainly be far greater than the accuracy gained from such small step.
// But if you want, you can do it. Read Readme.txt on extras/ to see how to create a table with the step you wish.
#define MAG_DEC_BY_COORD_ANGLE_STEP     2



int getDecByCoord(float* returnDeclination, float latitude, float longitude);



#endif