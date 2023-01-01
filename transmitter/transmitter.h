#pragma once

/** Size comparisons
Description         Size   Difference
Baseline            286.6k          0
Enable WiFi module  690.7k     404.1k // Need to slim this down
Enable BLE module   980.6k     694.0k // Need to slim this down
Enable GPS module   304.7k      18.0k
*/

#define TRACK_GPS
// #define TRACK_BLE
// #define TRACK_WIFI