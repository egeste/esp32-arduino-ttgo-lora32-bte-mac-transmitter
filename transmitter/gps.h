#pragma once

//#define T_BEAM_V07  // AKA Rev0 (first board released)
#define T_BEAM_V10  // AKA Rev1 for board versions T-beam_V1.0 and V1.1 (second board released)

#if defined(T_BEAM_V07)
#define GPS_RX_PIN      12
#define GPS_TX_PIN      15
#elif defined(T_BEAM_V10)
#define I2C_SDA         21
#define I2C_SCL         22
#define GPS_RX_PIN      34
#define GPS_TX_PIN      12
#endif