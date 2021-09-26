/*
   RadioLib SX127x Transmit Example

   This example transmits packets using SX1278 LoRa radio module.
   Each packet contains up to 256 bytes of data, in the form of:
    - Arduino String
    - null-terminated char array (C-string)
    - arbitrary binary data (byte array)

   Other modules from SX127x/RFM9x family can also be used.

   For full API reference, see the GitHub Pages
   https://jgromes.github.io/RadioLib/
*/

// include the library
#include <RadioLib.h>

// SX1278 has the following connections:
// NSS pin:   10
// DIO0 pin:  2
// DIO1 pin:  3
SX1276 lora = new Module(16,15,15);

// or using RadioShield
// https://github.com/jgromes/RadioShield
//SX1278 lora = RadioShield.ModuleA;

void setup() {
  Serial.begin(115200);

   Serial.print(F("SX127x_Transmit_WemosD1_868"));
#define CARRIER_FREQUENCY       868.1  // MHz
#define BANDWIDTH               125.0  // kHz dual sideband
#define SPREADING_FACTOR        7
#define CODING_RATE             8      // 4/8, Extended Hamming
#define OUTPUT_POWER            17      // dBm
#define CURRENT_LIMIT           100     // mA
#define PREAMBLE_LENGTH         8
#define SYNC_WORD               0x12 // make sure you have the matching SyncWord in your Groundstation
#define GAIN                    0

  int16_t state = lora.begin(CARRIER_FREQUENCY, BANDWIDTH, SPREADING_FACTOR, CODING_RATE, SYNC_WORD, OUTPUT_POWER, PREAMBLE_LENGTH, GAIN);
// int16_t SX1276::begin(float freq, float bw, uint8_t sf, uint8_t cr, uint8_t syncWord, int8_t power, uint16_t preambleLength, uint8_t gain)
//              int16_t begin(float freq = 434.0, float bw = 125.0, uint8_t sf = 9, uint8_t cr = 7, uint8_t syncWord = SX127X_SYNC_WORD, int8_t power = 10, uint16_t preambleLength = 8, uint8_t gain = 0);

   
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
}

void loop() {
  Serial.print(F("[SX1278] Transmitting packet ... "));

  // you can transmit C-string or Arduino string up to
  // 256 characters long
  // NOTE: transmit() is a blocking method!
  //       See example SX127x_Transmit_Interrupt for details
  //       on non-blocking transmission method.
  int state = lora.transmit("Hello World!");

  // you can also transmit byte array up to 256 bytes long
  /*
    byte byteArr[] = {0x01, 0x23, 0x45, 0x56, 0x78, 0xAB, 0xCD, 0xEF};
    int state = lora.transmit(byteArr, 8);
  */

  if (state == ERR_NONE) {
    // the packet was successfully transmitted
    Serial.println(F(" success!"));

    // print measured data rate
    Serial.print(F("[SX1278] Datarate:\t"));
    Serial.print(lora.getDataRate());
    Serial.println(F(" bps"));

  } else if (state == ERR_PACKET_TOO_LONG) {
    // the supplied packet was longer than 256 bytes
    Serial.println(F(" too long!"));

  } else if (state == ERR_TX_TIMEOUT) {
    // timeout occured while transmitting packet
    Serial.println(F(" timeout!"));

  } else {
    // some other error occurred
    Serial.print(F("failed, code "));
    Serial.println(state);

  }

  // wait for a second before transmitting again
  delay(5000);
}
