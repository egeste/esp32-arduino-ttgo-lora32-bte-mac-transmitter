#include <SPI.h>
#include <LoRa.h>
#include "lora.h"

void setup_lora() {
  // Configure the LoRA radio
  SPI.begin(SCK_GPIO, MISO_GPIO, MOSI_GPIO, NSS_GPIO);
  LoRa.setPins(NSS_GPIO, RESET_GPIO, DIO0_GPIO);
  if (!LoRa.begin(LORA_BAND * 1E6)) while (1);
}

void print_lora(String input) {
  LoRa.beginPacket();
  LoRa.print(input);
  LoRa.endPacket();
};