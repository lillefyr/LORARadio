/*
   RadioLib SX126x Receive Example

   This example listens for LoRa transmissions using SX126x Lora modules.
   To successfully receive data, the following settings have to be the same
   on both transmitter and receiver:
    - carrier frequency
    - bandwidth
    - spreading factor
    - coding rate
    - sync word
    - preamble length

   Other modules from SX126x family can also be used.

   For full API reference, see the GitHub Pages
   https://jgromes.github.io/RadioLib/
*/

// include the library
#include <RadioLib.h>

// SX1262 has the following connections:
// NSS pin:   10
// DIO1 pin:  2
// NRST pin:  3
// BUSY pin:  9
//SX1262 lora = new Module(10, 2, 3, 9);
SX1268 lora = new Module(5, 27, 14, 26);

// or using RadioShield
// https://github.com/jgromes/RadioShield
//SX1262 lora = RadioShield.ModuleA;

void setup() {
  Serial.begin(115200);

  // initialize SX1262 with default settings
  Serial.print(F("SX126x_Receive_lolin_d32_ok"));
  // carrier frequency:           434.0 MHz
  // bandwidth:                   125.0 kHz
  // spreading factor:            9
  // coding rate:                 7
  // sync word:                   0x12 (private network)
  // output power:                14 dBm
  // current limit:               60 mA
  // preamble length:             8 symbols
  // TCXO voltage:                1.6 V (set to 0 to not use TCXO)
  // regulator:                   DC-DC (set to true to use LDO)
  // CRC:                         enabled
#define LORA_CARRIER_FREQUENCY                          436.7f  // 413.9f  // MHz
#define LORA_BANDWIDTH                                  125.0f  // kHz dual sideband
#define LORA_SPREADING_FACTOR                           11
#define LORA_CODINGRATE                                  5     // 4/8, Extended Hamming
#define LORA_OUTPUT_POWER                                5     // dBm
#define LORA_CURRENT_LIMIT                              120    // mA
#define SYNC_WORD_6X                                    0x12   // RadioLib uses uint8_t for SX126x
#define LORA_PREAMBLE_LENGTH                             8     // Same for Tx and Rx
#define TCXO_VOLTAGE                                    0.0    // allowed voltages

  int state = lora.begin(LORA_CARRIER_FREQUENCY,
                          LORA_BANDWIDTH,
                          LORA_SPREADING_FACTOR,
                          LORA_CODINGRATE,
                          SYNC_WORD_6X,
                          LORA_OUTPUT_POWER,
                          LORA_PREAMBLE_LENGTH,
                          TCXO_VOLTAGE);
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
}

void loop() {
  Serial.print(F("[SX1262] Waiting for incoming transmission ... "));

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
    Serial.print(F("[SX1262] Data:\t\t"));
    Serial.println(str);

    // print the RSSI (Received Signal Strength Indicator)
    // of the last received packet
    float RSSI = lora.getRSSI();
    Serial.print(F("[SX1262] RSSI:\t\t"));
    Serial.print(RSSI);
    Serial.println(F(" dBm"));

    // print the SNR (Signal-to-Noise Ratio)
    // of the last received packet
    float SNR = lora.getSNR();
    Serial.print(F("[SX1262] SNR:\t\t"));
    Serial.print(SNR);
    Serial.println(F(" dB"));

//    char outbuf[20];
//    for (int i=0;i<20;i++){outbuf[i]=0x00;}
//    sprintf(outbuf, "RSSI=%f SNR=%f",RSSI,SNR);
//    Serial.print(outbuf);
//    state = lora.transmit(outbuf);

  } else if (state == ERR_RX_TIMEOUT) {
    // timeout occurred while waiting for a packet
    Serial.println(F("timeout!"));

  } else if (state == ERR_CRC_MISMATCH) {
    // packet was received, but is malformed
    Serial.println(F("CRC error!"));

  } else {
    // some other error occurred
    Serial.print(F("failed, code "));
    Serial.println(state);

  }
}
