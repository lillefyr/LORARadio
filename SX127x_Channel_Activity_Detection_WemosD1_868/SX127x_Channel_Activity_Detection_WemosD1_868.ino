/*
   RadioLib SX127x Channel Activity Detection Example

   This example scans the current LoRa channel and detects
   valid LoRa preambles. Preamble is the first part of
   LoRa transmission, so this can be used to check
   if the LoRa channel is free, or if you should start
   receiving a message.

   Other modules from SX127x/RFM9x family can also be used.

   For full API reference, see the GitHub Pages
   https://jgromes.github.io/RadioLib/
*/

// include the library
#include <RadioLib.h>

// SX1278 has the following connections:
// NSS pin:   10
// DIO0 pin:  2
// RESET pin: 9
// DIO1 pin:  3
//SX1278 lora = new Module(10, 2, 9, 3);

SX1276 lora = new Module(16, 15, 15);

// or using RadioShield
// https://github.com/jgromes/RadioShield
//SX1278 lora = RadioShield.ModuleA;

void setup() {
  Serial.begin(115200);


  Serial.print(F("SX127x_Channel_Activity_Detection_WemosD1_868"));
#define CARRIER_FREQUENCY       868.1f  // MHz
#define BANDWIDTH               125.0f  // kHz dual sideband
#define SPREADING_FACTOR        7
#define CODING_RATE             8       // 4/8, Extended Hamming
#define OUTPUT_POWER           17      // dBm
#define CURRENT_LIMIT          100     // mA
#define PREAMBLE_LENGTH        8U
#define SYNC_WORD                   0x12 // make sure you have the matching SyncWord in your Groundstation

  int state = lora.begin(CARRIER_FREQUENCY, BANDWIDTH, SPREADING_FACTOR, CODING_RATE, SYNC_WORD, OUTPUT_POWER, PREAMBLE_LENGTH, 0);
  
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
}

void loop() {
//  Serial.print(F("[SX1278] Scanning channel for LoRa preamble ... "));

  // start scanning current channel
  int state = lora.scanChannel();

  if (state == PREAMBLE_DETECTED) {
    // LoRa preamble was detected
    Serial.println(F(" detected preamble!"));

//  } else if (state == CHANNEL_FREE) {
    // no preamble was detected, channel is free
//    Serial.println(F(" channel is free!"));

  }

  // wait 100 ms before new scan
  delay(10);
}
