/* Theory: bandwidth definitions on SX1268 and SX1278 are different and hardcoded steps */
#include <RadioLib.h>
#define NSSpin (10)
#define DIO0pin (2)
#define RESETpin (9)
#define DIO1pin (3)
SX1278 fsk = new Module(10, 2, 9, 3);

volatile bool receivedFlag = false;
volatile bool enableInterrupt = true;
void setFlag(void) {
  if(!enableInterrupt) {
    return;
  }
  receivedFlag = true;
}

void setup() {
  Serial.begin(9600);
  //int state = fsk.beginFSK( 434.0f, 9.6f, 5.0f, 125.0f, 20, 140, 16, false );
  int state = fsk.beginFSK( );
  uint8_t syncWordFSK[2] = { 0x2D, 0x01 }; // Hardcoded in RadioLib SX127x.cpp
  //state = fsk.setSyncWord(syncWordFSK, 2);
  //state = fsk.setEncoding(0);
  //fsk.setDio0Action(setFlag);
  //state = fsk.startReceive();
  //Serial.println(state);
  enableInterrupt = true;
  receivedFlag = false;
}

void loop() {
  if(receivedFlag) {
    enableInterrupt = false;
    fsk.clearDio0Action();
    receivedFlag = false;
    String str;
    int state = fsk.readData(str);
    if (state == ERR_NONE) {
      Serial.println(str);
    }
    fsk.setDio0Action(setFlag);
    fsk.startReceive();
    enableInterrupt = true;
  }
  else
  {
    String str;
    int state = fsk.receive(str);
    if (state == ERR_NONE) {
      Serial.println(str);
    }else{
      Serial.println(state);
    }
  }
  delay(5);
}
