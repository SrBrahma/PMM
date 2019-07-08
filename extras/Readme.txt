All telemetry and Flash writes should happen on Modules.


To include the PMM in your project, add the following lines on your platformio.ini file, without the quotes.

"
[platformio]
lib_extra_dirs = pmm/lib
"

The entire code was written assuming the use on a little-endian and 32-bit architecture.
On the future, as always, it may be improved and tested to work on other platforms.


===== PMM Standards: =====

1) Coordinates (Latitude, Longitude) are on 32-int format. [https://github.com/SlashDevin/NeoGPS/blob/master/extras/doc/Location.md]
2) Date format is the Internation Date Format, ISO 8601. (also know as YYYYMMDD) [https://en.wikipedia.org/wiki/ISO_8601]
    Like "original_launch": "2007-03-21T01:10:00.000Z" [https://docs.spacexdata.com/?version=latest]
3) Distance: Meters
4) Speed: Meters/Second
5) Acceleration Meters/Second^2

