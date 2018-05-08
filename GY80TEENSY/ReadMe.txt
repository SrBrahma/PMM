ReadMe ver 1.0, library GY80TEENSY - February 20, 2018

This is a necessary adaptation of my own GY80 library for using the GY80 IMU module with the microcontroller Teensy version 3.6.
This the very basic code using the library.The library was written using the best libraries I could find at the moment for each GY80 sub-module (Gyro, Accel, Magne, BMP085) and
putting them together in an lightweight and easy to understand code.Dont use it with Arduino, there's a lighter version of GY80 library that doesnt need so much memory, check in my GitHub.
The libraries for each sub-modules are, in majority, adapted adafruit libraries, and because of it, 
they are very heav.But in the counterpart, they also are very robust and have methods for everything that you need to do with the sensor.

Libraries required for the code to work (they are all included into the AdafruitNeededLibraries.rar file) :
-Adafruit_ADXL345-master
-Adafruit_BMP085_Unified-master
-Adafruit_HMC5883_Unified-master
-Adafruit_Sensor-master
-l3g-arduino-master (Non-Adafruit library)


Contact : marcelomaronas at poli.ufrj.br
For more codes : github.com/engmaronas