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
// NSS pin:   16
// DIO0 pin:  15
// DIO1 pin:  15
SX1278 fsk = new Module(16,15,15);

void setup() {
  Serial.begin(115200);

  Serial.print(F("SX127x_Transmit_WemosD1_868_FSK"));
#define CARRIER_FREQUENCY       868.1f  // MHz
#define BITRATE                 48.0 //kbps
#define FREQUENCY_DEVIATION     50.0 //kHz
#define BANDWIDTH               125.0f  // kHz dual sideband
#define OUTPUT_POWER           13      // dBm
#define CURRENT_LIMIT          100     // mA
#define PREAMBLE_LENGTH        8U

  // data shaping:                Gaussian, BT = 0.3
  // sync word:                   0x2D  0x01
  // OOK modulation:              disabled
  //                       float freq, float br, float freqDev, float rxBw, int8_t power, uint8_t currentLimit, uint16_t preambleLength, bool enableOOK
  int state = fsk.beginFSK(CARRIER_FREQUENCY, BITRATE, FREQUENCY_DEVIATION, BANDWIDTH, OUTPUT_POWER, CURRENT_LIMIT, PREAMBLE_LENGTH, 0);
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
  
  // change parameters after start
  state = fsk.setDataShaping(0.5);
  uint8_t syncWord[] = {0x01, 0x23, 0x45, 0x67,
                        0x89, 0xAB, 0xCD, 0xEF};
  state = fsk.setSyncWord(syncWord, 8);
  if (state != ERR_NONE) {
    Serial.print(F("Unable to set configuration, code "));
    Serial.println(state);
    while (true);
  }
}

void loop() {
  Serial.print(F("[SX1278] Transmitting packet ... "));

  // FSK modem can use the same transmit/receive methods
  // as the LoRa modem, even their interrupt-driven versions
  // NOTE: FSK modem maximum packet length is 63 bytes!

  // transmit FSK packet
  int state = fsk.transmit("Hello World!");
  /*
    byte byteArr[] = {0x01, 0x23, 0x45, 0x56,
                      0x78, 0xAB, 0xCD, 0xEF};
    int state = lora.transmit(byteArr, 8);
  */
  if (state == ERR_NONE) {
    Serial.println(F("[SX1278] Packet transmitted successfully!"));
  } else if (state == ERR_PACKET_TOO_LONG) {
    Serial.println(F("[SX1278] Packet too long!"));
  } else if (state == ERR_TX_TIMEOUT) {
    Serial.println(F("[SX1278] Timed out while transmitting!"));
  } else {
    Serial.println(F("[SX1278] Failed to transmit packet, code "));
    Serial.println(state);
  }

  // wait for a second before transmitting again
  delay(10000);
}
