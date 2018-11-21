All telemetry and Flash writes should happen on Modules.


To include the PMM in your project, add the following lines on your platformio.ini file, without the quotes.

"
[platformio]
lib_extra_dirs = pmm/lib
"

The entire code was written assuming the use on a little-endian and 32-bit architecture.
On the future, as always, it may be improved and tested to work on other platforms.