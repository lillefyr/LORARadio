/*
   RadioLib SX127x Transmit Example

   This example transmits packets using SX1276 LoRa radio module.
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

// Pin mapping atmega328p Homebrew only chip! (defined in atmega328p_pinout.png)
//sck    13   standard (pin19)
//miso   12   standard (pin18)
//mosi   11   standard (pin17)
//ss     10   standard (pin16)
//reset  9    choice based on pcb (pin9)
//dio0   3    choice based on pcb (pin5)
//dio1   2    choice based on pcb (pin4)

// SX1276as has the following connections:
#define NSSpin (10)
#define BUSYpin (8)
#define RESETpin (9)
#define DIO1pin (2)
#define DIO0pin (3)
//SX1276 lora = new Module(NSSpin, DIO0pin, RESETpin, DIO1pin ); 
SX1276 lora = new Module(NSSpin, DIO0pin, DIO1pin );

void setup() {
  Serial.begin(115200);

   Serial.print(F("[SX1276] Initializing ... "));
  float DEFAULT_CARRIER_FREQUENCY = 868.1f;
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
  Serial.print(F("[SX1276] Transmitting packet ... "));

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
    Serial.print(F("[SX1276] Datarate:\t"));
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
  delay(1000);
}
