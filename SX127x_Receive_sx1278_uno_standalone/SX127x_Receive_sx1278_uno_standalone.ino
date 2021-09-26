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

SX1278 lora = new Module(10, 2, 3); 

void setup() {
  Serial.begin(9600);

  delay(1000);

  Serial.print(F("SX127x_Receive_sx1278_uno_standalone"));
  float CONNECTED_BANDWIDTH = 7.8f;
  
#define CARRIER_FREQUENCY       868.1f  // MHz
#define BANDWIDTH               125.0f  // kHz dual sideband
#define SPREADING_FACTOR        7
#define CODING_RATE             8       // 4/8, Extended Hamming
#define OUTPUT_POWER           17      // dBm
#define CURRENT_LIMIT          100     // mA
#define PREAMBLE_LENGTH        8U
#define SYNC_WORD                   0x12 // make sure you have the matching SyncWord in your Groundstation

  int state = lora.begin(CARRIER_FREQUENCY, BANDWIDTH, SPREADING_FACTOR, CODING_RATE, SYNC_WORD, OUTPUT_POWER, CURRENT_LIMIT, PREAMBLE_LENGTH, 0);
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
    //Serial.println(F("timeout!"));
    delay(100);

  } else if (state == ERR_CRC_MISMATCH) {
    // packet was received, but is malformed
    Serial.println(F("CRC error!"));

  } else {
    // some other error occurred
    Serial.print(F("failed, code "));
    Serial.println(state);
  }
}
