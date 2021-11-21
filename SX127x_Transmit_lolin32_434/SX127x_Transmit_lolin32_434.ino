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
#define PIN_SPI_SS   (5)
#define PIN_SPI_MOSI (23)
#define PIN_SPI_MISO (19)
#define PIN_SPI_SCK  (18)

#define PIN_D0       (12)
#define PIN_RESET    (14)
#define PIN_D1       (25) // 27 seems not to work as output
#define PIN_D2       (26)

// Module( cs, irq, rst, RADIOLIB_PIN_TYPE gpio
SX1278 radio = new Module(5, 12, 14, 25, 26);
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

  // initialize SX1276
  Serial.print(F("SX127x_Transmit_lolin32_443"));

#define CARRIER_FREQUENCY          436.7  // 413.9  // MHz
#define BANDWIDTH                  125.0  // kHz dual sideband
#define SPREADING_FACTOR           7
#define CODING_RATE                5     // 4/8, Extended Hamming
#define OUTPUT_POWER               20     // dBm
#define CURRENT_LIMIT              120    // mA
#define PREAMBLE_LENGTH            12     // Same for Tx and Rx
#define SYNC_WORD                  0x12   // RadioLib uses uint8_t for SX126x
#define GAIN                       1

  Serial.print ("CARRIER_FREQUENCY: "); Serial.println (CARRIER_FREQUENCY);
  Serial.print ("BANDWIDTH: "); Serial.println (BANDWIDTH);
  Serial.print ("SPREADING_FACTOR: "); Serial.println (SPREADING_FACTOR);
  Serial.print ("CODING_RATE: "); Serial.println (CODING_RATE);
  Serial.print ("PREAMBLE_LENGTH: "); Serial.println (PREAMBLE_LENGTH);
  Serial.print ("SYNC_WORD: "); Serial.println (SYNC_WORD);

  int state = ERR_RX_TIMEOUT;

  SPI.begin();

  while ( state != ERR_NONE ){
    state = radio.begin(CARRIER_FREQUENCY, 
                        BANDWIDTH, 
                        SPREADING_FACTOR, 
                        CODING_RATE, 
                        SYNC_WORD, 
                        OUTPUT_POWER, 
                        PREAMBLE_LENGTH, 
                        GAIN);
    if (state == ERR_NONE) {
      Serial.println(F("radio connect success!"));
    } else {
      Serial.print(F("radio connect failed, code "));
      Serial.println(state);

      if ( state == ERR_INVALID_FREQUENCY ){
        Serial.println("ERR_INVALID_FREQUENCY");
      }
      else {
        if ( state == ERR_CHIP_NOT_FOUND ){
          Serial.println("ERR_CHIP_NOT_FOUND");
        }
      }
      delay(3000);
    }
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
  int state = radio.transmit(outbuf);

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
  delay(1000);
}
