/*
   RadioLib SX126x Transmit Example

   This example transmits packets using SX1262 LoRa radio module.
   Each packet contains up to 256 bytes of data, in the form of:
    - Arduino String
    - null-terminated char array (C-string)
    - arbitrary binary data (byte array)

   Other modules from SX126x family can also be used.

   For full API reference, see the GitHub Pages
   https://jgromes.github.io/RadioLib/
*/

// include the library
#include <RadioLib.h>

#define USE_SX126X                    // uncomment to use SX126x

// SX1262 has the following connections:
// NSS pin:   10
// DIO1 pin:  2
// NRST pin:  3
// BUSY pin:  9
//SX1268 radio = new Module(5, 14, 27, 26);  //Port-Pin Input:  SPI select, Reset, Busy, Interrupt DIO1
SX1268 radio = new Module(5, 27, 14, 26);

// or using RadioShield
// https://github.com/jgromes/RadioShield
//SX1262 lora = RadioShield.ModuleA;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println(F("SX126x_Transmit_lolind32"));
  
  // initialize SX1262 with default settings
  // carrier frequency:           434.0 MHz
  // bandwidth:                   125.0 kHz
  // spreading factor:            9
  // coding rate:                 7
  // sync word:                   0x12 (private network)
  // output power:                14 dBm
  // current limit:               60 mA
  // preamble length:             8 symbols
  // TCXO voltage:                1.6 V (set to 0 to not use TCXO)
  // CRC:                         enabled
//  int state = radio.begin();
//#define LORA_CARRIER_FREQUENCY                          436.7f  // MHz
//#define LORA_BANDWIDTH                                  125.0f  // kHz dual sideband
//#define LORA_SPREADING_FACTOR                           11
//#define LORA_CODINGRATE                                  8     // 4/8, Extended Hamming
//#define LORA_OUTPUT_POWER                               20     // dBm
//#define LORA_CURRENT_LIMIT                              120    // mA
//#define SYNC_WORD_6X                                    0x12   // RadioLib uses uint8_t for SX126x
//#define LORA_PREAMBLE_LENGTH                             8     // Same for Tx and Rx
//#define TCXO_VOLTAGE                                    0.0

//  int state = radio.begin(LORA_CARRIER_FREQUENCY,
//                          LORA_BANDWIDTH,
//                          LORA_SPREADING_FACTOR,
//                          LORA_CODINGRATE,
//                          SYNC_WORD_6X,
//                          LORA_OUTPUT_POWER,
//                          (uint8_t)LORA_CURRENT_LIMIT,
//                          LORA_PREAMBLE_LENGTH,
//                          TCXO_VOLTAGE);

#define FREQUENCY             436.7   // MHz
#define BANDWIDTH             125.0   // kHz
#define SPREADING_FACTOR      11      // -
#define CODING_RATE           8       // 4/8
#define SYNC_WORD             0x1424  // used as LoRa "sync word", or twice repeated as FSK sync word (0x1212)
#define OUTPUT_POWER          20      // dBm
#define CURRENT_LIMIT         120     // mA
#define LORA_PREAMBLE_LEN     8       // symbols
#define BIT_RATE              9.6     // kbps
#define FREQ_DEV              5.0     // kHz SSB
#define RX_BANDWIDTH          39.0    // kHz SSB
#define FSK_PREAMBLE_LEN      16      // bits
#define DATA_SHAPING          0.5     // BT product
#define TCXO_VOLTAGE          0.0     // I have no working TCXO   1.6     // volts

  int state = radio.begin(FREQUENCY,
                          BANDWIDTH,
                          SPREADING_FACTOR,
                          CODING_RATE,
                          SYNC_WORD,
                          OUTPUT_POWER,
                          CURRENT_LIMIT,
                          LORA_PREAMBLE_LEN,
                          TCXO_VOLTAGE);
  radio.setCRC(true);


  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
}

void loop() {
  Serial.println(F("[SX1262] Transmitting packet ... "));

  // you can transmit C-string or Arduino string up to
  // 256 characters long
  // NOTE: transmit() is a blocking method!
  //       See example SX126x_Transmit_Interrupt for details
  //       on non-blocking transmission method.
  int state = radio.transmit("lolind32 says Hello World!");

  // you can also transmit byte array up to 256 bytes long
  /*
    byte byteArr[] = {0x01, 0x23, 0x45, 0x56, 0x78, 0xAB, 0xCD, 0xEF};
    int state = radio.transmit(byteArr, 8);
  */

  if (state == ERR_NONE) {
    // the packet was successfully transmitted
    Serial.println(F("success!"));

    // print measured data rate
    Serial.print(F("[SX1262] Datarate:\t"));
    Serial.print(radio.getDataRate());
    Serial.println(F(" bps"));

  } else if (state == ERR_PACKET_TOO_LONG) {
    // the supplied packet was longer than 256 bytes
    Serial.println(F("too long!"));

  } else if (state == ERR_TX_TIMEOUT) {
    // timeout occured while transmitting packet
    Serial.println(F("timeout!"));

  } else {
    // some other error occurred
    Serial.print(F("failed, code "));
    Serial.println(state);

  }

  // wait for a second before transmitting again
  delay(10000);
}
