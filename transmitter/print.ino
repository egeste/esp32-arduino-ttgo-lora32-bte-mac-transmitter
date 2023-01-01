#include "print.h"

void setup_print() {
  Serial.begin(115200);

  #ifdef TRANSMIT_LORA
  setup_lora();
  #endif
}

void print(String input) {
  Serial.println(input);

  #ifdef TRANSMIT_LORA
  print_lora(input);
  #endif
};