#include <SPI.h>
#include <LoRa.h>
#include <SSD1306.h>

#define LORA_BAND    915

#define OLED_SDA    4
#define OLED_SCL    15
#define OLED_RST    16

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)

SSD1306 display(0x3c, OLED_SDA, OLED_SCL);

// Forward declarations
void displayLoraData(int packetSize, String packet, String rssi);

void setup() {
  Serial.begin(115200);

  // Configure OLED by setting the OLED Reset HIGH, LOW, and then back HIGH
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, HIGH);
  delay(100);
  digitalWrite(OLED_RST, LOW);
  delay(100);
  digitalWrite(OLED_RST, HIGH);

  display.init();
  // display.flipScreenVertically();

  // Configure the LoRA radio
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DI0);
  if (!LoRa.begin(LORA_BAND * 1E6)) {
    while (1);
  }

  // Set the radio into receive mode
  LoRa.receive();
}

void loop() {
  int packetSize = LoRa.parsePacket();

  if (packetSize) {
    String packSize = String(packetSize, DEC);

    String packet = "";
    for (int i = 0; i < packetSize; i++) {
      char packetByte = LoRa.read();
      packet += packetByte;
    }

    Serial.print(packet);
    Serial.println();

    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawStringMaxWidth(0, 0, display.getWidth(), packet);
    display.display();
  }

  while(Serial.available()) {
    LoRa.beginPacket();
    LoRa.print(Serial.readString());
    LoRa.endPacket();
  }
}
