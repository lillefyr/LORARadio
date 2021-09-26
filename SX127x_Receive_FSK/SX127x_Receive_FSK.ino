/*
   RadioLib SX127x Receive Example

   This example listens for LoRa transmissions using SX127x Lora modules.
   To successfully receive data, the following settings have to be the same
   on both transmitter and receiver:
    - carrier frequency
    - bandwidth
    - spreading factor
    - coding rate
    - sync word
    - preamble length

   Other modules from SX127x/RFM9x family can also be used.

   For full API reference, see the GitHub Pages
   https://jgromes.github.io/RadioLib/
*/

// include the library
#include <RadioLib.h>

// SX1278 has the following connections:
SX1278 fsk = new Module(10, 2, 9, 3);

// or using RadioShield
// https://github.com/jgromes/RadioShield
//SX1278 lora = RadioShield.ModuleA;

void setup() {
  Serial.begin(9600);

  // initialize SX1278 FSK modem with default settings
  Serial.print(F("SX127x_Receive_FSK"));
  // carrier frequency:           434.0 MHz
  // bit rate:                    48.0 kbps
  // frequency deviation:         50.0 kHz
  // Rx bandwidth:                125.0 kHz
  // output power:                13 dBm
  // current limit:               100 mA
  // data shaping:                Gaussian, BT = 0.3
  // sync word:                   0x2D  0x01
  // OOK modulation:              disabled
  // int16_t beginFSK(
  //float freq = 434.0,
  //float br = 48.0,
  //float freqDev = 50.0,
  //float rxBw = 125.0,
  //int8_t power = 13,
  //uint8_t currentLimit = 100,
  //uint16_t preambleLength = 16,
  //bool enableOOK = false);

  int state = fsk.beginFSK( 434.0, 48.0, 50.0, 125, 13, 100, 16, false);
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
}

void loop() {
  Serial.print(F("[SX1278] Waiting for incoming transmission ... "));

  // receive FSK packet
  String str;
  int state = fsk.receive(str);
  /*
    byte byteArr[8];
    int state = lora.receive(byteArr, 8);
  */
  if (state == ERR_NONE) {
    Serial.println(F("[SX1278] Received packet!"));
    Serial.print(F("[SX1278] Data:\t"));
    Serial.println(str);
  } else if (state == ERR_RX_TIMEOUT) {
    Serial.println(F("[SX1278] Timed out while waiting for packet!"));
  } else {
    Serial.println(F("[SX1278] Failed to receive packet, code "));
    Serial.println(state);
  }

  
  state = fsk.disableAddressFiltering();
  if (state != ERR_NONE) {
    Serial.println(F("Unable to remove address filter, code "));
  }
}
