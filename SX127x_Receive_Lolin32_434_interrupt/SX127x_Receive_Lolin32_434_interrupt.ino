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

String str;

void setup() {
  Serial.begin(115200);

  Serial.println(F("\nSX127x_Receive_Lolin32_434"));
#define CARRIER_FREQUENCY       434.0 // 436.7  // MHz
#define BANDWIDTH               125.0  // kHz dual sideband
#define SPREADING_FACTOR        7
#define CODING_RATE             5       // 4/8, Extended Hamming
#define OUTPUT_POWER           20      // dBm
#define CURRENT_LIMIT          120     // mA
#define PREAMBLE_LENGTH        12
#define SYNC_WORD                   0x12 // make sure you have the matching SyncWord in your Groundstation
#define GAIN                    1

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

  uint8_t   len = 0;
  /*
  while(len == 0){
    state = radio.scanChannel();
    if (state == ERR_NONE) {
      Serial.println(F("Channel busy"));
      delay(20);
    }else {
      Serial.print(F("scan Channel failed, code "));
      Serial.println(state);
      delay(30);
    }
  }
*/
  state = ERR_RX_TIMEOUT;
  while  ( state != ERR_NONE ){
    radio.setDio0Action(setFlag);
    state = radio.startReceive(len, SX127X_RXCONTINUOUS);
    if (state == ERR_NONE) {
      Serial.println(F("Start receive success!"));
    } else {
      Serial.print(F("receive failed, code "));
      Serial.println(state);
      delay(3000);
    }
  }
}

// flag to indicate that a packet was received
volatile bool receivedFlag = false;

// disable interrupt when it's not needed
volatile bool enableInterrupt = true;

// this function is called when a complete packet
// is received by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
void setFlag(void) {
  // check if the interrupt is enabled
  if(!enableInterrupt) {
    return;
  }

  // we got a packet, set the flag
  receivedFlag = true;
}

void loop() {

  if(receivedFlag) {
    // disable the interrupt service routine while
    // processing the data
    enableInterrupt = false;

    // reset flag
    receivedFlag = false;
    
    // you can read received data as an Arduino String
    int state = radio.readData(str);

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
      Serial.print(radio.getRSSI());
      Serial.println(F(" dBm"));
  
      // print the SNR (Signal-to-Noise Ratio)
      // of the last received packet
      Serial.print(F("[SX1278] SNR:\t\t\t"));
      Serial.print(radio.getSNR());
      Serial.println(F(" dB"));
  
      // print frequency error
      // of the last received packet
      Serial.print(F("[SX1278] Frequency error:\t"));
      Serial.print(radio.getFrequencyError());
      Serial.println(F(" Hz"));
  
    } else if (state == ERR_RX_TIMEOUT) {
      // timeout occurred while waiting for a packet
      Serial.println(F("timeout!"));  
    } else if (state == ERR_CRC_MISMATCH) {
      // packet was received, but is malformed
      Serial.println(F("CRC error!"));
    }

    radio.startReceive();

    // we're ready to receive more packets,
    // enable interrupt service routine
    enableInterrupt = true;
  }
}
