/* Theory: bandwidth definitions on SX1268 and SX1278 are different and hardcoded steps */
#include <RadioLib.h>
#define NSSpin (10)
#define DIO0pin (2)
#define RESETpin (9)
#define DIO1pin (3)
SX1278 radio = new Module(10, 2, 9, 3);
int state;


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


void setup() {
  Serial.begin(115200);

  Serial.println("\nSX127_Receive_LoraHat_ok_minimal");

  // initialize SX1278 FSK modem with default settings
  state = radio.beginFSK();
  if (state == ERR_NONE) {
    Serial.println(F("beginFSK success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  // if needed, you can switch between LoRa and FSK modes
  //
  // radio.begin()       start LoRa mode (and disable FSK)
  // radio.beginFSK()    start FSK mode (and disable LoRa)

  // the following settings can also
  // be modified at run-time
  state = radio.setFrequency(868.1); //432.5);
  state = radio.setBitRate(10.0);
  state = radio.setEncoding(0);
  state = radio.setFrequencyDeviation(10.0);
  state = radio.setRxBandwidth(250.0);
  state = radio.setOutputPower(10.0);
  state = radio.setCurrentLimit(100);
  state = radio.setDataShaping(RADIOLIB_SHAPING_0_5);
  uint8_t syncWord[] = {0x01, 0x23, 0x45, 0x67,
                        0x89, 0xAB, 0xCD, 0xEF};
  state = radio.setSyncWord(syncWord, 8);
  if (state != ERR_NONE) {
    Serial.print(F("Unable to set configuration, code "));
    Serial.println(state);
    while (true);
  }

  // FSK modulation can be changed to OOK
  //doesnt work for me
  // NOTE: When using OOK, the maximum bit rate is only 32.768 kbps!
  //       Also, data shaping changes from Gaussian filter to
  //       simple filter with cutoff frequency. Make sure to call
  //       setDataShapingOOK() to set the correct shaping!
  /*
  state = radio.setOOK(true);
  state = radio.setDataShapingOOK(1);
  if (state != ERR_NONE) {
    Serial.print(F("Unable to change modulation, code "));
    Serial.println(state);
    while (true);
  }
  */
  // set the function that will be called
  // when new packet is received
  radio.setDio0Action(setFlag);
  radio.startReceive();
  receivedFlag = false;
  volatile bool enableInterrupt = true;
  
  if (state == ERR_NONE) {
    Serial.println(F("startReceive success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
}

String str;
void loop() {
  state = 0;
  
  // check if the flag is set
  if(receivedFlag) {
    // disable the interrupt service routine while
    // processing the data
    enableInterrupt = false;

    // reset flag
    receivedFlag = false;

    // receive FSK packet
    state = radio.readData(str);
    /*
      byte byteArr[8];
      int state = radio.receive(byteArr, 8);
    */
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

    // put module back to listen mode
    radio.setDio0Action(setFlag);
    radio.startReceive();

    // we're ready to receive more packets,
    // enable interrupt service routine
    enableInterrupt = true;
  }
}
