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
SX1278 fsk = new Module(10, 2, 9, 3);

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
    Serial.println("interrupt disabled");
    return;
  }

  Serial.println("got a packet");
  
  // we got a packet, set the flag
  receivedFlag = true;
}

void setup() {
  Serial.begin(9600);

  // initialize SX1278
  Serial.println(F("SX127x_Receive_LoRaHat_ok"));

#define FREQUENCY                 434.0   //436.7f  // 413.9f  // MHz
#define BITRATE                   9.6f    // 48.0 Kbit
#define FREQUENCY_DEVIATION       5.0f   //  50.0f
#define RX_BANDWIDTH              125.0f  // kHz dual sideband
#define OUTPUT_POWER              13   // dBm
#define CURRENT_LIMIT             140    // mA
#define PREAMBLE_LENGTH           16     // Same for Tx and Rx
#define DATA_SHAPING              0.5f    // GFSK filter BT product
#define TCXO_VOLTAGE              0.0f    // allowed voltages
  // regulator:                   DC-DC (set to true to use LDO)
  // sync word:                   0x2D  0x01
  // CRC:                         enabled, CRC16 (CCIT)
#define enableOOK                 false

  //int state = fsk.beginFSK( 434.0, 9.6, 5.0, 125.0, 13, 100, 16, false );
  int state = fsk.beginFSK( FREQUENCY, BITRATE, FREQUENCY_DEVIATION, RX_BANDWIDTH, OUTPUT_POWER, CURRENT_LIMIT, PREAMBLE_LENGTH, false );
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  uint8_t syncWordFSK[2] = { 0x2D, 0x01 }; //{SYNC_WORD, SYNC_WORD};
  fsk.setSyncWord(syncWordFSK, 2);
  fsk.setCRC(2);

  // set the function that will be called
  // when new packet is received
  // DIO0 will generate interrupt when data are available (on sx1278)
  // DIO1 will generate interrupt when data are available (on sx1268)
  fsk.clearDio0Action();
  fsk.clearDio1Action();
  fsk.setDio0Action(setFlag);
  
  // start listening for LoRa packets
  Serial.print(F("[SX1278] Starting to listen ... "));
  state = fsk.startReceive();
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
  
  enableInterrupt = true;
  receivedFlag = false;
}

void loop() {
 // Serial.print(F("[SX1278] Waiting for incoming transmission ... "));

 // check if the flag is set
  if(receivedFlag) {
    // disable the interrupt service routine while
    // processing the data
    enableInterrupt = false;
    fsk.clearDio0Action();
    
    // reset flag
    receivedFlag = false;

    // you can read received data as an Arduino String
    String str;
    int state = fsk.readData(str);
    if (state == ERR_NONE) {
      Serial.println(F("[SX1278] Received packet!"));
      Serial.print(F("[SX1278] Data:\t"));
      Serial.println(str);
    } else if (state == ERR_RX_TIMEOUT) {
      Serial.println(F("[SX1278] Timed out while waiting for packet!"));
    } else {
      Serial.println(F("[SX1278] Failed to receive packet, code "));
      Serial.println(state);
    }

    fsk.setDio0Action(setFlag);
    // put module back to listen mode
    fsk.startReceive();
    
    enableInterrupt = true; // ready for next package
  }
  //delay(100);
/*
  state = fsk.disableAddressFiltering();
  if (state != ERR_NONE) {
    Serial.println(F("Unable to remove address filter, code "));
  }
*/
}
