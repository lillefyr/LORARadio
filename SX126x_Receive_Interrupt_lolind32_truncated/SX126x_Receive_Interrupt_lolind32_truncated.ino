
#include <RadioLib.h>

SX1268 radio = new Module(5, 14, 26, 27);  //Port-Pin Input:  SPI select, Reset, Busy, Interrupt DIO1
//SX1262 radio = new Module(5, 27, 14, 26);  // nss, DIO1, rst, busy

void setup() {
  Serial.begin(115200);
  int state = radio.begin();
/*
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
*/
  radio.setDio1Action(setFlag);
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
delay(1000);
}
