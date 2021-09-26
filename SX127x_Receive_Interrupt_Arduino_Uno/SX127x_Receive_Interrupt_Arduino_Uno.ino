
// include the library
#include <RadioLib.h>

// SX1276 has the following connections:
// NSS pin:   10
// DIO0 pin:  2
// RESET pin: 9
// DIO1 pin:  3
//SX1276 radio = new Module(10, 2, 9, 3); //Lolind32 with 434 module
//SX1276 radio = new Module(10, 2, 3);  // Arduino with 434 module (toasted)
SX1276 radio = new Module(10, 2, 6);  // Arduino with LoraHat
//Interrupt does not work here as pin6 is not an interrupt pin


void setup() {
  Serial.begin(115200);

  // initialize SX1278 with default settings
  Serial.println();
  Serial.println(F("SX127x_Receive_Interrupt_Arduino_Uno"));
  // carrier frequency:           436.7 MHz
  // bandwidth:                   125.0 kHz
  // spreading factor:            9
  // coding rate:                 7
  // sync word:                   0x12
  // output power:                17 dBm
  // current limit:               100 mA
  // preamble length:             8 symbols
  // amplifier gain:              0 (automatic gain control)
  // begin(float freq = 434.0, float bw = 125.0, uint8_t sf = 9, uint8_t cr = 7, uint8_t syncWord = SX126X_SYNC_WORD_PRIVATE, int8_t power = 14, float currentLimit = 60.0, uint16_t preambleLength = 8, float tcxoVoltage = 1.6);
#define FREQUENCY             868.1 // 436.7   // MHz
#define BANDWIDTH             125.0   // kHz
#define SPREADING_FACTOR      11      // -
#define CODING_RATE           8       // 4/8
#define SYNC_WORD             0x12    // used as LoRa "sync word", or twice repeated as FSK sync word (0x1212)
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
                          (uint8_t)CURRENT_LIMIT);
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  // set the function that will be called
  // when new packet is received
  radio.setDio0Action(setFlag);

  // start listening for LoRa packets
  Serial.print(F("[SX1278] Starting to listen ... "));
  state = radio.startReceive();
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  // if needed, 'listen' mode can be disabled by calling
  // any of the following methods:
  //
  // radio.standby()
  // radio.sleep()
  // radio.transmit();
  // radio.receive();
  // radio.readData();
  // radio.scanChannel();
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
  // check if the flag is set
  if(receivedFlag) {
    // disable the interrupt service routine while
    // processing the data
    enableInterrupt = false;

    // reset flag
    receivedFlag = false;

    // you can read received data as an Arduino String
    String str;
    int state = radio.readData(str);

    // you can also read received data as byte array
    /*
      byte byteArr[8];
      int state = radio.receive(byteArr, 8);
    */

    if (state == ERR_NONE) {
      // packet was successfully received
      Serial.println(F("[SX1278] Received packet!"));

      // print data of the packet
      Serial.print(F("[SX1278] Data:\t\t"));
      Serial.println(str);

      // print RSSI (Received Signal Strength Indicator)
      Serial.print(F("[SX1278] RSSI:\t\t"));
      Serial.print(radio.getRSSI());
      Serial.println(F(" dBm"));

      // print SNR (Signal-to-Noise Ratio)
      Serial.print(F("[SX1278] SNR:\t\t"));
      Serial.print(radio.getSNR());
      Serial.println(F(" dB"));

      // print frequency error
      Serial.print(F("[SX1278] Frequency error:\t"));
      Serial.print(radio.getFrequencyError());
      Serial.println(F(" Hz"));

    } else if (state == ERR_CRC_MISMATCH) {
      // packet was received, but is malformed
      Serial.println(F("[SX1278] CRC error!"));

    } else {
      // some other error occurred
      Serial.print(F("[SX1278] Failed, code "));
      Serial.println(state);

    }

    // put module back to listen mode
    radio.setDio0Action(setFlag);
    radio.startReceive();

    // we're ready to receive more packets,
    // enable interrupt service routine
    enableInterrupt = true;
  }

}
