#include "transmitter.h"

// For some reason this only works when defined in the main file
#include <WiFi.h>
const wifi_promiscuous_filter_t filt={
  .filter_mask=WIFI_PROMIS_FILTER_MASK_MGMT|WIFI_PROMIS_FILTER_MASK_DATA
};

void setup() {
  setup_print();

  #ifdef TRACK_GPS
  setup_gps();
  #endif
  
  #ifdef TRACK_BLE
  setup_ble();
  #endif
  
  #ifdef TRACK_WIFI
  setup_wifi();
  #endif
};

void loop() {
  #ifdef TRACK_GPS
  loop_gps();
  delay(1000);
  #endif
  
  #ifdef TRACK_BLE
  loop_ble();
  delay(1000);
  #endif
  
  #ifdef TRACK_WIFI
  loop_wifi();
  delay(1000);
  #endif
};
