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
// RESET pin: 9
// DIO1 pin:  3
//SX1278 radio = new Module(10, 2, 9, 3);
SX1276 radio = new Module(10, 2, 3);


void setup() {
  Serial.begin(115200);

  // initialize SX1278 with default settings
  Serial.println(F("SX127x_Transmit_Arduino_Uno"));
  // carrier frequency:           434.0 MHz
  // bandwidth:                   125.0 kHz
  // spreading factor:            9
  // coding rate:                 7
  // sync word:                   0x12
  // output power:                17 dBm
  // current limit:               100 mA
  // preamble length:             8 symbols
  // amplifier gain:              0 (automatic gain control)
#define CARRIER_FREQUENCY                          868.1 //436.7f  // MHz
#define BANDWIDTH                                  125.0f  // kHz dual sideband
#define SPREADING_FACTOR                           11
#define CODINGRATE                                  8     // 4/8, Extended Hamming
#define OUTPUT_POWER                               20     // dBm
#define CURRENT_LIMIT                              120    // mA
#define SYNC_WORD                                       0x12   // RadioLib uses uint8_t for SX126x
#define PREAMBLE_LENGTH                             8     // Same for Tx and Rx


  int state = radio.begin(CARRIER_FREQUENCY,
                          BANDWIDTH,
                          SPREADING_FACTOR,
                          CODINGRATE,
                          SYNC_WORD,
                          OUTPUT_POWER,
                          (uint8_t)CURRENT_LIMIT);
//for sx1262                          PREAMBLE_LENGTH);
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
  Serial.println(CARRIER_FREQUENCY);
  
  // you can transmit C-string or Arduino string up to
  // 256 characters long
  // NOTE: transmit() is a blocking method!
  //       See example SX127x_Transmit_Interrupt for details
  //       on non-blocking transmission method.
  int state = radio.transmit("Arduino LoraHat says Hello World!");

  // you can also transmit byte array up to 256 bytes long
  /*
    byte byteArr[] = {0x01, 0x23, 0x45, 0x56, 0x78, 0xAB, 0xCD, 0xEF};
    int state = radio.transmit(byteArr, 8);
  */

  if (state == ERR_NONE) {
    // the packet was successfully transmitted
    Serial.println(F(" success!"));

    // print measured data rate
    Serial.print(F("[SX1278] Datarate:\t"));
    Serial.print(radio.getDataRate());
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
  delay(10000);
}
