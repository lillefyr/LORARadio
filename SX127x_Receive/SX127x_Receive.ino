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
// NSS pin:   10
// DIO0 pin:  2
// DIO1 pin:  3
//SX1278 lora = new Module(10, 2, 6);
SX1278 lora = new Module(5, 26, 12); // lolin32dev

// or using RadioShield
// https://github.com/jgromes/RadioShield
//SX1278 lora = RadioShield.ModuleA;

void setup() {
  Serial.begin(9600);

  Serial.print(F("[SX1278] Initializing ... "));
  float CARRIER_FREQUENCY = 436.7f;
  float DEFAULT_CARRIER_FREQUENCY = 436.7f;
  float BANDWIDTH = 125.0f;
  float CONNECTED_BANDWIDTH = 7.8f;
  int   SPREADING_FACTOR = 11;  // defined by fossasat-1
  int   CODING_RATE = 8; // can be 4 or 8.
  char  SYNC_WORD = 0xff;  //0f0f for SX1262
  int   OUTPUT_POWER = 17; // dBm
  // current limit:               100 mA
  // preamble length:             8 symbols
  // amplifier gain:              0 (automatic gain control)

  int state = lora.begin(DEFAULT_CARRIER_FREQUENCY, BANDWIDTH, SPREADING_FACTOR, CODING_RATE, SYNC_WORD, OUTPUT_POWER, 100, 8, 0);
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

  // you can receive data as an Arduino String
  // NOTE: receive() is a blocking method!
  //       See example ReceiveInterrupt for details
  //       on non-blocking reception method.
  String str;
  int state = lora.receive(str);

  // you can also receive data as byte array
  /*
    byte byteArr[8];
    int state = lora.receive(byteArr, 8);
  */

  if (state == ERR_NONE) {
    // packet was successfully received
    Serial.println(F("success!"));

    // print the data of the packet
    Serial.print(F("[SX1278] Data:\t\t\t"));
    Serial.println(str);

    // print the RSSI (Received Signal Strength Indicator)
    // of the last received packet
    Serial.print(F("[SX1278] RSSI:\t\t\t"));
    Serial.print(lora.getRSSI());
    Serial.println(F(" dBm"));

    // print the SNR (Signal-to-Noise Ratio)
    // of the last received packet
    Serial.print(F("[SX1278] SNR:\t\t\t"));
    Serial.print(lora.getSNR());
    Serial.println(F(" dB"));

    // print frequency error
    // of the last received packet
    Serial.print(F("[SX1278] Frequency error:\t"));
    Serial.print(lora.getFrequencyError());
    Serial.println(F(" Hz"));

  } else if (state == ERR_RX_TIMEOUT) {
    // timeout occurred while waiting for a packet
    Serial.println(F("timeout!"));
    delay(10);

  } else if (state == ERR_CRC_MISMATCH) {
    // packet was received, but is malformed
    Serial.println(F("CRC error!"));

  } else {
    // some other error occurred
    Serial.print(F("failed, code "));
    Serial.println(state);

  }
}
