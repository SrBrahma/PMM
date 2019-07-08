# declinationGridToArray
Wants to convert your GPS coordinates to the magnetic declination? AUTOMATICALLY? Concerned about the accuracy of it? OR NOT? 3AM here and I finished it!

Later I will write this readme properly.

### About:
Multiple tables of different sizes to fit your project needs.

### Quick story:
I was member of a university rocketry team, manager of the Avionics. One year ago, I found the  WMM2015v2 (https://www.ngdc.noaa.gov/geomag/WMM/soft.shtml) software, which calculates the magnetic declination based on your coordinates. Yesterday, 21h, I thought it was a good time to integrate it to my system, to automatically calculate it based on the GPS coordinates. I was struggling to reverse engineer the code of it, which is the hell on Earth. So I googled if anyone had already this idea. Yeap. https://github.com/ArduPilot/ardupilot/tree/master/libraries/AP_Declination.

:-(

BUT! I planned this mission for a whole year. I wouldn't just use someone's else work... - but I decided to copy the table idea, anyway, as the WMM code is really a mess, and I don't know if it would run on a microcontroller.

First I was thinking to use RegEx to create the tables, but then I decided to just change the WMM code and remake it, to automatically output in a C array.


Uses the wmm_grid.exe program, which can create a declination map for the entire planet.
