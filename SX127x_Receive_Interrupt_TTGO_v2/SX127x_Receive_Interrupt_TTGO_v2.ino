
// include the library
#include <RadioLib.h>

// SX1276 has the following connections:
// NSS pin:   10
// DIO0 pin:  2
// RESET pin: 9
// DIO1 pin:  3
//SX1276 radio = new Module(10, 2, 9, 3); //Lolind32 with 434 module
//SX1276 radio = new Module(10, 2, 3);  // Arduino with 434 module (toasted)
//SX1276 radio = new Module(10, 2, 6);  // Arduino with LoraHat
SX1276 radio = new Module(18, 26, 33);  // TTGO v2, oled
//L_NSS, L_DI00, L_DI01, L_RST,  L_MISO, L_MOSI, L_SCK
//   18,     26,     33,    14,      19,     27,     5
//Interrupt does not work here as pin6 is not an interrupt pin

float RSSI = 0.0;
float SNR = 0.0;
float FreqErr = 0.0;
String str;
char outbuf[100];

void setup() {
  Serial.begin(115200);

  // initialize SX1278 with default settings
  Serial.println();
  Serial.println(F("SX127x_Receive_Interrupt_Transmit_TTGO_v2"));

#define FREQUENCY             436.7   // MHz
#define BANDWIDTH             125.0   // kHz
#define SPREADING_FACTOR      11      // -
#define CODING_RATE           8       // 4/8    WAS 5
#define SYNC_WORD             0x12    // used as radio "sync word", or twice repeated as FSK sync word (0x1212)
#define OUTPUT_POWER          20      // dBm
#define CURRENT_LIMIT         120     // mA
#define radio_PREAMBLE_LEN     8       // symbols
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
    Serial.println(F("Lora success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  radio.setDio0Action(setFlag);

  // start listening for radio packets
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
    int state = radio.readData(str);

    if (state == ERR_NONE) {
      
      RSSI = radio.getRSSI();
      SNR = radio.getSNR();
      FreqErr = radio.getFrequencyError();
      sprintf(outbuf, "%s RSSI=%f SNR=%f FreqErr=%f\0",str, RSSI, SNR, FreqErr);
      Serial.println(outbuf);
      state = radio.transmit(outbuf);

    } else if (state == ERR_CRC_MISMATCH) {
      // packet was received, but is malformed
      Serial.println(F("[SX1278] CRC error!"));
    } else {
      // some other error occurred
      Serial.print(F("[SX1278] Failed, code "));
      Serial.println(state);
    }
    
    radio.setDio0Action(setFlag);
    radio.startReceive();

    // we're ready to receive more packets,
    // enable interrupt service routine
    enableInterrupt = true;

  }
}
