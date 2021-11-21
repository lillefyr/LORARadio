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
#define PIN_SPI_SS   (5)
#define PIN_SPI_MOSI (23)
#define PIN_SPI_MISO (19)
#define PIN_SPI_SCK  (18)

#define PIN_D0       (14)
#define PIN_RESET    (12)
#define PIN_D1       (25) // 27 seems not to work as output
#define PIN_D2       (26)

SX1278 lora = new Module(5, 14, 12, 25, 26);
/*
RFM95 freq, 862.0, 1020.0      *)
RFM96 freq, 410.0, 525.0
SX1262 freq, 150.0, 960.0
SX1268 freq, 410.0, 810.0
SX1272 freq, 860.0, 1020.0
SX1276 freq, 137.0, 1020.0     *)
SX1277 freq, 137.0, 1020.0
SX1278 freq, 137.0, 525.0
SX1279 freq, 137.0, 960.0
*/

/*
static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14; // SCK
static const uint8_t D6   = 12; // MISO
static const uint8_t D7   = 13; // MOSI
static const uint8_t D8   = 15; // NSS
static const uint8_t D9   = 3;
static const uint8_t D10  = 1;
*/


void setup() {
  Serial.begin(115200);

  Serial.println(F("\nSX127x_Receive_Lolin32_434"));
#define CARRIER_FREQUENCY       434.0 // 436.7  // MHz
#define BANDWIDTH               125.0  // kHz dual sideband
#define SPREADING_FACTOR        11
#define CODING_RATE             8       // 4/8, Extended Hamming
#define OUTPUT_POWER           20      // dBm
#define CURRENT_LIMIT          120     // mA
#define PREAMBLE_LENGTH        8
#define SYNC_WORD                   0x12 // make sure you have the matching SyncWord in your Groundstation
#define GAIN                    0

Serial.print ("CARRIER_FREQUENCY: "); Serial.println (CARRIER_FREQUENCY);
Serial.print ("BANDWIDTH: "); Serial.println (BANDWIDTH);
Serial.print ("SPREADING_FACTOR: "); Serial.println (SPREADING_FACTOR);
Serial.print ("CODING_RATE: "); Serial.println (CODING_RATE);
Serial.print ("PREAMBLE_LENGTH: "); Serial.println (PREAMBLE_LENGTH);
Serial.print ("SYNC_WORD: "); Serial.println (SYNC_WORD);

  int state = ERR_RX_TIMEOUT;
  char outByte [20] = "TEST SPI MESSAGE\n";
  char inByte;

  SPI.begin();

  while ( state != ERR_NONE ){
    state = lora.begin(CARRIER_FREQUENCY, 
                        BANDWIDTH, 
                        SPREADING_FACTOR, 
                        CODING_RATE, 
                        SYNC_WORD, 
                        OUTPUT_POWER, 
                        PREAMBLE_LENGTH, 
                        GAIN);
    if (state == ERR_NONE) {
      Serial.println(F("success!"));
    } else {
      Serial.print(F("failed, code "));
      Serial.println(state);

      if ( state == ERR_INVALID_FREQUENCY ){
        Serial.println("ERR_INVALID_FREQUENCY");
      }
      delay(3000);
    }
  }
}

void loop() {
//  Serial.print(F("[SX1278] Waiting for incoming transmission ... "));

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
    Serial.println(F("packet received!"));

    if ( str.length() > 0 ) {
      // print the data of the packet
      Serial.print(F("[SX1278] Data:\t\t\t"));
      Serial.println(str);
    }

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
  }
}
