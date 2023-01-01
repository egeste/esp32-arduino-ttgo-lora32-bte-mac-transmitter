#include <BLEDevice.h>
#include <BLEAdvertisedDevice.h>
#include "ble.h"
#include "print.h"

BLEScan* pBLEScan;

class AdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    String address = String(advertisedDevice.getAddress().toString().c_str());
    address.toUpperCase();
    print("bte|" + address);
  }
};

void setup_ble() {
  // Config the bluetooth to scan for devices
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);
}

void loop_ble() {
  // Scan for bluteooth devices
  BLEScanResults foundDevices = pBLEScan->start(BLE_SCAN_TIME, false);
  pBLEScan->clearResults();
}