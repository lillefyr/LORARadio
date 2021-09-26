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

// SX1276 has the following connections:
// NSS pin:   10
// DIO0 pin:  2
// RESET pin: 9
// DIO1 pin:  3
//SX1276 lora = new Module(10, 2, 9, 3);
//SX1276 lora = new Module(10, 2, 3);

// Norbi Simulator
// Debug enabled
// MISO=12
// MOSI=11
// SCL =19
// NSS =10
// SYNC_WORD =12
// Initializing ... success!
// 
// Carrier Frequency:436.70
// Spreading factor: 10
// Coding rate:      5

SX1276 lora = new Module(10, 2, 6);  // Arduino with LoraHat

// or using RadioShield
// https://github.com/jgromes/RadioShield
//SX1278 lora = RadioShield.ModuleA;

void setup() {
  Serial.begin(115200);

  // initialize SX1278
  Serial.print(F("SX127x_Transmit_LoRaHat_ok"));

  pinMode(LED_BUILTIN, OUTPUT);

#define LORA_CARRIER_FREQUENCY                          436.7f  // 413.9f  // MHz
#define LORA_BANDWIDTH                                  125.0f  // kHz dual sideband
#define LORA_SPREADING_FACTOR                           11
#define LORA_CODINGRATE                                  8     // 4/8, Extended Hamming
#define LORA_OUTPUT_POWER                                10     // dBm
#define LORA_CURRENT_LIMIT                              120    // mA
#define SYNC_WORD_6X                                    0x12   // RadioLib uses uint8_t for SX126x
#define LORA_PREAMBLE_LENGTH                             8     // Same for Tx and Rx


  int state = lora.begin(LORA_CARRIER_FREQUENCY,
                          LORA_BANDWIDTH,
                          LORA_SPREADING_FACTOR,
                          LORA_CODINGRATE,
                          SYNC_WORD_6X,
                          LORA_OUTPUT_POWER,
                          LORA_PREAMBLE_LENGTH);
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
}

int cnt=0;
char outbuf[12];
void loop() {
  Serial.print(F("[SX1278] Transmitting packet ... "));

  // you can transmit C-string or Arduino string up to
  // 256 characters long
  // NOTE: transmit() is a blocking method!
  //       See example SX127x_Transmit_Interrupt for details
  //       on non-blocking transmission method.
  
  sprintf(outbuf, "Msgno=%d",cnt++);
  Serial.print(outbuf);
  
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  Serial.println("ON");
  int state = lora.transmit(outbuf);
  
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  Serial.println("OFF");

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
  delay(3000);
}
