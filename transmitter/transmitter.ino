#include <SPI.h>
#include <LoRa.h>
#include <esp_wifi.h>
#include <BLEDevice.h>
#include <BLEAdvertisedDevice.h>

#define BLE_SCAN_TIME  5
#define WIFI_MAX_CH    11
#define LORA_BAND      915

#define SCK            5    // GPIO5  -- SX1278's SCK
#define MISO           19   // GPIO19 -- SX1278's MISO
#define MOSI           27   // GPIO27 -- SX1278's MOSI
#define SS             18   // GPIO18 -- SX1278's CS
#define RST            14   // GPIO14 -- SX1278's RESET
#define DI0            26   // GPIO26 -- SX1278's IRQ(Interrupt Request)

int scanChannel = 1;
int macAddressCount = 0;

String macAddresses[64][3];
String defaultTTL = "10";

BLEScan* pBLEScan;

const wifi_promiscuous_filter_t filt={ //Idk what this does
  .filter_mask=WIFI_PROMIS_FILTER_MASK_MGMT|WIFI_PROMIS_FILTER_MASK_DATA
};

typedef struct { // or this
  uint8_t mac[6];
} __attribute__((packed)) MacAddr;

typedef struct { // still dont know much about this
  int16_t fctl;
  int16_t duration;
  MacAddr da;
  MacAddr sa;
  MacAddr bssid;
  int16_t seqctl;
  unsigned char payload[];
} __attribute__((packed)) WifiMgmtHdr;

void print(String input) {
  Serial.println(input);
  LoRa.beginPacket();
  LoRa.print(input);
  LoRa.endPacket();
};

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    String address = String(advertisedDevice.getAddress().toString().c_str());
    print("bte|"+address);
  }
};

void purge() {
  for (int i=0; i<=63; i++) {
    if (!(macAddresses[i][0] == "")) {
      int ttl = (macAddresses[i][1].toInt()) - 1;
      if (ttl <= 0) {
        macAddresses[i][2] = "OFFLINE";
        macAddresses[i][1] = defaultTTL;
      } else{
        macAddresses[i][1] = String(ttl);
      }
    }
  }
};

void updatetime() { // This updates the time the device has been online for
  for (int i=0; i<=63; i++) {
    if (!(macAddresses[i][0] == "")) {
      if (macAddresses[i][2] == "") macAddresses[i][2] = "0";
      if (!(macAddresses[i][2] == "OFFLINE")) {
        int timehere = (macAddresses[i][2].toInt());
        macAddresses[i][2] = String(++timehere);
      }
    }
  }
};

void sniffer(void* buf, wifi_promiscuous_pkt_type_t type) { //This is where packets end up after they get sniffed
  wifi_promiscuous_pkt_t *p = (wifi_promiscuous_pkt_t*)buf; // Dont know what these 3 lines do
  int len = p->rx_ctrl.sig_len;
  WifiMgmtHdr *wh = (WifiMgmtHdr*)p->payload;
  len -= sizeof(WifiMgmtHdr);
  if (len < 0) return;

  String packet;
  String macAddress;
  int fctl = ntohs(wh->fctl);
  for (int i=8; i<=15; i++) packet += String(p->payload[i],HEX);
  for (int i=4; i<=15; i++) macAddress += packet[i];
  macAddress.toUpperCase();

  int added = 0;
  for (int i=0; i<=63; i++) { // checks if the MAC address has been added before
    if (macAddress == macAddresses[i][0]) {
      macAddresses[i][1] = defaultTTL;
      if (macAddresses[i][2] == "OFFLINE") macAddresses[i][2] = "0";
      added = 1;
    }
  }

  if (added == 0) {
    print("mac|"+macAddress);
    macAddresses[macAddressCount][0] = macAddress;
    macAddresses[macAddressCount][1] = defaultTTL;
    if (++macAddressCount >= 64) macAddressCount = 0;
  }
};

void setup() {
  Serial.begin(115200);

  // Config the bluetooth to scan for devices
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);

  // Start the wifi stuff
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filt);
  esp_wifi_set_promiscuous_rx_cb(&sniffer);
  esp_wifi_set_channel(scanChannel, WIFI_SECOND_CHAN_NONE);

  // Configure the LoRA radio
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DI0);
  if (!LoRa.begin(LORA_BAND * 1E6)) while (1);
};

void loop() {
  pBLEScan->clearResults();
  BLEScanResults foundDevices = pBLEScan->start(BLE_SCAN_TIME, false);
  delay(1000);

  if (scanChannel++ > WIFI_MAX_CH) scanChannel = 1;
  esp_wifi_set_channel(scanChannel, WIFI_SECOND_CHAN_NONE);
  delay(1000);
};
