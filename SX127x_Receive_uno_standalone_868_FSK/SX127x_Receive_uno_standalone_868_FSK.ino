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

// My setup atmega328p standalone
#define MOSI 11
#define MISO 12
#define SCK 13

// My SX1278 has the following connections:
// NSS pin:   10
// DIO0 pin:  2
// DIO1 pin:  3

SX1278 fsk = new Module(10, 2, 3); 

void setup() {
  Serial.begin(9600);

  Serial.println(F("SX127x_Receive_uno_standalone_868_FSK"));
  
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
  Serial.println(F("[SX1278] Waiting for incoming transmission ... "));

  // you can receive data as an Arduino String
  // NOTE: receive() is a blocking method!
  //       See example ReceiveInterrupt for details
  //       on non-blocking reception method.
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
    delay(100);
  } else if (state == ERR_RX_TIMEOUT) {
    //Serial.println(F("[SX1278] Timed out while waiting for packet!"));
    delay(50);
  } else {
    Serial.println(F("[SX1278] Failed to receive packet, code "));
    Serial.println(state);
  }

}
