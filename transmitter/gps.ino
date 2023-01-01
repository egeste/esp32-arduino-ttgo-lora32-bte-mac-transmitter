#include "gps.h"
#include <SparkFun_Ublox_Arduino_Library.h>

#if defined(T_BEAM_V10)
#include <Wire.h>
#include <axp20x.h>
AXP20X_Class axp;
#endif

SFE_UBLOX_GPS myGPS;
int state = 0;
HardwareSerial SerialGPS(1);

void setup_gps() {
  Wire.begin(I2C_SDA, I2C_SCL);

  #if defined(T_BEAM_V10)
    axp.setPowerOutPut(AXP192_LDO3, AXP202_ON); // GPS main power
    axp.setPowerOutPut(AXP192_LDO2, AXP202_ON); // provides power to GPS backup battery
    axp.setPowerOutPut(AXP192_LDO3, AXP202_ON);
    axp.setPowerOutPut(AXP192_DCDC2, AXP202_ON);
    axp.setPowerOutPut(AXP192_EXTEN, AXP202_ON);
    axp.setPowerOutPut(AXP192_DCDC1, AXP202_ON); // enables power to ESP32 on T-beam
    axp.setPowerOutPut(AXP192_DCDC3, AXP202_ON); // I foresee similar benefit for restting T-watch 
                                                 // where ESP32 is on DCDC3 but remember to change I2C pins and GPS pins!
  #endif 

  SerialGPS.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  delay(100);

  do {
    if (myGPS.begin(SerialGPS)) {
      myGPS.setUART1Output(COM_TYPE_NMEA); //Set the UART port to output NMEA only
      myGPS.saveConfiguration(); //Save the current settings to flash and BBR
      myGPS.disableNMEAMessage(UBX_NMEA_GLL, COM_PORT_UART1);
      myGPS.disableNMEAMessage(UBX_NMEA_GSA, COM_PORT_UART1);
      myGPS.disableNMEAMessage(UBX_NMEA_GSV, COM_PORT_UART1);
      myGPS.disableNMEAMessage(UBX_NMEA_VTG, COM_PORT_UART1);
      myGPS.disableNMEAMessage(UBX_NMEA_RMC, COM_PORT_UART1);
      myGPS.enableNMEAMessage(UBX_NMEA_GGA, COM_PORT_UART1);
      myGPS.saveConfiguration(); //Save the current settings to flash and BBR
      break;
    }
    delay(1000);
  } while(1);
}

void loop_gps() {
  if (SerialGPS.available()) {
    print("gps|" + String(SerialGPS.read()));  // print anything comes in from the GPS
  }
}